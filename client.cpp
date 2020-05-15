#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include<iostream>
#include <chrono>
#include <sys/time.h>
#include <ctime>
#include <cmath>

using namespace std;
  
#define PORT     12000 
#define MAXLINE 1024 
  
// Driver code 
int main() { 
    int sockfd; 
    char buffer[MAXLINE]; 
    char *hello = "ping"; 
    struct sockaddr_in     servaddr; 

    //Initializing server addresses
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    
    int n; //will hold the int return value from method recvfrom() or say response from server
    socklen_t len;
    
    int total_count = 10; // total pings to be made
    int lost_count = 0; // Lost packets count tracker
    int found_count = 0; // Actual packets received tracker

    // String array used for index based printing of the time
    string count[10] = {"first","second","third","fourth","fifth", "sixth", "seventh","eighth", "ninth","tenth"};

    //For loop to send 10 pings
    for(int i = 0; i < total_count; i++){

        // Creating UDP socket file descriptor; SOCK_DGRAM used for UDP packets.
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        }   

        //timeout definiton and value set to 1
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

        //assigning timeout values to setsockopt()
		setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
        setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

        //getting time when client pinged the server
		struct timeval sent_time;
        gettimeofday(&sent_time, NULL);
        int sent_milli = sent_time.tv_usec / 1000;
        char sent_buffer[80];
        strftime(sent_buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&sent_time.tv_sec));
        char currentTime2[84] = "";
        sprintf(currentTime2, "%s:%03d", sent_buffer, sent_milli);

        // CLient now send a ping to the destined server
        sendto(sockfd, (const char *)hello, strlen(hello), 
            0, (const struct sockaddr *) &servaddr,  
                sizeof(servaddr)); 
        cout << "\n- - - - - * * * * * - - - - -" << endl;
        cout << "Pinged the server the '"<< count[i] << "' time" << endl;
        cout << "Waiting for response..."<< endl;

        // receives response from server : 'int n' 
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                    MSG_WAITALL, (struct sockaddr *) &servaddr, 
                    &len);

        //getting time when client receives response "pong" from server
		struct timeval recv_time;
        gettimeofday(&recv_time, NULL);
        int recv_milli = recv_time.tv_usec / 1000;
        char recv_buffer[80];
        strftime(recv_buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&recv_time.tv_sec));
        char currentTime[84] = "";
        sprintf(currentTime, "%s:%03d", recv_buffer, recv_milli);

        //Round Trip Time (RTT) calculation in microseconds
		int microsec = (recv_time.tv_sec - sent_time.tv_sec) * pow(10,6) + ((int)recv_time.tv_usec - (int)sent_time.tv_usec);
		int millisec = microsec/pow(10,3);
		int seconds = millisec /pow(10,3);

		//if method recvfrom() returns -1 or say response is -1, the packet is said to be lost!
		if(n == -1) {
		    cout << "Uh oh! Response not received in the required time frame! "<< endl ;
            cout << "Packet might have been lost" << endl;
            lost_count += 1;
		}

		//else, packet received successfully; print the rtt and response received
		else {
            buffer[n] = '\0'; 
            cout << "Response received from Server: " << buffer << endl;
		    cout << "Round Trip Time Delay (RTT): "  << microsec << " microseconds" << endl;
            found_count += 1;
		}

        // closes the socket file descriptor
        close(sockfd);
    }

    // Analysis of Pings and packets , sent, received , lost
    cout << "\nNumber of pings sent: " << total_count << endl;
    cout << "Actual number of ping back received: " << found_count << endl;
    cout << "Number of Packets lost: " << lost_count << endl << "\n";
    close(sockfd); 
    return 0; 
}