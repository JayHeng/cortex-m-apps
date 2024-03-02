# How to compile
position : bsp->armgcc

## linux on bash
$ build_debug.sh

## win10 on powershell
$ build_debug.bat

# How to debug
Example : JLinkGDB

## linux
1. run JLinkGDBServer

    $ ./JLinkGDBServer

    Config parameter for target MCU On the GUI.
2. run arm-none-eabi-gdb

    $ arm-none-eabi-gdb debug/coremark.elf

    $ (gdb) target remote 192.168.1.xxx:2331

    $ (gdb) monitor reset

    $ (gdb) monitor halt

    $ (gdb) load

    $ (gdb) c

## win10
1. run JLinkGDBServer.exe

    Config parameter for target MCU On the GUI.

2. run arm-none-eabi-gdb.exe

    $ arm-none-eabi-gdb.exe debug/coremark.elf

    $ (gdb) target remote 192.168.1.xxx:2331

    $ (gdb) monitor reset

    $ (gdb) monitor halt

    $ (gdb) load
    
    $ (gdb) c    


