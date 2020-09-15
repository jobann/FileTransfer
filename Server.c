
#include <stdio.h> 
#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>

#define PORT 4455
#define MAX 80 

// Put file

static int count =1;
void putfile(char *file_name){
	printf("Putting %s\n",file_name);
}

// Function designed for chat between client and server. 
void serviceClient(int sockfd) 
{ 
	char buff[50], cmd[5], file_name[50];
	for(;;){
		bzero(buff, sizeof(buff));
		int i=0;
		while(read(sockfd, buff, sizeof(char))){
			if(buff[0] == '\n'){
				cmd[i] = '\0';
				i=0;
				break;
			}
			cmd[i++] = buff[0];
		}
		i=0;
		while(read(sockfd, buff, sizeof(char))){
			if(buff[0] == '\n' || buff[0] == '\0'){
				file_name[i] = '\0';
				break;
			}
			file_name[i++] = buff[0];
		}
		
		//write(1, cmd, sizeof(cmd));
		
		
		if(!strncmp(cmd, "quit", 4)){//If quit
			write(sockfd, cmd, sizeof(cmd));
			write(1, "\nServer Shutdown...\n", sizeof("\nServer Shutdown...\n"));
			close(sockfd);
			exit(0);
		}// end of exit command
		else if(!strncmp(cmd, "put", 3)){//if command is put
			
			//creating and writing to file
			int fd = open("downloaded_file_from_client.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
			
			if(fd < 0){
				perror("File written error");
			}else{
			
				while(read(sockfd, buff, sizeof(char))){
					if(buff[0] == '\0'){
						close(fd);
						char msg[] = "\nUploaded...\n";
						msg[strlen(msg)] = '\0';
						write(sockfd, msg, strlen(msg)+1);
						break;
					}
					write(fd, buff, sizeof(char));
				}
			
			}	
			write(1, "\nFile Saved successfully...\n", strlen("\nFile Saved successfully...\n"));	
		}// end of put command
		
		
		else if(!strncmp(cmd, "get", 3)){//if command is get
			
		write(1, "\nGET\n", sizeof("\nGET\n"));
		
		//opening and copying content file in buffer
		int fd = open(file_name, O_RDONLY, 0644);
		
		//cheking for error while opening
		if(fd < 0 ){
			write(1, "\nNo file found !!\n", strlen("\nNo file found !!\n"));
			write(sockfd, "\nNo file found !!\n", strlen("\nNo file found !!\n"));
		}
		//writing into file
		else{
			int fsize = lseek(fd, 0, SEEK_END);
			char *content = (char *) malloc(fsize+5);
			
			bzero(content, sizeof(content));
				
			lseek(fd, 0, SEEK_SET);	
			read(fd, content, fsize);
			close(fd);
			
			content[strlen(content)] = '\0';
			
			
			char msg[] = "\nUploaded...\n";
			msg[strlen(msg)] = '\0';
			write(sockfd, msg, strlen(msg)+1);	
			
			
			//write(sockfd, content, strlen(content)+1);
			
			
			write(1, "\nFile sent successfully...\n", sizeof("\nFile sent successfully...\n"));
			}
		}//end of get command
		
	}
	exit(0);
}

int main(){
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;
	
	//Creating and verifying socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		printf("Error while creating Socket...");
		exit(1);
	}else{
		printf("Socket created Successful...");
	}
	bzero(&servaddr, sizeof(servaddr));


	//Setting IP address and PORT
	servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 


	printf("local address: %s\n", inet_ntoa( servaddr.sin_addr));
	printf("local port: %d\n", (int) ntohs(servaddr.sin_port));
	
	//Binding socket with Ip address and PORT
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
		perror("Bind Error:");
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
	
	
	// Now Server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
	len = sizeof(cli); 
	
	//Accept the data packet from client and verification
	connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
	if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else{
        printf("server acccepted the client...\n"); 
		/*pid_t pid = fork();
		if(pid < 0 ){
			perror("\nFork failed");
		}else if(pid > 0){// If parent
			connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
		}else if(pid == 0){// If child
			serviceClient(connfd);
		}*/
	}
	serviceClient(connfd);
	close(sockfd);
	exit(0);

}