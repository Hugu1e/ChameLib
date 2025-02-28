# ChameLib

![Build Status](https://github.com/Hugu1e/ChameLib/actions/workflows/test.yml/badge.svg)

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

1. Run all the Tests

   ```
   cd build
   ctest
   ```

2. Run individual test

   ```
   cd build
   ./bin/[TEST_NAME]
   ```
