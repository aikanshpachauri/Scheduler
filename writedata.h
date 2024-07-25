#include <ctime>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <sstream> 
#include <stdio.h>

using namespace std;

float parse_log_line(string str){
    /*Reads the log files and parses it to fetch the 
    float value of the metrix.*/

    float number;
    stringstream ss;
    ss<<str;
    string temp;
    bool flag = false;
    while(!ss.eof()){
        ss >> temp;
        
        //Checks for the end of the timestamp. 
        if(!temp.compare("]")){
            flag = true;
        }
        if(stringstream(temp) >> number && flag == true){
            return number;
        }
        temp = "";
    }

    //default return
    return 0.0;
}

stringstream track_metrics(char *probe = "network.txt"){
    /*Reads the network and system log dumps and read the
    values to calculate the average, maximum and minimum
    and returns a stringstream to write in a logfile.*/

    ifstream file_in;
    float maximum = INT_MIN;
    float minimum = INT_MAX;
    float average = 0;
    int count = 0;
    file_in.open(probe);
    string entry;
    while(getline(file_in, entry)){
        float value = parse_log_line(entry);
        if(maximum < value){
            maximum = value;
        }
        if(minimum > value){
            minimum = value;
        }
        average += value;
        count++;
    }
    //Check for empty files.
    if(count != 0){
        average /= count;
    }
    file_in.close();

    //Write values for metrics in a file.
    stringstream file_out;
    //file_out.open("metric.txt");
    if(!strcmp(probe, "network.txt")){
        file_out<<"Network Probe (Time to connect to TCP Server) (ms)"<<endl;
    }else{
        file_out<<"System Probe (Current Physical Memory Used) (KB)"<<endl;
    }
    file_out<<"Maximum: "<<maximum<<endl;
    file_out<<"Minimum: "<<minimum<<endl;
    file_out<<"Average: "<<average<<endl;
    return file_out;
}

class Logger{
    /*Logger class to write values from system
    and network probes to their respective dump
    files.*/

    public:
    //constructor
    Logger(const string& filename){
        logfile.open(filename, ios::app);
        if(!logfile.is_open()){
            cout<<"Trouble opening logfile";
        }
    }

    //destructor
    ~Logger(){ logfile.close(); };

    void log(const string& message){
        /*Writes the string message to the 
        log file.*/
        time_t now = time(0);
        string dt = ctime(&now); 
        dt.erase(dt.size() - 1);

        ostringstream log_entry;
        log_entry <<"[ " << dt << " ]" <<" : "<<message<<endl;
        if(logfile.is_open()){
            logfile<<log_entry.str();
            logfile.flush();
        }
    }


    private:
    ofstream logfile;
};

//Test Function to validate logging.
void dump_data(char *source, char *buffer){
    Logger logf(source);
    logf.log(buffer);
}