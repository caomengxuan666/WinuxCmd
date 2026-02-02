#pragma once
#include <cstdint>
#include <string>
#include <string_view>

// Macro for defining custom members
#define DECLARE_CUSTOM_MEMBER(type, name, default_val)                \
  type m_##name = default_val;                                        \
  __declspec(property(get = get_##name, put = set_##name)) type name; \
  constexpr type get_##name() const noexcept { return m_##name; }     \
  constexpr void set_##name(type value) noexcept { m_##name = value; }

// Macro for defining numeric options
#define DECLARE_NUMERIC_OPTION(type, name, default_val) \
  DECLARE_CUSTOM_MEMBER(type, name, default_val)

// Macro for defining string options
#define DECLARE_STRING_OPTION(name, default_val)                             \
  std::string m_##name = default_val;                                        \
  __declspec(property(get = get_##name, put = set_##name)) std::string name; \
  const std::string& get_##name() const noexcept { return m_##name; }        \
  void set_##name(const std::string& value) noexcept { m_##name = value; }   \
  void set_##name(std::string_view value) noexcept { m_##name = value; }
// Main macro: Create auto flags class
#define CREATE_AUTO_FLAGS_CLASS(ClassName, ...)                     \
  class ClassName {                                                 \
   private:                                                         \
    uint64_t m_flags = 0;                                           \
                                                                    \
   public:                                                          \
    constexpr ClassName() = default;                                \
    constexpr ClassName(uint64_t flags) : m_flags(flags) {}         \
                                                                    \
    /* Common methods */                                            \
    constexpr uint64_t raw() const noexcept { return m_flags; }     \
    constexpr void clear() noexcept { m_flags = 0; }                \
    constexpr bool empty() const noexcept { return m_flags == 0; }  \
                                                                    \
    /* Flag testing and setting */                                  \
    constexpr bool test(uint64_t flag) const noexcept {             \
      return (m_flags & flag) != 0;                                 \
    }                                                               \
    constexpr void set(uint64_t flag, bool value = true) noexcept { \
      if (value)                                                    \
        m_flags |= flag;                                            \
      else                                                          \
        m_flags &= ~flag;                                           \
    }                                                               \
                                                                    \
    /* Operator overloads */                                        \
    constexpr ClassName operator|(uint64_t flag) const noexcept {   \
      return ClassName(m_flags | flag);                             \
    }                                                               \
    constexpr ClassName operator|(ClassName other) const noexcept { \
      return ClassName(m_flags | other.m_flags);                    \
    }                                                               \
    constexpr ClassName& operator|=(uint64_t flag) noexcept {       \
      m_flags |= flag;                                              \
      return *this;                                                 \
    }                                                               \
                                                                    \
    /* Custom members and flags */                                  \
    __VA_ARGS__                                                     \
  };

// Macro for defining single flags
#define DEFINE_FLAG(name, shift)                                      \
  __declspec(property(get = get_##name, put = set_##name)) bool name; \
  constexpr bool get_##name() const noexcept {                        \
    return (m_flags & (1ULL << shift)) != 0;                          \
  }                                                                   \
  constexpr void set_##name(bool value) noexcept {                    \
    if (value)                                                        \
      m_flags |= (1ULL << shift);                                     \
    else                                                              \
      m_flags &= ~(1ULL << shift);                                    \
  }

#define END_AUTO_FLAGS_CLASS
