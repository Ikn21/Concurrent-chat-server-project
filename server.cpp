#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <set>
using namespace std;

const int MESSAGE_SIZE = 40001;

int main(int argc, char* argv[]){
    //Verification of the adequate number of parameters
    if(argc != 2){
        cerr << "Usage: "<< argv[0] << " <Port> " << endl;
        exit(1);
    }

    string END_MSSG = "END OF COMMUNICATION"; //String for endind communication between server and client
    int SERVER_PORT = atoi(argv[1]);
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

    //In this initial version the server will only echo the message received by its only client (the client will receive it's own message)
    //Client sockaddr creation
    sockaddr_in client_addr;
    //Accept
    int client_fd = accept(server_fd,(struct sockaddr *)&client_addr,(socklen_t * )sizeof(client_addr));

    if(client_fd == -1){
        cerr << "Socket accepting error" << endl;
        cerr << "Error: "<< errno << endl;
        //We close the socket
        close(server_fd);
        exit(1);
    }

    //Message buffer
    char* buffer;
    int recvBytes = 0;
    int sendBytes = 0;
    bool end = false;
    while(!end){
        //receive the client's message
        recvBytes = recv(client_fd,buffer,MESSAGE_SIZE,0);

        if(recvBytes == -1){
            cerr << "Failed attempt at receiving the client's message" << endl;
            cerr << "Error: "<< errno << endl;
            //We close both client and server sockets
            close(client_fd);
            close(server_fd);
            exit(1);
        }

        cout << "Message received" << endl;

        if(buffer == END_MSSG){
            end = true;
        }
        else{
            cout << "Client's message: " << buffer << endl;

            //We send the client's message to the client
            sendBytes = send(client_fd,buffer,sizeof(buffer),0);

            if(sendBytes == -1){
                cerr << "Failed attempt at sending the client's message" << endl;
                cerr << "Error: "<< errno << endl;
                //We close both client and server sockets
                close(client_fd);
                close(server_fd);
                exit(1);
            }
        }
    }
    //Once the communication is finished we close the client socket
    cerr << "Closing client: " << client_fd << endl;
    int error_code = close(client_fd);
    if(error_code == -1){
        cerr << "Failed attempt at closing the client's socket" << endl;
    }
    //We close the server socket 
    error_code = close(server_fd);
    if(error_code == -1){
        cerr << "Failed attempt at closing the server's socket" << endl;
    }

    return error_code;
}
