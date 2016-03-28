#include <config.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
 
#include <expat.h>
 
#define BUFF_SIZE 8192
 
int parse_file(file, coding)
  char * file;
  char * coding;
{
  
  FILE * f = fopen(file, "r");
  if (f == NULL)
    {
      fprintf(stderr, "Error while parsing %s: %s\n", file, strerror(errno));
      return XML_STATUS_ERROR;
    }
  

  fclose(f);
}

