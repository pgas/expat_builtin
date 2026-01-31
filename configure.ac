AC_INIT([expat],[0.1])

# Many autoconf project creates a config.h.  however the bash headers
# already use their own version of config.h that, since bash 4.4, is
# installed in include/bash/. As a result you shoud NOT include the
# classic AC_CONFIG_HEADERS([config.h]) choose another name
# if needed ... (the provided config.h already has a a lot)

AM_INIT_AUTOMAKE

# we only want to build a shared library
LT_INIT([dlopen disable-static])

# we don't need ldconfig after installing the .so
AC_SUBST([finish_cmds], [""])

#install the .so in $(libdir)/bash
AC_SUBST([libdir], ["$libdir/bash"])

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([src/expat.c])

AC_PROG_CC
AC_PROG_INSTALL

# an option to tell configure where the bash headers are
AC_ARG_WITH([bash],
              [AS_HELP_STRING([--with-bash],[The prefix where the bash headers are installed (default is /usr/include)])],
              [with_bash=$withval],
              [with_bash="/usr/include"])

# try with the bash subdir created when installing headers (bash>=4.4)
# or without if someone tries to link against the sources directly
if test -e $with_bash/bash/builtins.h
  then
   CPPFLAGS="$CPPFLAGS -I$with_bash/bash -I$with_bash/bash/include -I$with_bash/bash/builtins"
  else
CPPFLAGS="$CPPFLAGS -I$with_bash -I$with_bash/include -I$with_bash/builtins"
fi
    
AC_CHECK_HEADERS([builtins.h],[bashfound=yes], [])
if test x"$bashfound" = x
 then
   AC_MSG_ERROR([unable to find the bash headers please use --with-bash=/path/to/headers/])

fi		     

AC_CHECK_HEADERS([expat.h],[],[])
AC_CHECK_LIB([expat],[XML_Parse],[])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT


