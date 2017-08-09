#ifndef Data_Listener_hpp
#define Data_Listener_hpp

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>

#define MAXPENDING 5    /* Max connection requests */
#define BUFFSIZE 1024
void Die(char *mess) { perror(mess); exit(1); }

void HandleClient(int sock);

#endif