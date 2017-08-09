//used http://gnosis.cx/publish/programming/sockets.html as a reference

#include "DataListener.hpp"

using namespace std;

extern void Die(string mess);

int main(int argc, char *argv[]) {

    int serversock, clientsock;
    struct sockaddr_in echoserver, echoclient;

    if (argc != 2) {
        fprintf(stderr, "USAGE: echoserver <port>\n");
        exit(1);
    }
    /* Create the TCP socket */
    if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        Die((char*)"Failed to create socket");
    }
    /* Construct the server sockaddr_in structure */
    memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
    echoserver.sin_family = AF_INET;                  /* Internet/IP */
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
    echoserver.sin_port = htons(atoi(argv[1]));       /* server port */

    /* Bind the server socket */
    if (bind(serversock, (struct sockaddr *) &echoserver, sizeof(echoserver)) < 0) {
        Die((char*)"Failed to bind the server socket");
    }
    /* Listen on the server socket */
    if (listen(serversock, MAXPENDING) < 0) {
        
    }

    /* Run until cancelled */
    while (1) {
        unsigned int clientlen = sizeof(echoclient);
        /* Wait for client connection */
        if ((clientsock = accept(serversock, (struct sockaddr *) &echoclient, &clientlen)) < 0) {
            Die((char*)"Failed to accept client connection");
        }
        fprintf(stdout, "Client connected: %s\n", inet_ntoa(echoclient.sin_addr));
        HandleClient(clientsock);
    }


    
    return 0;
}

void HandleClient(int sock) {
    char buffer[BUFFSIZE];
    int received = -1;

    char compare[] = "potato\0";

    /* Receive message */
    if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
        Die((char*)"Failed to receive initial bytes from client");
    }
    /* Send bytes and check for more incoming data in loop */
    while (received > 0) {
        cout << received << endl;
        if (received == sizeof(compare)) {
            for (int i = 0; i < received; i++) {
                if (buffer[i] != compare[i]) {
                    cout << "Doesn't match" << endl;
                    break;
                }
            }
            cout << "you got it right" << endl;
        }
        else {
            cout << "wrong length" << endl;
        }

        
        
        cout << endl;

        if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
            Die((char*)"Failed to receive additional bytes from client");
        }


        
        // This is the sample stuff

        /* Send back received data */
        //if (send(sock, buffer, received, 0) != received) {
        //Die((char*)"Failed to send bytes to client");
        //}
        /* Check for more data */
        //if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
        //Die((char*)"Failed to receive additional bytes from client");
        //}
    }
    close(sock);
}