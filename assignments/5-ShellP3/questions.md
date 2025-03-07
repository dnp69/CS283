1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

To make sure all child processes finish before the shell accepts new commands, the shell uses waitpid() to wait for each child that was created with fork(). This happens after setting up and running all the piped commands. If waitpid() is not used, the child processes might become zombie processes, which are processes that have finished running but haven't been properly cleaned up. Over time, these zombie processes can use up system resources and cause the shell to behave incorrectly.


2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

After using dup2() to set up the correct input and output for pipes, it is important to close any pipe ends that are no longer needed. If these extra pipe ends stay open, it can cause problems like deadlocks, where a process keeps waiting for input or output that never finishes because another process still has the pipe open. It also wastes system resources by keeping file descriptors open longer than needed.


3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command is a built-in because it changes the shell's current directory. If cd were run as an external command, only the child process created by fork() would change its directory, and that change would disappear once the child process ends. The parent shell's directory would stay the same. This makes cd useless as an external command, so it has to be built directly into the shell.


4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To allow unlimited piped commands, the code would need to use dynamic memory with functions like malloc() and realloc() to grow the list of commands as needed. The trade-off is that this makes the code more complex and requires careful handling of memory. It also uses more resources, and there are still limits on how many pipes the system can handle at once.
