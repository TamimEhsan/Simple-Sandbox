
Now we try to set the resource usage of a spawned program.


The resource usage of a process can be set by `setrlimit(int resource, struct rlimit *limits)` . The who parameter decides what resource we want to limit and by how much. See `man` for more usage. But for now we will work with only memory. 


At first compile all external program and sandbox. At first the array size of file 4 is kept small.
```
gcc -o extern4 extern4.c
gcc -o extern5 extern5.c
g++ sandbox.cpp -o sandbox -lseccomp
```
To check the programs are working correctly perform
```
./extern4
./extern5
```
You should see the programs running normally 
Then we run the external programs inside sandbox
```
./sandbox extern4
./sandbox extern5
```
The first program will run normally. The second one will close fail to allocate memory but close gracefully due to error handling.

Now if we increase the array size of extern4 and repeat the steps again we will see that it exists with memory limit exceed. 