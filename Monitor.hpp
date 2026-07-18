#pragma once

#include <mutex>
#include <vector>
using namespace std;

const static int MESSAGE_SIZE = 40001;

struct Connected_client{
    int client_socket;
    bool MarkedForDelete;
};

class Monitor{
    private:
        vector<Connected_client> sockets;
        mutex mtxMonitor;
    public:
        Monitor(); //Constructor
        ~Monitor(); //Destructor
        void RealizeSend(int socket_sender,char buffer[MESSAGE_SIZE]); //First operation made for sending the message from the sender to the rest of the clients
        void FreeBlock(int socket_sender); //Second operation made for deleting from the socket vector the socket who ends it communication
};