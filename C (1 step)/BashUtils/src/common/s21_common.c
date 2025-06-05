#include "s21_common.h"

int check_flags(int argc, char **argv) {
  int flag = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      flag = 1;
      break;
    }
  }
  return flag;
}

void free_space_char(char **str_list, int n) {
  for (int i = 0; i < n; i++) {
    free(str_list[i]);
  }
  free(str_list);
}

char **add_null(char **str_list, int files_count, int count) {
  if (files_count == count) {
    char **new_list = realloc(str_list, (files_count + 1) * sizeof(char *));
    if (!new_list) {
      free_space_char(str_list, count);
      new_list = NULL;
    }
    str_list = new_list;
  }
  if (str_list) {
    str_list[files_count] = NULL;
  }
  return str_list;
}

char **upgrade_space_char(char **str_list, int *count) {
  *count *= 2;
  char **new_files = realloc(str_list, *count * sizeof(char *));
  if (!new_files) free_space_char(str_list, *count);
  return new_files;
}

int listlen(char **str_list) {
  char **ptr = str_list;
  int count = 0;
  while (*ptr) {
    count += 1;
    ptr++;
  }
  return (count + 1);
}