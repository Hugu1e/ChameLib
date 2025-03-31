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

1. **Run all the Tests**

   ```
   cd build
   ctest
   ```

2. **Run individual test**

   ```
   cd build
   ./bin/[TEST_NAME]
   ```
   **Command-Line Arguments**

   The test programs support the following command-line arguments:

   - `--repeat` or `-r`: Set the number of repetitions for the test (default: 100).
     Example:
     ```
     ./bin/[TEST_NAME] --repeat 200
     ```
     or
     ```
     ./bin/[TEST_NAME] -r 200
     ```

   - `--checktime` or `-c`: Enable time checking to verify runtime consistency (default: disabled).
     Example:
     ```
     ./bin/[TEST_NAME] --checktime
     ```
     or
     ```
     ./bin/[TEST_NAME] -c
     ```

   - `--help` or `-h`: Display help information about the available arguments.
     Example:
     ```
     ./bin/[TEST_NAME] --help
     ```
     or
     ```
     ./bin/[TEST_NAME] -h
     ```

   **Example Usage**

   - Run a test with 500 repetitions and enable time checking:
     ```
     ./bin/[TEST_NAME] --repeat 500 --checktime
     ```
     or
     ```
     ./bin/[TEST_NAME] -r 500 -c
     ```

   - Run a test with default settings:
     ```
     ./bin/[TEST_NAME]
     ```
