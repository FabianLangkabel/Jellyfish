/** \file task.hpp */
#ifndef TASK_HPP_INCLUDED
#define TASK_HPP_INCLUDED

#include <future>
#include <functional>
#include <any>
#include <atomic>
#include <memory>
#include <type_traits>
#include <limits> // std::numeric_limits

#ifndef MAX_PLUGINS_ARGS
#define MAX_PLUGINS_ARGS 6
#endif

namespace micro {

  /**
    \class task
    \brief Extended functor
    \author Dmitrij Volin
    \date august of 2018 year
    \copyright Boost Software License - Version 1.0

    Template functor with returning value type std::any and any type of arguments.

    \code
    micro::task<int,int> t2 = [](int a, int b)->std::any{return a+b;};
    std::shared_future<std::any> result = t2(10, 90); result.wait();
    std::cout << std::any_cast<int>(result.get()) << std::endl;

    t2.name("sum2");
    t2.help("function for sum two integers; returns std::any");

    if (!t2.empty()) t2.reset();
    \endcode
  */
  template<typename... Ts>
  class task final {
  private:

    std::string name_, help_;
    decltype(std::function<std::any(Ts...)>()) fn_;

  public:

    /** Creates empty task. */
    task():name_(),help_(),fn_(nullptr) {}

    /** Creates task. \param[in] nm name of task \param[in] t function/method/lambda \param[in] hlp message help for task \see name(), help(), run(Args&&... args), run_once(Args&&... args) */
    task(const std::string& nm, const decltype(std::function<std::any(Ts...)>()) &t, const std::string& hlp = {}):task() {
      name_ = nm;
      fn_ = t;
      help_ = hlp;
    }

    /** Creates task. \param[in] t function/method/lambda */
    task(const decltype(std::function<std::any(Ts...)>()) &t):task() { *this = t; }

    /** Creates task by copyable constructor. \param[in] rhs task for copying */
    task(const task<Ts...>& rhs):task() { *this = rhs; }

    /** Creates task by movable constructor. \param[in] rhs task for moving */
    task(task<Ts...>&& rhs):task() { *this = rhs; }

    ~task() {}

    /** \returns Amount of arguments for task (calculatings in compile time). */
    std::size_t max_args() const noexcept {
      constexpr static const std::size_t nargs = sizeof...(Ts);
      return nargs;
    }

    /** \returns Shared future for result task called. \param[in] args arguments for task */
    template<typename... Args>
    inline std::shared_future<std::any> run(Args&&... args) {
      if (!fn_) { return {}; }
      else {
        return std::async(std::launch::async, fn_, std::forward<Args>(args)...);
      }
    }

    /** \see run(Args&&... args) */
    template<typename... Args>
    inline std::shared_future<std::any> operator()(Args&&... args) { return run(std::forward<Args>(args)...); }

    /** \returns Name of task. \see name(const std::string& nm) */
    const std::string& name() const noexcept { return name_; }

    /** Sets name for task. \param[in] nm name for task \see name() */
    void name(const std::string& nm) noexcept { name_ = nm; }

    /** Resets pointer to function. */
    void reset() noexcept { fn_ = nullptr; }

    /** \returns True if task is nulled. */
    bool empty() const noexcept { return (fn_.target() == nullptr); }

    /** Assignment. \param[in] t function/method/lambda */
    task<Ts...>& operator=(const decltype(std::function<std::any(Ts...)>()) &t) {
      fn_ = t;
      return *this;
    }

    /** Copyable assignment. \param[in] rhs task for copying */
    task<Ts...>& operator=(const task<Ts...>& rhs) noexcept {
      if (this != &rhs) {
        name_ = rhs.name_;
        help_ = rhs.help_;
        fn_ = rhs.fn_;
      } return *this;
    }

    /** Movable assignment. \param[in] rhs task for moving */
    task<Ts...>& operator=(task<Ts...>&& rhs) noexcept {
      if (this != &rhs) {
        name_ = std::move(rhs.name_);
        help_ = std::move(rhs.help_);
        fn_ = std::move(rhs.fn_);
      } return *this;
    }

  };

} // namespace micro

#endif // TASK_HPP_INCLUDED
