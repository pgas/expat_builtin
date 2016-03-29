#include <config.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
 
#include "parse.h"


#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"
#include "common.h"


/* let's explore gettext later */
#define _(String) String

 
#define BUFF_SIZE 8192

typedef struct _callbacks {
  char * start;
  char * end;
  char * data;
  char * comment;
} callbacks;

void XMLCALL startElementHandler(userData, name, atts) 
     void *userData;
     const XML_Char *name;
     const XML_Char **atts;
{  
  /* don't save the code in history*/
  int flags =  SEVAL_NOHIST;
  int i;
  SHELL_VAR *attributes;

  bind_variable("XML_NAME", savestring(name), 0);
  unbind_variable("XML_ATTRIBUTES");
  attributes = make_new_assoc_variable("XML_ATTRIBUTES");  
  for (i=0;atts[i] != NULL;i+=2) 
    {
      bind_assoc_variable (attributes, 
			   "XML_ATTRIBUTES",
			   savestring(atts[i]),
			   savestring(atts[i+1]),
			   0);
    }
  evalstring (savestring(((callbacks *)userData)->start), NULL, flags);
}

void XMLCALL endElementHandler(userData, name) 
     void *userData;
     const XML_Char *name;
{  
  /* don't save the code in history*/
  int flags =  SEVAL_NOHIST;
  bind_variable("XML_NAME", savestring(name), 0);
  evalstring (savestring(((callbacks *)userData)->end), NULL, flags);
}


void XMLCALL characterDataHandler(userData, s, len)
     void *userData;
     const XML_Char *s;
     int len;
{
  /* s doesn't contain tbe terminal  null byte*/
  char * string = xmalloc(len+1);
  string[len] = '\0';
  strncpy(string, s, len);
  int flags =  SEVAL_NOHIST;
  bind_variable("XML_DATA", string, 0);
  evalstring (savestring(((callbacks *)userData)->data), NULL, flags);
}


void XMLCALL commentHandler(userData, s)
     void *userData;
     const XML_Char *s;
{
  /* s doesn't contain tbe terminal  null byte*/
  int flags =  SEVAL_NOHIST;
  bind_variable("XML_COMMENT", savestring(s), 0);
  evalstring (savestring(((callbacks *)userData)->comment), NULL, flags);
}


void setHandlers(p, c)
     XML_Parser p;
     callbacks* c;
{
  XML_SetUserData(p, c);
  if (c->start != NULL) 
    {
      XML_SetStartElementHandler(p, startElementHandler);
    }
  if (c->end != NULL) 
    {
      XML_SetEndElementHandler(p, endElementHandler);
    }
  if (c->data != NULL) 
    {
      XML_SetCharacterDataHandler(p, characterDataHandler);
    }
  if (c->comment != NULL) 
    {
      XML_SetCommentHandler(p, commentHandler);
    }

}

XML_Parser getParser(coding, start, end, data, comment) 
     char * coding;
     char * start;
     char *end;
     char *data;
     char *comment;
{
  XML_Parser p = XML_ParserCreate(coding);
  callbacks *c = (callbacks *) xmalloc(sizeof(callbacks));
  c->start = start;
  c->end = end;
  c->data = data;
  c->comment = comment;
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

