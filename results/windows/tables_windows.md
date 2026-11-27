## Single Call

### Logging Numbers

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  26.6  |  27.7  |  38.5  |  54.3  |  86.3  | 218.6  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  27.7  |  30.4  |  41.2  |  54.8  | 112.3  | 270.2  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  27.7  |  29.3  |  44.5  |  56.4  | 117.2  | 305.4  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  30.9  |  36.9  |  48.3  |  67.8  | 144.3  | 338.5  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  28.2  |  43.9  |  55.9  |  65.1  | 123.7  | 348.3  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 113.9  | 126.4  | 151.4  | 183.4  | 390.1  | 1507.2 |
| [spdlog](https://github.com/gabime/spdlog)                                | 1376.4 | 1422.5 | 1547.9 | 1710.6 | 2838.0 | 16054.7 |
| [g3log](https://github.com/KjellKod/g3log)                                | 3491.8 | 3591.6 | 3809.2 | 4235.1 | 7314.0 | 33038.4 |
| [Boost.Log](https://www.boost.org)                                        | 3864.5 | 3984.4 | 4236.1 | 4777.0 | 8363.8 | 34241.2 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  27.7  |  28.8  |  40.1  |  58.1  | 117.2  | 264.2  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  29.3  |  32.0  |  42.3  |  49.9  | 124.8  | 277.2  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  28.8  |  29.8  |  45.6  |  62.4  | 135.1  | 303.3  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  32.0  |  39.6  |  57.5  |  76.0  | 138.9  | 295.1  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  51.0  |  58.6  |  67.3  |  76.5  | 136.2  | 314.7  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 148.7  | 163.3  | 196.4  | 223.0  | 357.5  | 1567.9 |
| [spdlog](https://github.com/gabime/spdlog)                                | 1458.3 | 1947.2 | 2379.6 | 2824.4 | 11655.9 | 19256.3 |
| [Boost.Log](https://www.boost.org)                                        | 4983.7 | 5396.6 | 5914.7 | 6279.3 | 7452.3 | 25042.4 |
| [g3log](https://github.com/KjellKod/g3log)                                | 6365.0 | 6847.9 | 7380.7 | 7804.4 | 9044.1 | 21322.2 |

### Logging Large Strings

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  29.3  |  42.3  |  55.9  |  70.0  | 153.5  | 406.9  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  35.3  |  48.8  |  56.4  |  71.6  | 174.7  | 429.7  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  39.6  |  42.9  |  57.0  |  65.6  | 138.3  | 312.5  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  29.8  |  45.6  |  57.0  |  85.2  | 171.4  | 405.3  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  49.9  |  60.2  |  72.7  |  92.8  | 192.6  | 542.0  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 103.1  | 113.4  | 154.6  | 181.2  | 417.2  | 1473.0 |
| [spdlog](https://github.com/gabime/spdlog)                                | 1336.8 | 1383.5 | 1503.9 | 1704.1 | 2762.1 | 16360.2 |
| [g3log](https://github.com/KjellKod/g3log)                                | 3215.6 | 3315.4 | 3503.7 | 3802.1 | 6186.0 | 22048.7 |
| [Boost.Log](https://www.boost.org)                                        | 3531.9 | 3630.1 | 3813.0 | 4381.5 | 10130.3 | 77159.1 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  30.4  |  38.0  |  55.9  |  75.4  | 144.9  | 326.6  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  36.3  |  42.9  |  56.4  |  72.2  | 153.0  | 301.7  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  42.3  |  49.4  |  67.3  |  90.1  | 185.0  | 334.7  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  31.5  |  48.8  |  71.1  | 106.9  | 202.4  | 398.2  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  59.7  |  68.4  |  82.5  | 101.5  | 191.5  | 400.4  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 143.8  | 163.8  | 198.6  | 232.2  | 409.1  | 1418.7 |
| [spdlog](https://github.com/gabime/spdlog)                                | 1450.2 | 1919.0 | 2371.4 | 2757.2 | 11238.7 | 21809.5 |
| [Boost.Log](https://www.boost.org)                                        | 4114.1 | 4497.1 | 4950.7 | 5284.9 | 6369.9 | 23940.0 |
| [g3log](https://github.com/KjellKod/g3log)                                | 5531.2 | 5941.3 | 6365.6 | 6678.1 | 7753.4 | 24421.2 |

### Logging Complex Types

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  93.3  | 112.3  | 135.6  | 156.3  | 238.2  | 578.9  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           | 105.3  | 116.1  | 141.1  | 169.8  | 317.9  | 620.7  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                | 108.0  | 129.7  | 154.6  | 182.8  | 331.5  | 739.5  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 180.1  | 209.4  | 251.7  | 298.9  | 527.9  | 1300.5 |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 763.3  | 797.5  | 839.3  | 881.6  | 1101.9 | 3112.0 |
| [Boost.Log](https://www.boost.org)                                        | 6734.5 | 6901.6 | 7264.6 | 7744.7 | 12706.7 | 47844.7 |
| [spdlog](https://github.com/gabime/spdlog)                                | 11013.5 | 11142.1 | 11366.2 | 11676.5 | 14234.0 | 43787.1 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  99.3  | 124.8  | 150.3  | 173.1  | 273.4  | 541.5  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           | 111.8  | 129.7  | 152.5  | 180.1  | 300.0  | 601.7  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                | 112.3  | 136.2  | 162.2  | 188.3  | 324.4  | 604.4  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 202.9  | 234.4  | 266.9  | 294.6  | 434.6  | 1205.0 |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 798.1  | 837.1  | 880.0  | 914.7  | 1087.8 | 2951.9 |
| [Boost.Log](https://www.boost.org)                                        | 8945.9 | 9440.7 | 10021.2 | 10477.5 | 12521.8 | 36775.3 |
| [spdlog](https://github.com/gabime/spdlog)                                | 11124.7 | 11576.7 | 12064.9 | 12519.6 | 20843.2 | 38172.9 |

## Burst of 20 Calls

### Logging Numbers

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  11.6  |  12.6  |  14.3  |  15.9  |  22.8  |  32.4  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  12.8  |  13.6  |  15.4  |  16.8  |  25.0  |  40.7  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  13.3  |  14.7  |  16.9  |  19.5  |  25.3  |  45.9  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  13.8  |  15.5  |  17.7  |  20.1  |  29.8  | 101.1  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  17.3  |  19.0  |  21.2  |  23.7  |  33.6  |  48.4  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  59.7  |  64.3  |  69.3  |  75.0  | 103.5  | 359.1  |
| [spdlog](https://github.com/gabime/spdlog)                                | 237.8  | 254.7  | 288.3  | 323.9  | 434.1  | 1566.1 |
| [g3log](https://github.com/KjellKod/g3log)                                | 1457.3 | 1549.8 | 1807.0 | 1925.1 | 2774.6 | 4695.6 |
| [Boost.Log](https://www.boost.org)                                        | 2068.0 | 2198.1 | 2308.6 | 2414.0 | 2957.9 | 7137.1 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  12.4  |  13.6  |  15.4  |  16.8  |  23.2  |  33.0  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  14.2  |  15.9  |  17.0  |  18.2  |  25.3  |  39.9  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  15.1  |  16.2  |  17.8  |  19.6  |  25.9  |  48.1  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  14.6  |  16.3  |  18.5  |  20.7  |  28.2  | 141.1  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  19.4  |  20.9  |  22.7  |  25.2  |  32.3  | 137.2  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  78.0  |  86.5  |  99.4  | 111.6  | 132.7  | 751.5  |
| [spdlog](https://github.com/gabime/spdlog)                                | 1089.6 | 1258.1 | 1432.5 | 1557.7 | 2071.5 | 3018.7 |
| [Boost.Log](https://www.boost.org)                                        | 5041.0 | 5209.7 | 5354.1 | 5468.1 | 5850.7 | 10259.9 |
| [g3log](https://github.com/KjellKod/g3log)                                | 5389.2 | 5868.7 | 6209.0 | 6373.6 | 6647.1 | 8305.3 |

### Logging Large Strings

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  16.7  |  18.5  |  21.8  |  26.0  |  35.2  | 116.2  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  16.9  |  19.1  |  23.8  |  29.7  |  42.9  |  93.6  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  17.9  |  19.7  |  24.0  |  29.0  |  41.9  | 110.8  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  19.5  |  22.3  |  27.3  |  36.1  |  52.1  | 151.8  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  23.2  |  24.9  |  28.9  |  33.0  |  42.8  | 170.8  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  68.1  |  74.1  |  83.8  |  94.1  | 122.9  | 330.1  |
| [spdlog](https://github.com/gabime/spdlog)                                | 216.9  | 238.4  | 302.5  | 328.5  | 453.8  | 1623.2 |
| [g3log](https://github.com/KjellKod/g3log)                                | 1179.7 | 1237.0 | 1397.2 | 1595.2 | 2515.8 | 6349.6 |
| [Boost.Log](https://www.boost.org)                                        | 1686.9 | 1800.7 | 1912.3 | 2005.3 | 2712.5 | 7117.8 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  17.5  |  19.4  |  22.4  |  26.0  |  34.6  |  53.7  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  19.0  |  20.6  |  23.4  |  26.7  |  36.1  | 158.6  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  19.7  |  21.6  |  26.0  |  28.9  |  37.7  | 121.9  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  20.6  |  23.6  |  28.5  |  33.5  |  45.1  | 139.9  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  25.3  |  26.8  |  29.9  |  33.1  |  43.5  | 360.9  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  77.4  |  88.5  | 103.8  | 114.0  | 135.6  | 548.8  |
| [spdlog](https://github.com/gabime/spdlog)                                | 1058.7 | 1247.3 | 1449.0 | 1598.0 | 2270.5 | 3343.3 |
| [Boost.Log](https://www.boost.org)                                        | 3461.2 | 3592.7 | 3715.2 | 3795.6 | 4084.4 | 11219.6 |
| [g3log](https://github.com/KjellKod/g3log)                                | 3986.3 | 4105.8 | 4273.2 | 4439.3 | 5084.8 | 8040.6 |

### Logging Complex Types

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  74.2  |  78.5  |  86.9  | 111.7  | 141.8  | 235.4  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  74.9  |  79.6  |  88.3  | 104.4  | 139.4  | 271.1  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  74.8  |  81.0  |  90.7  | 110.1  | 140.0  | 402.3  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 108.0  | 116.5  | 130.7  | 147.8  | 191.9  | 546.5  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 590.7  | 606.1  | 619.8  | 632.9  | 709.9  | 1907.2 |
| [Boost.Log](https://www.boost.org)                                        | 4903.6 | 5138.2 | 5390.1 | 5663.1 | 7872.6 | 13979.5 |
| [spdlog](https://github.com/gabime/spdlog)                                | 10643.2 | 10814.1 | 10995.8 | 11185.2 | 14184.7 | 20918.2 |

**4 Threads Logging Simultaneously**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           |  76.8  |  80.0  |  85.8  |  92.4  | 133.8  | 186.0  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  75.1  |  79.3  |  87.0  |  95.0  | 136.0  | 259.9  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  78.8  |  82.6  |  90.2  |  98.4  | 131.9  | 285.8  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 118.0  | 126.6  | 137.5  | 148.4  | 182.9  | 568.0  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 664.6  | 708.4  | 747.4  | 769.5  | 870.0  | 1975.6 |
| [Boost.Log](https://www.boost.org)                                        | 6445.7 | 6765.4 | 7084.7 | 7317.6 | 8262.3 | 20563.6 |
| [spdlog](https://github.com/gabime/spdlog)                                | 10415.4 | 10520.3 | 10641.7 | 10835.5 | 12190.8 | 18769.5 |

## Cold Cache

### Logging Numbers

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           | 122.1  | 134.0  | 148.1  | 160.6  | 198.6  | 324.4  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  | 129.7  | 142.7  | 159.5  | 175.2  | 238.7  | 420.5  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                | 136.2  | 150.3  | 173.6  | 198.0  | 256.6  | 854.0  |
| [BqLog](https://github.com/Tencent/BqLog)                                 | 141.1  | 164.4  | 188.3  | 204.5  | 264.2  | 1086.7 |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 156.8  | 178.5  | 202.9  | 227.9  | 299.5  | 370.0  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 206.2  | 227.9  | 274.5  | 404.7  | 609.8  | 2336.2 |
| [spdlog](https://github.com/gabime/spdlog)                                | 2993.2 | 3111.5 | 3269.9 | 3490.7 | 4548.6 | 15278.9 |
| [g3log](https://github.com/KjellKod/g3log)                                | 6250.6 | 6397.1 | 6686.2 | 7023.7 | 10159.6 | 42755.2 |
| [Boost.Log](https://www.boost.org)                                        | 6686.8 | 6866.9 | 7269.4 | 7516.8 | 10397.7 | 52311.9 |

### Logging Large Strings

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  | 139.4  | 156.3  | 173.6  | 199.7  | 277.8  | 491.5  |
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           | 129.1  | 147.6  | 175.2  | 229.5  | 371.6  | 562.6  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                | 143.2  | 159.5  | 179.6  | 201.3  | 288.6  | 561.0  |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 157.9  | 184.5  | 218.1  | 248.5  | 332.0  | 436.2  |
| [BqLog](https://github.com/Tencent/BqLog)                                 | 177.4  | 207.8  | 251.2  | 310.3  | 425.3  | 1317.3 |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 227.9  | 250.7  | 306.5  | 518.1  | 839.3  | 2601.5 |
| [spdlog](https://github.com/gabime/spdlog)                                | 3019.8 | 3190.7 | 3366.4 | 3538.4 | 4363.1 | 23581.9 |
| [g3log](https://github.com/KjellKod/g3log)                                | 5500.8 | 5664.1 | 5970.6 | 6316.8 | 9373.4 | 17203.3 |
| [Boost.Log](https://www.boost.org)                                        | 6036.8 | 6208.3 | 6548.4 | 6968.3 | 10392.8 | 42709.1 |

### Logging Complex Types

**1 Thread Logging**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [Quill Bounded Dropping Queue](https://github.com/odygrd/quill)           | 185.0  | 208.3  | 261.0  | 314.7  | 527.3  | 792.1  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  | 194.2  | 219.2  | 264.8  | 314.7  | 491.5  | 881.6  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                | 199.7  | 226.2  | 266.9  | 307.6  | 442.7  | 785.1  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 279.4  | 315.2  | 501.3  | 639.1  | 939.1  | 3289.4 |
| [fmtlog](https://github.com/MengRao/fmtlog)                               | 830.1  | 876.2  | 943.5  | 1017.8 | 1309.1 | 4982.1 |
| [Boost.Log](https://www.boost.org)                                        | 9531.3 | 9791.7 | 10127.5 | 10511.7 | 14739.6 | 52912.0 |
| [spdlog](https://github.com/gabime/spdlog)                                | 12756.1 | 13081.1 | 13445.2 | 13860.7 | 17533.2 | 59975.3 |

## Disabled Statement

**Disabled Statement, Hot**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  0.3   |  0.3   |  0.3   |  0.3   |  0.5   |  0.6   |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  0.3   |  0.4   |  0.4   |  0.4   |  0.4   |  0.6   |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  0.9   |  0.9   |  0.9   |  0.9   |  0.9   |  1.4   |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  2.2   |  2.2   |  2.2   |  2.2   |  2.3   |  4.4   |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     |  5.1   |  5.2   |  5.4   |  5.5   |  6.3   |  9.4   |
| [spdlog](https://github.com/gabime/spdlog)                                |  6.6   |  6.6   |  6.6   |  6.6   |  6.9   |  8.2   |
| [Boost.Log](https://www.boost.org)                                        |  67.6  |  68.5  |  70.6  |  71.7  |  97.8  | 165.4  |

**Disabled Statement, Cold**

| Library                                                                   |  50th  |  75th  |  90th  |  95th  |  99th  | 99.9th |
|---------------------------------------------------------------------------|:------:|:------:|:------:|:------:|:------:|:------:|
| [fmtlog](https://github.com/MengRao/fmtlog)                               |  48.8  |  53.7  |  64.6  |  74.3  | 111.2  | 197.5  |
| [Quill Unbounded Queue](https://github.com/odygrd/quill)                  |  64.6  |  77.0  |  89.5  | 106.3  | 143.2  | 233.8  |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                |  68.9  |  76.5  |  90.1  | 108.0  | 137.3  | 229.5  |
| [spdlog](https://github.com/gabime/spdlog)                                |  76.0  |  82.5  |  94.4  | 115.6  | 150.8  | 284.3  |
| [BqLog](https://github.com/Tencent/BqLog)                                 |  74.3  |  83.0  |  99.8  | 127.0  | 174.2  | 309.8  |
| [MS BinLog](https://github.com/Morgan-Stanley/binlog)                     | 105.3  | 121.5  | 140.0  | 156.3  | 194.8  | 295.1  |
| [Boost.Log](https://www.boost.org)                                        | 446.0  | 478.0  | 667.3  | 810.6  | 1079.1 | 2366.0 |

## Throughput

**1 Producer Thread**

| Library                                                                    | million msg/second | elapsed time | producers done |
|----------------------------------------------------------------------------|:------------------:|:------------:|:--------------:|
| [BqLog (binary log)](https://github.com/Tencent/BqLog)                     |       15.12        |    264 ms    |     263 ms     |
| [MS BinLog (binary log)](https://github.com/Morgan-Stanley/binlog)         |       15.11        |    264 ms    |     264 ms     |
| [BqLog](https://github.com/Tencent/BqLog)                                  |        6.52        |    613 ms    |     612 ms     |
| [spdlog](https://github.com/gabime/spdlog)                                 |        2.28        |   1755 ms    |    1752 ms     |
| [fmtlog](https://github.com/MengRao/fmtlog)                                |        1.53        |   2613 ms    |    2597 ms     |
| [Quill](https://github.com/odygrd/quill)                                   |        1.51        |   2656 ms    |    2654 ms     |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                 |        0.92        |   4340 ms    |    4336 ms     |
| [Boost.Log](https://www.boost.org)                                         |        0.52        |   7741 ms    |    7741 ms     |

**4 Producer Threads**

| Library                                                                    | million msg/second | elapsed time | producers done |
|----------------------------------------------------------------------------|:------------------:|:------------:|:--------------:|
| [MS BinLog (binary log)](https://github.com/Morgan-Stanley/binlog)         |       32.25        |    124 ms    |     123 ms     |
| [BqLog (binary log)](https://github.com/Tencent/BqLog)                     |       14.29        |    279 ms    |     277 ms     |
| [BqLog](https://github.com/Tencent/BqLog)                                  |        6.20        |    645 ms    |     639 ms     |
| [Quill](https://github.com/odygrd/quill)                                   |        4.46        |    896 ms    |     880 ms     |
| [Quill - Macro Free Mode](https://github.com/odygrd/quill)                 |        3.15        |   1270 ms    |    1268 ms     |
| [fmtlog](https://github.com/MengRao/fmtlog)                                |        1.46        |   2739 ms    |    2675 ms     |
| [spdlog](https://github.com/gabime/spdlog)                                 |        0.82        |   4892 ms    |    4889 ms     |
| [Boost.Log](https://www.boost.org)                                         |        0.58        |   6853 ms    |    4090 ms     |

