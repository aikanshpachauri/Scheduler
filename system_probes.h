#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "writedata.h"

using namespace std;

int parse_line(char* line){
    /* Parses the strings to get the 
    physical memory consumption.*/
    
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int get_process_value(){
    /*Checks the Physical memory used
    by the current process and 
    return 1 when successful.*/

    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parse_line(line);
            break;
        }
    }
    fclose(file);
    cout<<result;
    Logger syslogger("system.txt");
    stringstream ss;
    ss<<result;
    syslogger.log(ss.str());
    return 1;
}