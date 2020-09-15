#include <stdio.h> 
#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 4455
#define MAX 80 

static int count = 1;
void process(int sockfd) 
{ 
     
	for (;;) { 
         off_t fsize;
		int buff_size=10;
		char *cmd,*file_name;
		
		int n; 
		char buff[MAX];
		n = 0; 
		bzero(buff, sizeof(buff));
		
		printf("\nEnter a command:\n1. get filename\n2. put filename\n3. quit\n");
		printf("\nEnter your command: "); 
		char input[256];
		while ((input[n++] = getchar()) != '\n'); 
		input[strlen(input)] = '\0';
		
		bzero(buff, sizeof(buff));
		int i=0;
		for(i=0;i<strlen(input);i++){
			buff[i] = input[i];
		}
		buff[strlen(buff)]='\0';
		
		cmd = strtok(buff," ");
		file_name = strtok(NULL, " ");
		
		
		//if quit command
		if(!strncmp(cmd, "quit", 4)){
			write(sockfd, "quit\nexit\ndsd\0", sizeof("quit\nexit\0")+1);
			write(1, "\nBye Bye...\n", sizeof("\nBye Bye...\n"));
			close(sockfd);
			exit(0);
			
		}//end of exit command
		
		
		
		else if(!strncmp(cmd, "put", 3)){//if Put command
			//If user does not pass file name
			if(file_name == NULL) {
				printf("\nPlease specify file name!!\n");
				continue;
			}
			
			file_name[strlen(file_name)-1] = '\0';
			//opening and copying content file in buffer
			int fd = open(file_name, O_RDONLY, 0644);
			
			//cheking for error while opening
			if(fd < 0 ){
				perror("File error");
			}//writing into file
			else{
				fsize = lseek(fd, 0, SEEK_END);
				char *content = (char *) malloc(fsize);
				char contentBuff[strlen(buff)+ fsize];
				
				lseek(fd, 0, SEEK_SET);	
				read(fd, content, fsize);
				
				//Copying user inout and file content in contentBUFF
				strcpy(contentBuff, cmd);
				strcat(contentBuff, "\n");
				strcat(contentBuff, file_name);
				strcat(contentBuff, "\n");
				strcat(contentBuff, content);
				contentBuff[strlen(contentBuff)] = '\0';
				
				//write(1, contentBuff, strlen(contentBuff));
				write(sockfd, contentBuff, strlen(contentBuff)+1);
				close(fd);
				
				bzero(content, strlen(content));
				bzero(buff, strlen(buff));
				
				// Printing the received message from server 
				while(read(sockfd, buff, sizeof(char))){
					if(buff[0] == '\0'){
						break;
					}
					write(1, buff, sizeof(char));
				}
				
				 
			}
			
			
			
		}//end of put command
		
		else if(!strncmp(cmd, "get", 3)){//if Get command

			//If user does not pass file name
			if(file_name == NULL) {
				printf("\nPlease specify file name!!\n");
				continue;
			}
			
			file_name[strlen(file_name)-1] = '\0';
			
			
			char contentBuff[strlen(cmd)+ strlen(file_name)+2];
			
			//Copying user input and file content in contentBUFF
			strcpy(contentBuff, cmd);
			strcat(contentBuff, "\n");
			strcat(contentBuff, file_name);
			contentBuff[strlen(contentBuff)] = '\0';
			
			//wrting to server socket
			write(sockfd, contentBuff, strlen(contentBuff)+1);
			
			bzero(contentBuff, strlen(contentBuff));
			bzero(buff, strlen(buff));
			
			//opening and copying content file in buffer
			int fd = open("downloaded_file_from_server.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
			
			//cheking for error while opening
			if(fd < 0 ){
				perror("File error");
			}//writing into file
			else{
				
				// Printing the received message from server 
			while(1){
				
				read(sockfd, buff, sizeof(char));
				if(buff[0] == '\0'){
					write(fd, "\0", sizeof(char));
					close(fd);
					write(1, "\nFile downloaded from Server successfully...\n", sizeof("\nFile downloaded from Server successfully...\n"));
					break;
				}
				write(fd, buff, sizeof(char));
			}
				
			}
			
		}//if unknown command
		else{
			printf("\nInvalid command!!\n");
			continue;
		}	
		
    } 
} 
  
int main() 
{ 
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    // function for chat 
    process(sockfd); 
  
    // close the socket 
    close(sockfd); 
} 