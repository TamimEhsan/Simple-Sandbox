
Now we try to find the resource usage of a spawned program. But we need to do some refactoring first

### Refactoring 
We will spawn a child process and use execv on that. And we will monitor the child process from the parent process. We will wait till the child exits and then measure the resource usage. 

### Memory usage status
The resource usage of a process can be get by `getrusage(int who, struct rusage *rusage)` . The who parameter decides whether we want the process itself or its children. The rusage struct will contain many useful info. See `man` for more usage. But for now we will work with only `rusage.ru_maxrss` which stands for maximum resident set size.



At first compile all external program and sandbox
```
gcc -o extern4 extern4.c
g++ sandbox.cpp -o sandbox -lseccomp
```
To check the programs are working correctly perform
```
./extern4
```
You should see program running normally. \
Then we run the external programs inside sandbox
```
./sandbox extern4
```
You will see the memory usage of the external program hopefully. \
To check if this is actually correct you can run the program in any memory usage tool or use `time` library locally
```
/usr/bin/time -v ./extern4
```

The output will be pretty close.