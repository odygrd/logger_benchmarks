/*
 * Copyright (C) 2024 THL A29 Limited, a Tencent company.
 * BQLOG is licensed under the Apache License, Version 2.0.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "bq_common/bq_common.h"
#if BQ_MAC
#import <Foundation/Foundation.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

namespace bq {
	namespace platform {
        //According to test result
        //gettimeofday has higher performance than "mach_absolute_time"
        //and "CLOCK_REALTIME_COARSE clock_gettime" on mac.
        //TSC is not recommended because of different hardware architectures.
        uint64_t high_performance_epoch_ms()
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            uint64_t ret = tv.tv_usec;
            ret /= 1000;
            ret += ((uint64_t)tv.tv_sec * 1000);
            return ret;
        }
    
		struct ___base_dir_initializer {
			bq::string base_dir;
			___base_dir_initializer()
			{
				bq::array<char> tmp;
				tmp.fill_uninitialized(1024);
				while (getcwd(&tmp[0], (int32_t)tmp.size()) == NULL)
				{
					tmp.fill_uninitialized(1024);
				}
				base_dir = &tmp[0];
			}
		};
		const bq::string& get_base_dir(bool is_sandbox)
		{
			(void)is_sandbox;
			static ___base_dir_initializer base_dir_init_inst;
			return base_dir_init_inst.base_dir;
		}

        bool share_file(const char* file_path)
		{
            (void)file_path;
			return false;
		}
	}
}
#endif
