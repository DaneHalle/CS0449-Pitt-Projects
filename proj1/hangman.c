//Dane Halle (dmh148)
//have a nice day --> from my gf
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

//Given
void get_line(char* buffer, int size) {
	fgets(buffer, size, stdin);
	char len = strlen(buffer);
	
	if(len != 0 && buffer[len - 1] == '\n')
		buffer[len - 1] = '\0';
}

//Given
int streq_nocase(const char* a, const char* b) {
	for(; *a && *b; a++, b++) if(tolower(*a) != tolower(*b)) return 0;
	return *a == 0 && *b == 0;
}

//Assists in the printing of a segmented guess
void printGuess(char* rightGuess, int size)
{
	int j=0;
	char segmentedGuess[100];
	while(j<size*2-2){
		if(j%2==0){
			segmentedGuess[j]=rightGuess[j/2];
		}else{
			segmentedGuess[j]=' ';
		}
		j++;
	}
	segmentedGuess[size*2-2]='\0';
	printf("%s\tGuess a letter: ", segmentedGuess);
}

//Assists in applying strike. Just made writing it once easeir than 2 or 3 times
int applyStrike(int strike, char* word)
{
	strike++;
	printf("Strike %d!\n", strike);
	if(strike>=5){
		printf("You lost, the word was %s\n", word);
		exit(1);
	}
	return strike;
}

//Win
void win(char* word)
{
	printf("You got it! The word was %s\n", word);
	exit(1);
}

//The main program that runs. Takes in guesses and checks them and yeah. 
void guess_Letter(char* word, int size, int strike, char* rightGuess)
{
	while(!streq_nocase(word, rightGuess)){
		char input[100];
		get_line(input, sizeof(input));
		*input=tolower(*input);
		int i=0; int found=0;
		if(streq_nocase(word, input)){
			win(word);
		}else if(strlen(input)>1){
			strike=applyStrike(strike, word);
		}else{
			while(i<size){
				if(input[0]==word[i] && input[0]!='\0'){
					rightGuess[i]=input[0];
					found=1;
				}
				i++;
			}
			if(found==0 || input[0]=='\0'){
				strike=applyStrike(strike, word);
			}
			if(streq_nocase(word, rightGuess)){
				win(word);
			}
		}
		printGuess(rightGuess, size);
	}
}

//main
int main()
{
	srand((unsigned int)time(NULL));

	FILE* f=fopen("dictionary.txt", "r");
	if(f==NULL){
		printf("That file does not exist.");
		exit(1);
	}

	char firstLine[5]; fgets(firstLine, sizeof(firstLine), f);
	
	int fileLength; sscanf(firstLine, "%d", &fileLength);
	char words[fileLength][20]; int i=1;
	while(i<=fileLength){
		fgets(words[i], sizeof(words[i]), f);
		i++; 
	}
	fclose(f);

	int randWord=rand()%(fileLength+1); 
	int wordLength=strlen(words[randWord]);
	while(wordLength<1){ //I got a weird case once so yeah. 
		randWord=rand()%(fileLength+1);
		wordLength=strlen(words[randWord]);
	}

	if(wordLength != 0 && words[randWord][wordLength - 1] == '\n'){
		words[randWord][wordLength-1] = '\0';
	}

	char guess[wordLength-1];
	strcpy(guess, words[randWord]);
	int j=0;
	while(j<wordLength){
		guess[j]='_';
		j++;
	}
	guess[wordLength-1]='\0';
	printGuess(guess, wordLength);

	int strike=0; 
	guess_Letter(words[randWord], wordLength, strike, guess);

	return 0;
}