set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -g3")

add_subdirectory(call_site_latency)

add_executable(benchmark_quill_spsc_queue)
target_sources(benchmark_quill_spsc_queue PRIVATE benchmark_quill_spsc_queue.cpp)
target_link_libraries(benchmark_quill_spsc_queue PRIVATE quill Threads::Threads)