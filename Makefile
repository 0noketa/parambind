CC=cl /MD /arch:IA32

parambind.lib:
    cd src && nmake -f Makefile.msvc32 "CC=$(CC)"

test_stack_main.exe: parambind.lib test_stack_sub.obj
    $(CC) test_stack_main.c test_stack_sub.obj parambind.lib

test_sum.exe: parambind.lib test_stack_sub.obj
    $(CC) test_sum.c test_stack_sub.obj parambind.lib

clean:
    cd src && nmake -f Makefile.msvc32 clean
