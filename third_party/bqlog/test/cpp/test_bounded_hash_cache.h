#pragma once

#include "bq_log/types/bounded_hash_cache.h"
#include "test_base.h"

namespace bq {
    namespace test {
        class test_bounded_hash_cache : public test_base {
        private:
            static uint64_t make_key(uint64_t value)
            {
                value += UINT64_C(0x9E3779B97F4A7C15);
                value = (value ^ (value >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
                value = (value ^ (value >> 27)) * UINT64_C(0x94D049BB133111EB);
                return value ^ (value >> 31);
            }

            template <typename CACHE>
            static bool find_value(CACHE& cache, uint64_t key, uint32_t expected)
            {
                uint32_t value = 0;
                typename CACHE::insert_token token;
                return cache.find(key, value, token) && value == expected;
            }

            static void test_basic(test_result& result)
            {
                bq::bounded_hash_cache<32> cache;
                uint32_t value = 0;
                bq::bounded_hash_cache<32>::insert_token token;
                result.add_result(!cache.find(7, value, token), "bounded cache empty find");
                result.add_result(!token.is_valid(), "bounded cache empty insert token");

                cache.insert(7, 11, token);
                result.add_result(find_value(cache, 7, 11), "bounded cache token insert");
                result.add_result(find_value(cache, 7, 11), "bounded cache repeat find");

                cache.insert(7, 29);
                result.add_result(find_value(cache, 7, 29), "bounded cache update value");

                cache.insert(9, 17);
                result.add_result(find_value(cache, 9, 17), "bounded cache direct insert");
                result.add_result(!cache.find(10, value, token) && token.is_valid(), "bounded cache missing key token");
            }

            static void test_insert_token(test_result& result)
            {
                bq::bounded_hash_cache<64> cache;
                for (uint32_t i = 0; i < 3; ++i) {
                    cache.insert(make_key(i), i + 1);
                }

                uint32_t value = 0;
                bq::bounded_hash_cache<64>::insert_token token;
                const uint64_t token_key = make_key(100);
                bool success = !cache.find(token_key, value, token) && token.is_valid();
                cache.insert(token_key, 101, token);
                success &= find_value(cache, token_key, 101);
                result.add_result(success, "bounded cache reusable insert token");

                bq::bounded_hash_cache<64>::insert_token stale_token;
                const uint64_t stale_key = make_key(101);
                success = !cache.find(stale_key, value, stale_token) && stale_token.is_valid();
                cache.insert(make_key(102), 103);
                cache.insert(stale_key, 102, stale_token);
                success &= find_value(cache, stale_key, 102);
                result.add_result(success, "bounded cache stale insert token fallback");

                bq::bounded_hash_cache<64>::insert_token resize_token;
                const uint64_t resize_key = make_key(103);
                success = !cache.find(resize_key, value, resize_token) && resize_token.is_valid();
                cache.insert(resize_key, 104, resize_token);
                success &= find_value(cache, resize_key, 104);
                result.add_result(success, "bounded cache resize invalidates token safely");
            }

            static void test_resize_allocation_failure(test_result& result)
            {
                bq::bounded_hash_cache<64> cache;
                for (uint32_t i = 0; i < 4; ++i) {
                    cache.insert(make_key(i), i + 1);
                }

                uint32_t value = 0;
                bq::bounded_hash_cache<64>::insert_token token;
                const uint64_t failed_key = make_key(1000);
                bool success = !cache.find(failed_key, value, token);
                cache.fail_allocation_after_for_test(0);
                cache.insert(failed_key, 1001, token);
                for (uint32_t i = 0; i < 4; ++i) {
                    success &= find_value(cache, make_key(i), i + 1);
                }
                success &= !cache.find(failed_key, value, token);
                result.add_result(success, "bounded cache resize first allocation failure preserves entries");

                cache.fail_allocation_after_for_test(1);
                cache.insert(failed_key, 1001, token);
                for (uint32_t i = 0; i < 4; ++i) {
                    success &= find_value(cache, make_key(i), i + 1);
                }
                success &= !cache.find(failed_key, value, token);
                result.add_result(success, "bounded cache resize second allocation failure preserves entries");

                cache.clear_allocation_failure_for_test();
                cache.insert(failed_key, 1001, token);
                result.add_result(find_value(cache, failed_key, 1001), "bounded cache resize recovers after allocation failure");
            }

            static void test_runtime_max_size(test_result& result)
            {
                bq::bounded_hash_cache<128> cache(16);
                result.add_result(cache.get_max_size() == 16, "bounded cache runtime max size");
                result.add_result(cache.set_max_size(32) && cache.get_max_size() == 32, "bounded cache resize max while empty");
                cache.insert(make_key(1), 1);
                result.add_result(cache.set_max_size(64) && cache.get_max_size() == 64, "bounded cache increase max while populated");
                result.add_result(!cache.set_max_size(16) && cache.get_max_size() == 64, "bounded cache reject shrink while populated");
                cache.clear();
                result.add_result(cache.set_max_size(16) && cache.get_max_size() == 16, "bounded cache max change after clear");
            }

            static void test_growth_and_update(test_result& result)
            {
                // This key family targeted the old XOR-fold table hash; the
                // fmix64 hash spreads it, so the test now covers plain growth
                // and in-place updates (natural collisions still occur).
                bq::bounded_hash_cache<128> cache;
                for (uint32_t i = 0; i < 100; ++i) {
                    const uint64_t key = (static_cast<uint64_t>(i) << 32) | i;
                    cache.insert(key, i + 1);
                }

                bool success = true;
                for (uint32_t i = 0; i < 100; ++i) {
                    const uint64_t key = (static_cast<uint64_t>(i) << 32) | i;
                    success &= find_value(cache, key, i + 1);
                }
                result.add_result(success, "bounded cache growth");

                for (uint32_t i = 0; i < 100; i += 3) {
                    const uint64_t key = (static_cast<uint64_t>(i) << 32) | i;
                    cache.insert(key, i + 1000);
                }
                success = true;
                for (uint32_t i = 0; i < 100; ++i) {
                    const uint64_t key = (static_cast<uint64_t>(i) << 32) | i;
                    const uint32_t expected = (i % 3 == 0) ? (i + static_cast<uint32_t>(1000)) : (i + static_cast<uint32_t>(1));
                    success &= find_value(cache, key, expected);
                }
                result.add_result(success, "bounded cache growth update");
            }

            static void test_exact_limit_and_eviction(test_result& result)
            {
                bq::bounded_hash_cache<64> exact_limit_cache;
                for (uint32_t i = 0; i < 64; ++i) {
                    exact_limit_cache.insert(make_key(i), i);
                }
                bool success = true;
                for (uint32_t i = 0; i < 64; ++i) {
                    success &= find_value(exact_limit_cache, make_key(i), i);
                }
                result.add_result(success, "bounded cache exact limit");

                bq::bounded_hash_cache<8> eviction_cache;
                for (uint32_t i = 0; i < 8; ++i) {
                    const uint64_t key = (static_cast<uint64_t>(i) << 32) | i;
                    eviction_cache.insert(key, i);
                }

                // The admission gate is 1/64: only attempt 64 is admitted and
                // evicts one clock-hand victim (1 eviction in total).
                uint64_t admitted_key = 0;
                for (uint32_t i = 0; i < 64; ++i) {
                    admitted_key = (static_cast<uint64_t>(1000 + i) << 32) | (1000 + i);
                    uint32_t value = 0;
                    bq::bounded_hash_cache<8>::insert_token token;
                    success &= !eviction_cache.find(admitted_key, value, token);
                    eviction_cache.insert(admitted_key, 1000 + i, token);
                }
                result.add_result(success, "bounded cache admission misses");
                result.add_result(find_value(eviction_cache, admitted_key, 1063), "bounded cache admitted key");

                uint32_t old_key_count = 0;
                for (uint32_t i = 0; i < 8; ++i) {
                    uint32_t value = 0;
                    bq::bounded_hash_cache<8>::insert_token token;
                    const uint64_t key = (static_cast<uint64_t>(i) << 32) | i;
                    old_key_count += eviction_cache.find(key, value, token) ? static_cast<uint32_t>(1) : static_cast<uint32_t>(0);
                }
                result.add_result(old_key_count == 7, "bounded cache admission eviction count");
            }

            static void test_clear_and_reuse(test_result& result)
            {
                bq::bounded_hash_cache<256> cache;
                for (uint32_t i = 0; i < 200; ++i) {
                    cache.insert(make_key(i), i + 1);
                }

                bool success = find_value(cache, make_key(0), 1);
                for (uint32_t i = 0; i < 8192; ++i) {
                    const uint32_t key_index = i % 200;
                    success &= find_value(cache, make_key(key_index), key_index + 1);
                }
                for (uint32_t i = 0; i < 200; ++i) {
                    success &= find_value(cache, make_key(i), i + 1);
                }
                result.add_result(success, "bounded cache repeated finds");

                cache.clear();
                cache.clear();
                uint32_t value = 0;
                bq::bounded_hash_cache<256>::insert_token token;
                success = true;
                for (uint32_t i = 0; i < 200; ++i) {
                    success &= !cache.find(make_key(i), value, token);
                }
                result.add_result(success, "bounded cache clear");

                for (uint32_t i = 0; i < 128; ++i) {
                    cache.insert(make_key(1000 + i), i + 7);
                }
                success = true;
                for (uint32_t i = 0; i < 128; ++i) {
                    success &= find_value(cache, make_key(1000 + i), i + 7);
                }
                result.add_result(success, "bounded cache reuse after clear");
            }

            static void test_production_capacity(test_result& result)
            {
                bq::bounded_hash_cache<100000> cache;
                for (uint32_t i = 0; i < 100000; ++i) {
                    cache.insert(make_key(i), i);
                }

                bool success = true;
                for (uint32_t i = 0; i < 100000; ++i) {
                    success &= find_value(cache, make_key(i), i);
                }
                result.add_result(success, "bounded cache production capacity");

                uint64_t admitted_key = 0;
                for (uint32_t i = 0; i < 64; ++i) {
                    admitted_key = make_key(200000 + i);
                    uint32_t value = 0;
                    bq::bounded_hash_cache<100000>::insert_token token;
                    success &= !cache.find(admitted_key, value, token);
                    cache.insert(admitted_key, 200000 + i, token);
                }
                result.add_result(success, "bounded cache production admission");
                result.add_result(find_value(cache, admitted_key, 200063), "bounded cache production eviction");
            }

        public:
            virtual test_result test() override
            {
                test_result result;
                test_basic(result);
                test_insert_token(result);
                test_resize_allocation_failure(result);
                test_runtime_max_size(result);
                test_growth_and_update(result);
                test_exact_limit_and_eviction(result);
                test_clear_and_reuse(result);
                test_production_capacity(result);
                return result;
            }
        };
    }
}
