# ChameLib

![test](https://github.com/Hugu1e/ChameLib/actions/workflows/test.yml/badge.svg)

## Dependency

[PBC library](http://crypto.stanford.edu/pbc/)

[openssl](https://github.com/openssl/openssl.git)

## Quick start

1. **Clone the Repository**

   ```
   git clone https://github.com/hyj666-maker/ChameLib.git
   cd ChameLib
   ```

2. **Build**

   ```
   mkdir build
   cd build
   cmake ..
   make
   ```

## Testing

1. **Run base tests**

   ```
   cd build
   ctest -L "basetest"
   ```

    **Run individual base test**

   ```
   cd build
   ./bin/[BASETEST_NAME]
   ```
3. **Run benchmark tests**
   ```
   cd build
   ctest -L "benchmarktest"
   ```
    **Run individual benchmark test**
   ```
   cd build
   ./bin/[BENCHMARKTEST_NAME]
   ```
   **Command-Line Arguments**

   The test programs support the following command-line arguments:

   - `--repeat` or `-r`: Set the number of repetitions for the test (default: 100).
     Example:
     ```
     ./bin/[BENCHMARKTEST_NAME] --repeat 200
     ```
     or
     ```
     ./bin/[BENCHMARKTEST_NAME] -r 200
     ```

   - `--checktime` or `-c`: Enable time checking to verify runtime consistency (default: disabled).
     Example:
     ```
     ./bin/[BENCHMARKTEST_NAME] --checktime
     ```
     or
     ```
     ./bin/[BENCHMARKTEST_NAME] -c
     ```

   - `--help` or `-h`: Display help information about the available arguments.
     Example:
     ```
     ./bin/[BENCHMARKTEST_NAME] --help
     ```
     or
     ```
     ./bin/[BENCHMARKTEST_NAME] -h
     ```
