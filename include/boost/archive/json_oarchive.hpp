#ifndef BOOST_JSON_ARCHIVE_OARCHIVE_H
#define BOOST_JSON_ARCHIVE_OARCHIVE_H

// C++ Standard Library
#include <iterator>
#include <ostream>
#include <type_traits>

// Boost
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/archive/detail/register_archive.hpp>

// Boost Archive JSON
#include "boost/JsonContext.hpp"
#include "boost/archive/TraitsDetailsHelper.hpp"

namespace boost {
namespace archive {

class BOOST_SYMBOL_EXPORT json_oarchive : public detail::common_oarchive<json_oarchive> {
public:
  explicit json_oarchive(std::ostream &os, unsigned int = 0, const bool prettify = false);

  ~json_oarchive();

  template <typename T> void save_fundamental(const T &value) {
    if (m_ctx.empty()) {
      boost::json::object o;
      o[m_ctx.currentTag()] = value;
      m_ctx.setRoot(m_ctx.currentTag(), std::make_shared<boost::json::value>(o));
    } else if (m_ctx.top().second->is_object()) {
      JsonContext::emplace(*m_ctx.top().second, value);
    } else if (m_ctx.top().second->is_array()) {
      m_ctx.top().second->as_array().push_back(boost::json::value(value));
    }
  }

  template <typename T>
  std::enable_if_t<detail::is_std_vector<T>::type::value or detail::is_fixed_size_array<T>::type::value>
  save_array(const T &value) {
    boost::json::array &array = m_ctx.current().second->get_array();
    int index = 0;
    for (const auto &v : value) {
      if constexpr ((std::is_class<typename T::value_type>::value && !std::is_same<typename T::value_type, std::string>::value) ||
                    std::is_pointer<typename T::value_type>::value) {
        if constexpr ((detail::is_std_vector<typename T::value_type>::value or
                       detail::is_fixed_size_array<typename T::value_type>::value) or
                      detail::is_fixed_size_old_school_array<typename T::value_type>::value) {
          m_ctx.push(std::to_string(index), std::make_shared<json::value>(boost::json::array()));
        } else {
          m_ctx.push(std::to_string(index), std::make_shared<json::value>(boost::json::object()));
        }
        save(v);
        array.push_back(*m_ctx.top().second);
      } else {
        save(v);
      }
      if constexpr ((std::is_class<typename T::value_type>::value && !std::is_same<typename T::value_type, std::string>::value) ||
                    std::is_pointer<typename T::value_type>::value) {
        m_ctx.pop();
      }
      index++;
    }
  }

  template <typename T> void save(const T &value) {
    if constexpr (std::is_fundamental<T>::value || std::is_same<T, std::string>::value) {
      save_fundamental(value);
    } else if constexpr (std::is_enum<T>::value) {
      save_fundamental(static_cast<int64_t>(value));
    } else if constexpr ((detail::is_std_vector<T>::value or detail::is_fixed_size_array<T>::value) and
                         !detail::is_fixed_size_old_school_array<T>::value) {
      save_array(value);
    } else if constexpr (detail::is_fixed_size_old_school_array<T>::value) {
      std::array<typename std::remove_all_extents<T>::type, detail::is_fixed_size_old_school_array<T>::size> &arr =
          reinterpret_cast<
              std::array<typename std::remove_all_extents<T>::type, detail::is_fixed_size_old_school_array<T>::size> &>(
              *const_cast<typename std::remove_const<T>::type *>(&value));
      save_array(arr);
    } else if constexpr (detail::is_std_map<T>::value) {
      std::vector<std::pair<typename T::key_type, typename T::mapped_type>> vec;
      std::transform(value.begin(), value.end(), std::back_inserter(vec),
                     [](const std::pair<typename T::key_type, typename T::mapped_type> &p) { return p; });
      save_array(vec);
    } else if constexpr (detail::is_shared_ptr<T>::value or detail::is_unique_ptr<T>::value) {
      typename T::element_type *ptr = value.get();
      detail::common_oarchive<json_oarchive>::save_override(ptr);
    } else if constexpr (detail::is_weak_ptr<T>::value) {
      typename T::element_type *ptr = value.lock().get();
      detail::common_oarchive<json_oarchive>::save_override(ptr);
    } else {
      detail::common_oarchive<json_oarchive>::save_override(value);
    }
  }

  template <class T> void save_override(T &t) {
    // If your program fails to compile here, its most likely due to
    // not specifying an nvp wrapper around the variable to
    // be serialized.
    BOOST_MPL_ASSERT((serialization::is_wrapper<T>));
    detail::common_oarchive<json_oarchive>::save_override(t);
  }

  /*************************************************************************
   *                              NVP                                      *
   *************************************************************************/
  template <class T> void save_override(const boost::serialization::nvp<T> &kv) {
    boost::json::object o;
    std::shared_ptr<boost::json::value> root_ptr = nullptr;
    std::string name = kv.name() ? kv.name() : "px";
    if (!m_ctx.empty()) {
      root_ptr = m_ctx.top().second;
      boost::json::value &root = *root_ptr;
      if (root.is_object()) {
        if constexpr (detail::is_std_vector<T>::value || detail::is_fixed_size_array<T>::value || detail::is_std_map<T>::value) {
          root.as_object().emplace(name, boost::json::array());
        } else {
          root.as_object().emplace(name, o);
        }
        m_ctx.push(name, std::make_shared<json::value>(root.as_object()[name]));
      } else if (root.is_array()) {
        root.as_array().push_back(boost::json::object{});
        m_ctx.push(name, std::make_shared<json::value>(root.as_array().at(root.as_array().size() - 1)));
      }
    } else if constexpr (!std::is_same<T, std::string>::value && !detail::is_std_vector<T>::value &&
                         !detail::is_std_map<T>::value && !detail::is_fixed_size_array<T>::value &&
                         (std::is_class<T>::value || std::is_pointer<T>::value)) {
      o[demangle(typeid(T).name())] = kv.name();
      m_ctx.setRoot(name, std::make_shared<boost::json::value>(o));
      root_ptr = m_ctx.root();
    } else if constexpr (detail::is_std_vector<T>::value || detail::is_fixed_size_array<T>::value ||
                         detail::is_std_map<T>::value) {
      boost::json::array a;
      o[kv.name()] = a;
      m_ctx.setRoot(name, std::make_shared<boost::json::value>(o));
      m_ctx.push(name, std::make_shared<json::value>(a));
      root_ptr = m_ctx.root();
    } else {
      m_ctx.setCurrentTag(kv.name());
    }
    this->save(kv.const_value());
    if (root_ptr && m_ctx.current().second != root_ptr) {
      if (root_ptr->is_object() && m_ctx.current().second->is_object()) {
        root_ptr->get_object()[name] = m_ctx.current().second->get_object();
      } else if (root_ptr->is_object() && m_ctx.current().second->is_array()) {
        root_ptr->get_object()[name] = m_ctx.current().second->get_array();
      } else if (!m_ctx.current().second->is_null()) {
        if (root_ptr->is_object()) {
          root_ptr->get_object()[name] = *m_ctx.current().second;
        }
      }
    }
    if (m_ctx.size() > 1) {
      m_ctx.pop();
    }
  }

  /*************************************************************************
   *                          METATYPES                                    *
   *************************************************************************/
  void save_override(const class_name_type &t);
  void save_override(const version_type &t);
  void save_override(const object_id_type &t);
  void save_override(const object_reference_type &t);
  void save_override(const class_id_type &t);
  void save_override(const class_id_optional_type &t);
  void save_override(const class_id_reference_type &t);
  void save_override(const tracking_type &t);

private:
  JsonContext m_ctx;
  std::ostream &os_;
  bool prettify_ = false;
};

} // namespace archive
} // namespace boost

BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::json_oarchive)

#endif // BOOST_JSON_ARCHIVE_OARCHIVE_H
