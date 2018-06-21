cmake_minimum_required(VERSION 3.10)
project(lab4)

set(CMAKE_CXX_STANDARD 11)

include_directories(boot/include)
include_directories(include)

add_executable(lab4
        boot/include/fat12hdr.inc
        boot/include/load.inc
        boot/include/pm.inc
        boot/boot.asm
        boot/boot.bin
        boot/loader.asm
        boot/loader.bin
        include/console.h
        include/const.h
        include/global.h
        include/keyboard.h
        include/keymap.h
        include/proc.h
        include/protect.h
        include/proto.h
        include/queue.h
        include/sconst.inc
        include/string.h
        include/tty.h
        include/type.h
        kernel/clock.c
        kernel/console.c
        kernel/global.c
        kernel/i8259.c
        kernel/kernel.asm
        kernel/keyboard.c
        kernel/main.c
        kernel/printf.c
        kernel/proc.c
        kernel/protect.c
        kernel/queue.c
        kernel/start.c
        kernel/syscall.asm
        kernel/systask.c
        kernel/tty.c
        kernel/vsprintf.c
        lib/klib.c
        lib/kliba.asm
        lib/misc.c
        lib/string.asm
        a.img
        bochsrc
        kernel.bin
        krnl.map
        Makefile)
