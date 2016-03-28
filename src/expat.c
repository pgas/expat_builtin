/* xml parser based on expat as builtin to be dynamically loaded with enable -f and create a new
   builtin. */

#include <config.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"
#include "common.h"

#include "parse.h"

static char * encoding ();


int
expat_builtin (list)
     WORD_LIST *list;
{
  int c;
  char * start_callback = NULL;
  char * charset;
  
  reset_internal_getopt ();
  while ((c = internal_getopt (list, "s:")) != -1)
    {
      switch (c)
	{
	case 's':
	  start_callback = list_optarg;
	  break;
	case GETOPT_HELP:
	  builtin_help (); 
	  return (EX_USAGE);
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
      
    }
  list = loptend;
  if (list == NULL)
    {
      	  builtin_usage ();
	  return (EX_USAGE);

    }
  charset = encoding();
  printf("expat parsing callback: %s\n", start_callback);
  fflush (stdout);

  while (list != NULL)
    {
      parse_file(list->word->word, charset);
      list = list->next;
    }
    
  return (EXECUTION_SUCCESS);
}

int
expat_builtin_load (s)
     char *s;
{
  printf ("expat builtin loaded\n");
  fflush (stdout);
  return (1);
}

void
expat_builtin_unload (s)
     char *s;
{
  printf ("expat builtin unloaded\n");
  fflush (stdout);
}

/* An array of strings forming the `long' documentation for a builtin xxx,
   which is printed by `help xxx'.  It must end with a NULL.  By convention,
   the first line is a short description. */
char *expat_doc[] = {
	"Sample builtin.",
	"",
	"this is the long doc for the sample expat builtin",
	(char *)NULL
};

/* The standard structure describing a builtin command.  bash keeps an array
   of these structures.  The flags must include BUILTIN_ENABLED so the
   builtin can be used. */
struct builtin expat_struct = {
	"expat",		/* builtin name */
	expat_builtin,		/* function implementing the builtin */
	BUILTIN_ENABLED,	/* initial flags for builtin */
	expat_doc,		/* array of long documentation strings. */
	"expat",		/* usage synopsis; becomes short_doc */
	0			/* reserved for internal use */
};


/* Declarations for functions defined in locale.c */
extern char *get_locale_var __P((char *));

/*from fnxform.c*/
static char *
curencoding ()
{
  char *loc;
#if defined (HAVE_LOCALE_CHARSET)
  loc = (char *)locale_charset ();
  return loc;
#else
  char *dot, *mod;

  loc = get_locale_var ("LC_CTYPE");
  if (loc == 0 || *loc == 0)
    return "";
  dot = strchr (loc, '.');
  if (dot == 0)
    return loc;
  mod = strchr (dot, '@');
  if (mod)
    *mod = '\0';
  return ++dot;
#endif
}  

static char *
encoding () {
  char * cur = curencoding();
  if ((strncmp(cur, "UTF-8",5) != 0) &&
      (strncmp(cur, "ISO-8859-1",10) != 0))
    {
      fprintf(stderr, "warning: unsupported encoding %s defaulting to ISO8859-1\n");
      cur = "ISO-8859-1";
    }
  return cur;
}
