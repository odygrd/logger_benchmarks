## Single Call

### Logging Numbers

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  14.1  |  14.6  |  15.2  |  15.7  |  21.7  |  32.6  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  26.0  |  26.6  |  35.3  |  55.9  |  95.5  | 214.8  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  27.1  |  28.8  |  39.1  |  57.0  | 102.0  | 228.4  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  27.7  |  35.8  |  42.3  |  49.9  |  97.7  | 247.4  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  30.4  |  38.5  |  46.7  |  57.5  | 100.9  | 270.7  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  36.9  |  49.4  |  60.2  |  66.7  | 126.4  | 377.1  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 100.4  | 117.7  | 149.2  | 163.3  | 276.2  | 935.3  |
| [spdlog](https://github.com/gabime/spdlog)                                | 1374.2 | 1416.6 | 1506.6 | 1691.1 | 2606.3 | 11704.2 |
| [g3log](https://github.com/KjellKod/g3log)                                | 3567.2 | 3653.4 | 3823.8 | 4183.0 | 7057.9 | 20365.2 |
| [Boost.Log](https://www.boost.org)                                        | 3775.0 | 3868.3 | 4088.0 | 4577.9 | 8030.6 | 26206.1 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  14.6  |  15.2  |  15.7  |  16.3  |  19.0  |  34.7  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  27.1  |  27.7  |  40.7  |  54.8  | 110.7  | 210.0  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  28.8  |  29.3  |  41.8  |  58.1  | 110.7  | 234.9  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  28.8  |  36.3  |  42.3  |  52.1  | 106.3  | 242.0  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  31.5  |  38.5  |  48.3  |  66.2  | 119.4  | 240.9  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  51.0  |  57.0  |  65.6  |  74.9  | 133.5  | 317.9  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 140.0  | 154.1  | 175.8  | 199.1  | 377.6  | 1928.2 |
| [spdlog](https://github.com/gabime/spdlog)                                | 1482.2 | 1972.1 | 2379.6 | 2817.4 | 11346.6 | 20351.1 |
| [Boost.Log](https://www.boost.org)                                        | 6249.5 | 6667.2 | 7180.5 | 7590.6 | 8861.8 | 29766.3 |
| [g3log](https://github.com/KjellKod/g3log)                                | 7158.2 | 7615.6 | 8129.9 | 8541.7 | 9793.3 | 29568.8 |

### Logging Large Strings

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  14.1  |  14.6  |  15.2  |  15.7  |  23.3  |  31.5  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  38.5  |  45.0  |  55.3  |  64.6  | 146.5  | 343.4  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  35.8  |  43.4  |  55.9  |  72.7  | 146.5  | 420.5  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  37.4  |  51.0  |  57.0  |  77.0  | 148.1  | 361.3  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  36.9  |  54.3  |  62.4  |  80.8  | 162.8  | 414.0  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  43.9  |  54.8  |  66.2  |  78.1  | 183.4  | 474.7  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 106.9  | 113.9  | 149.2  | 172.0  | 414.0  | 1245.1 |
| [spdlog](https://github.com/gabime/spdlog)                                | 1353.1 | 1390.0 | 1478.4 | 1671.0 | 2553.2 | 11290.7 |
| [g3log](https://github.com/KjellKod/g3log)                                | 3236.2 | 3321.4 | 3507.5 | 4172.7 | 6375.9 | 24337.1 |
| [Boost.Log](https://www.boost.org)                                        | 3354.5 | 3434.8 | 3646.9 | 4165.6 | 6695.4 | 21254.4 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  15.2  |  15.7  |  16.3  |  16.3  |  17.9  |  33.1  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  37.4  |  45.6  |  54.3  |  68.4  | 148.1  | 308.2  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  36.9  |  40.1  |  56.4  |  73.8  | 155.7  | 366.8  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  38.5  |  46.7  |  58.6  |  76.0  | 153.0  | 313.6  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  38.5  |  44.5  |  65.1  |  82.5  | 155.7  | 320.1  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  55.9  |  60.8  |  73.8  |  92.2  | 195.3  | 399.3  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 141.6  | 158.4  | 186.1  | 208.3  | 352.6  | 1381.3 |
| [spdlog](https://github.com/gabime/spdlog)                                | 1391.1 | 1914.1 | 2418.1 | 3010.0 | 11795.9 | 22784.4 |
| [Boost.Log](https://www.boost.org)                                        | 4887.7 | 5291.4 | 5754.2 | 6088.4 | 7128.9 | 25336.5 |
| [g3log](https://github.com/KjellKod/g3log)                                | 5949.5 | 6379.2 | 6870.2 | 7239.6 | 8325.2 | 24987.6 |

### Logging Complex Types

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  14.1  |  14.6  |  14.6  |  15.2  |  21.7  |  37.4  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  96.6  | 118.8  | 144.3  | 164.4  | 290.3  | 606.0  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  97.1  | 122.1  | 148.7  | 170.9  | 304.9  | 833.9  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  | 106.9  | 121.0  | 149.2  | 182.3  | 359.7  | 708.6  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 175.2  | 215.9  | 251.2  | 284.3  | 595.2  | 1751.9 |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 767.1  | 797.0  | 830.6  | 858.3  | 998.3  | 3218.3 |
| [Boost.Log](https://www.boost.org)                                        | 6540.3 | 6670.0 | 6979.2 | 7343.8 | 11348.3 | 28490.3 |
| [spdlog](https://github.com/gabime/spdlog)                                | 10922.4 | 11072.6 | 11283.7 | 11677.6 | 14434.7 | 40967.0 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  15.2  |  15.7  |  15.7  |  16.3  |  16.8  |  27.7  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  97.7  | 122.6  | 143.8  | 162.8  | 259.3  | 528.4  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  | 111.2  | 127.0  | 151.4  | 172.5  | 278.3  | 536.0  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                | 104.7  | 129.7  | 153.0  | 173.6  | 259.3  | 560.4  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 200.2  | 232.7  | 265.3  | 292.4  | 437.8  | 1338.4 |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 791.6  | 825.7  | 863.2  | 893.0  | 1039.5 | 2847.2 |
| [Boost.Log](https://www.boost.org)                                        | 9638.7 | 10165.0 | 10731.9 | 11150.2 | 12595.5 | 33716.5 |
| [spdlog](https://github.com/gabime/spdlog)                                | 11143.7 | 11585.3 | 12091.5 | 12540.7 | 21071.6 | 37858.7 |

## Burst of 20 Calls

### Logging Numbers

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  0.9   |  1.0   |  1.4   |  1.5   |  2.1   |  7.5   |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  11.3  |  12.3  |  14.1  |  15.4  |  21.9  |  35.3  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  12.7  |  13.8  |  15.5  |  17.7  |  25.0  |  39.0  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  12.4  |  13.4  |  15.5  |  18.0  |  29.8  |  64.3  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  13.3  |  14.4  |  16.5  |  19.2  |  30.5  |  78.1  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  17.2  |  18.3  |  20.0  |  22.0  |  32.2  |  79.4  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  62.2  |  67.6  |  74.5  |  84.4  | 113.1  | 371.2  |
| [spdlog](https://github.com/gabime/spdlog)                                | 238.0  | 254.3  | 298.5  | 337.8  | 433.5  | 1772.2 |
| [g3log](https://github.com/KjellKod/g3log)                                | 1437.4 | 1467.7 | 1602.8 | 1730.3 | 2400.0 | 3976.2 |
| [Boost.Log](https://www.boost.org)                                        | 1596.9 | 1637.6 | 1803.6 | 2008.3 | 2519.0 | 6668.6 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  1.0   |  1.1   |  1.5   |  1.6   |  2.2   |  5.6   |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  12.3  |  13.0  |  14.7  |  15.5  |  20.2  |  31.0  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  13.6  |  15.1  |  16.7  |  18.2  |  24.2  |  95.0  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  13.5  |  14.7  |  16.7  |  18.9  |  27.3  |  56.8  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  14.5  |  16.1  |  18.1  |  19.9  |  26.9  | 128.9  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  18.8  |  20.4  |  22.2  |  24.2  |  31.4  | 112.6  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  76.0  |  83.8  |  92.4  | 102.7  | 124.4  | 456.5  |
| [spdlog](https://github.com/gabime/spdlog)                                | 1167.0 | 1342.9 | 1514.2 | 1653.8 | 2332.4 | 3351.2 |
| [g3log](https://github.com/KjellKod/g3log)                                | 5037.5 | 5411.8 | 5875.6 | 6042.8 | 6333.3 | 8098.1 |
| [Boost.Log](https://www.boost.org)                                        | 6029.4 | 6161.5 | 6493.6 | 6864.6 | 7230.2 | 9588.6 |

### Logging Large Strings

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  0.9   |  1.0   |  1.5   |  1.5   |  2.0   |  4.4   |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  17.0  |  19.0  |  22.5  |  28.7  |  39.6  | 116.1  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  17.1  |  19.6  |  23.9  |  28.0  |  38.4  | 103.5  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  18.6  |  21.4  |  25.1  |  31.0  |  47.8  | 217.6  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  18.3  |  21.2  |  27.3  |  34.5  |  47.0  |  89.5  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  22.6  |  24.1  |  28.5  |  33.2  |  43.7  | 282.7  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  68.0  |  74.0  |  84.5  |  94.7  | 126.5  | 339.2  |
| [spdlog](https://github.com/gabime/spdlog)                                | 212.6  | 254.5  | 295.4  | 320.9  | 427.2  | 1592.8 |
| [Boost.Log](https://www.boost.org)                                        | 1266.6 | 1295.7 | 1406.4 | 1514.4 | 2118.1 | 3861.8 |
| [g3log](https://github.com/KjellKod/g3log)                                | 1174.4 | 1214.9 | 1542.0 | 1596.0 | 2128.0 | 6088.1 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  1.0   |  1.1   |  1.2   |  1.6   |  2.2   |  6.3   |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  17.6  |  19.4  |  22.1  |  25.4  |  35.2  | 133.5  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  18.2  |  20.3  |  23.6  |  26.9  |  36.8  | 116.7  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  18.7  |  20.9  |  25.2  |  29.9  |  41.4  | 121.6  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  19.4  |  22.0  |  26.2  |  30.5  |  41.1  | 137.6  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  23.9  |  25.6  |  28.7  |  32.3  |  41.2  | 722.4  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  73.9  |  83.7  |  92.5  | 100.9  | 125.9  | 532.6  |
| [spdlog](https://github.com/gabime/spdlog)                                | 1132.6 | 1317.4 | 1495.4 | 1652.5 | 2456.9 | 3545.5 |
| [Boost.Log](https://www.boost.org)                                        | 4126.1 | 4468.3 | 4774.0 | 5002.6 | 5472.8 | 6955.0 |
| [g3log](https://github.com/KjellKod/g3log)                                | 4569.7 | 4678.2 | 4987.9 | 5242.7 | 5905.8 | 7670.0 |

### Logging Complex Types

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  0.9   |  1.0   |  1.4   |  1.5   |  2.3   |  4.7   |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  74.6  |  79.0  |  86.0  |  98.5  | 121.8  | 258.4  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  74.6  |  79.6  |  88.9  | 113.1  | 134.5  | 280.9  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  79.4  |  84.9  |  93.1  | 120.8  | 141.2  | 720.0  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 111.3  | 118.6  | 129.4  | 142.8  | 193.2  | 864.2  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 605.4  | 622.5  | 640.0  | 656.7  | 760.3  | 1240.2 |
| [Boost.Log](https://www.boost.org)                                        | 4355.0 | 4485.8 | 4710.9 | 4970.0 | 6913.7 | 10979.6 |
| [spdlog](https://github.com/gabime/spdlog)                                | 10698.4 | 10835.1 | 11011.2 | 11217.0 | 13444.9 | 21116.0 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  1.0   |  1.0   |  1.3   |  1.6   |  2.3   |  4.7   |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  76.8  |  80.1  |  85.3  |  93.2  | 129.9  | 213.5  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  77.7  |  81.0  |  86.5  |  92.7  | 122.7  | 207.7  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  84.9  |  91.2  |  99.0  | 107.0  | 141.2  | 586.5  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 115.3  | 123.2  | 133.8  | 146.7  | 179.4  | 838.2  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 680.7  | 718.2  | 751.5  | 772.8  | 884.3  | 2065.3 |
| [Boost.Log](https://www.boost.org)                                        | 6677.1 | 7031.5 | 7410.7 | 7672.0 | 8462.3 | 11487.6 |
| [spdlog](https://github.com/gabime/spdlog)                                | 10372.2 | 10480.7 | 10608.5 | 10892.7 | 12014.8 | 14556.9 |

## Cold Cache

### Logging Numbers

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  23.9  |  38.0  |  40.1  |  41.2  |  93.9  | 158.4  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           | 117.2  | 132.9  | 155.2  | 169.3  | 215.4  | 330.4  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  | 131.3  | 144.9  | 162.8  | 179.0  | 234.9  | 349.4  |
| [BqLog](https://github.com/Tencent/BqLog)                                 | 143.8  | 159.0  | 176.3  | 201.8  | 284.8  | 1423.1 |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                | 143.8  | 161.7  | 186.6  | 208.9  | 296.2  | 498.6  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 141.6  | 165.5  | 192.6  | 216.5  | 278.3  | 368.4  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 218.6  | 240.9  | 404.7  | 486.1  | 676.0  | 3377.3 |
| [spdlog](https://github.com/gabime/spdlog)                                | 3033.9 | 3163.0 | 3333.3 | 3492.3 | 4383.7 | 15526.9 |
| [g3log](https://github.com/KjellKod/g3log)                                | 6132.3 | 6289.1 | 6558.7 | 7037.2 | 10411.3 | 22385.6 |
| [Boost.Log](https://www.boost.org)                                        | 6434.5 | 6620.0 | 6891.3 | 7244.0 | 10518.7 | 22472.4 |

### Logging Large Strings

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  38.5  |  39.6  |  40.7  |  43.4  | 113.9  | 351.0  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           | 126.4  | 142.7  | 168.7  | 204.5  | 261.5  | 415.0  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 125.3  | 144.3  | 172.5  | 210.5  | 304.4  | 462.8  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  | 141.6  | 156.8  | 186.6  | 228.4  | 301.1  | 551.8  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                | 148.1  | 167.6  | 198.6  | 239.3  | 348.9  | 632.1  |
| [BqLog](https://github.com/Tencent/BqLog)                                 | 156.3  | 178.5  | 227.9  | 307.1  | 395.5  | 1476.8 |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 207.8  | 230.0  | 466.0  | 566.4  | 757.4  | 5012.5 |
| [spdlog](https://github.com/gabime/spdlog)                                | 2997.0 | 3135.9 | 3306.8 | 3483.1 | 4848.7 | 26584.9 |
| [g3log](https://github.com/KjellKod/g3log)                                | 5524.1 | 5657.6 | 5859.4 | 6189.3 | 9298.6 | 16315.7 |
| [Boost.Log](https://www.boost.org)                                        | 5963.6 | 6149.1 | 6402.5 | 6735.6 | 9315.9 | 19289.9 |

### Logging Complex Types

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  33.6  |  42.9  |  45.0  |  84.1  |  93.9  | 104.7  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           | 178.5  | 198.0  | 224.6  | 253.4  | 353.2  | 674.9  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  | 193.7  | 210.5  | 244.1  | 287.0  | 400.4  | 661.4  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                | 200.7  | 229.0  | 281.6  | 334.7  | 542.5  | 864.8  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 290.3  | 314.7  | 361.9  | 535.5  | 710.2  | 1948.8 |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 787.8  | 832.8  | 897.9  | 957.6  | 1188.2 | 4749.4 |
| [Boost.Log](https://www.boost.org)                                        | 9237.8 | 9528.0 | 9821.5 | 10174.7 | 13556.4 | 28595.5 |
| [spdlog](https://github.com/gabime/spdlog)                                | 12917.3 | 13135.4 | 13405.6 | 13626.9 | 16595.1 | 42462.8 |

## Disabled Statement

**Disabled Statement, Hot**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  0.2   |  0.2   |  0.2   |  0.2   |  0.2   |  0.3   |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  0.3   |  0.3   |  0.3   |  0.3   |  0.3   |  0.4   |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  0.3   |  0.3   |  0.4   |  0.4   |  0.4   |  0.4   |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  0.9   |  0.9   |  0.9   |  0.9   |  0.9   |  1.2   |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  2.2   |  2.2   |  2.2   |  2.2   |  2.3   |  3.6   |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  5.7   |  5.8   |  5.9   |  6.0   |  7.4   |  35.1  |
| [spdlog](https://github.com/gabime/spdlog)                                |  6.8   |  6.8   |  6.8   |  6.8   |  7.7   |  10.0  |
| [Boost.Log](https://www.boost.org)                                        |  78.4  |  80.2  |  81.6  |  82.3  |  95.2  | 197.4  |

**Disabled Statement, Cold**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Timer only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  37.4  |  39.1  |  40.7  |  86.8  | 153.0  | 262.0  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  35.3  |  50.5  |  53.2  |  60.2  | 110.7  | 213.2  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  60.8  |  72.2  |  79.2  |  92.2  | 125.9  | 193.1  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  77.6  |  86.8  |  97.1  | 118.3  | 144.9  | 210.0  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  69.4  |  82.5  |  97.7  | 105.8  | 184.5  | 332.0  |
| [spdlog](https://github.com/gabime/spdlog)                                |  71.6  |  87.3  | 101.5  | 108.5  | 164.9  | 303.8  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 104.7  | 119.4  | 136.2  | 150.3  | 227.3  | 396.6  |
| [Boost.Log](https://www.boost.org)                                        | 439.5  | 478.5  | 710.2  | 796.4  | 1211.5 | 5419.4 |

**Disabled Statement, Expensive Argument**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |  Argument |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|:---------:|
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  0.2   |  0.2   |  0.2   |  0.2   |  0.3   |  0.6   |  skipped  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  0.3   |  0.3   |  0.3   |  0.3   |  0.3   |  0.4   |  skipped  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  3.5   |  3.5   |  3.5   |  3.5   |  5.4   |  27.1  |  skipped  |
| [Boost.Log](https://www.boost.org)                                        |  79.1  |  80.4  |  80.8  |  81.3  |  98.3  | 158.5  |  skipped  |
| [spdlog](https://github.com/gabime/spdlog)                                |  79.8  |  79.9  |  82.0  |  83.3  |  96.0  | 163.5  | evaluated |
| [Argument only (no logging)](https://github.com/odygrd/logger_benchmarks/blob/master/benchmarks/call_site_latency/benchmark_timer_only_call_site_latency.cpp) |  80.1  |  80.2  |  83.0  |  84.3  |  94.3  | 157.8  | evaluated |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  83.2  |  83.2  |  83.2  |  85.5  |  88.0  | 155.1  | evaluated |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  83.2  |  83.2  |  84.9  |  86.7  |  89.2  | 170.5  | evaluated |

