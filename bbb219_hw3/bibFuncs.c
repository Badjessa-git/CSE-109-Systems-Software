/**
 *  Badjessa B. Bahoumda
 *  CSE 271
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "bibFuncs.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

struct BibEntry_t *header;
int first = 0;
//Planning to use the access functions to check the different
//permsission and return 0 only if the file is present nonempty and readable
int findFile(char *fileName, int isVerbose)
{
  errno = 0;
  if (access(fileName, R_OK) == 0 && access(fileName, W_OK) == 0 && access(fileName, X_OK) == 0)
  {
    FILE *fp;
    if (((fp=fopen(fileName, "r")) != NULL))
      {
	printVerbose("Found bib.txt", isVerbose);

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

	fclose(fp);
      return 0;
      }
    
    else
    {
      exitHandler(11);
    }
	
  }
  else
  {
    switch(errno) {
      case EACCES:
      case EROFS:
	   exitHandler(10);
	   break;

      case ENOENT:
	   exitHandler(2);
           break;
    }
    return -1;
  }
  return 0;
}

//Allocate space for a new BibEntry and set the pointer to the next
//Entry to null
struct BibEntry_t *createBib(char *curTitle, char *curAuthor, char *curPublication, char *curPublisher, char *curDate, char *curPages) {
  struct BibEntry_t *newBib = malloc(sizeof(struct BibEntry_t));
  if (strlen(curTitle) > 0) {
    newBib->title = malloc(strlen(curTitle));
    strcpy(newBib->title, curTitle);
  }

  if (strlen(curAuthor) > 0) {
    newBib->author = malloc(strlen(curAuthor));
    strcpy(newBib->author, curAuthor);
  }

  if (strlen(curPublication) > 0) {
    newBib->publication = malloc(strlen(curPublication));
    strcpy(newBib->publication, curPublication);
  }

  if (strlen(curPublisher) > 0) {
    newBib->publisher = malloc(strlen(curPublisher));
    strcpy(newBib->publisher, curPublisher);
  }

  if (strlen(curDate) > 0) {
    newBib->date = malloc(strlen(curDate));
    strcpy(newBib->date, curDate);
  }
  
  if (strlen(curPages) > 0) {
    newBib->pages = malloc(strlen(curPages));
    strcpy(newBib->pages, curPages);
  }
  
  newBib->next = NULL;

  return newBib;
}


//Read the bib file and create BibEntry by calling the bibEntry functions
int readBibFile(char *fileName, int isVerbose)
{
  int first = 0;
  FILE *fp;
  if (((fp=fopen(fileName, "r")) != NULL))
    {
      size_t len = 140;
      ssize_t read;
      char * line = NULL;
      char *curTitle = malloc(128*sizeof(char));
      char *curAuthor = malloc(64*sizeof(char));
      char *curPublication = malloc(32*sizeof(char));
      char *curPublisher = malloc(32*sizeof(char));
      char *curDate = malloc(11*sizeof(char));
      char *curPages = malloc(16*sizeof(char));

      struct BibEntry_t *curEntry;
      while ((read = getline(&line, &len, fp)) != -1)
      {
	if (strstr(line, "Title: ") != NULL)
	{
	  strncpy(curTitle, line+7, 127);
	}
	else if (strstr(line, "Author: ") != NULL)
        {
	  strncpy(curAuthor, line+8, 63);
	  
	}
	else if (strstr(line, "Publication: ") != NULL)
        {
          strncpy(curPublication, line+13, 32);
	    
	}
	else if (strstr(line, "Publisher: ") != NULL)
        {
	  strncpy(curPublisher, line+11, 16);
	  
	}
	else if (strstr(line, "Date: ") != NULL)
        {
	  strncpy(curDate, line+6, 11);
	  
	}
	else if (strstr(line, "Pages: ") != NULL)
        {
	  strncpy(curPages, line+7, 11);
	  
        }
	else if (strstr(line, ";") != NULL)
        {
	  printVerbose("Creating new BibEntry..." , isVerbose);
	  //if header does not exist, create the first bibEntry
	  if (first == 0) {
	    header = curEntry = malloc(sizeof(struct BibEntry_t));
	    header = createBib(curTitle, curAuthor, curPublication, curPublisher, curDate, curPages);
	    curEntry =  header;
	    first = 1;
	  }
	  //else create a nextNode, assign a point from header(curNode
	  //to it) and move header pointer to that current node
	  else {
	    curEntry->next = malloc(sizeof(struct BibEntry_t));
	    curEntry->next = createBib(curTitle, curAuthor, curPublication, curPublisher, curDate, curPages);
	    curEntry = curEntry->next;
	  }
	}
	else
        {
	  exitHandler(11);
	}
      }
      if (line) {
	  free(line);
      }

      free(curTitle);
      free(curAuthor);
      free(curPublication);
      free(curPublisher);
      free(curDate);
      free(curPages);	
      return 0;
   }
  return -1;
}

//Prints the current bibEntry
//Implemented recursively, print the curEntry, check that there is a
//next entry and go to it to print it out
int printBib(struct BibEntry_t *curEntry, int mode, int space, int isVerbose) {
  printVerbose("Printing all the BibEntries...", isVerbose);
  mode = 0;
  while (curEntry != NULL)
  {
  
  if (curEntry->title != NULL)
      fprintf(stdout, "Title: %s", curEntry->title);

  if (curEntry->author != NULL)
      fprintf(stdout, "Author: %s", curEntry->author);

  if (curEntry->author != NULL)
      fprintf(stdout, "Publication: %s", curEntry->publication);

  if (curEntry->publisher != NULL)
      fprintf(stdout, "Publisher: %s", curEntry->publisher);

  if (curEntry->date != NULL)
      fprintf(stdout, "Date: %s", curEntry->date);

  if (curEntry->pages != NULL)
      fprintf(stdout, "Pages: %s", curEntry->pages);

  if (space > 0) {
    int count = 0;
    while(count < space) {
      printf("\n");
      count++;
     }
  } else {
    printf("\n");
  }
  curEntry = curEntry->next;
  }
  printf("\n");
  return 0;
}

//Free the space allocated for the current bibEntry
int freeBib(struct BibEntry_t *curEntry, int mode, int isVerbose) {
  if (curEntry != NULL)
  {  
  free(curEntry->title);
  free(curEntry->author);
  free(curEntry->publication);
  free(curEntry->publisher);
  free(curEntry->date);
  free(curEntry->pages);
  if (curEntry->next != NULL)
    freeBib(curEntry->next, mode, isVerbose);
  free(curEntry);
  }
  return 0;
}

//Check if the flag is set and then print
void printVerbose(char* message, int isVerbose) {
  if (isVerbose)
    fprintf(stdout, "%s\n", message);

}

void printUsage() {
    printf("Usage: bib <COMMAND_LINE_OPTIONS> <ARGS>\n");
    printf("-s, --space  <num>        specifies the number of lines to be spread apart the bib entried, 1 <= range <= 9\n");
    printf("-h, --help   <no_argument> shows how to use program\n");
    printf("-v. --verbose <no_argument> shows output\n");
}

//Handles error and exit
void exitHandler(int code) {
  switch(code) {
     case 0:
        fprintf(stdout, "successful execution, no errors\n");
        break;

     case 1:
        printUsage();
        break;

     case 10:
        fprintf(stderr, "bib.txt is present but not readable or accessible\n");
        break;

     case 11:
        fprintf(stderr, "bib.txt is empty or has malformed entry\n");
        break;

     case 12:
        fprintf(stderr, "bib.txt does not exist\n");
	break;
	
     case 20:
        fprintf(stderr, "argument following -s is not a positive integer\n");
        break;

     case 21:
        fprintf(stderr, "argument following -s is > 9\n");
        break;

     case 99:
        fprintf(stderr, "invalid or missing argument\n");
        printUsage();
        break;

     default:
        fprintf(stderr, "Some uninditified error occured\n");
  }

  fprintf(stdout, "Exit Code: %d\n", code);
  exit(code);
}
