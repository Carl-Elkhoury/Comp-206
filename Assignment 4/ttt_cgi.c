#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
int main(void){
    char *data;
	char *input[7];
	printf("%s%c%c\n", "Content-Type:text/html;charset=iso-8859-1",13,10);
	data = getenv("QUERY_STRING");
	if(data == NULL)
		  printf("<P>Error! Error in passing data from form to script.");
	else{
		// printf("%s<br>",data);
		input[0]= strtok(data, "&");
		for(int i=1; i<7;i++){
			input[i]= strtok(NULL, "&");
		}
		for(int i=0; i<7;i++){
			strtok(input[i],"=");
			input[i]=strtok(NULL,"=");
		}
		printf("\
 		<html>\
 		<body>\
 		<form action=\"./../cgi-bin/ttt.cgi\">\
 		 		<b>Server Address: <input type=\"text\" name=\"address\" size=\"20\"value=\"%s\"><br />\
 		 		<b>Server Port: <input type=\"text\" name=\"port\" size=\"20\"value=\"%s\"><br />\
 		 		<b>Username: <input type=\"text\" name=\"username\" size=\"20\"value=\"%s\"><br />\
 		 		<b>Password: <input type=\"text\" name=\"password\" size=\"20\"value=\"%s\"><br />\
 		 		<b>Gamename: <input type=\"text\" name=\"gamename\" size=\"20\" value=\"%s\"><br />\
 		 		<b>Square: <input type=\"text\" name=\"square\" size=\"20\"value=\"%s\"><br />\
 		 		<input type=\"submit\" value=\"LOGIN\" name=\"LOGIN\">\
 		 		<input type=\"submit\" value=\"CREATE\" name=\"CREATE\">\
 		 		<input type=\"submit\" value=\"JOIN\" name=\"JOIN\">\
 		 		<input type=\"submit\" value=\"MOVE\" name=\"MOVE\">\
 		 		<input type=\"submit\" value=\"LIST\" name=\"LIST\">\
 		 		<input type=\"submit\" value=\"SHOW\" name=\"SHOW\">\
 		</form>"
,input[0],input[1],input[2],input[3],input[4],input[5]);
	char finalCommand[500];
	sprintf(finalCommand,"%s,%s,%s",input[6],input[2],input[3]);
	if(strcmp(input[6],"LOGIN")!=0 && strcmp(input[6],"LIST")!=0){
		sprintf(finalCommand,"%s,%s",finalCommand,input[4]);
		if(strcmp(input[6],"SHOW")!=0 && strcmp(input[6],"CREATE")!=0)
			sprintf(finalCommand,"%s,%s",finalCommand,input[5]);
	}
	int sock;
	struct sockaddr_in server;
	char message[200];
	char* addr = "127.0.0.1";
	unsigned short int port = 8888;

	addr=input[0];
	port=(unsigned short int)atoi(input[1]);	
	server.sin_addr.s_addr = inet_addr( input[0] );
	server.sin_family = AF_INET;
	server.sin_port = htons( port );
	
	sock = socket(AF_INET , SOCK_STREAM , 0);
		if (sock == -1)
		{
			printf("Could not create socket");
			return -1;
		}
		
		//Connect to remote server
		if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
		{
			perror("connect failed. Error");
			return 1;
		}
		
		//Send some data
		if( send(sock , finalCommand , 200, 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		
		size_t read_size;
		int bytes_read = 0;
		char incoming_msg[2000];
		char temp[2000];
		while( bytes_read < 2000 ){
			read_size = recv(sock , temp, 2000, 0);
			if(read_size <= 0){
			    puts("Server disconnected");
			    fflush(stdout);
			    close(sock);
			    return 0;
			}
			memcpy( incoming_msg+bytes_read, temp, read_size );
			bytes_read += read_size;
		}
		char *tem =incoming_msg;
		int count =0;
		while(*tem){
			if(tem[0] == '\r' || temp[0] == '\n'){	
				count++;
			}
			tem++;
		}
		char *toPrint = (char*) malloc(sizeof(char) * (strlen(incoming_msg)*20));
		tem = incoming_msg;
		count = 0;
		while(*tem){
			if(tem[0] == '\r'){	
				toPrint[count++]= '<';
				toPrint[count++]= 'b';
				toPrint[count++]= 'r';
				toPrint[count++]= '>';
				tem+=2;
			}else if(tem[0] == ' '){
				toPrint[count++]= '&';
				toPrint[count++]='n';
				toPrint[count++]='b';
				toPrint[count++]='s';
				toPrint[count++] = 'p';
				toPrint[count++]=';';
				tem++;
			}else{
				toPrint[count++]=tem[0];
				tem++;
			}
		}
		toPrint[count]='\0';
		printf( "<P>Server response: %s\n", toPrint );
		close(sock);

	printf("</body>\
		</html>");

	}
	


	return 0;
}

