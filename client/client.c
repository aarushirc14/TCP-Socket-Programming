/*
    Author: Aarushi Roy Choudhury
    Version: 5.FINAL
    Filename: client.c
*/

/* The following references were used in this program. 
    https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
    https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
    https://youtu.be/LtXEMwSG5-8
    
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX 1024

int main(int argc, char *argv[]) {

    int server_socket;
    int port_number;

    int passcode;
    int id;

    //for writing the data recieved from the server
    char data_buffer[MAX];
    int bytes_received;


    printf("======================= Client Terminal =======================\n");

    //print error if command line arg not provided
    if (argc < 2) {
    printf( "You must enter a port number. Terminating program.\n");
    exit(1);
    }

    //create client socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket <0){
        printf("Socket failed \n");
        exit(1);
    }
    
    //set up address
    struct sockaddr_in server_address;
    port_number = atoi(argv[1]); //convert command line arg port num to int
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number); //use port number from command line arg
    server_address.sin_addr.s_addr= INADDR_ANY;


    //create connection to address
    int connection_status= connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
   
    if(connection_status == -1){
    printf("Connection failed \n");
    exit(1);
    }
    
    

    //buff will store user-entered ID
    char buff[MAX];
    int n;
        
    bzero(buff, sizeof(buff));
    printf("Enter your ID: ");
    
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
    ;
    id = atoi(buff); //user input goes into ID - convert to int
  
    write(client_socket, buff, sizeof(buff)); //send ID to server

   

    char date_from_server[MAX];
    recv(client_socket, &date_from_server, sizeof(date_from_server),0); //get the date from server

   
    time_t rawtime;
    struct tm *time_info;
    time(&rawtime);
    time_info = localtime(&rawtime);
    int sec = time_info->tm_sec;
    strftime(date_from_server, sizeof(date_from_server), "%c", time_info);

   

    //print out server resposne
    printf("Date received from server: %s\n", date_from_server);

    printf("Seconds is %02d\n", sec);

   
    int id_last_four = id % 10000; //get last 4 digits of the ID
    passcode = sec + id_last_four; //client calculates passocode
    printf("Passcode sent to server: %04d\n", passcode);
    send(client_socket, &passcode, sizeof(passcode), 0);  //send passcode to server


    FILE *fp = fopen("received_data.txt", "w"); //write data into a file called "received_data.txt"
    if (fp == NULL) {
        perror("Error opening file \n");
        return -1;
    }
    // print the contents of the file sent by the server in "received_data.txt"
    // go line by line
    while ((bytes_received = recv(client_socket, data_buffer, MAX, 0)) > 0) {
    data_buffer[bytes_received] = '\0';
    fprintf(fp, "%s", data_buffer);
    printf("Bytes recieved from the server: %d\n", bytes_received);
    }
    fclose(fp);

    close(server_socket);
    close(client_socket);

    return 0;
}