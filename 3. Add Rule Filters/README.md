
Here we have 3 programs and a sandbox. The sandbox has seccomp rules added to only allow reading from files. So it will allow reading from file but not allow writing to any file.

In extern1 we write to a file and in extern3 we read from it. 

At first compile all external program and sandbox
```
gcc -o extern1 extern1.c
gcc -o extern2 extern1.c
gcc -o extern3 extern1.c
g++ sandbox.cpp -o sandbox -lseccomp
```
To check the programs are working correctly perform
```
./extern1
./extern3
```
You should see data writen to input.txt and data being read from it. \
Then we run the external programs inside sandbox
```
./sandbox extern3
./sandbox extern1
```

extern3 would work nicely but running extern1 would throw bad system call error