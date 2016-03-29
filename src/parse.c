#include <config.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
 
#include "parse.h"

/* let's explore gettext later */
#define _(String) String

 
#define BUFF_SIZE 8192

typedef struct _callbacks {
  char * start;
  char * end;
  char * character;
} callbacks;

void XMLCALL startElementHandler(userData, name, atts) 
     void *userData;
     const XML_Char *name;
     const XML_Char **atts;
{
  printf("start element %s calling %s\n", name, ((callbacks *)userData)->start);
}

void setHandlers(p, c)
     XML_Parser p;
     callbacks* c;
{
  XML_SetUserData(p, c);
  if (c->start != NULL) {
    XML_SetStartElementHandler(p, startElementHandler);
  }
}

XML_Parser getParser(coding, start, end, character) 
     char * coding;
     char * start;
     char *end;
     char *character;
{
  XML_Parser p = XML_ParserCreate(coding);
  callbacks *c = (callbacks *) xmalloc(sizeof(callbacks));
  c->start = start;
  c->end = end;
  c->character = character;
  setHandlers(p, c);
  return p;
}

void resetParser(p, coding) 
     XML_Parser p;
     char * coding;
{
  callbacks * c = (callbacks *) XML_GetUserData(p);
  XML_ParserReset(p, coding);
  setHandlers(p, c);
}

void freeParser(p) 
  XML_Parser p;
{
  free(XML_GetUserData(p));
  XML_ParserFree(p);
}
 

int parse_file(file, p)
  char * file;
  XML_Parser p;
{  
  FILE * f = fopen(file, "r");
  if (f == NULL)
    {
      builtin_error (_("Error while parsing %s: %s\n"), file, strerror(errno));
      return XML_STATUS_ERROR;
    }
  size_t bytes_read = 1; 
  enum XML_Status status;
  while (bytes_read != 0) 
    {
      void *buff = XML_GetBuffer(p, BUFF_SIZE);
      bytes_read = fread(buff, 1, BUFF_SIZE, f);
      status = XML_ParseBuffer(p, bytes_read, bytes_read == 0);
      if (status == XML_STATUS_ERROR) 
	{
	  break;
	}
    }
  fclose(f);
}

