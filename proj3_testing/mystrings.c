// Dane Halle (dmh148)
#include <ctype.h>
#include <stdio.h>



int main(int argc, char** argv)
{
	FILE* f=fopen(argv[1], "rb");
	if(f==NULL){
		printf("File does not exist\n");
		return 0;
	}

	char string[4];
	char current;
	int space=0;

	while (!feof(f)){
		while (space<4){
			if (!feof(f)){
				current=fgetc(f); 

				if (isprint(current)){ 
					string[space]=current;
					space++;
					if (space==4){
						printf("%s", string);
					}
				}else{ 
					space=0;
				}	
			}else{
				break;
			}
		}
		if (!feof(f)){
			current=fgetc(f);
			
			if (isprint(current)){
				printf("%c",current); 
			}else{
				printf("\n");
				space=0; 
			}	
		}
	}

	fclose(f);
	return 0; 
}