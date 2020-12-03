#!/bin/sh
cmake -DCMAKE_TOOLCHAIN_FILE="../../../../tools/cmake_toolchain_files/armgcc.cmake" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=release  .
make -j8
arm-none-eabi-size release/coremark.elf
arm-none-eabi-objcopy -O ihex release/coremark.elf coremark.hex
echo "output coremark.hex seccussful"
