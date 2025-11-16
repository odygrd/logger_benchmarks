Logger Benchmarks

Performance benchmarks for various C++ logging libraries.

## Building and Running

1. **Compile all targets**
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release -S . -B cmake-build-release
   cmake --build cmake-build-release
   ```

2. **Run all benchmarks**
   ```bash
   python3 run_all.py
   ```

3. **Generate result tables**
   ```bash
   python3 generate_tables.py
   ```

   This will parse all `bench_results_*.txt` files in the current directory and output formatted markdown tables with
   averaged results.