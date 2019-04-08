// Dane Halle (dmh148)
// Slight "error" where if you just randomly <ctrl+c>, the 
//  "myshell>  " isn't printed but you can still type programs
#define _GNU_SOURCE
#include <ctype.h>	#include <errno.h>	#include <stdio.h>
#include <stdlib.h>	#include <signal.h>	#include <string.h>
#include <sys/types.h>	#include <sys/wait.h>	#include <unistd.h>

char* argv[500]; 		//arguments
int argc=0; 			//number of arguments
int IOwrite=0; 			//0=none; 1=write
int IOread=0; 			//0=none; 1=read
char* writeFile=NULL;	//File if > is found
char* readFile=NULL;	//File if < is found

//Gets the input from user
void getData(char* str, int length)
{
	fgets(str, length, stdin);
	int strLen=strlen(str);
	if(strLen>0){
		str[strLen-1]='\0';
	}
}

//Handler for when SIGINT is signalled
void interrupt(int status)
{
	if(WIFSIGNALED(status)){
		printf("\nProgram terminated due to signal: %s\n", strsignal(WTERMSIG(status)));
		signal(status, SIG_IGN);
	}
}

//Code for running built in UNIX commands
//Also handles the writing and reading if > or < are found
void regular(char* command)
{
	if(fork()==0){
		argv[argc]=NULL;
		FILE* IOFile=NULL;

		if(IOwrite==1){
			IOFile=freopen(writeFile, "w", stdout);
			argc-=2;
			argv[argc]=NULL;
		}
		if(IOread==1){
			IOFile=freopen(readFile, "r", stdin);
			argc-=2;
			argv[argc]=NULL;
		}

		int execute=execvp(argv[0], argv);
		if(execute==-1){
			printf("\tInvalid Command\n");
		}
		exit(0);
	}else{
		signal(SIGINT, &interrupt);
		int status;
		wait(&status);
	}
}

//Parses through str and makes it useable. 
//Also checks for exit and exit codes along with cd
//When a > or < is found, it sets global variables to the direction
int tokenizer(char* str)
{
	const char* delim = " \t\n";
	char* token = strtok(str, delim);
	int i;
	for(i = 0; token != NULL; i++)
	{
		argv[i] = token;
		argc++;
		token = strtok(NULL, delim);
		if(!strcmp(argv[i], "exit")){
			if(token != NULL){
				argv[i+1]=token;
				exit(atoi(argv[i+1]));
			}
			exit(0);
		}else if(!strcmp(argv[i], "cd")){
			if(token != NULL){
				argv[i+1]=token;
				int worked=chdir(argv[i+1]);
				if(worked==-1){
					printf("\tInvalid Directory\n");
				}
			}else{
				printf("\tYou need to specify a directory\n");
			}
			return 1;
		}
		if(!strcmp(argv[i], ">")){
			if(IOwrite==1){
				printf("\tInvalid use of > argument\n");
				return 1;
			}
			writeFile=token; //sets writeFile to the proper destination
			IOwrite=1;
		}else if(!strcmp(argv[i], "<")){
			if(IOread==1){
				printf("\tInvalid use of > argument\n");
				return 1;
			}
			readFile=token; //sets readFile to the proper destination
			IOread=1;
		}
	}
	argv[i] = NULL; 
	return 0;
}

//Main and holds loop for running myshell
int main()
{
	char str[500];

	while(1){
		printf("myshell>  ");
		getData(str, 500);

		if(str==NULL || strlen(str)==1){
			printf("\tPlease enter a command\n");
		}else{
			int weird=tokenizer(str);
			if(!weird){
				regular(str);
			}
		}
		argc=0;
		IOwrite=0;
		IOread=0;
	}	
	exit(1);
}