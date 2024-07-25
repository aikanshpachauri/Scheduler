1. The scheduler is written for an Ubuntu (Linux) based system, and requires OpenSSL and Boost (libboost).

2. To compile the code use makefile.

3. To run the scheduler:
./scheduler <time1> <time2>
Here time1 -> the frequency of the network probe task (in seconds) and time2 -> the frequency of the system probe task (in seconds) 

4. To interact with the scheduler use:
./scheduler <command>
Here command can be 'x' - to terminate the scheduler,
		    'p' - to pause the scheduler,
		and 's' - to resume the scheduler.

5. The outputs from the probes are stored as text log files ("network.txt" and "system.txt"), and the updated average, max and minimum from the probes is stored in a separate text log file ("metric.txt") that is updated after each new probe.

6. The program utilises fifo pipes to communicate with the scheduler daemon.

7. The network log file stores the time taken in milliseconds and system log file stores the memory consumed by the process in KB.

8. Part of the daemon logs are dumped in /var/log/syslog. These are used to track the daemon state.