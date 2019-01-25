//Dane Halle (dmh148)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>

//Outline for a .wav file. 
typedef struct WAVHeader
{
	char riff_id[4];
	uint32_t file_size;
	char wave_id[4];
	char fmt_id[4];
	uint32_t fmt_size;
	uint16_t data_format;
	uint16_t number_of_channels;
	uint32_t samples_per_second;
	uint32_t bytes_per_second;
	uint16_t block_alignment;
	uint16_t bits_per_sample;
	char data_id[4];
	uint32_t data_size;
} WAVHeader;

//Given
int streq_nocase(const char* a, const char*b) {
	for(; *a && *b; a++, b++) if(tolower(*a) != tolower(*b)) return 0;
	return *a == 0 && *b == 0;
}

//Help menu. Shows how to use -reverse and -rate [VALUE]
//Also has links at the bottom of pictures and H I M
void displayHelp()
{
	srand((unsigned int)time(NULL));
	printf("Hello! Welcome to my wavedit program. ");
	printf("This will help you navigate the program.\n");
	printf("Usage: wavedit [FILE] [OPTION] [VALUE]\n\n");
	printf("\t-rate [VALUE]\t\t0<VALUE<192000 ~ ");
	printf("Changes bytes and samples per second \n");
	printf("\t\t\t\t\taccording to VALUE.\n");
	printf("\t-reverse\t\tWill change the file by ");
	printf("reversing the samples.\n\n");
	printf("Report wavedit bugs to: \t<dmh148@pitt.edu>\n");
	printf("Have a picture of a cat: \t<https://bit.ly/2ga5Cpu>\n");
	printf("Have a hedgehog: \t\t<https://bit.ly/2CvfLbH>\n");
	printf("Gecko smiling ☺: \t\t<https://bit.ly/2Mo1fah>\n\n");
	printf("H͉͓ͦ̈͆̒ ̠̮̱̏ͯ̉E͓͇͉ͭ͗ͮ̓̅̽ ̪̓͂͆̏͒C̤̯̦̠̪ͬ̎ͅ ͌̒Ó̹̝̘̱͖͓͓̈́ͨͨ̋̂ ̥̝̣̤̼̪͌M̠ͨ ̏͆̍̌ͫ̾̈E͇͌͊̿́̑̽ͣ ̟͖̳̝̩̙̤̑̂ͦ̈́S͕̯̤̏: ");
	printf("\t\t\t");
	int randLink=rand()%(100);
	if(randLink<10){
		printf("<https://bit.ly/2U2OtAC>\n\n");
	}else if(randLink<20){
		printf("<https://bit.ly/2FEPo6i>\n\n");
	}else if(randLink<30){
		printf("<https://bit.ly/2sB7wX2>\n\n");
	}else if(randLink<40){
		printf("<https://bit.ly/2RQWP0H>\n\n");
	}else if(randLink<50){
		printf("<https://bit.ly/2szBB9d>\n\n");
	}else if(randLink<60){
		printf("<https://bit.ly/2FPmJLA>\n\n");
	}else if(randLink<70){
		printf("<https://bit.ly/2RBiI4I>\n\n");
	}else if(randLink<80){
		printf("<https://bit.ly/2HyxkgP>\n\n");
	}else if(randLink<90){
		printf("<https://bit.ly/2HmZFqd>\n\n");
	}else{
		printf("<https://bit.ly/2CtmId8>\n\n");
	}
	exit(1);
}

//Helps the file checker. Writing once is easier than 11 times.
void invalidFile()
{
	printf("Invalid .wav file.\n");
	exit(1);
}

//Checks thisFile to make sure it is a valid .wav file and not a 
//nasty other file in disguise
void checkFile(WAVHeader thisFile)
{
	char riff_id[4]; strcpy(riff_id, "RIFF");
	char wave_id[4]; strcpy(wave_id, "WAVE");
	char ftm_id[4]; strcpy(ftm_id, "fmt ");
	char data_id[4]; strcpy(data_id, "data");
	if(strncmp(thisFile.riff_id, riff_id, 4)!=0){
		invalidFile();
	}
	if(strncmp(thisFile.wave_id, wave_id, 4)!=0){
		invalidFile();
	}
	if(strncmp(thisFile.fmt_id, ftm_id, 4)!=0){
		invalidFile();
	}
	if(strncmp(thisFile.data_id, data_id, 4)!=0){
		invalidFile();
	}
	if(thisFile.fmt_size!=16){
		invalidFile();
	}
	if(thisFile.data_format!=1){
		invalidFile();
	}
	if(thisFile.number_of_channels!=1 && thisFile.number_of_channels!=2){
		invalidFile();
	}
	if(thisFile.samples_per_second<=0 || thisFile.samples_per_second>192000){
		invalidFile();
	}
	if(thisFile.bits_per_sample!=8 && thisFile.bits_per_sample!=16){
		invalidFile();
	}
	int bytes=thisFile.samples_per_second*(thisFile.bits_per_sample)/8;
	bytes*=thisFile.number_of_channels;

	if(thisFile.bytes_per_second!=bytes){
		invalidFile();
	}
	int block=(thisFile.bits_per_sample)/8*thisFile.number_of_channels;
	if(thisFile.block_alignment!=block){
		invalidFile();
	}
}


//Run if program is called with just a file an no modifyers.
//Tells user about the file.
void fileCall(char* file)
{
	FILE* sound=fopen(file, "rb");
	if(sound==NULL){
		printf("That file does not exist.\n");
		exit(1);
	}
	WAVHeader thisFile;
	fread(&thisFile, sizeof(thisFile), 1, sound);
	checkFile(thisFile);
	fclose(sound);

	int bits=thisFile.bits_per_sample;
	int sampleRate=thisFile.samples_per_second;
	char audioType[6];
	if(thisFile.number_of_channels==1){
		strcpy(audioType, "mono");
	}else{
		strcpy(audioType, "stero");
	}
	int lengthInSamples=thisFile.data_size/thisFile.block_alignment;
	float lengthInSeconds=(float)lengthInSamples/thisFile.samples_per_second;

	printf("This is a %d-bit %dHz %s sound file.\n", bits, sampleRate, audioType);
	printf("It is %d samples (%.3f in seconds) long\n", lengthInSamples, lengthInSeconds);
}

//Updates the samples_per_second in the file to rate
void changeRate(char* file, int rate)
{
	FILE* sound=fopen(file, "rb+");
	if(sound==NULL){
		printf("That file does not exist.\n");
		exit(1);
	}
	WAVHeader thisFile;
	fread(&thisFile, sizeof(thisFile), 1, sound);
	checkFile(thisFile);

	thisFile.samples_per_second=rate;
	thisFile.bytes_per_second=thisFile.samples_per_second*(thisFile.bits_per_sample)/8;
	thisFile.bytes_per_second*=thisFile.number_of_channels;
	fseek(sound, 0, SEEK_SET);

	fwrite(&thisFile, sizeof(thisFile), 1, sound);
	fclose(sound);
	exit(1);
}

//Helper method for reverse
void reverse8Bit(FILE* sound, WAVHeader thisFile, uint8_t* samples, int length)
{
	int i=0; int j=length-1; uint8_t newSample[length];
	while(i<length){
		newSample[i]=samples[j];
		i++; j--;
	}
	fseek(sound, sizeof(thisFile), SEEK_SET);
	fwrite(&newSample, sizeof(newSample), 1, sound);
	fclose(sound);
	exit(1);
}

//Helper method for reverse
void reverse16Bit(FILE* sound, WAVHeader thisFile, uint16_t* samples, int length)
{
	int i=0; int j=length-1; uint16_t newSample[length];
	while(i<length){
		newSample[i]=samples[j];
		i++; j--;
	}
	fseek(sound, sizeof(thisFile), SEEK_SET);
	fwrite(&newSample, sizeof(newSample), 1, sound);
	fclose(sound);
	exit(1);
}

//Helper method for reverse
void reverse32Bit(FILE* sound, WAVHeader thisFile, uint32_t* samples, int length)
{
	int i=0; int j=length-1; uint32_t newSample[length];
	while(i<length){
		newSample[i]=samples[j];
		i++; j--;
	}
	fseek(sound, sizeof(thisFile), SEEK_SET);
	fwrite(&newSample, sizeof(newSample), 1, sound);
	fclose(sound);
	exit(1);
}

//Reverses the file 
void reverse(char* file)
{
	FILE* sound=fopen(file, "rb+");
	if(sound==NULL){
		printf("That file does not exist.\n");
		exit(1);
	}
	WAVHeader thisFile;
	fread(&thisFile, sizeof(thisFile), 1, sound);
	checkFile(thisFile);

	int lengthInSamples=thisFile.data_size/thisFile.block_alignment;
	int type=thisFile.bits_per_sample*thisFile.number_of_channels;
	if(type==8){
		uint8_t samples[lengthInSamples];
		fread(&samples, sizeof(samples), 1, sound);
		reverse8Bit(sound, thisFile, samples, lengthInSamples);
	}else if(type==16){
		uint16_t samples[lengthInSamples];
		fread(&samples, sizeof(samples), 1, sound);
		reverse16Bit(sound, thisFile, samples, lengthInSamples);
	}else if(type==32){
		uint32_t samples[lengthInSamples];
		fread(&samples, sizeof(samples), 1, sound);
		reverse32Bit(sound, thisFile, samples, lengthInSamples);
	}
}

//main
int main(int argc, char** argv)
{
	if(argc==1){
		displayHelp();
	}else if(argc==2){
		fileCall(argv[1]);
	}else if(argc==4){
		if(!streq_nocase(argv[2], "-rate")){
			printf("Not a valid command. Please refer to the help guide.\n\n");
			displayHelp();
		}
		if(atoi(argv[3])<=0){
			printf("Value less than 0 or equal to 0. Please refer to the help guide.\n\n");
			displayHelp();
		}else if(atoi(argv[3])>192000){
			printf("Value more than 192000. Please refer to the help guide.\n\n");
			displayHelp();
		}
		changeRate(argv[1], atoi(argv[3]));
	}else if(argc==3){
		if(!streq_nocase(argv[2], "-reverse")){
			printf("Not a valid command. Please refer to the help guide.\n\n");
			displayHelp();
		}
		reverse(argv[1]);
	}

	return 0;
}