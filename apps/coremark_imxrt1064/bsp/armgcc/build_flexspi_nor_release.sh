#!/bin/sh
cmake -DCMAKE_TOOLCHAIN_FILE="../../../../tools/cmake_toolchain_files/armgcc.cmake" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=flexspi_nor_release  .
# make -j 2>&1 | tee build_log.txt
make -j8
arm-none-eabi-size flexspi_nor_release/coremark.elf
arm-none-eabi-objcopy -O ihex flexspi_nor_release/coremark.elf coremark.hex
echo "output coremark.hex seccussful"
