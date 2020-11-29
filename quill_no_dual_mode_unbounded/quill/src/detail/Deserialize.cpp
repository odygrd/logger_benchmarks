#include "quill/detail/serialize/Deserialize.h"
#include "quill/QuillError.h"
#include "quill/detail/misc/Attributes.h"
#include "quill/detail/misc/Utilities.h"
#include <cstdint>
#include <cstring>
#include <string>

namespace quill
{
namespace detail
{

namespace
{
template <typename TValue>
QUILL_NODISCARD size_t get_value(unsigned char const*& read_buffer,
                                 fmt::dynamic_format_arg_store<fmt::format_context>& fmt_store)
{
  using value_t = TValue;

  value_t value;
  std::memcpy(&value, read_buffer, sizeof(value_t));
  fmt_store.push_back(value);

  read_buffer += sizeof(value_t);
  return sizeof(value_t);
}

template <>
QUILL_NODISCARD size_t get_value<char const*>(unsigned char const*& read_buffer,
                                              fmt::dynamic_format_arg_store<fmt::format_context>& fmt_store)
{
  // string in buffer is null terminated
  std::string value{reinterpret_cast<char const*>(read_buffer)};
  fmt_store.push_back(value);

  read_buffer += value.length() + 1;
  return value.length() + 1;
}

} // namespace
size_t deserialize_argument(unsigned char const*& read_buffer,
                            fmt::dynamic_format_arg_store<fmt::format_context>& fmt_store,
                            std::string const& type_descriptor)
{
  // size of argument we deserialized
  size_t read_size{0};

  if (std::strcmp(type_descriptor.data(), "B") == 0)
  {
    read_size = get_value<bool>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "IS") == 0)
  {
    read_size = get_value<short>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "I") == 0)
  {
    read_size = get_value<int>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "IL") == 0)
  {
    read_size = get_value<long>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "ILL") == 0)
  {
    read_size = get_value<long long>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "UIS") == 0)
  {
    read_size = get_value<unsigned short>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "UI") == 0)
  {
    read_size = get_value<unsigned int>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "UIL") == 0)
  {
    read_size = get_value<unsigned long>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "UILL") == 0)
  {
    read_size = get_value<unsigned long long>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "D") == 0)
  {
    read_size = get_value<double>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "LD") == 0)
  {
    read_size = get_value<long double>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "F") == 0)
  {
    read_size = get_value<float>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "C") == 0)
  {
    read_size = get_value<char>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "UC") == 0)
  {
    read_size = get_value<unsigned char>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "CS") == 0)
  {
    read_size = get_value<signed char>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "P") == 0)
  {
    read_size = get_value<void*>(read_buffer, fmt_store);
  }
  else if (std::strcmp(type_descriptor.data(), "SC") == 0 ||
           std::strcmp(type_descriptor.data(), "S") == 0)
  {
    read_size = get_value<char const*>(read_buffer, fmt_store);
  }
  else
  {
    QUILL_THROW(QuillError{"Unknown type descriptor. [" + type_descriptor + "] Can not de-serialize."});
  }

  return read_size;
}
} // namespace detail
} // namespace quill