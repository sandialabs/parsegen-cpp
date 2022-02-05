#pragma once

#include <utility>
#include <stdexcept>

namespace parsegen {

// this is like unique_ptr, except for a few changes:
// 1. a copy constructor is provided, but it throws if actually called.
//    this is to allow object_pointer to be used as the value type to std::any
// 2. the dereference operator throws if the pointer is null

template <class T>
class object_pointer {
  T* m_pointer{nullptr};
  T* get_non_null() const
  {
    if (m_pointer == nullptr) {
      throw std::logic_error("tried to dereference an object_pointer with null pointer.");
    }
    return m_pointer;
  }
 public:
  object_pointer() = default;
  object_pointer(T* arg)
   :m_pointer(arg)
  {}
  object_pointer(object_pointer const&)
  {
    throw std::logic_error("object_pointer copy constructor called");
  }
  template <class U>
  object_pointer(object_pointer<U>&& other)
   :m_pointer(other.release())
  {
  }
  object_pointer& operator=(object_pointer const&)
  {
    throw std::logic_error("object_pointer copy assignment operator called");
  }
  template <class U>
  object_pointer& operator=(object_pointer<U>&& other)
  {
    delete m_pointer;
    m_pointer = other.release();
  }
  ~object_pointer()
  {
    delete m_pointer;
    m_pointer = nullptr;
  }
  explicit operator bool() const
  {
    return m_pointer != nullptr;
  }
  T* get() const
  {
    return m_pointer;
  }
  T& operator*() const
  {
    return *(get_non_null());
  }
  T* operator->() const
  {
    return get_non_null();
  }
  T* release()
  {
    T* result = m_pointer;
    m_pointer = nullptr;
    return result;
  }
};

template <class T, class ... Args>
[[nodiscard]] object_pointer<T> make_object(Args&& ... args)
{
  return object_pointer<T>(new T(std::forward<Args>(args)...));
}

}
