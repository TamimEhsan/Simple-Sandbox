Here we try to run a program by another program ie the sandbox. Here we will not add any restrictions to it.
```
gcc -o exteral external.c
g++ -o sandbox sandbox.cpp
./sandbox external
```