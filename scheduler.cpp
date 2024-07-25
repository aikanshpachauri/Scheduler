#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <future> 
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sstream> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/asio.hpp>

#include "system_probes.h"
#include "network_probes.h"
#include "daemon.h"

using namespace std;
#define MSG_PIPE "/tmp/msgfifo"

long long network_timer = 0;
long long system_timer = 0;
bool update_flag = false;

void run_network_probes(int network_time){
    //Run network probe and manage its frequency.
    if(network_timer == 0 || time(0) - network_timer >= network_time){
        time_to_connect("google.com", 443);
        update_flag = true;
        network_timer = time(0);
    }
}

void run_system_probes(int sys_time){
    //Run the system probe and manage its frequency.
    if(system_timer == 0 || time(0) - system_timer >= sys_time){
        get_process_value();
        update_flag = true;
        system_timer = time(0);
    }
}

void update_metrics(){
    /*Get the average, max and min values from the network and
    system probe log files and log them in the metric log file.
    */

    auto future_network = async(track_metrics, (char *)"network.txt");
    auto future_system = async(track_metrics, (char *)"system.txt");
    stringstream write_metric;
    
    time_t now = time(0);
    string dt = ctime(&now);     
    
    write_metric<<"Last Updated : "<<dt;
    write_metric<<future_network.get().str();
    write_metric<<future_system.get().str();
    
    ofstream file_out;
    file_out.open("metric.txt");
    file_out<<write_metric.str();
    file_out.close();
}

void scheduler(int network_time, int sys_time){
    //Run the network and sytem probes asynchronously.

    async(run_network_probes, network_time);
    async(run_system_probes, sys_time);

    if(update_flag){
        //Update the metric log file if the
        //system or network probes are updated.

        async(update_metrics);
        update_flag = false;
    }
}

bool async_fifo_read(char (& buffer) [2]){
    /*Reads from the fifo queue asynchronously
    for user interaction with blocking the daemon.*/

    int pipefifo;
    bool flag = false;
    pipefifo = open(MSG_PIPE, O_RDONLY | O_NONBLOCK);
    boost::asio::io_service io_service;
    boost::asio::posix::stream_descriptor fifo(io_service, pipefifo);
    
    boost::asio::async_read(
        fifo,
        boost::asio::buffer(buffer),
        [&](const boost::system::error_code &ec, std::size_t size)
        {
            if(!strcmp(buffer, "x") || !strcmp(buffer, "p") || !strcmp(buffer, "s")){
                flag = true;
            }
        }
    );
    
    io_service.run();
    close(pipefifo);
    return flag;
}

int main(int argc, char *argv[]){
    
    if(argc == 2){
        int pipefifo, returnval;
        char *buffer = argv[1];

        returnval = mkfifo(MSG_PIPE, 0666);
        pipefifo = open(MSG_PIPE, O_WRONLY);
        if (pipefifo == -1)
        {
            cout << "Error, cannot open fifo" << std::endl;
            return 1;
        }
        write(pipefifo, buffer, sizeof(buffer));
        close(pipefifo);
        return 0;
    }

    //Fork the daemon.
    daemon_base();

    syslog (LOG_NOTICE, "Daemon started.");

    int network_time = 10;
    int sys_time = 20;
    bool run = true;
    network_time = atoi(argv[1]);
    sys_time = atoi(argv[2]);

    while (1){
        bool flag = false;
        char buffer[2];

        flag = async_fifo_read(buffer);

        if(flag == true){
            if(!strcmp(buffer, "x")){
                //Terminate the daemon.
                syslog (LOG_NOTICE, "Exiting out.");
                closelog();
                exit(0);
            }else if(!strcmp(buffer, "p") && run == true){
                //Do not run the scheduler, the daemon is
                //suspended.
                syslog (LOG_NOTICE, "Scheduler suspended.");
                run = false;
            }else if(!strcmp(buffer, "s") && run == false){
                //Resume the scheduler.
                syslog (LOG_NOTICE, "Scheduler Resumed.");
                run = true;
            }
        }   

        //Runs the scheduler function.
        if(run)scheduler(network_time,sys_time);
    }

    closelog();
    return 0;
}