#ifndef __PARSE_H_
#define __PARSE_H_
#include <expat.h>

XML_Parser getParser(char * coding, char * start, char *end, char *data, char* comment);
void resetParser(XML_Parser p, char * coding);
void freeParser(XML_Parser p);
int parse_file(char * file, XML_Parser p);

#endif
