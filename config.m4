dnl $Id$
dnl config.m4 for extension opdumper

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(opdumper, for opdumper support,
dnl Make sure that the comment is aligned:
dnl [  --with-opdumper             Include opdumper support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(opdumper, whether to enable opdumper support,
Make sure that the comment is aligned:
[  --enable-opdumper           Enable opdumper support])

if test "$PHP_OPDUMPER" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-opdumper -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/opdumper.h"  # you most likely want to change this
  dnl if test -r $PHP_OPDUMPER/$SEARCH_FOR; then # path given as parameter
  dnl   OPDUMPER_DIR=$PHP_OPDUMPER
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for opdumper files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       OPDUMPER_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$OPDUMPER_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the opdumper distribution])
  dnl fi

  dnl # --with-opdumper -> add include path
  dnl PHP_ADD_INCLUDE($OPDUMPER_DIR/include)

  dnl # --with-opdumper -> check for lib and symbol presence
  dnl LIBNAME=opdumper # you may want to change this
  dnl LIBSYMBOL=opdumper # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $OPDUMPER_DIR/lib, OPDUMPER_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_OPDUMPERLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong opdumper lib version or lib not found])
  dnl ],[
  dnl   -L$OPDUMPER_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(OPDUMPER_SHARED_LIBADD)

  PHP_NEW_EXTENSION(opdumper, opdumper.c, $ext_shared)
fi
