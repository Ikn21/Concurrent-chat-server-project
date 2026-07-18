#include "Monitor.hpp"
#include <unistd.h>
#include <sys/socket.h>

Monitor::Monitor(){}

Monitor::~Monitor(){
    for(auto socket : sockets){
        close(socket.client_socket); // For each client, at the end of the life of the monitor, all of the client's socket must be closed
    }
}
void Monitor::addVector(int socket_sender){
    unique_lock<mutex> lock(mtxMonitor);
    Connected_client client;
    client.client_socket = socket_sender;
    client.MarkedForDelete = false;
    sockets.push_back(client);
}

void Monitor::Broadcast(int socket_sender,char buffer[MESSAGE_SIZE]){
    unique_lock<mutex> lock(mtxMonitor);
    int sendBytes = 0;
    //We send the message receive to all client cockets in the sockets vector
    for(auto socket : sockets){
        if(socket.client_socket != socket_sender){
            sendBytes = send(socket.client_socket,buffer,sizeof(buffer),0);

            if(sendBytes == -1){
                //We mark the socket for it's elimination from the client's vector
                socket.MarkedForDelete = true;
            }
        }
    }

    for(auto it = begin(sockets); it != end(sockets); it++){
        if(it->MarkedForDelete){
            close(it->client_socket);
            it = sockets.erase(it);
        }
    }

}
void Monitor::FreeBlock(int socket_sender){
    unique_lock<mutex> lock(mtxMonitor);
    for(auto it = begin(sockets); it != end(sockets); it++){
        if(it->client_socket == socket_sender){
            close(it->client_socket);
            it = sockets.erase(it);
        }
    }
}