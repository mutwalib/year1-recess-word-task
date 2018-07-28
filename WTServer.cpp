#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <winsock2.h>
#include <time.h>
#include <string>
#include <fstream>  
#include <iostream> 

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")


using namespace std;

#define DEFAULT_PORT 55000
#define DEFAULT_BUFLEN 512
#define MAX_CLIENTS 50

char recvbuf[DEFAULT_BUFLEN] = "";
int len, receiveres, clients_connected = 0;
bool active = TRUE;

SOCKET server_socket = INVALID_SOCKET;
SOCKET client_fd;
sockaddr_in server;
char * toclient;
char * clientbuffer;

//
typedef struct{
	char * timesubmitted;
	int JobID;
	char * processingDuration;
	char * type;
	char * word;
	char * status;
	int priority;
} student_task;

//table for clients sockets
struct _clients_b {
	bool connected;
	SOCKET ss;
	student_task TASK;
	char * input_stream;
	char * student_no;
	int waitingjobs;
};

_clients_b clients[MAX_CLIENTS];
char * ALL_TASKS[5000]; //pointer to store all student tasks
char * STUDENT_TASK_MAPPING[5000]; //pointer to store all student tasks mapping

//function declarations

void start_server();
/////////////
int partition(int *a, int m, int n)
{
	int i, j, pindex, pivot;
	pindex = m;
	pivot = a[n];
	for (i = m; i<n; i++)
	{
		if (a[i] <= pivot)
		{
			swap(a[pindex], a[i]);
			pindex++;
		}
	}
	swap(a[pindex], a[n]);
	return pindex;
}
/////////////////////////////////////
int quicksort(int *a, int m, int n)
{
	int index;
	if (m >= n)
		return 0;
	{
		index = partition(a, m, n);
		quicksort(a, m, index - 1);
		quicksort(a, index + 1, n);
	}
}
/////////////////////////////////////
/**
* Remove leading whitespace characters from string
*/
void trimLeading(char * str)
{
	int index, i, j;
	index = 0;
	/* Find last index of whitespace character */
	while (str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
	{
		index++;
	}
	if (index != 0)
	{
		/* Shit all trailing characters to its left */
		i = 0;
		while (str[i + index] != '\0')
		{
			str[i] = str[i + index];
			i++;
		}
		str[i] = '\0'; // Make sure that string is NULL terminated
	}
}

/*conver int to char*/
char intAsChar(int p){
	char thisChar[53] = "ABCDEFGHIJAKMLOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	int m;
	if (p > 65 && p < 92){
		m = p - 66;
		return thisChar[m];
	}
	if (p > 96 && p < 124){
		m = p - 97;
		return thisChar[m];
	}
}

/// returns the current time on machine
char * get_time(){
	char * _time_;
	time_t curtime;
	// Don't forget to check the return value !
	curtime = time(NULL);
	if (curtime == -1)
	{
		perror("time()");
		return "ERROR: time()";
	}
	strftime(_time_, 0x100, "%c", localtime(&curtime));
	printf("TIME: %s", time);
	return _time_;
}

//reversing a word
char * reversestr(char *str)
{
	char *p1, *p2;
	if (!str || !*str)
		return str;
	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
	{
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
	return str;
}

//double word
char * doublestr(char *str){
	char ret_str[60];
	int j;
	for (j=0; j < strlen(str); j++)
	{
		ret_str[j] = str[j];
	}

	for (int k = j; k < strlen(str); k++)
	{

		ret_str[k] = str[k];
	}
	return ret_str;
}


///////////////////////////////////get assigned position of character.
char CHARACTERS[54] = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
int getpos(char n){
	int i = 0;
	for (i = 0; i < strlen(CHARACTERS);i++){
		if (CHARACTERS[i] == n){
			return i-1;
		}
	}
	return -1;
}

/////////////////////////////////////////////
char * encryptAlgorithm(char * num1, int pos){
	char num2[4];
	int numx = 10;
	int numx1 = 10;	
	for (int i = 0; i < strlen(num1); i++){
		if (i>0){
			num2[i] = tolower(CHARACTERS[pos-1]);//Subtract 1 from 26, and that is the character in small letters to insert between the string
			numx = pos - 9;//Subtract 9 from the previous letter holder e.g. 26-9 = 17
			//////////////////////
			num2[i + 1] = numx / 10;
			//Put the above result into in between the numbers but after the character in the second sub section. Eg 2y17 6
			//////////////////////////
			//Subtract 1 from 17 and identify the character at that position eg.  2y1p76
			////////////////////////////////////////////////
			numx1 = numx - 1;				////////
			num2[i + 1] = getpos(numx);		////////
			////////////////////////////////////////
			num2[i + 1] = numx1 % 10;
			/////////////////////////////////////
			//Subtract 9 from 17 = 8. If it is a single digit number, 
			//pick the corresponding character and insert the character 
			//in capital letters in the middle of the string eg. 2y1Hp76
			numx1 = numx - 9;
			if (numx1 < 10){
				getpos(numx1);
			}
			num2[i + 1] = numx % 10;
			num2[i + 1] = num1[i];
		}
		else{
			num2[i] = num1[i];
		}
	}
	return num2;
}
/////////////////////////////////////////////
char * decryptAlgorithm(char * num1, int pos){
	char num2[4];
	int numx = 10;
	int numx1 = 10;
	for (int i = 0; i < strlen(num1); i++){
		if (i>0){
			num2[i] = toupper(CHARACTERS[pos - 1]);//Subtract 1 from 26, and that is the character in small letters to insert between the string
			numx = pos + 9;//Subtract 9 from the previous letter holder e.g. 26-9 = 17
			//////////////////////
			num2[i + 1] = numx / 10;
			//Put the above result into in between the numbers but after the character in the second sub section. Eg 2y17 6
			//////////////////////////
			//Subtract 1 from 17 and identify the character at that position eg.  2y1p76
			////////////////////////////////////////////////
			numx1 = numx + 1;				////////
			num2[i + 1] = getpos(numx);		////////
			////////////////////////////////////////
			num2[i + 1] = numx1 % 10;
			/////////////////////////////////////
			//Subtract 9 from 17 = 8. If it is a single digit number, 
			//pick the corresponding character and insert the character 
			//in capital letters in the middle of the string eg. 2y1Hp76
			numx1 = numx + 9;
			if (numx1 < 10){
				getpos(numx1);
			}
			num2[i + 1] = numx % 10;
			num2[i + 1] = num1[i];
		}
		else{
			num2[i] = num1[i];
		}
	}
	return num2;
}

//////////////////////////////////////////////

//////////////////////////////////////////////
//encrypt word
char * encryptstr(char *str){
	int i = 0;
	char * num1;
	char * num2;
	int pos = getpos(str[i]);
	for (i = 0; i<strlen(str); i++){
		///loop over the word
		if (pos > 9){
			//Z = 2 6
			num1[i] = intAsChar(pos);
			num2 = encryptAlgorithm(num1, pos);
		}
		else{
			num2[i] = toupper(str[i]);
		}
	}
	return num2;
}

//encrypt word
char * decryptstr(char *str){
	int i = 0;
	char * num1;
	char *  num2;
	int pos = getpos(str[i]);
	for (i = 0; i<strlen(str); i++)
	{
		///loop over the word
		if (pos > 9)
		{
			//Z = 2 6
			num1[i] = intAsChar(pos);
			num2 = decryptAlgorithm(num1, pos);
		}
		else
		{
			num2[i] = tolower(str[i]);
		}
	}
	return num2;
}

//Replace char at pos n in word
char * replace_(char *str1, char * params){
	char * token;
	char * toReplaceWith;
	token = strtok(params, ",");
	if (token != NULL){
		trimLeading(token);
	}
	/////////////////////////////
	int pos = token[0]-49;// strtok(token, "-");
	char ch = token[2];//strtok(NULL, "-");
	////////////////////////////
	str1[pos] = ch;
	//////////////////////////
	////DO THIS FOR ALL REPLACEMENTS
	do//look through params
	{//middle 1-h, 2-u, 3-s, 4-t generates a word hustle
		//token = strtok(NULL, ",");
		int pos = token[0] - 49;// strtok(token, "-");
		char ch = token[2];//strtok(NULL, "-");
		////////////////////////////
		//if (islower(pos)){
		//	str1[pos] = ch;
		//}
		//else{
		str1[pos] = ch;
		//}
		//char * pos = strtok(token, "-");
		//trimLeading(pos);
		//char * ch = strtok(NULL, "-");
		//trimLeading(ch);
		////////////////////////////
		//////////////////////////
	}
	while (strtok(NULL, ",") != NULL);
	cout << str1 << endl;
	///DONE REPLACING
	return str1;
}

void ready_job(char * resp, _clients_b client){
	std::string s = resp;
	std::ofstream os("READY_JOBS");
	if (!os)
	{
		std::cerr << "Error writing to ..." << std::endl;
	}
	else 
	{
		os 
			<< "ID:" <<
			client.student_no 
			<< ","  <<
			"TYPE:" 
			<< client.TASK.type 
			<< "," <<
			"PROCESSING_TIME:" 
			<< client.TASK.processingDuration 
			<< "," <<
			"SUBMITTED_TIME:"
			<< client.TASK.timesubmitted
			<< "," <<
			"OUTPUT:"
			<< s;
	}
}

int index = -1;
int pending_tasks = 0;
void handle_student(char * buff, _clients_b client, int busy){
	char * toclient = NULL;
	const char * delim1 = ";";
	const char * delim2 = ",";
	const char * delim3 = "-";
	const char * delim4 = " ";	
	//process the string.
	// detect multiple tasks, DELIMITER ';'
	char * token;
	char * task;
	char * word;
	char * wordparams;
	char * resp;
	int priority;
	// loop through TASKS array of all clients
	// Priority 1, which is the highest if job contains the least number of characters
	// Priority 2, if owner has the highest number of waiting jobs
	// Priority 3, if replace function
	for(int i = 0; i < pending_tasks; i++){
		if (strlen(word) < strlen(ALL_TASKS[i]))
		{
			client.TASK.priority = 1;
		}
		if (strlen(word))
		{
			client.TASK.priority = 2;
		}
		if (strcmp(task, "REPLACE") == 0)
		{
			client.TASK.priority = 3;
		}
	}
	if (busy == 1){
		std::ofstream os("BUSY_JOBS");
		if (!os)
		{
			std::cerr << "Error writing to ..." << std::endl;
		}
		else
		{
			/* get the first token */
			client.TASK.word = client.input_stream;//
			token = strtok(client.TASK.word, delim1);
			while (1) {
				//if (strlen(token)<strlen(client.input_stream)){
				//	token = strtok(NULL, delim1);
				//}
				//else{
				//	token = strtok(client.TASK.word, delim1);
				//}
				//trimLeading(token);				
				task = strtok(NULL, delim4);//THE TASK .. DOUBLE, ENCRYPT etc
				word = strtok(NULL, delim4); //THE WORD -sample-
				wordparams = strtok(NULL, delim4); //THE WORD -sample-
				// LOOP TO EXTRACT INDIVIDUAL COMMANDS WHILE SETTING 
				// PRIORITY FOR EACH TASK.
				client.waitingjobs++;
				client.TASK.JobID = index;
				client.TASK.timesubmitted = "H:M:I:S"; //get_time(); //exception thrown
				client.TASK.word = word;//


				os << client.student_no << "-" << client.TASK.priority << ">" << task << " " << word << " " << wordparams << " " << endl;
				cout << task << word << endl;
				index++;
				ALL_TASKS[index] = word;
				STUDENT_TASK_MAPPING[index] = client.student_no;
				pending_tasks++;
				if (token == NULL){
					break;
				}
				token = strtok(NULL, delim1);
			}
		}
	}
	else{
		// LOOP TO EXTRACT INDIVIDUAL COMMANDS WHILE SETTING 
		// PRIORITY FOR EACH TASK.
		cout << task << word << endl;
		task = strupr(task);//CONVERT TO UPPER CASE
		if (strcmp(task, "DOUBLE")){
			resp = doublestr(word);
		}
		if (strcmp(task, "ENCRYPT") == 0){
			resp = encryptstr(word);
		}
		if (strcmp(task, "DECRYPT") == 0){
			resp = decryptstr(word);
		}
		if (strcmp(task, "REVERSE") == 0){
			resp = reversestr(word);
		}
		if (strcmp(task, "DELETE") == 0){
			resp = reversestr(word);
		}
		if (strcmp(task, "REPLACE") == 0){
			resp = replace_(word, wordparams);
		}
		resp[strlen(resp) - 1] = '\0';
		ready_job(resp, client);
		int id = 0;
		while (token != NULL) {
			token = strtok(NULL, delim1);
			trimLeading(token);//remove leading space from string.
			task = strtok(token, delim4);//THE TASK .. DOUBLE, ENCRYPT etc
			word = strtok(NULL, delim4); //THE WORD -sample-
			wordparams = strtok(NULL, delim4); //THE WORD -sample-
			// LOOP TO EXTRACT INDIVIDUAL COMMANDS WHILE SETTING 
			// PRIORITY FOR EACH TASK.
			cout << task << word << endl;
			task = strupr(task);//CONVERT TO UPPER CASE
			if (strcmp(task, "DOUBLE")){
				resp = doublestr(word);
			}
			if (strcmp(task, "ENCRYPT") == 0){
				resp = encryptstr(word);
			}
			if (strcmp(task, "DECRYPT") == 0){
				resp = decryptstr(word);
			}
			if (strcmp(task, "REVERSE") == 0){
				resp = reversestr(word);
			}
			if (strcmp(task, "DELETE") == 0){
				resp = reversestr(word);
			}
			if (strcmp(task, "REPLACE") == 0){
				resp = replace_(word, wordparams);
			}
			ready_job(resp, client);
			client.waitingjobs--;
			id = client.TASK.JobID;
			client.TASK.JobID = NULL;
			client.TASK.priority = NULL;
			client.TASK.processingDuration = NULL;
			client.TASK.timesubmitted = NULL;
			client.TASK.status = NULL;
			client.TASK.type = NULL;
			client.TASK.word = NULL;
			ALL_TASKS[id] = NULL;
		}
	}
}

char * get_stno(char * buff){
	const char * delim4 = ">";
	char * stno;
	stno = strtok(buff, delim4);//THE TASK .. DOUBLE, ENCRYPT etc
	return stno;
}


int main() {
	cout << "Server starting..." << endl;
	//start the server and do basic tcp setup ------------------
	start_server();
	//the main loop
	while (active) 
	{
		//start accepting clients ------------------------------
		len = sizeof(server);
		client_fd = accept(server_socket, (struct sockaddr*)& server, &len);
		//our client is a real thing?
		if (client_fd != INVALID_SOCKET) 
		{
			//save client socket into our struct table
			clients[clients_connected].ss = client_fd;
			clients[clients_connected].connected = TRUE;
			//and of course we need a calculator too
			clients_connected++;
			cout << "\nNew client: " << client_fd << "\n" << endl;
		}
		//plus we don't need to loop that fast anyways
		Sleep(1);
		//receiving and sending data ---------------------------
		//we have clients or no?
		const char * delim4 = ">";
		char * buff = ">";
		
		if (clients_connected > 0) {
			//lets go through all our clients
			for (int cc = 0; cc < clients_connected; cc++) {
				memset(&recvbuf, 0, sizeof(recvbuf));
				if (clients[cc].connected) {
					//receive data
					receiveres = recv(clients[cc].ss, recvbuf, DEFAULT_BUFLEN, 0);
					//and echo it back if we get any
					clients[cc].student_no = get_stno(recvbuf);
					buff = strtok(NULL, ">");//THE TASK .. DOUBLE, ENCRYPT etc
					clients[cc].input_stream = buff;
					if (receiveres > 0) 
					{
						Sleep(10);
						cout << "Client data received: " << recvbuf << "\n\n" << endl;
						if (strcmp(recvbuf, "NEW_CLIENT") == 0)
						{
							send(client_fd, recvbuf, strlen(recvbuf), 0);
						}
						else
						{
							handle_student(recvbuf, clients[cc], 1);
						}
					}
					// how to close connection
					// this just for quick example
					// so you are just getting rid off client's socket data
					else if (receiveres == 0 || strcmp(recvbuf, "disconnect") == 0) 
					{
						cout << "Client disconnected." << endl;
						clients[cc].connected = FALSE;
						clients_connected--;
						//delete [cc] clients;
					}
				}
			}
		}
	}

	//when we shut down our server
	closesocket(server_socket);

	// Clean up winsock
	WSACleanup();

	return 0;
}



void start_server() {
	int wsaresult, i = 1;

	WSADATA wsaData;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(DEFAULT_PORT);

	// Initialize Winsock
	wsaresult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//if error
	if (wsaresult != 0) {
		printf("WSAStartup failed with error: %d\n", wsaresult);
	}


	// Create a SOCKET for connecting to server
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (server_socket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
	}

	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&i, sizeof(i));

	//Binding part
	wsaresult = bind(server_socket, (sockaddr*)&server, sizeof(server));

	if (wsaresult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
	}

	// Setup the TCP listening socket
	wsaresult = listen(server_socket, 5);

	unsigned long b = 1;

	//make it non blocking
	ioctlsocket(server_socket, FIONBIO, &b);

	if (wsaresult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
	}
	printf("Server Ready for connections");

}