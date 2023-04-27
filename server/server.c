/*
    Author: Aarushi Roy Choudhury
    Version: 5.FINAL
    Filename: server.c
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
#include <time.h>

#define MAX 1024

int main(int argc, char *argv[]) {

    printf("====================== Server Terminal =======================\n");

    int server_socket;
    int client_socket;
    struct sockaddr_in server_address;
    int client_address;
    int port_number; 

    int passcode;
    int id;
    int passcode_received;
    
    FILE *fp;
    char read_buffer[MAX];
    int bytes_sent;

    //print error if command line arg not provided
    if (argc < 2) {
        printf ("You must enter a port number. Terminating program.\n");
        exit(1);
    }

    //create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Socket failed \n");
        exit(1);
    }

    //set up address
    port_number = atoi(argv[1]);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    // bind to address
    int bind_status = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (bind_status < 0){
        printf("Bind failed\n");
        exit(1);
    }
        //infinite loop for server to keep listening for requests from client
        while (1){

            //listen    
            int listening_status = listen(server_socket, 1);
            if (listening_status < 0){
                printf("Listening failed\n");
                exit(1);
            }

            //accept
            int accept_status = client_socket = accept(server_socket, NULL, NULL);
            if (accept_status < 0){
                printf("Accepting failed\n");
                exit(1);
            }

            char buff[MAX]; //buff will store ID from client
            int n;
        
            bzero(buff, MAX);
             
            read(client_socket, buff, sizeof(buff)); // read the message from client and copy it in buffer
            
            id = atoi(buff); //convert ID to int
           
            printf("ID received from client: %s", buff);  //print ID
                
            char date[MAX]; //will store date
            bzero(date, MAX);

           

            time_t rawtime;
            struct tm *time_info;
            time(&rawtime);
            time_info = localtime(&rawtime);
            int sec = time_info->tm_sec; // get seconds
            snprintf(date, sizeof(date), "%.30s", ctime(&rawtime));

          

            printf("Date: %s", date); // print date
            printf("Seconds is %02d\n", sec);


            //send date to client
            write(client_socket, date, strlen(date));

            int id_last_four = id % 10000;
            passcode = sec + id_last_four; //server calculates passocode
            
            //server gets passcode from client
            recv(client_socket, &passcode_received, sizeof(passcode_received), 0); 

            printf("Passcode received from client: %d\n", passcode_received);

            //if the calculated and recieved passcodes match (they should!)
            //then open data.txt
            if(passcode==passcode_received){

                fp = fopen("data.txt", "r");
                if (fp == NULL) {
                    printf("Error opening file.\n");
                    return 1;
                }
                //read data.txt
                while (fgets(read_buffer, MAX, fp) != NULL) {

                //send data to client
                bytes_sent = send(client_socket, read_buffer, strlen(read_buffer), 0);
                if (bytes_sent == -1) {
                    printf("Error sending data.\n");
                    break;
                }
                }
                fclose(fp);

            }
            else{
                printf("Client and Server passcodes did NOT match.\n");
                exit(1);
            }

            close(client_socket);
            close(server_socket);

            return 0;
        }


}

