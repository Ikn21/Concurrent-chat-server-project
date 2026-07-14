#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <set>

using namespace std;

const int MESSAGE_SIZE = 40001;

int main(int argc, char* argv[]){
    //Verification of the adequate number of parameters
    if(argc != 3){
        cerr << "Usage: "<< argv[0] << " <Port> <Server Adress> " << endl;
        exit(1);
    }
    string END_MSSG = "END OF COMMUNICATION";
    int SERVER_PORT = atoi(argv[1]);
    char* SERVER_ADRESSS = argv[2];

    //Client sockaddr creation
    sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    //We use the inet_pton to convert the IPV4 address to binary form
    int address_code = inet_pton(AF_INET,SERVER_ADRESSS,&server_addr.sin_addr);

    if(address_code == -1){
        cerr << "Not valid IP address" << endl;
        exit(1);
    }

    //Socket creation
    int client_fd = socket(AF_INET,SOCK_STREAM,0);

    if(client_fd == -1){
        cerr << "Socket creation error" << endl;
        cerr << "Error: "<< errno << endl;
        exit(1);
    }

    //Connection
    int connect_code = connect(client_fd,(struct sockaddr* )&server_addr,sizeof(server_addr));

    if(connect_code == -1){
        cerr << "Failed attempt at establishing connection" << endl;
        cerr << "Error: "<< errno << endl;
        exit(1);
    }

    //Message buffer
    char buffer[MESSAGE_SIZE];
    int recvBytes = 0;
    int sendBytes = 0;
    bool end = false;
    while(!end){
        //Send a message to the server
        cout << "Write a message for the server: ", cin >> buffer;
        sendBytes = send(client_fd,buffer,sizeof(buffer),0);

        if(sendBytes == -1){
            cerr << "Failed attempt at sending the client's message" << endl;
            cerr << "Error: "<< errno << endl;
            //We close the client's socket
            close(client_fd);
            exit(1);
        }
        //receive the server's response
        recvBytes = recv(client_fd,buffer,MESSAGE_SIZE,0);

        if(recvBytes == -1){
            cerr << "Failed attempt at receiving the client's message" << endl;
            cerr << "Error: "<< errno << endl;
            //We close the client's socket
            close(client_fd);
            exit(1);
        }
        if(recvBytes == 0){
            cout << "server closed the connection"<<endl;
            break;
        }

        cout << "Server's response received" << endl;

        if(buffer == END_MSSG){
            end = true;
        }
        else{
            cout << "Server's response: " << buffer << endl;
        }
    }

    //Once the communication is finished we close the client's socket
    cerr << "Closing client: " << client_fd << endl;
    int error_code = close(client_fd);
    if(error_code == -1){
        cerr << "Failed attempt at closing the client's socket" << endl;
    }

    return error_code;
}