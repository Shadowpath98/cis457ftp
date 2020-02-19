#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

char args[3][50];
int datacount=0;

void parse(char* str){
  int j=0;
  int i=0;
  datacount=0;

  for(i=0;i<=strlen(str);i++){
    if(str[i]==' '){
      args[datacount][j]='\0';
      datacount++;
      j=0;
    }
    else{
      args[datacount][j]=str[i];
      j++;
    }
  }
      datacount++;
}


int main(int argc, char *argv[])
{
  int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    char input[256];


    int run = 1;
    while(run){      
      int needconnection = 1;
      while(needconnection){
	printf("Type CONNECT 'servername' 'serverport' to connect to a server.\n");
	fgets(input,255,stdin);
	input[strlen(input) - 1] = '\0';
	parse(input);

	if(strcmp(args[0],"CONNECT")==0 && datacount== 3){
	  portno = atoi(args[2]);
	  sockfd = socket(AF_INET, SOCK_STREAM, 0);
	  if (sockfd < 0) 
	    perror("ERROR opening socket");
	  server = gethostbyname(args[1]);
	  if (server == NULL) 
	    fprintf(stderr,"ERROR, no such host\n");
	 
	  bzero((char *) &serv_addr, sizeof(serv_addr));
	  serv_addr.sin_family = AF_INET;
	  bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	  serv_addr.sin_port = htons(portno);
	  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
	    perror("ERROR connecting");
	  else needconnection=0;
	}
	else printf("Incorrect input\n");
 
      }

      printf("Connection has been established.\n");
      int inconnection=1;
      while(inconnection){
      
	printf("Please input command: ");
	fgets(input,255,stdin);
	input[strlen(input) - 1] = '\0';
	parse(input);

	if(strcmp(args[0],"LIST")==0 && datacount== 1){
		printf("Files in current directory of server:\n");

		n = write(sockfd,"LIST",strlen("LIST"));
    		if (n < 0) 
         		perror("ERROR writing to socket");

		bzero(buffer,256);
		n = read(sockfd,buffer,1000);
		if (n < 0) 
		  perror("ERROR reading from socket");
		printf("%s\n",buffer);
	}
	else if(strcmp(args[0],"RETRIEVE")==0 && datacount== 2){
		printf("Get file from server\n");
	}
	else if(strcmp(args[0],"STORE")==0 && datacount== 2){
		printf("Send file to server to store.\n");

	}
	else if(strcmp(args[0],"QUIT")==0 && datacount== 1){
		printf("Close Socket Connection.\n");
		n = write(sockfd,"QUIT",strlen("QUIT"));
    		if (n < 0) 
         		perror("ERROR writing to socket");
		//run=0;
		inconnection=0;
		close(sockfd);
	}
	else printf("Incorrect input\n");


	//bzero(buffer,256);
	//fgets(buffer,255,stdin);
	//n = write(sockfd,buffer,strlen(buffer));
	//if (n < 0) 
	//  perror("ERROR writing to socket");
	//bzero(buffer,256);
	//n = read(sockfd,buffer,255);
	//if (n < 0) 
	//  perror("ERROR reading from socket");
	//printf("%s\n",buffer);
      }
      //close(sockfd);
    }
    
    return 0;
}
