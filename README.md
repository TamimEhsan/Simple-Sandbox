# Simple Sandbox

Let's try to create a simple sandbox and in turn learn the inner workings of linux security primitives like seccomp, cgroups and namespaces

You can find detailed explanations inside each folder.

At first let's try to work with seccomp which means secure computing mode. 
### 1. Running simple C program inside  sandbox
At first we will try to run a simple c program inside a sandbox using `execve`
### 2. Seccomp strict mode
We will see how seccomp strict mode allows a very strict pool of system call
### 3. Add filter to sandbox
Using seccomp rules we initialize sandbox to block everything, then add rules to allow the bare minimum.
### 4. Get Resource Usage
We will try to find the resource usage of the sandboxed process. We will also refactor the code to allow monitoring the child.
### 5. Set Resource Usage
In this part we will limit the resource usage of the sandboxed program to a certain upper bound.