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


char ** split(char * sourse,const char *d){
        char ** str_arr = malloc(10*sizeof(char));
        char * p = strtok(sourse,d);
        int i = 0;
        while(p)
        {
               // printf("%s\n",p);
                str_arr[i] = malloc(sizeof(p));
                strcpy(str_arr[i],p);
                p = strtok(NULL,d);
                i++;
        }
        return str_arr;
}


int main()
{

	int server_sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int reuse = 0;
	setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
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
        //char ** requestArr = malloc(10*sizeof(char));
		char * requestHeader = malloc(1024);
		requestCacheFile = fopen("requestCacheFile","r");
        	fgets(requestHeader,1024,requestCacheFile);
	//请求头分割
		char ** requestArr = split(requestHeader," ");	
		strcpy(REQUEST_METHON,requestArr[0]);
		free(requestArr[0]);
		strcpy(SERVER_PROTOCOL,requestArr[2]);
		free(requestArr[2]);
	//请求url路径分割	
//	char ** url_arr = malloc(10 * sizeof(char));
		char ** url_arr = split(requestArr[1],"?");
		free(requestArr[1]);
		printf("%s\n",url_arr[0]);
		//strcpy(PATH_INFO,url_arr[0]);
		//strcpy(QUERY_STRING,url_arr[1]);
		free(requestArr);	
        	char ** request_temp = NULL;      
        	while(fgets(requestHeader,1024,requestCacheFile)){
			//request_temp = split(requestHeader,":");
		//	if(strcmp(request_temp[0],"Host")){
		//		strcpy(REMOTE_HOST,request_temp[1]);
		//	}
			printf("%s\n",requestHeader);
			free(request_temp);
        	}
	//响应请求
		fclose(requestCacheFile);

		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"%s\n%s\n%s\n\r\n\r\n%s","HTTP/1.1 200 OK","Content-Type:text/html; charset=UTF-8","Server:zginx",REQUEST_METHON);
		send(conn,buffer,1024,0);
		close(conn);
	}
	close(server_sockfd);
	return 0;
}
