#include <fstream>
#include <syslog.h>

void signal_handler(int signum) {
    /*Generic signal handler to handle
    signals (SIGKILL in general)*/ 
    ofstream file;
    file.open("metric.txt",  ios::out | ios::app);
    file<<"Signal Encountered";
    file.close();
    exit(signum);  
}

void daemon_base(){
    /*Separates the scheduler from the program
    using fork() to create a daemon.*/

    pid_t pid;

    pid = fork();
    if(pid < 0){
        exit(EXIT_FAILURE);
    }
    if(pid > 0){
        exit(EXIT_SUCCESS);
    }

    if(setsid() < 0){
        exit(EXIT_FAILURE);
    }

    signal(SIGKILL, signal_handler);
    signal(SIGHUP, signal_handler);

    pid = fork();
    if(pid < 0){
        exit(EXIT_FAILURE);
    }

    if(pid > 0){
        exit(EXIT_SUCCESS);
    }

    umask(0);
    
    int z;
    for (z = sysconf(_SC_OPEN_MAX); z>=0; z--)
    {
        close (z);
    }
    
    openlog ("schedulerdaemon", LOG_PID, LOG_DAEMON);
}
