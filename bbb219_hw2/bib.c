/**
 *  Badjessa(Romeo) B. Bahoumda
 *  CSE 271
 *  Goal: ltimately, develop a C program named bib.c that uses structures (with typedef) and malloc 
 *  (with sizeof) to allow users to build a bibliography reference file. Each bibliographic entry 
 *  should allow the user to enter the author or authors (single entry), title of the article,
 *  publication in which the article was printed, publisher, date of publication, and page or 
 *  pages in which the article appeared.
 *
 *  Current Submission: The first submission on your way to writing this program is to write a
 *  program, bib.c, to parse the command line, open and print the contents of a bib.txt file to an 
 * output file (or stdout, if no output file is specified), and report appropriate warnings and/or 
 *  exit.
 **/
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

//Bibliography struct, where our bibliography would be stored

void printUsage();
void printVerbose(char *message, int isVerbose);
void exitHandler(int code);

typedef struct Bibliograpy
{
  char * author;
  char * title;
  char * publication;
  char * publisher;
  char * date;
  char * pages;
} Bibliography;

//command line options options table for our input
//Got the idea from StackOverFlow
//https://stackoverflow.com/questions/7489093/getopt-long-proper-way-to-use-it
static struct option long_options[] =
{
  {"o", required_argument, NULL, 'o'},
  {"output", required_argument, NULL, 'o'},
  {"s", required_argument, NULL, 's'},
  {"space", required_argument, NULL, 's'},
  {"h", no_argument, NULL, 'h'},
  {"help", no_argument, NULL, 'h'},
  {"v",no_argument, NULL, 'v'},
  {"verbose", no_argument, NULL, 'v'},
  {NULL,0,NULL,0}
};

int isVerbose = 0;
int hasOutput = 0;
int main(int argc, char *argv[]){
  Bibliography bib;
  char *output_file;
  int numPages = 0;

  int ch;
  //Check for the command line options in the input
  while ((ch = getopt_long(argc, argv, "o:outputs:spacehhelpvverbose", long_options, NULL)) != -1)
    {
        switch(ch)
	{
	      //if the optional tag is verbose make isVerbose true
	      case 'v':
		   isVerbose = 1;
	           break;

	      case 'h':
		   printUsage();
                   exit(0);
		   break;

	      case 'o':
		   hasOutput = 1;
		   output_file = optarg;
		   break;

	      case 's':
	      	   numPages = atoi(optarg);
		   if (numPages > 9) {
		      exitHandler(22);
		   } else if (numPages < 0) {
		      exitHandler(21);
		   }
		   break;

	      default:
		   exitHandler(99);
		   break;
	}
	
    }

  FILE *fp;
  errno = 0;
  printVerbose("Looking for bib.txt...", isVerbose);
  if (((fp=fopen("bib.txt", "r")) != NULL))
  {
    printVerbose("Found open bib.txt", isVerbose);
    
    //Check for the size of File
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    printVerbose("Checking if file is empty...", isVerbose);
    if (0 == size) {
       fclose(fp);
       exitHandler(4);
    } 
    printVerbose("File is not empty...", isVerbose);
    //retrun to begeining of file
    fseek(fp, 0, SEEK_SET);
        
    size_t len = 32;
    ssize_t read;
    char * line = NULL;
    int count = 0;
    printVerbose("Reading bib.txt...", isVerbose);
    while ((read = getline(&line, &len, fp)) != -1) {
      switch(count) {
	  case 0:
	        bib.author = malloc(strlen(line));
	        strcpy(bib.author, line);
		count++;
		break;
          case 1:
	        bib.title = malloc(strlen(line));
		strcpy(bib.title, line);
		count++;
		break;
          case 2:
	        bib.publication = malloc(strlen(line));
		strcpy(bib.publication, line);
		count++;
		break;
          case 3:
	        bib.publisher = malloc(strlen(line));
	 	strcpy(bib.publisher, line);
		count++;
		break;
          case 4:
	        bib.date = malloc(strlen(line));
		strcpy(bib.date, line);
		count++;
		break;
          case 5:
	        bib.pages = malloc(strlen(line));
		strcpy(bib.pages, line);
		count++;
		break;
          default:
	    count = 0;
	    break;
	}
      
    }
    
    fclose(fp);
    if (line)
        free(line);

    if (0 == hasOutput)
    {//the user has not specified, print to screen
       fprintf(stdout, "List of Bibliography(s)");
       fprintf(stdout, "Author: %s", bib.author);
       fprintf(stdout, "Title: %s", bib.title);
       fprintf(stdout, "Publication: %s", bib.publication);
       fprintf(stdout, "Publisher: %s", bib.publisher);
       fprintf(stdout, "Date: %s", bib.date);
       fprintf(stdout, "Pages: %s", bib.pages);
    }
    else
    {//The user specified the output file
       if ((fp=fopen(output_file, "r")) == NULL)  
       {
          fp=fopen(output_file, "w+");
	  char * curMessage = "Writing to file ";
	  char * finalMessage = malloc(strlen(curMessage)+strlen(output_file));
	  strcpy(finalMessage, curMessage);
	  strcat(finalMessage, output_file);
	  printVerbose(finalMessage, isVerbose);
	  fprintf(fp, "%s", bib.author);
	  fprintf(fp, "%s", bib.title);
	  fprintf(fp, "%s", bib.publication);
	  fprintf(fp, "%s", bib.publisher);
	  fprintf(fp,"%s", bib.date);
	  fprintf(fp,"%s", bib.pages);
	  int count = 0;
	  if (numPages > 0) {
	         while (count < numPages) {
	               fprintf(fp,";\n");
		       count++;
         	  }
	  }
       }
       else if (errno == EPERM || errno == EACCES)
       {
	 exitHandler(12); //File cannot be opened, permission or access error
       }
       else
       {
	 exitHandler(11); //file already exists
       }
     }
  }
  else if (errno == ENOENT)
  {
    exitHandler(1);
  }
  else if (errno == EPERM)
  {
    exitHandler(2);
  }
  else if (errno == EACCES)
  {
    exitHandler(3);
  }

  exitHandler(0);
  return 0; //Satisfies warnings, theoreticly never hit
}

//Check if the flag is set and then print
void printVerbose(char* message, int isVerbose) {
  if (isVerbose)
    fprintf(stdout, "%s\n", message);
       
}

//if Help is called print the Usage of the Program
void printUsage() {
  printf("Usage: bib <bib.txt> <COMMAND_LINE_OPTIONS> <ARGS>\n");
  printf("-o, --output <output_file> specifies the file to which the bib entried are to be written\n");
  printf("-s, --space  <num>         specifies the number of lines to be spread apart the bib entried, 1 <= range <= 9\n");
  printf("-h, --help   <no_argument> shows how to use program\n");
}


void exitHandler(int code) {
  switch(code) {
      case 0:
	fprintf(stdout, "successful execution, no errors\n");
	break;

      case 1:
	fprintf(stderr, "bib.txt does not exist\n");
	printUsage();
	break;

      case 2:
	fprintf(stderr, "bib.txt is not a regular file\n");
	break;

      case 3:
	fprintf(stderr, "bib.txt is not readable\n");
	break;

      case 4:
	fprintf(stderr, "bib.txt has size zero\n");
	break;

      case 11:
	fprintf(stderr, "output_file already exists\n");
	break;
	
      case 12:
	fprintf(stderr, "output_file cannot be opened\n");
	break;

      case 21:
	fprintf(stderr, "argument following -s is not a positive integer\n");
	break;

      case 22:
	fprintf(stderr, "argument following -s is > 9\n");
	break;

      case 99:
	fprintf(stderr, "invalid or missing argument\n");
	printUsage();
	break;
  }
  
  fprintf(stdout, "Exit Code: %d\n", code);
  exit(code);
}
