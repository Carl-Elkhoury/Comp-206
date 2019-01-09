/*********************************************************
* FILE: ttt_server.c
* 
* A starter code for your answer to A4 Q1. Most of the
* networking is already done for you, and we even gave
* suggested data types (structs and linked lists of these)
* to represent users and tic-tac-toe games. You must just
* figure out how to fill in the various functions required
* to make the games happen.
*
* Good luck, and rember to ask questions quickly if you get 
* stuck. My Courses Dicussions is the first place to try, 
* then office hours.
*
* AUTHOR: YOU!
* DATE: Before Dec 3rd
***********************************************************/

#include <stdio.h>
#include <string.h>	
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <unistd.h>	
#include <stdlib.h>
#include <time.h>

struct USER{
	char username[100];
	char password[100];
	struct USER *next;
};

enum GAME_STATE{ 
	CREATOR_WON=-2,
	IN_PROGRESS_CREATOR_NEXT=-1,
	DRAW=0,
	IN_PROGRESS_CHALLENGER_NEXT=1,
	CHALLENGER_WON=2
};

struct GAME{
	char gamename[100];
	struct USER *creator;
	struct USER *challenger;
	enum GAME_STATE state;
	char ttt[3][3];
	struct GAME *next;
};

struct USER *user_list_head = NULL;
struct GAME *game_list_head = NULL;

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , read_size;
	struct sockaddr_in server , client;
	char client_message[2000];

	unsigned short int port = 8888;

	if( argc > 1 )
		port = (unsigned short int)atoi(argv[1]);
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port );
	
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("bind failed. Error");
		return 1;
	}

	listen(socket_desc , 3);

	printf( "Game server ready on port %d.\n", port );

	while( 1 ){
		c = sizeof(struct sockaddr_in);

		//accept connection from an incoming client
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0)
		{
			perror("accept failed");
			return 1;
		}

		char temp[200];
		memset(client_message, '\0', 200);
		int bytes_read = 0;
		while( bytes_read < 200 ){
			read_size = recv(client_sock , temp , 200, 0);
			if(read_size <= 0){
				puts("Client disconnected");
				fflush(stdout);
				close(client_sock);
				close(socket_desc);
				return 0;
			}
			memcpy( client_message+bytes_read, temp, read_size );
			bytes_read += read_size;
		}

	  	char response[2000];
	  	response[0] = '\0';
	  	char* command = strtok( client_message, "," );
	  	char *username = strtok( NULL, "," );
	  	char *password = strtok( NULL, ",");

		if( command == NULL || username == NULL || password == NULL ){
			sprintf( response, "MUST ENTER A VALID COMMAND WITH ARGUMENTS FROM THE LIST:\n" );
			sprintf( response+strlen(response), "LOGIN,USER,PASS\n" );
			sprintf( response+strlen(response), "CREATE,USER,PASS,GAMENAME\n" );
			sprintf( response+strlen(response), "JOIN,USER,PASS,GAMENAME,SQUARE\n" );
			sprintf( response+strlen(response), "MOVE,USER,PASS,GAMENAME,SQUARE\n" );
			sprintf( response+strlen(response), "LIST,USER,PASS\n" );
			sprintf( response+strlen(response), "SHOW,USER,PASS,GAMENAME\n" );
			write(client_sock , response , 2000);  
		  	close(client_sock);
			continue;
		}

		if( strcmp( command, "LOGIN" ) == 0 ){
			struct USER *linkUser = user_list_head;
			int found = 0; 
			while(linkUser != NULL){
				if(strcmp(linkUser->username, username) == 0){
					if(strcmp(linkUser->password, password) == 0){
						strcpy( response,"You are logged in");
					}else{
						strcpy( response,"Wrong password");
					}
					found=1;
					break;
				}
				linkUser = linkUser->next;
			}
			if(found==0){
				linkUser =(struct USER*) malloc(sizeof(struct USER));
				strcpy(linkUser->username, username);
				strcpy(linkUser->password, password);
				linkUser->next = user_list_head;
				user_list_head = linkUser;
				strcpy( response, "NEW USER CREATED OK");
			}
	  	}
	  	else if( strcmp( command, "CREATE" ) == 0 ){
	  		char *game_name = strtok( NULL, ",");

			if( game_name == NULL ){
				sprintf( response, "CREATE COMMAND MUST BE CALLED AS FOLLOWS:\n" );
				sprintf( response+strlen(response), "CREATE,USER,PASS,GAMENAME\n" );
				write(client_sock , response , 2000);  
		  		close(client_sock);
		  		continue;
			}
			struct GAME* gim =game_list_head;
			while(gim != NULL){
				if(strcmp(gim->gamename, game_name)==0){
					sprintf( response, "THERE IS ALREADY A GAME WITH THAT NAME\n" );
					write(client_sock , response , 2000);  
		  			close(client_sock);
		  			continue;
				}
				gim = gim->next;
			}
			struct USER* current = user_list_head;
			int flag=0;
			struct USER* creator = NULL;
			while(current != NULL){
				if(strcmp(current->username,username) == 0){
					if(strcmp(current->password,password) == 0){
						creator=current;
						flag =1;
					}else{
						sprintf(response, "WRONG PASSWORD FOR USER %s \r\n",username);
						write(client_sock , response , 2000);
						close(client_sock);
						continue;
					}
					
					break;
				}
				current = current->next;
			}
			if(flag==0){
				sprintf(response, "USERNAME %s NOT FOUND \r\n", username);
				write(client_sock , response , 2000);  
				close(client_sock);
				continue;
			}

			struct GAME *game = (struct GAME*)malloc( sizeof(struct GAME) );
			strcpy( game->gamename, game_name );
			sprintf(response , "GAME %s CREATED WAITING FOR OPPONENT TO JOIN\r\n",game_name);
			game->creator = creator;
			game->next = game_list_head;
			game->state = 1;
			game_list_head = game;
			for( int row=0; row<3; row++ )
				for( int col=0; col<3; col++ )
					game->ttt[row][col] = ' ';
			
			sprintf( response, "%sa  %c | %c | %c \r\n",response,  game->ttt[0][0],  game->ttt[0][1],  game->ttt[0][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sb  %c | %c | %c \r\n", response, game->ttt[1][0],  game->ttt[1][1],  game->ttt[1][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sc  %c | %c | %c \r\n", response, game->ttt[2][0],  game->ttt[2][1],  game->ttt[2][2]);
			sprintf( response, "%s\r\n", response );
			sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );
		}
		else if( strcmp( command, "JOIN" ) == 0 ){
			char* gamename = strtok(NULL, ",");
			struct USER* current = user_list_head;
			int flag=0;
			struct USER*  challenger = NULL;
			while(current != NULL){
				if(strcmp(current->username,username) == 0){
					if(strcmp(current->password,password) == 0){
						challenger=current;
						flag =1;
					}else{
						sprintf(response, "WRONG PASSWORD FOR USER %s \r\n",username);
						write(client_sock , response , 2000);
						close(client_sock);
						continue;
					}
					
					break;
				}
				current = current->next;
			}
			if(flag==0){
				sprintf(response, "USERNAME %s NOT FOUND \r\n", username);
				write(client_sock , response , 2000);  
				close(client_sock);
				continue;
			}

			struct GAME* currentGame = game_list_head;

			while(currentGame != NULL){
				if(strcmp(currentGame->gamename,gamename)==0)
					break;
				currentGame=currentGame->next;
			}
			if(currentGame == NULL){
				sprintf(response,"THE GAME DOESN'T EXIST");
				write(client_sock , response , 2000);  
				close(client_sock);
				continue;
			}
			if(currentGame->challenger != NULL){
				sprintf(response,"THERE IS ALREADY A CHALLENGER");
				write(client_sock , response , 2000);  
				close(client_sock);
				continue;
			}
			currentGame->challenger =  challenger;
			sprintf(response,"%s GAME %s BETWEEN %s AND %s \r\n",response,currentGame->gamename, currentGame->creator->username, currentGame->challenger->username);
			char *input = strtok( NULL, ",");
			if(input[0]-'a' >2 || input[1] - '1' >2){
				sprintf(response,"WRONG INPUT %i for %c and %i for %c ", input[0]-'a',input[0],input[1] - '1',input[1]);
				write(client_sock , response , 2000);  
				close(client_sock);
				continue;
			}
			currentGame->ttt[input[0]-'a'][input[1] - '1'] = 'x';
			currentGame->state = -1;
			struct GAME* game = currentGame;
			sprintf( response, "%sa  %c | %c | %c \r\n",response,  game->ttt[0][0],  game->ttt[0][1],  game->ttt[0][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sb  %c | %c | %c \r\n", response, game->ttt[1][0],  game->ttt[1][1],  game->ttt[1][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sc  %c | %c | %c \r\n", response, game->ttt[2][0],  game->ttt[2][1],  game->ttt[2][2]);
			sprintf( response, "%s\r\n", response );
			sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );
		}
		else if( strcmp( command, "MOVE" ) == 0 ){ 
			char* gamename = strtok(NULL, ",");
			char* input = strtok(NULL, ",");
			struct GAME* current = game_list_head;
			int flag = 0;
			while(current != NULL){
				if(strcmp(current->gamename, gamename) ==0){
					flag = 1; 
					break;
				}
				current = current->next;
			}
			if(flag == 0){
				sprintf(response ,"%s %s not found\r\n" ,response, gamename);
				write(client_sock , response , 2000);
				close(client_sock);
				continue;
			}
			flag = 0;
		if(strcmp(username,current->challenger->username) ==0 )
			flag =1;
		if(strcmp(username,current->creator->username) == 0)
			flag =-1;
		if(flag == 0){
			sprintf(response ,"%s Username %s not in the game\r\n" ,response, username);
			write(client_sock , response , 2000);
			close(client_sock);
			continue;
		}

		if(flag != current->state){
			sprintf(response ,"%s It is not your turn to play\r\n" ,response);
			write(client_sock , response , 2000);
			close(client_sock);
			continue;
		}
		if(input[0]-'a' >2 || input[1] - '1' >2 || current->ttt[input[0]-'a'][input[1] - '1'] != ' '){
			sprintf(response ,"%s Wrong input\r\n" ,response);
			write(client_sock , response , 2000);
			close(client_sock);
			continue;
		}
		if(current->state == 1){
			current->ttt[input[0]-'a'][input[1] - '1'] = 'x';
			current->state=-1;
		}else if(current->state == -1){
			current->ttt[input[0]-'a'][input[1] - '1'] = 'o';
			current->state=1;
		}else{
			sprintf(response ,"%s The game is aleady finished\r\n" ,response);
			write(client_sock , response , 2000);
			close(client_sock);
			continue;
		}
		
		char targetX =' ';
		char winningSign = ' ';
		int Xmatch =1;
		for(int i=0; i<3 ; i++){
			targetX = current->ttt[i][0];
			Xmatch = 1;
			if(targetX == ' '){
				Xmatch = 0;
				continue;
			}
			for(int j=0; j<3; j++){
				if(targetX != current->ttt[i][j]){
					Xmatch = 0;
					break;
				}
			}
			if(Xmatch){
				winningSign = targetX;
				break;
			}
		}
		if( winningSign == ' ')
			for(int i=0; i<3; i++){
				targetX = current->ttt[0][i];
				Xmatch =1;
				if(targetX == ' '){
					Xmatch=0;
					continue;
				}
				for(int j=0; j<3 ;j++){
					if(targetX != current->ttt[j][i]){
						Xmatch = 0;
						break;
					}
				}
				if(Xmatch){
					winningSign = targetX;
					break;
				}
			}
		if(winningSign == ' '){
			if(current->ttt[0][0] != ' ')
				if(current->ttt[1][1]==current->ttt[2][2] && current->ttt[0][0]==current->ttt[1][1]){
					winningSign = current->ttt[0][0];
				}
			if(current->ttt[0][2] != ' ')
				if(current->ttt[1][1]==current->ttt[0][2] && current->ttt[2][0]==current->ttt[1][1]){
					winningSign = current->ttt[0][0];
				}
		}
		int draw=0;
		if(winningSign == ' '){
			draw=1;
			for(int i=0; i<3 ; i++)
				for(int j=0; j<3; j++)
					if(current->ttt[i][j] == ' ')
						draw =0;
		}
		if(draw && winningSign == ' '){
			sprintf(response,"%s THE GAME IS A DRAW \r\n",response);
			current->state = 0;
		}else{
			if(winningSign == ' '){
				if(strcmp(username,	current->challenger->username)==0)
					sprintf(response,"%s %s TO MOVE NEXT AS o \r\n",response ,current->creator->username);
				else
					sprintf(response,"%s %s TO MOVE NEXT AS x \r\n" ,response, current->challenger->username);
			}else   if(winningSign == 'x'){
						sprintf(response, "%s %s WINS\r\n" ,response,current->challenger->username);
						current->state = 2;
					}else{
						sprintf(response, "%s %s WINS\r\n", response, current->creator->username);
						current->state = -2;
					}
		}

		struct GAME* game = current;
		sprintf( response, "%sa  %c | %c | %c \r\n",response,  game->ttt[0][0],  game->ttt[0][1],  game->ttt[0][2]);
		sprintf( response, "%s  ---|---|---\r\n", response );
		sprintf( response, "%sb  %c | %c | %c \r\n", response, game->ttt[1][0],  game->ttt[1][1],  game->ttt[1][2]);
		sprintf( response, "%s  ---|---|---\r\n", response );
		sprintf( response, "%sc  %c | %c | %c \r\n", response, game->ttt[2][0],  game->ttt[2][1],  game->ttt[2][2]);
		sprintf( response, "%s\r\n", response );
		sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );	
		
		}
		else if( strcmp( command, "LIST" ) == 0 ){ 
			struct USER* current = user_list_head;
			int flag=0;
			struct USER*  challenger = NULL;
			while(current != NULL){
				if(strcmp(current->username,username) == 0){
					if(strcmp(current->password,password) == 0){
						challenger=current;
						flag =1;
					}else{
						sprintf(response, "WRONG PASSWORD FOR USER %s \r\n",username);
						write(client_sock , response , 2000);
						close(client_sock);
						continue;
					}
					
					break;
				}
				current = current->next;
			}
			if(flag==0){
				sprintf(response, "USERNAME %s NOT FOUND \r\n", username);
				write(client_sock , response , 2000);  
				close(client_sock);
				continue;
			}
			struct GAME* game = game_list_head;
			sprintf(response,"%s LIST OF GAMES: \r\n",response);
			while(game != NULL){
				char* state[] = {"CREATOR WON","IN_PROGRESS:","DRAW","IN_PROGRESS:","CHALLENGER WON"};
				sprintf(response, "%s GAME %s: CREATED BY %s ",response,game->gamename, game->creator->username);
				if(game->challenger == NULL)
					sprintf(response,"%s CHALLENGED BY: (null). IN PROGRESS: (null) TO MOVE NEXT AS x\r\n",response);
				else{
					sprintf(response,"%s CHALLENGED BY: %s. %s", response,game->challenger->username,state[game->state+2]);
					if(game->state == 1 || game->state == -1){
						char toPlay = 'x';
						char *nextToPlay[] = {game->creator->username,game->challenger->username};
						int slot;
						if(game->state==1)
							slot=1;
						else
							slot=0;
					
						if(game->state ==-1)
							toPlay = 'o';
						
						sprintf(response,"%s %s TO MOVE NEXT AS %c \r\n", response,nextToPlay[slot], toPlay);
					}else{
						sprintf(response,"%s \r\n",response);
					}
				}
				game=game->next;
			}
		}
		else if( strcmp( command, "SHOW" ) == 0 ){
			int flag=0;
			struct USER*  challenger = NULL;
			struct USER* current =user_list_head;
			while(current != NULL){
				if(strcmp(current->username,username) == 0){
					if(strcmp(current->password,password) == 0){
						challenger=current;
						flag =1;
					}else{
						sprintf(response, "WRONG PASSWORD FOR USER %s \r\n",username);
						write(client_sock , response , 2000);
						close(client_sock);
						continue;
					}
					
					break;
				}
				current = current->next;
			}
			if(flag==0){
				sprintf(response, "USERNAME %s NOT FOUND \r\n", username);
				write(client_sock , response , 2000);  
				close(client_sock);
				continue;
			}
		
		struct GAME* game = game_list_head;
		char* gamename = strtok(NULL, ",");
		
		while(game != NULL){
			if(strcmp(game->gamename,gamename) ==0)
				break;
			game= game->next;
		}
		if(game == NULL){
			sprintf(response, "GAME NAME %s NOT FOUND",gamename);
			write(client_sock , response , 2000);  
			close(client_sock);
			continue;
		}
		char* state[] = {"CREATOR WON","IN_PROGRESS:","DRAW","IN_PROGRESS:","CHALLENGER WON"};
		sprintf(response, "%s GAME %s: CREATED BY %s ",response,game->gamename, game->creator->username);
		if(game->challenger == NULL)
			sprintf(response,"%s CHALLENGED BY: (null). IN PROGRESS: (null) TO MOVE NEXT AS x\r\n",response);
		else{
			sprintf(response,"%s CHALLENGED BY: %s. %s", response,game->challenger->username,state[game->state+2]);
			if(game->state == 1 || game->state == -1){
				char toPlay = 'x';
				char *nextToPlay[] = {game->creator->username,game->challenger->username};
				int slot;
				if(game->state==1)
					slot=1;
				else
					slot=0;
			
				if(game->state ==-1)
					toPlay = 'o';
						
				sprintf(response,"%s %s TO MOVE NEXT AS %c \r\n", response,nextToPlay[slot], toPlay);
			}else{
				sprintf(response,"%s \r\n",response);
			}
		}
		sprintf( response, "%sa  %c | %c | %c \r\n",response,  game->ttt[0][0],  game->ttt[0][1],  game->ttt[0][2]);
		sprintf( response, "%s  ---|---|---\r\n", response );
		sprintf( response, "%sb  %c | %c | %c \r\n", response, game->ttt[1][0],  game->ttt[1][1],  game->ttt[1][2]);
		sprintf( response, "%s  ---|---|---\r\n", response );
		sprintf( response, "%sc  %c | %c | %c \r\n", response, game->ttt[2][0],  game->ttt[2][1],  game->ttt[2][2]);
		sprintf( response, "%s\r\n", response );
		sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );
		
		}else{
	  		sprintf( response, "COMMAND %s NOT IMPLEMENTED", command );
		}

		write(client_sock , response , 2000);  
		close(client_sock);
	}

	close(socket_desc);	
	
	return 0;
}

