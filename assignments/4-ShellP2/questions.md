1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  We use fork() with execvp() instead of just calling execvp() directly because fork() creates a separate child process to execute the command. This allows our shell to remain running while the command executes in the background. If we called execvp() directly in the parent process, it would replace the shell with the new program, preventing the user from continuing to use the shell after the command completes. Using fork() ensures that we can execute commands while maintaining control over the shell environment.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it means the system could not create a new process, usually due to resource limits. In our implementation, we check if fork() returns -1 and print an error message using perror("fork"). This prevents the shell from crashing and allows it to continue running. Without this check, the shell might attempt to execute commands without a valid child process, leading to undefined behavior or failures.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for the command in the directories listed in the PATH environment variable. If a full path is given (e.g., /bin/ls), execvp() tries to execute that file directly. Otherwise, it checks each directory in PATH to find an executable with the matching name. This is why commands like ls work without specifying /bin/ls—the shell relies on PATH to locate the correct executable.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  Calling waitpid() in the parent process ensures that the shell waits for the child process to complete before displaying another prompt. If we didn’t call wait(), the shell would continue running while the child process executes, potentially leading to interleaved output or orphaned processes. Worse, without waiting, zombie processes could accumulate, wasting system resources. By using waitpid(), we ensure proper process cleanup and orderly execution.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() extracts the exit code of the child process after it finishes running. This is important because it lets the shell determine whether a command was successful or if it failed. In our implementation, we use WEXITSTATUS() to check if the exit code is nonzero, and if so, we print an error message. This feedback helps users understand when a command fails and why.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  Our build_cmd_buff() implementation handles quoted arguments by treating everything inside quotes as a single argument, even if it contains spaces. This is necessary because some commands require arguments with spaces, such as echo "Hello World", which should be treated as one argument instead of two. Without this handling, commands with spaces in arguments would break, making the shell less functional.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  Compared to the previous assignment, we improved our parsing logic by properly handling quotes and ensuring that arguments are correctly separated. We also added better error handling and space trimming to prevent unexpected behaviors. One challenge was handling cases where users entered empty quotes or multiple spaces between arguments, which required extra checks to ensure everything was parsed correctly.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in Linux allow the system or other processes to communicate asynchronously with a running program. Unlike other forms of interprocess communication (IPC) like message queues or shared memory, signals do not require direct coordination between processes. They are often used to notify processes about events like termination requests, interruptions, or state changes.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  Three commonly used signals are SIGKILL, SIGTERM, and SIGINT. SIGKILL immediately stops a process and cannot be ignored, making it useful for forcefully terminating unresponsive programs. SIGTERM is a more graceful termination signal that allows the process to clean up before exiting. SIGINT is sent when the user presses Ctrl+C, typically used to interrupt a running process without killing it outright.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it is paused and cannot continue execution until it receives SIGCONT. Unlike SIGINT, which can be caught and handled by the process, SIGSTOP cannot be ignored or blocked because it is enforced directly by the operating system. This makes it useful for debugging or pausing processes without terminating them.
