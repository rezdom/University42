#ifndef S21_COMMON_H
#define S21_COMMON_H

#include <stdlib.h>

int check_flags(int argc, char **argv);
void free_space_char(char **str_list, int n);
char **add_null(char **str_list, int files_count, int count);
char **upgrade_space_char(char **str_list, int *count);
int listlen(char **str_list);

#endif