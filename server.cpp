// UDP Pinger

// Must have this server running before you can run the UDP Pinger Client code

#include <iostream>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <time.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT	 12000



int main() { 
	int sockfd, n;
	socklen_t len;
	char buffer[1024];
	char *response = "pong";
	struct sockaddr_in servaddr, cliaddr; 
	
	// Here we create the Socket, sockfd: Socket decriptor, an integer
	// new socket(Domain, Type, Protocol), where arguments-
	// Domain : AF_INET(IPv4 protocol), AF_INET6(IPv6 protocol)
	// Type : SOCK_DGRAM (for UDP), SOCK_STREAM(TCP)
	// Protocol: 0 (default Protocol val)
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (sockfd < 0) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    }   
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Fill server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; // localhost
	servaddr.sin_port = htons(PORT); // port number
	
	// Here we BIND the new created socket object i.e. sockfd with the server address
	// bind(socketobj, servaddr, servaddrlen), where arguments-
	// socketobj: sockfd
	// servaddr: server address for the sockfd to be binded to
	// servaddrlen: server address structure size
	int return_val = bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
	if(return_val < 0){
		perror("bind failed"); 
        exit(EXIT_FAILURE);
	}

	
	// random generator
	srand(time(0));

	while (1) {
		// RECEIVE the client packet along with the address it is coming from
		// Arguments :
		// sockfd – newly created and binded socket object or say File descriptor of socket
		// buffer – Application buffer in which to receive the data from client
		// len – Size of application buffer
		// flags – to modify socket behaviour
		// cliaddr – Structure containing the client address is returned
		// len – Variable in which size of cliaddr structure is returned
		n = recvfrom(sockfd, (char *)buffer, sizeof(buffer), 
			MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);

		buffer[n] = '\0';
		printf("\n- - - - - * * * * * - - - - -\n");
		printf("Message received from client : %s\n", buffer); 

		//If a random number in the range of 0 to 10 is less than 4,
		//we consider the packet lost and do not respond
		if (rand()%10 < 4) continue;
		
		//Otherwise, the server responds
		sendto(sockfd, response, strlen(response), 
			0, (const struct sockaddr *) &cliaddr, len);
		printf("Response successfully sent to the client.\n");
	}
	return 0; 
} 
