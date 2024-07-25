#include <arpa/inet.h>  
#include <chrono>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>  
#include <unistd.h> 


SSL *ssl;
using namespace std;  

void log_ssl()
{
    //To log ssl errors.
    int err;
    while (err = ERR_get_error()) {
        char *error = ERR_error_string(err, 0);
        if (!error)
            return;
        
        //log error to corresponding files.
        Logger network_logger("network.txt");
        stringstream ss;
        ss<<error<<endl;
        network_logger.log(ss.str());
    }
}

int time_to_connect(char* name, int port){
    /*Measures the time taken to connect to 
    "google.com"'s TCP sever (HTTP or HTTPS) 
    by default. The TCP server to resolve can 
    be modified by user.*/  

    //initiate logger.
    Logger network_logger("network.txt");

    //start time
    auto start = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
    
    //Get hostinfo from name.
    hostent *host_info = gethostbyname(name);
	if(host_info == NULL)
	{
		cout<<name<<"is unavailable\n";
		exit(1);
	}

	in_addr * address = (in_addr * )host_info->h_addr;
	char *ip_address = inet_ntoa(* address);

    //Debugging
    //cout<<"IP Address : "<<ip_address<<endl;
    //cout<<host_info->h_name<<endl;

    //network specifications
    int sock = 0, valread, client_fd;  
    struct sockaddr_in serv_addr;  
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  
        cout<<"\n Socket creation error \n";  
        return -1;  
    }  
  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_port = htons(port);  
    serv_addr.sin_addr.s_addr = inet_addr(ip_address);
  
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {  
        cout<<"\nConnection Failed \n";  
        return -1;  
    }  

    //For SSL connection
    if(port == 443){

        SSL_library_init();
        SSLeay_add_ssl_algorithms();
        SSL_load_error_strings();
        const SSL_METHOD *method = TLSv1_2_client_method();
        SSL_CTX *ctx = SSL_CTX_new (method);
        ssl = SSL_new (ctx);

        if (!ssl) {
            cout<<"Error creating SSL.\n";
            log_ssl();
            return -1;
        }

        int sock_ssl = SSL_get_fd(ssl);
        SSL_set_fd(ssl, sock);
        
        int err = SSL_connect(ssl);
        if (err <= 0) {
            cout<<"Error creating SSL connection.  err="<<err<<endl;
            log_ssl();
            return -1;
        }
        //cout<<"SSL connection using"<<SSL_get_cipher (ssl)<<endl;
    }

    // closing the connected socket  
    close(client_fd);

    //end time 
    auto end = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
    
    //log time taken.
    stringstream ss;
    ss<<(((float)(end-start))/1000);
    network_logger.log(ss.str());

    return 1;
}
