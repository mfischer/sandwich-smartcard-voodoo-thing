%module swig_shop
%include typemaps.i
%{
#include "swig_shop.h"
%}

void buy_python (char * global_priv_file, char* shop_priv_file, char* shop_name);

char *read_log_python (char *global_priv_file, char *keydir, int number, int *OUTPUT, int *OUTPUT);

