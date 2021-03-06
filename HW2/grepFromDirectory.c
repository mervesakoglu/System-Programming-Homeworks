
/***********************************************************/
/*      121044055                                          */
/*    Merve SAKOĞLU                                        */
/*  HW02_121044055_merve_sakoglu                           */
/*    Bu program arguman olarak girilen directory icinde   */
/*      -aranan kelime sayisini                            */
/*      -directory altindaki toplam file sayisini          */
/*      -toplam directory sayisini                         */
/*      -toplam child sayisini bulur                       */
/*                                                         */
/***********************************************************/

#include<stdio.h> 
#include<stdlib.h> 
#include<string.h> 
#include<dirent.h> 
#include<errno.h> 
#include<time.h> 
#include<sys/stat.h> 
#include<sys/types.h> 
#include<sys/param.h> 
#include<sys/mman.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<termios.h> 
#include<unistd.h>

#undef MAX
#define MAX 500

/*Dosyalardaki toplam kelime sayisini tutar*/
static int totalW = 0;
/*Toplam file sayisini tutar*/
static int totalFile = 0;
/*Toplam child sayisini tutar*/
static int totalChild = 0;
/*Ana directory ile birlikte toplam directory sayisini tutar*/
static int totalDirectory = 1;

/*
*Bu fonksiyon dosya icinde aranan kelimenin kac kez gectigini bulur
*kacıncı satir ve sütunda gectigini dosyaya yazdirir
*/
	int myGrep(FILE *inputFile,FILE *outputFile,int size,char *fileName,char *argv);
/*
*Bu fonksiyon dosyanin size ini bulur
*/
	int findSize(FILE *inputFile,char *fileName);
/*
*Asil isi yapan fonksiyon static olarak tanimladigim toplam kelime,
*file,directory ve child degiskenlerimi burada buldum
*myGrep() fonksiyonumu burada cagirarak dosyalardaki tum kelime
*sayisini burada buldum
*/
	int fileOperations(FILE *outputFile,char *directoryName,char *findString);
	
void usage();

int main(int argc, char *argv[]){

	FILE *outputFile;

	outputFile = fopen("gfd.log","w");

	/*Arguman kontrolu yapar*/
	if ((argv[1] == NULL) || (argv[2] == NULL) || argc!=3) 
 	{ 
		if(argc > 3)
			printf("Fazla argüman girdiniz\n");
		else if(argc < 3)
			printf("Az argüman girdiniz\n");

		/*Dogru argumanlari cagirir*/
		usage();
		fclose(outputFile);
	        return -1; 
	} 

	if(outputFile == NULL){
		printf("Dosya açılamadı\n");
		fclose(outputFile);
		exit (0);
	}

        fileOperations(outputFile,argv[1],argv[2]);

	printf("******************************************************************\n");
	printf("Total Words  *  Total Directory  *  Total File  *  Total Child  \n");
	printf("******************************************************************\n");
	printf("    %d               %d                  %d             %d        \n",totalW,totalDirectory,totalFile,totalChild);
	printf("******************************************************************\n");

	fclose(outputFile);
	return 0;
} 

/*
*Dosyadaki karakter sayisini sayar
*/
int findSize(FILE *inputFile,char *fileName){

	char character;
	int count ;
	inputFile = fopen(fileName,"r");

	for(count =0 ; fscanf(inputFile,"%c",&character) != EOF ; ++count){
	
	}
  
   	fclose(inputFile);

	return (count);

}
int myGrep(FILE *inputFile,FILE *outputFile,int size,char *fileName,char *argv){
	
	int coloumnCount = 0, totalWord = 0;
	int  i , j,row = 0;
	char *string=(char*)malloc(size*sizeof(char));

	inputFile = fopen(fileName,"r");

	if (string == NULL) 
 	{ 
 	    printf("Memory not allocated\n");
	    fclose(inputFile); 
	    free(string);
	    exit (0); 
 	} 

	fprintf(outputFile,"*******************************************\n");
	fprintf(outputFile,"        %d.File # %s                       \n",totalFile,fileName);
	fprintf(outputFile,"*******************************************\n");

	/*Dosyadan ilk satiri okur*/
	while(fgets(string ,size, inputFile) != 0){

		/*Satir sonuna kadar kontrol eder*/
		for(i=0 ; i<strlen(string) ; ++i){
			/*Karakter aranan kelimenin ilk karakterine eşit ise o satiri row da tutar*/
			if(string[i] == argv[0]){
				row = 0;  row = i;
				
				j = 1;
				/*aranan kelimenin butun  karakterleri esit oldugu surece j i artirir */
				while((string[i+j] == argv[j]) && (j<strlen(argv))){
					++j;

				}
				/*kelimenin butun karakterleri esit ise kelime sayisisni artirir ve dosyaya bastirir*/
				if(strlen(argv) == j){

					++totalWord;
					fprintf(outputFile,"-->%d. Line # %d. Column\n",coloumnCount,row);
				}

			}

		}
	++coloumnCount;
	}
	
	if(totalWord == 0)
		fprintf(outputFile,"There is no word in the file\n");

	fprintf(outputFile,"\n\n");
	fclose(inputFile);

	free(string);

	/*Kelime sayisini dondurur*/
	return totalWord;
}
int fileOperations(FILE *outputFile,char *directoryName,char *findString){

	DIR *openFile;
	FILE *inputFile=NULL;
	struct dirent *input;
	char directory[MAX];
	char fullDirectory[MAX];
	struct stat statFile;
	pid_t  pid;
	int size = 0;
	int stat = 0;

	openFile = opendir(directoryName);

	if(openFile == NULL){
		printf("Dosya acilamadi\n");
		/*fclose(inputFile);*/
		closedir(openFile);
		exit (1);
	}

	strncpy(directory,directoryName, MAX - 1);

        /*Directory okudum*/
	while((input = readdir(openFile)) != NULL){
		
		if(strcmp(input->d_name,".")!=0 && strcmp(input->d_name,"..")!=0){
			
			/*Directory ve altindaki dosyayi bir arraye attim*/
			sprintf(fullDirectory, "%s/%s",directory, input -> d_name );

			if (lstat(fullDirectory,&statFile) < 0){
          			 perror(fullDirectory);
			}
			/*Dosya mi directory mi oldugunu kontrol ettim dosya ise*/
			if (S_ISDIR(statFile.st_mode) == 0){

				/*Dosya sayimi artirdim*/
				totalFile = totalFile + 1;
				
				if((pid = fork())== -1){
				
					perror("Not fork\n");
					exit (1);
				}if(pid == 0){
				
					size = findSize(inputFile,fullDirectory);
					/*Her seferinde buldugum sayiyi toplam kelimeye ekledim*/
					totalW += myGrep(inputFile,outputFile,size,fullDirectory,findString);
					exit (totalW);
				}else{
					/*Asagidaki fonksiyonlari http://www.kaanaslan.com/resource/article/display_article.php?id=87
					 * sitesinden referans alarak kullandim */
					wait(&stat);
					totalW = WEXITSTATUS(stat);
				}
				/*Childlari topladim*/
				totalChild = totalChild + 1;
			}else{/*Dosya degil ise recursive olarak fonksiyonumu tekrar cagirdim ki dosyayi bulsun*/

				/*Directory sayimi buldum*/
				totalDirectory = totalDirectory + 1;
				fileOperations(outputFile,fullDirectory,findString);
			}
			
		
		}
	}
	/*fclose(inputFile);*/
	closedir(openFile);
	return 0;
}
void usage(){
	printf("\n------------\n");
	printf("Usage : \n");
	printf("./grepFromDirectory dirname *(aranmak istenen karakter)");
	printf("\n------------\n");

} 

