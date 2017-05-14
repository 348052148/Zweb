#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#define PORT 8881
#define QUEUE 20
#define BUFFER_SIZE 1024


int main()
{

	int server_sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1){
		perror("bind error");
		exit(1);
	}

	if(listen(server_sockfd,QUEUE)==-1){
		perror("listen error");
		exit(1);
	}

	char buffer[BUFFER_SIZE];
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	int conn = 0;
	while(1){
        char * SERVER_NAME = malloc(1024);
        char * SERVER_PROTOCOL = malloc(1024);
        char * SERVER_PORT = malloc(10);
        char * REMOTE_HOST = malloc(10);
        char * REQUEST_METHON = malloc(10);
        char * PATH_INFO = malloc(1024);
        char * HTTP_ACCEPT = malloc(1024);
        char * HTTP_USER_AGENT = malloc(1024);
        char * GETWAY_INTERFACE = "FGI/1.1\0"; 
        char * PATH_TRANSLATED = malloc(1024);
        char * SCRIPT_NAME = malloc(1024);
        char * QUERY_STRING = malloc(1024);
        char * REMOTE_ADDR = malloc(1024);
        char * REMOTE_USER = malloc(1024);
        char * CONTENT_TYPE = malloc(1024);
        char * CONTENT_LENGTH = malloc(1024);

		conn = accept(server_sockfd,(struct sockaddr *)&client_addr,&length);
		if(conn < 0){
			perror("connect error");
			exit(1);
		}


		memset(buffer,0,sizeof(buffer));
		int len = recv(conn,buffer,sizeof(buffer),0);
		//parse
		FILE * requestCacheFile = fopen("requestCacheFile","w+");
		fwrite(buffer,BUFFER_SIZE,1,requestCacheFile);
		fclose(requestCacheFile);
		//read
        char * requestHeader = malloc(1024);
        char * requestHeader1 = malloc(1024);
		requestCacheFile = fopen("requestCacheFile","r");
        fgets(requestHeader,1024,requestCacheFile);

        char * requestKey = malloc(1024);
        char * requestVal = malloc(1024);        
        while(fgets(requestHeader,1024,requestCacheFile)){
            //sscanf(requestHeader,"%s %s %s\n",REQUEST_METHON,PATH_INFO,SERVER_PROTOCOL); 
            sscanf(requestHeader,"%s:%s",requestKey,requestVal);
            if(strcmp(requestKey,"Host")==0){
                printf("%s",requestVal);
            }else{
                printf("%s","NOT");
            }
            
            printf("%s",REMOTE_HOST);
            break;
        }
		fclose(requestCacheFile);

		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"%s\n%s\n%s\n\r\n\r\n%s","HTTP/1.1 200 OK","Content-Type:text/html; charset=UTF-8","Server:zginx",REMOTE_HOST);
		send(conn,buffer,1024,0);
		close(conn);
	}
	close(server_sockfd);
	return 0;
}
