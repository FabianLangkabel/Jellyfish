#ifndef PLUGINS_HPP_INCLUDED
#define PLUGINS_HPP_INCLUDED

#include "iplugins.hpp"
#include "shared_library.hpp"
#include "singleton.hpp"

#include <iostream>

#ifdef __has_include
#if __has_include(<filesystem>)
#include <filesystem>
namespace std_filesystem = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace std_filesystem = std::experimental::filesystem;
#else
#error "Missing <filesystem>"
#endif
#else
#include <filesystem>
namespace std_filesystem = std::filesystem;
#endif


namespace micro {

  //Class for loading and unloading plugins. Plugins has shared pointer to it.

  template<std::size_t L = MAX_PLUGINS_ARGS>
  class plugins final : public iplugins<>, public singleton<plugins<L>>, public std::enable_shared_from_this<plugins<L>> {
  private:

    friend class singleton<plugins<L>>;
    std::string path_; // paths for plugins

    std::map<
      std::string,
      std::tuple<std::shared_ptr<shared_library>,std::shared_ptr<iplugin<>>
      >
    > plugins_;

    /**
      Creates plugins object

      \param[in] v version of plugins kernel
      \param[in] nm name of the plugins kernel
      \param[in] path0 paths for search plugins exploded by ':', see env $PATH

      \see singleton::get(Args&&... args), storage::storage(int v, const std::string& nm), storage::version(), storage::name()
    */
    explicit plugins(int v = make_version(1,0), const std::string& nm = "microplugins service", const std::string& path0 = "microplugins") : iplugins<>(v, nm),singleton<plugins<L>>(),std::enable_shared_from_this<plugins<L>>(),path_(path0),plugins_() {}

  public:

    /** \see storage::subscribe(const std::string& nm, const T& t, const std::string& hlp) */
    using storage<>::subscribe;

    /** \see storage::unsubscribe(const T& nm) */
    using storage<>::unsubscribe;

    plugins& operator=(const plugins& rhs) = delete;

    plugins& operator=(plugins&& rhs) = delete;

    ~plugins() override { unload(); }

    /** \returns Shared pointer to interface. \see iplugins::iplugins(int v, const std::string& nm), storage::storage(int v, const std::string& nm) */
    std::shared_ptr<iplugins<>> get_shared_ptr() override {
      return std::shared_ptr<iplugins<>>(plugins<>::shared_from_this());
    }

    /** Stops thread of management plugins. \see run(), is_run() */
    void unload() noexcept {
      unload_plugins();
    }

    /** \returns Amount of loaded plugins in this moment. \see iplugins::count_plugins() */
    std::size_t count_plugins() const override {
      return std::size(plugins_);
    }

    /** \returns Shared pointer to plugin. \param[in] nm name of plugin \see iplugins::get_plugin(const std::string& nm) */
    /*
    std::shared_ptr<iplugin<>> get_plugin(const std::string& nm) override {
      // search in loaded dll's
      if (auto it = plugins_.find(nm); it != std::end(plugins_)) { return std::get<1>(it->second); }
      #if (!defined(NDEBUG) || defined(DEBUG))
      std::clog << "[JellyfishCore] status of loading plugin '" << nm << "': ";
      #endif
      // try to load dll from system
      std::shared_ptr<iplugin<>> ret = nullptr;
      if (auto dll = std::make_shared<shared_library>(nm, path_); dll && dll->is_loaded()) {
        if (auto loader = dll->get<import_plugin_cb_t>("import_plugin"); loader) {
          if (auto ii = dll->get<std::shared_ptr<micro::iinfo>()>("import_plugin"); ii && ii() && ii()->type_info() == type_info() && (ret = loader())) {
            ret->plugins_ = get_shared_ptr();
            plugins_[nm] = {dll, ret};
            #if (!defined(NDEBUG) || defined(DEBUG))
            std::clog << "success" << std::endl;
            #endif
          }
        }
      }
      if (!ret) {
        #if (!defined(NDEBUG) || defined(DEBUG))
        std::clog << "fail" << std::endl;
        #endif
      } 
      return ret;
    }
    */

    std::shared_ptr<iplugin<>> get_plugin(const std::string& nm) override {
      if (auto it = plugins_.find(nm); it != std::end(plugins_)) { return std::get<1>(it->second); }
      else {std::clog << "[JellyfishCore] Pluginfile '" << nm << "' not found!" << std::endl;}
      return nullptr;
    }

    std::vector<std::tuple<std::string,std::shared_ptr<iplugin<>>>> get_all_plugins(){
      std::vector<std::tuple<std::string,std::shared_ptr<iplugin<>>>> ret;
      for (auto const& [key, val] : plugins_)
      {
        ret.push_back({key, std::get<1>(val)});
      }
      return ret;
    }

    //std::vector<std::shared_ptr<iplugin<>>> get_all_plugins() {
    void load_all_plugins() {
      std::cout << "[JellyfishCore] Load all Plugins" << std::endl;
      std::string ExecutablePath = std_filesystem::current_path().string();
      ExecutablePath.substr(0, ExecutablePath.find_last_of("\\/"));
      //std::cout << "Current path is " << ExecutablePath << '\n';
      std::string PluginPath = ExecutablePath + "/Plugins";
      for (const auto & entry : std_filesystem::directory_iterator(PluginPath))
      {
        //std::cout << entry.path() << std::endl;
        std::shared_ptr<iplugin<>> ret = nullptr;
        #if (!defined(NDEBUG) || defined(DEBUG))
        std::clog << "[JellyfishCore] status of loading pluginfile '" << entry.path().filename().string() << "': ";
        #endif
        if (auto dll = std::make_shared<shared_library>(entry.path().string()); dll && dll->is_loaded()) {
          if (auto loader = dll->get<import_plugin_cb_t>("import_plugin"); loader) {
            if (auto ii = dll->get<std::shared_ptr<micro::iinfo>()>("import_plugin"); ii && ii() && ii()->type_info() == type_info() && (ret = loader())) {
              ret->plugins_ = get_shared_ptr();
              plugins_[entry.path().filename().string()] = {dll, ret};
              #if (!defined(NDEBUG) || defined(DEBUG))
              std::clog << "success" << std::endl;
              #endif
            }
          }
        }
        if (!ret) {
          #if (!defined(NDEBUG) || defined(DEBUG))
          std::clog << "fail" << std::endl;
          #endif
        } 
      }
      std::cout << "[JellyfishCore] All Plugins loaded" << std::endl;
    }

    /** \returns Shared pointer to plugin. \param[in] i index of plugin \see count_plugins(), iplugins::get_plugin(int i) */
    std::shared_ptr<iplugin<>> get_plugin(std::size_t i) override {
      if (i < std::size(plugins_)) {
        for (auto it = std::begin(plugins_); it != std::end(plugins_); ++it) {
          if (!i--) { return std::get<1>(it->second); }
        }
      } return nullptr;
    }

    /** Unloads plugin. \param[in] nm name of plugin */
    void unload_plugin(const std::string& nm) noexcept {
      if (auto it = plugins_.find(nm); it != std::end(plugins_)) {
        #if (!defined(NDEBUG) || defined(DEBUG))
        std::clog << "[JellyfishCore] terminate plugin: '" << nm << "'" << std::endl;
        #endif
        plugins_.erase(it);
      }
    }

    /** Unloads plugin. \param[in] i index of plugin \see count_plugins() */
    void unload_plugin(std::size_t i) noexcept {
      if (i < std::size(plugins_)) {
        for (auto it = std::begin(plugins_); it != std::end(plugins_); ++it) {
          if (!i--) {
            #if (!defined(NDEBUG) || defined(DEBUG))
            std::clog << "[JellyfishCore] terminate plugin: '" << std::get<1>(it->second)->name() << "'" << std::endl;
            #endif
            plugins_.erase(it);
          }
        }
      }
    }

  private:

    void unload_plugins() noexcept {
      for (auto it = std::begin(plugins_); it != std::end(plugins_); ++it) {
        #if (!defined(NDEBUG) || defined(DEBUG))
        std::clog << "[JellyfishCore] terminate plugin: '" << std::get<1>(it->second)->name() << "'" << std::endl;
        #endif
        plugins_.erase(it++);
      } 
      if (!std::empty(plugins_)) { std::clog << "NOCH NICHT ALLE PLUGINS ENTLADEN" << std::endl; }
    }

    /*
    void unload_plugins() noexcept {
      while (!std::empty(plugins_)) {
        for (auto it = std::begin(plugins_); it != std::end(plugins_); ++it) {
          if (std::get<1>(it->second).use_count() > 1) {
            #if (!defined(NDEBUG) || defined(DEBUG))
            std::clog << "[JellyfishCore] wait termination plugin: '" << std::get<1>(it->second)->name() << "'" << std::endl;
            #endif
          } else { plugins_.erase(it++); }
        } 
        if (!std::empty(plugins_)) { micro::sleep<micro::seconds>(1); }
      }
    }
    */

  };

}

#endif
