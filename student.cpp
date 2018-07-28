///////////////
#define UNICODE
#define _UNICODE

#include <stdio.h>      /* for printf(), fprintf() */
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>     /* for exit() */
#include <string.h>

////////////////////////////////////////////////
//// WINDOWS SPECIFIC DIRECTIVES TO ENEBLE SOCKETS
#pragma comment (lib,"Ws2_32.lib")
#pragma comment (lib,"Mswsock.lib")
#pragma comment (lib,"AdvApi32.lib")
////////////////////////////////////////////////
#define SERVER_IP "127.0.0.1"
#define RCVBUFSIZE 100   /* Size of receive buffer */
#define LINE " ================================================================= \n" 

int i, j;
char * send_recieve(int sock, char * msg, int tosend);
	char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */

void menu(char* stno)
{
	fprintf(stderr, LINE);
	printf("\n\tWELCOME [ %s ]\n", stno);
	fprintf(stderr, "\n\n\t***\tPRESS HELP     ---> To Show this help\r\n");
	fprintf(stderr, "\t***\tENTER ENCRYPT word ---> To Encrypt word\r\n");
	fprintf(stderr, "\t***\tENTER DECRYPT word ---> To Double word\r\n");
	fprintf(stderr, "\t***\tENTER DOUBLE  word ---> To Double word\r\n");
	fprintf(stderr, "\t***\tENTER REVERSE word ---> To Reverse word\r\n");
	fprintf(stderr, "\t***\tENTER DELETE  word 1,2 ---> To delete characters 1 and 2 from word\r\n");
	fprintf(stderr, "\t***\tENTER REPLACE word pos1-K, pos1-G ---> To Replace character at position with given character\r\n");
	fprintf(stderr, "\t***\tENTER EXIT ---> EXIT CONSOLE \r\n");
	fprintf(stderr, LINE);
}
/* Error handling function */
void DieWithError(char *errorMessage)
{
	fprintf(stderr, "%s: %d\n", errorMessage, WSAGetLastError());
	exit(1);
}
char * send_recieve(int sock, char * msg, int tosend){
	char * echoBuffer = "";            /* Print the echo buffer */
	int echoStringLen = sizeof(msg);               /* Length of string to echo */
	int bytesRcvd = 0, totalBytesRcvd = 0;   /* Bytes read in single recv() and total bytes read */
	if (tosend = 1){
		if (send(sock, msg, echoStringLen, 0) != echoStringLen){
			DieWithError("send() sent a different number of bytes than expected");
		}
	}
	else if (tosend = 0){
		if ((bytesRcvd = recv(sock, echoBuffer, echoStringLen - 1, 0)) <= 0){
			DieWithError("recv() failed or connection closed prematurely");
		}
		totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
		echoBuffer[bytesRcvd] = '\0';  /* Add \0 so printf knows where to stop */
	}
	printf("%s >> ", echoBuffer);
	return echoBuffer;            /* Print the echo buffer */
}
void clrscr()
{
	system("@cls||clear");
}

char sep[4] = " > ";

char * create_task(char * command, char * stno){
	char thecmd[5000];
	int len = strlen(stno);
	int len1 = strlen(sep);
	int len2 = strlen(command);
	j = 0;
	for (size_t i = 0; i < len; i++)
	{
		thecmd[j] = stno[i];
		j++;
	}
	for (size_t k = 0; k < len1; k++)
	{
		thecmd[j] = sep[k];
		j++;
	}
	for (size_t m = 0; m < len2; m++)
	{
		thecmd[j] = command[m];
		j++;
	}
	thecmd[j] = '\0';
	return thecmd;
}

int main(int argc, char *argv[])
{
	int sock;                        /* Socket descriptor */
	struct sockaddr_in echoServAddr; /* Echo server address */
	unsigned short echoServPort;     /* Echo server port */
	char *servIP;				     /* Server IP address (dotted quad) */
	WSADATA wsaData;				 /* Structure for WinSock setup communication */
	servIP = SERVER_IP;				 /* First arg: server IP address (dotted quad) */
	echoServPort = 55000;
	///////////////
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
	{
		fprintf(stderr, "WSAStartup() failed");
		exit(1);
	}

	/* Create a reliable, stream socket using TCP */
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
	echoServAddr.sin_family = AF_INET;             /* Internet address family */
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
	echoServAddr.sin_port = htons(echoServPort); /* Server port */
	//////////////////////////////////////////////////////////////
	int stop = 0;
	char cmd[100];
	char thistask[1000];
	char * command;
	char stno[15];
	char * task;
	//////////////////////////////////////////////////////////////

	//printf("\n\t**** ENTER MORE THAN 1 CHARACTER FOR STUDENT MUMBER ****\n");
	//////////////////////////////////
	//printf("\n\tENTER STUDENT No: > ");
	///////////////////////////
	//scanf("%s", stno);
	stno[0] = '2';
	stno[1] = '1';
	stno[2] = '4';
	stno[3] = '0';
	stno[4] = '0';
	stno[5] = '\0';
	//printf("\n\tID => %s \n", stno);
	//////////////////////////
	/////////////////////////
	//printf("\n\t: >----<: \n");
	//printf(LINE);

	/* Establish the connection to the echo server */
	if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("\n\t >> WORD TASK SERVER UNAVAILABLE <<");
	//task = send_recieve(sock, stno, 1);//tell server this client. 
	while (stop == 0)
	{
		clrscr();
		menu(stno);
		printf("\nWaiting For Commands...\n");
		printf("\n\t: >");
		scanf("%s", cmd);
		//command = cmd;
		command = strupr(cmd); //convert all to uppercase.
		if (strcmp("EXIT", command) == 0)
		{
			printf("\nEXITING .....\n");
			task = send_recieve(sock, "Q", 1);//tell server to close this client.
			stop = 1;
		}
		else
		{
			task = create_task(command, stno);
			for (size_t i = 0; i < strlen(task); i++)
			{
				thistask[i] = task[i];
			}
			thistask[i + 1] = '\0';
			task = send_recieve(sock, thistask, 1);//send command of this client to server. 
		}
	}
	printf("\n\n\tQUIT() \n");    /* Print a final linefeed */
	closesocket(sock);
	WSACleanup();  /* Cleanup Winsock */
	//exit(0);
}
