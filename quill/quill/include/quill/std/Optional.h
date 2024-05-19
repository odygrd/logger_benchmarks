/**
 * Copyright(c) 2020-present, Odysseas Georgoudis & quill contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include "quill/core/Codec.h"

#include "quill/bundled/fmt/args.h"
#include "quill/bundled/fmt/core.h"
#include "quill/bundled/fmt/std.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace quill::detail
{
/***/
template <typename T>
struct ArgSizeCalculator<std::optional<T>>
{
  static size_t calculate(std::vector<size_t>& conditional_arg_size_cache, std::optional<T> const& arg) noexcept
  {
    // We need to store the size of the vector in the buffer, so we reserve space for it.
    // We add sizeof(bool) bytes to accommodate the size information.
    size_t total_size{sizeof(bool)};

    if (arg.has_value())
    {
      total_size += ArgSizeCalculator<T>::calculate(conditional_arg_size_cache, *arg);
    }

    return total_size;
  }
};

/***/
template <typename T>
struct Encoder<std::optional<T>>
{
  static void encode(std::byte*& buffer,
                     std::vector<size_t> const& conditional_arg_size_cache,
                     uint32_t& conditional_arg_size_cache_index,
                     std::optional<T> const& arg) noexcept
  {
    Encoder<bool>::encode(buffer, conditional_arg_size_cache, conditional_arg_size_cache_index, arg.has_value());

    if (arg.has_value())
    {
      Encoder<T>::encode(buffer, conditional_arg_size_cache, conditional_arg_size_cache_index, *arg);
    }
  }
};

/***/
template <typename T>
struct Decoder<std::optional<T>>
{
  static std::optional<T> decode(std::byte*& buffer,
                                 fmtquill::dynamic_format_arg_store<fmtquill::format_context>* args_store)
  {
    std::optional<T> arg{std::nullopt};

    bool const has_value = Decoder<bool>::decode(buffer, nullptr);
    if (has_value)
    {
      arg = Decoder<T>::decode(buffer, nullptr);
    }

    if (args_store)
    {
      args_store->push_back(arg);
    }

    return arg;
  }
};
} // namespace quill::detail