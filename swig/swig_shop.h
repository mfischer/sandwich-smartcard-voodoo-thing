#ifndef SHOP_SWIG_H
#define SHOP_SWIG_H

void buy_python (char * global_priv_file, char* shop_priv_file, char* shop_name);

char *read_log_python (char *global_priv_file,
									           char *keydir, int number,
														 int *out_sigok, int *out_count);
#endif /* SHOP_SWIG_H */
