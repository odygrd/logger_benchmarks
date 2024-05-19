/**
 * Copyright(c) 2020-present, Odysseas Georgoudis & quill contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include "quill/core/Codec.h"

#include "quill/bundled/fmt/args.h"
#include "quill/bundled/fmt/core.h"
#include "quill/bundled/fmt/ranges.h"

#include <cstddef>
#include <cstdint>
#include <utility>

namespace quill::detail
{
/***/
template <typename T1, typename T2>
struct ArgSizeCalculator<std::pair<T1, T2>>
{
  static size_t calculate(std::vector<size_t>& conditional_arg_size_cache, std::pair<T1, T2> const& arg) noexcept
  {
    return ArgSizeCalculator<T1>::calculate(conditional_arg_size_cache, arg.first) +
      ArgSizeCalculator<T2>::calculate(conditional_arg_size_cache, arg.second);
  }
};

/***/
template <typename T1, typename T2>
struct Encoder<std::pair<T1, T2>>
{
  static void encode(std::byte*& buffer,
                     std::vector<size_t> const& conditional_arg_size_cache,
                     uint32_t& conditional_arg_size_cache_index,
                     std::pair<T1, T2> const& arg) noexcept
  {
    Encoder<T1>::encode(buffer, conditional_arg_size_cache, conditional_arg_size_cache_index, arg.first);
    Encoder<T2>::encode(buffer, conditional_arg_size_cache, conditional_arg_size_cache_index, arg.second);
  }
};

/***/
template <typename T1, typename T2>
struct Decoder<std::pair<T1, T2>>
{
  static std::pair<T1, T2> decode(std::byte*& buffer,
                                  fmtquill::dynamic_format_arg_store<fmtquill::format_context>* args_store)
  {
    std::pair<T1, T2> arg;

    arg.first = Decoder<T1>::decode(buffer, nullptr);
    arg.second = Decoder<T2>::decode(buffer, nullptr);

    if (args_store)
    {
      args_store->push_back(arg);
    }

    return arg;
  }
};
} // namespace quill::detail