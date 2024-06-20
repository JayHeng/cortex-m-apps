#!/bin/sh
cmake -DCMAKE_TOOLCHAIN_FILE="../../../../tools/cmake_toolchain_files/armgcc.cmake" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=sdram_txt_release  .
make -j8
arm-none-eabi-size sdram_txt_release/coremark.elf
arm-none-eabi-objcopy -O ihex sdram_txt_release/coremark.elf coremark.hex
echo "output coremark.hex seccussful"
