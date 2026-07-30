#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <set>
#include <thread>
#include "Monitor.hpp"
using namespace std;

const int MESSAGE_SIZE = 40001;

void clientService(int client_fd, Monitor &monitor){
    string END_MSSG = "END OF COMMUNICATION"; //String for endind communication between server and client
    //We add the client's socket to the monitor's vector
    monitor.addVector(client_fd);
    //Message buffer
    char buffer[MESSAGE_SIZE];
    int recvBytes = 0;
    int sendBytes = 0;

    bool out = false;
    while(!out){
        //receive the client's message
        recvBytes = recv(client_fd,buffer,MESSAGE_SIZE,0);
        if(recvBytes == -1){
            cerr << "Failed attempt at receiving the client's message" << endl;
            cerr << "Error: "<< errno << endl;
            //We close the client's socket
            monitor.FreeBlock(client_fd);
            out = true;
        }
        else if(recvBytes == 0){
            cout << "client closed the connection" << endl;
            monitor.FreeBlock(client_fd);
        }
        else{
            if(buffer != END_MSSG){
                //The sender broadcast the message to each one of the client's int the monitor's socket vector
                monitor.Broadcast(client_fd,buffer);
            }
            else{
                monitor.FreeBlock(client_fd);
                out = true;
            }
        }
    }

}

int main(int argc, char* argv[]){
    Monitor clientMonitor;
    //Verification of the adequate number of parameters
    if(argc != 2){
        cerr << "Usage: "<< argv[0] << " <Port> " << endl;
        exit(1);
    }

    int SERVER_PORT = atoi(argv[1]);
    vector<thread> clients;
    //server sockaddr_creation
    sockaddr_in addr;
    memset(&addr,0,sizeof(addr)); // We initialize at 0 the memory values of the struct to avoid residual values
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    //Socket creation
    int server_fd = socket(AF_INET,SOCK_STREAM,0);

    if(server_fd == -1){
        cerr << "Socket creation error" << endl;
        cerr << "Error: "<< errno << endl;
        exit(1);
    }

    //Bind
    int server_bind = bind(server_fd,(struct sockaddr *) &addr,sizeof(addr));

    if(server_bind == -1){
        cerr << "Socket binding error" << endl;
        cerr << "Error: "<< errno << endl;
        //We close the socket
        close(server_fd);
        exit(1);
    }

    //Listen
    int listen_code = listen(server_fd,5); //In v.01 we leave the backlog as 5, the backlog is an arbitrary decision because we are working in this version with only one client
    
    if(listen_code == -1){
        cerr << "Socket listening error" << endl;
        cerr << "Error: "<< errno << endl;
        //We close the socket
        close(server_fd);
        exit(1);
    }

    bool end = false;
    int client_fd;
    sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    while(!end){
        //Accept
        client_fd = accept(server_fd,(struct sockaddr *)&client_addr,&client_addr_size);
        
        if(client_fd == -1){
            cerr << "Socket accepting error" << endl;
            cerr << "Error: "<< errno << endl;
            //We close the socket
            close(server_fd);
            exit(1);
        }
        else{
            if(!end){
                //Introduce the client into the client's thread vector
                clients.push_back(thread(&clientService,client_fd,clientMonitor));
                cout << "New client accepted: " << client_fd << endl;
            }
            else{
                cout << "Finished" << endl;
            }
        }
    }
    //We close the server socket 
    int error_code = close(server_fd);
    if(error_code == -1){
        cerr << "Failed attempt at closing the server's socket" << endl;
    }
}