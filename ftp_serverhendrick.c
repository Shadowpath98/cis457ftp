/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
   gcc server2.c -lsocket
*/

// Adding list functionality soon, establishing hendrick branch

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void dostuff(int); /* function prototype */
void fSend(int);
void error(char *msg)
{
    perror(msg);
    exit(1);
}

/*void showlist(char* list)
{
    char dirname[256];
    char dirbuffer[256]; 
    int num = 0;
    int socket;
    char *line[num];
    FILE *filelist = fopen("list.txt","r");
    if (filelist == NULL) {
        printf("No files in this directory");
    }
    while(!feof(filelist)) {
        line[num] = fgets(dirname, 255, filelist);
        num++;
    }
    fclose(filelist);
    send(socket, dirbuffer, 256, 0);
}
*/
            
int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, pid, status, run;
     struct sockaddr_in serv_addr, cli_addr;
  
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0){ 
        error("ERROR opening socket");
     }
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY; //inet_addr("192.168.110.7");
     serv_addr.sin_port = htons(6550);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) {
              error("ERROR on binding");
     }
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     run = 1;
     while (run) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) { 
             error("ERROR on accept");
         }
         pid = fork();
         if (pid < 0){
             error("ERROR on fork");
         }
         if (pid == 0)  {
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else {
             wait (&status);
             close(newsockfd);
             run = 0;
         }
     } /* end of while */
     return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
   int n;
   char buffer[256];
      
   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18);
   if (n < 0) error("ERROR writing to socket");
}

void fSend (int sock)
{
   char fName[256];
   char fBuff[256];
   FILE *fPoint;
   read(sock, fName, 256);
   if (fPoint == NULL)
   {
      printf("Error opening file.\n");
   }
   else
   {
      while(read(sock, fBuff, 256) > 0)
      {
         fprintf(fPoint, "%s", fBuff);
      }
      printf("File received.");
   }
   fclose (fPoint);
}

void list(int sock){
   char files[1000];
   int n;
   DIR *d;
   struct dirent *dir;
   d = opendir(".");
   if(d){
	    while((dir = readdir(d)) != NULL){
		    strcat(files,dir->d_name);
		    strcat(files,"\n");
	    }
	closedir(d);
   }
   
   n = write(sock,files,strlen(files));
   if (n < 0) error("ERROR writing to socket");
}
