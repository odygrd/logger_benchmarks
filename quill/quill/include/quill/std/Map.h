/**
 * Copyright(c) 2020-present, Odysseas Georgoudis & quill contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include "quill/core/Codec.h"
#include "quill/std/Pair.h"

#include "quill/bundled/fmt/args.h"
#include "quill/bundled/fmt/core.h"
#include "quill/bundled/fmt/ranges.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <type_traits>
#include <utility>
#include <vector>

namespace quill::detail
{
/***/
template <template <typename...> class MapType, typename Key, typename T, typename Compare, typename Allocator>
struct ArgSizeCalculator<
  MapType<Key, T, Compare, Allocator>,
  std::enable_if_t<std::disjunction_v<std::is_same<MapType<Key, T, Compare, Allocator>, std::map<Key, T, Compare, Allocator>>,
                                      std::is_same<MapType<Key, T, Compare, Allocator>, std::multimap<Key, T, Compare, Allocator>>>>>
{
  static size_t calculate(std::vector<size_t>& conditional_arg_size_cache,
                          MapType<Key, T, Compare, Allocator> const& arg) noexcept
  {
    // We need to store the size of the set in the buffer, so we reserve space for it.
    // We add sizeof(size_t) bytes to accommodate the size information.
    size_t total_size{sizeof(size_t)};

    if constexpr (std::conjunction_v<std::disjunction<std::is_arithmetic<Key>, std::is_enum<Key>>,
                                     std::disjunction<std::is_arithmetic<T>, std::is_enum<T>>>)
    {
      // For built-in types, such as arithmetic or enum types, iteration is unnecessary
      total_size += (sizeof(Key) + sizeof(T)) * arg.size();
    }
    else
    {
      // For other complex types it's essential to determine the exact size of each element.
      // For instance, in the case of a collection of std::string, we need to know the exact size
      // of each string as we will be copying them directly to our queue buffer.
      for (auto const& elem : arg)
      {
        total_size += ArgSizeCalculator<std::pair<Key, T>>::calculate(conditional_arg_size_cache, elem);
      }
    }

    return total_size;
  }
};

/***/
template <template <typename...> class MapType, typename Key, typename T, typename Compare, typename Allocator>
struct Encoder<MapType<Key, T, Compare, Allocator>,
               std::enable_if_t<std::disjunction_v<std::is_same<MapType<Key, T, Compare, Allocator>, std::map<Key, T, Compare, Allocator>>,
                                                   std::is_same<MapType<Key, T, Compare, Allocator>, std::multimap<Key, T, Compare, Allocator>>>>>
{
  static void encode(std::byte*& buffer,
                     std::vector<size_t> const& conditional_arg_size_cache,
                     uint32_t& conditional_arg_size_cache_index,
                     MapType<Key, T, Compare, Allocator> const& arg) noexcept
  {
    Encoder<size_t>::encode(buffer, conditional_arg_size_cache, conditional_arg_size_cache_index,
                            arg.size());

    for (auto const& elem : arg)
    {
      Encoder<std::pair<Key, T>>::encode(buffer, conditional_arg_size_cache,
                                         conditional_arg_size_cache_index, elem);
    }
  }
};

/***/
template <template <typename...> class MapType, typename Key, typename T, typename Compare, typename Allocator>
struct Decoder<MapType<Key, T, Compare, Allocator>,
               std::enable_if_t<std::disjunction_v<std::is_same<MapType<Key, T, Compare, Allocator>, std::map<Key, T, Compare, Allocator>>,
                                                   std::is_same<MapType<Key, T, Compare, Allocator>, std::multimap<Key, T, Compare, Allocator>>>>>
{
  static MapType<Key, T, Compare, Allocator> decode(std::byte*& buffer,
                                                    fmtquill::dynamic_format_arg_store<fmtquill::format_context>* args_store)
  {
    MapType<Key, T, Compare, Allocator> arg;

    // Read the size of the set
    size_t const number_of_elements = Decoder<size_t>::decode(buffer, nullptr);

    for (size_t i = 0; i < number_of_elements; ++i)
    {
      arg.insert(Decoder<std::pair<Key, T>>::decode(buffer, nullptr));
    }

    if (args_store)
    {
      args_store->push_back(arg);
    }

    return arg;
  }
};
} // namespace quill::detail