# python3.7-in-cpp-example

A test to call python3 functions from c++. Adapted from the python 2.7 docs

Requirements:
- Python3
- Your favorite flavour of C++ compiler

Compile:  
To find your compiler/linker flags navigate to your python install dir and run your: python3.xm-config --ldflags  
if my case this produces:  
g++ main.cpp -L/usr/lib/python3.7/config-3.7m-x86_64-linux-gnu -L/usr/lib -lpython3.7m -lcrypt -lpthread -ldl  -lutil -lm

Run:  
./a.out multiply multiply 2 3
Number of arguments 5
Will compute 2 times 3
Result of call: 6

