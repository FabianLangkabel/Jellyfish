/** \file storage.hpp */
#ifndef STORAGE_HPP_INCLUDED
#define STORAGE_HPP_INCLUDED

#include "iinfo.hpp"
#include "tasks.hpp"

#include <shared_mutex>
#include <tuple>

namespace micro {

  inline int is_le() noexcept { static const std::uint32_t i = 0x04030201; return (*((std::uint8_t*)(&i)) == 1); }
  inline int make_version(int Major, int Minor) noexcept { return ((Major << 8) | Minor); }
  inline int get_major(int Version) noexcept { return (Version >> 8); }
  inline int get_minor(int Version) noexcept { return (is_le() ? Version & 0xff : Version & 0xff000000); }

  /**
    \class storage
    \brief Storage for tasks
    \author Dmitrij Volin
    \date august of 2018 year
    \copyright Boost Software License - Version 1.0

    All plugins inherits from this class. It is container for tasks.

    All tasks has returning and arguments type is std::any.

    Maximum arguments for tasks is 6, minimum is 0.

    You can change it for your needs by defining constant with cmake while configure:

    > ~/build $ cmake -DMAX_PLUGINS_ARGS=12 ../

    \see subscribe(const std::string& nm, const T& t, const std::string& hlp), unsubscribe(const T& nm)
  */
  template<std::size_t L = MAX_PLUGINS_ARGS>
  class storage : public iinfo {
  private:

    template<std::size_t> friend class plugins;

    mutable std::shared_mutex mtx_;
    int version_;
    std::string name_;

    template<typename T, std::size_t N, typename... Args>
    struct gen_tasks_type { using type = typename gen_tasks_type<T, N-1, T, Args...>::type; };

    template<typename T, typename... Args>
    struct gen_tasks_type<T, 0, Args...> { using type = tasks<Args...>; };

    template<typename T, std::size_t N, typename... Args>
    struct gen_storage_type { using type = typename gen_storage_type<T, N-1, typename gen_tasks_type<T, N>::type, Args...>::type; };

    template<typename T, typename... Args>
    struct gen_storage_type<T, 0, Args...> { using type = std::tuple<typename gen_tasks_type<T, 0>::type, Args...>; };

    typename gen_storage_type<std::any, L>::type tasks_;

  protected:

    /** Creates storage of tasks. \param[in] v version of storage \param[in] nm name of storage */
    explicit storage(int v = make_version(1,0), const std::string& nm = {}):iinfo(typeid(storage<L>)),
    mtx_(),version_(v),name_(nm),tasks_() {
      static_assert(L > 0, "\n\nPlease, set up MAX_PLUGINS_ARGS constant as least to value 1 by: /path/to/build $ cmake -DMAX_PLUGINS_ARGS=12 ../ or what you need...\n");
    }

    /** Adds task into storage for given number arguments in I. \param[in] nm name of task \param[in] t function/method/lambda \param[in] hlp message help for task */
    template<std::size_t I, typename T>
    void subscribe(const std::string& nm, const T& t, const std::string& hlp = {}) {
      static_assert((L > 0 && I < L), "\n\nOut of range for valid number arguments of plugin's function. \nPlease, set it to larger value by: /path/to/build $ cmake -DMAX_PLUGINS_ARGS=12 ../ or what you need...\n");
      std::unique_lock<std::shared_mutex> lock(mtx_);
      std::get<I>(tasks_).subscribe(nm, t, hlp);
    }

    /** Removes task from storage for given number arguments in I. \param[in] nm index or name of task */
    template<std::size_t I, typename T>
    void unsubscribe(const T& nm) {
      std::unique_lock<std::shared_mutex> lock(mtx_);
      if constexpr (I < L) {
        std::get<I>(tasks_).unsubscribe(nm);
      }
    }

  public:

    ~storage() override {}

    /** \returns Version of storage. */
    int version() const noexcept { return version_; }

    /** \returns Major version of storage. */
    int major() const noexcept { return get_major(version_); }

    /** \returns Minor version of storage. */
    int minor() const noexcept { return get_minor(version_); }

    /** \returns Name of storage. */
    const std::string& name() const noexcept { return name_; }

    /** \returns Maximum arguments for tasks of storage. */
    std::size_t max_args() const noexcept { return L; }

    /** Runs task for given number arguments in I. \param[in] nm index or name of task \param[in] args arguments for task \returns Shared future for result \see std::shared_future, std::any, std::async */
    template<std::size_t I, typename T, typename... Args>
    inline std::any run(const T& nm, Args&&... args) {
      std::shared_lock<std::shared_mutex> lock(mtx_);
      if constexpr (I < L) { return std::get<I>(tasks_)[nm](std::forward<Args>(args)...); }
      else { return {}; }
    }

    /** \returns Amount tasks in storage for given number arguments in I. */
    template<std::size_t I>
    inline std::size_t count() const noexcept {
      std::shared_lock<std::shared_mutex> lock(mtx_);
      if constexpr (I < L) { return std::get<I>(tasks_).count(); }
      else { return 0; }
    }

    /** \returns True if tasks in storage has task for given number arguments in I. \param[in] nm index or name of task */
    template<std::size_t I, typename T>
    inline bool has(const T& nm) const noexcept {
      std::shared_lock<std::shared_mutex> lock(mtx_);
      if constexpr (I < L) { return std::get<I>(tasks_).has(nm); }
      else { return false; }
    }

    /** \returns Name of task in storage for given number arguments in I. \param[in] nm index or name of task */
    template<std::size_t I, typename T>
    std::string name(const T& nm) const noexcept {
      std::shared_lock<std::shared_mutex> lock(mtx_);
      if constexpr (I < L) { return std::get<I>(tasks_)[nm].name(); }
      else { return {}; }
    }
  };

} // namespace micro

#endif // STORAGE_HPP_INCLUDED
