#include "s21_cat.h"

int main(int argc, char **argv) {
  int flag_error = 0;
  Flags cat_flags = {0};
  char **flags_list = get_flags_list(argc, argv);
  if (flags_list) {
    flag_error = change_flags(flags_list, &cat_flags);
  } else if (!flags_list && check_flags(argc, argv))
    flag_error = 2;

  char **files_list = NULL;
  if (!flag_error) {
    files_list = get_files(argc, argv);
    if (!files_list && check_files(argc, argv)) flag_error = 3;
  }

  if (!flag_error) {
    if (files_list) {
      char **file_ptr = files_list;
      while (*file_ptr) {
        files_work(*file_ptr, &cat_flags);
        file_ptr++;
      }
    } else {
      reader(stdin, &cat_flags);
    }
  }

  free_memory(files_list, flags_list);
  check_error(flag_error);
  return 0;
}

void free_memory(char **str1, char **str2) {
  if (str1) free_space_char(str1, listlen(str1));
  if (str2) free_space_char(str2, listlen(str2));
}

void check_error(int error) {
  if (error == 2)
    printf("s21_cat: Get flags memory error!\n");
  else if (error == 3)
    printf("s21_cat: Get templates memory error!\n");
}

void files_work(char *file_name, Flags *cat_flags) {
  FILE *file = fopen(file_name, "r");
  if (file) {
    reader(file, cat_flags);
    fclose(file);
  } else
    printf("s21_cat: %s: No such file or directory\n", file_name);
}

int check_files(int argc, char **argv) {
  int flag = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      flag = 1;
      break;
    }
  }
  return flag;
}

void reader(FILE *file, Flags *cat_flags) {
  char line[MAX_STR];
  static int num_line = 1;
  int prev_empty = 0;

  while (fgets(line, sizeof(line), file)) {
    if (cat_flags->s_flag && is_empty(line)) {
      if (prev_empty) continue;
      prev_empty = 1;
    } else
      prev_empty = 0;

    if (cat_flags->b_flag && !is_empty(line)) {
      printf("%6d\t", num_line++);
    } else if (cat_flags->n_flag && !cat_flags->b_flag) {
      printf("%6d\t", num_line++);
    }

    if (cat_flags->e_flag || cat_flags->t_flag || cat_flags->v_flag)
      non_visible_print(line, cat_flags);
    else
      printf("%s", line);
  }
}

void non_visible_print(char *line, Flags *cat_flags) {
  int len = strlen(line);
  int has_newline = (len > 0 && line[len - 1] == '\n');

  for (int i = 0; i < len; i++) {
    if (cat_flags->t_flag && line[i] == '\t') {
      printf("^I");
      continue;
    }
    if (cat_flags->v_flag &&
        ((unsigned char)line[i] < 32 || (unsigned char)line[i] == 127 ||
         (unsigned char)line[i] > 127)) {
      if (line[i] == '\t') {
        putchar('\t');
        continue;
      }
      if (line[i] == '\n') continue;
      flag_v_work(line[i]);
      continue;
    }
    if (line[i] == '\n') continue;
    putchar(line[i]);
  }

  if (cat_flags->e_flag && has_newline) putchar('$');
  if (has_newline) putchar('\n');
}

void flag_v_work(char c) {
  if (c < 32 || c == 127) {
    printf("^%c", (c == 127) ? '?' : c + 64);
  } else {
    printf("M-^%c", c - 64);
  }
}

int is_empty(char *str) { return (str[0] == '\0' || str[0] == '\n'); }

char **get_files(int argc, char **argv) {
  int flag = 0;
  int count = MIN_STR_LIST, files_count = 0;
  char **files = malloc(count * sizeof(char *));
  if (files) {
    for (int i = 0; i < argc - 1; i++) {
      if (argv[i + 1][0] == '-') continue;
      files[files_count] = malloc(strlen(argv[i + 1]) + 1);
      if (!files[files_count]) {
        free_space_char(files, count);
        flag = 1;
      }
      if (!flag) {
        strcpy(files[files_count], argv[i + 1]);
        files_count++;
      }

      if (!flag && files_count == count - 1) {
        files = upgrade_space_char(files, &count);
        if (!files) flag = 1;
      }
      if (flag) break;
    }
  } else
    flag = 1;

  if (flag) {
    files = NULL;
  } else {
    if (files_count > 0)
      files = add_null(files, files_count, count);
    else {
      free(files, count);
      files = NULL;
    }
  }
  return files;
}

char **get_flags_list(int argc, char **argv) {
  int flag = 0;
  int count = MIN_STR_LIST, flags_count = 0;
  char **flags = malloc(count * sizeof(char *));
  if (flags) {
    for (int i = 0; i < argc - 1; i++) {
      if (argv[i + 1][0] != '-') continue;
      flags[flags_count] = malloc(strlen(argv[i + 1]) + 1);
      if (!flags[flags_count]) {
        free_space_char(flags, count);
        flag = 1;
      }
      if (!flag) {
        strcpy(flags[flags_count], argv[i + 1]);
        flags_count++;
      }

      if (!flag && flags_count == count - 1) {
        flags = upgrade_space_char(flags, &count);
        if (!flags) flag = 1;
      }
      if (flag) break;
    }
  } else
    flag = 1;

  if (flag) {
    flags = NULL;
  } else {
    if (flags_count > 0)
      flags = add_null(flags, flags_count, count);
    else {
      free(flags, count);
      flags = NULL;
    }
  }
  return flags;
}

int change_flags(char **flags_list, Flags *flags) {
  int error_flag = 0;
  char **ptr = flags_list;
  while (*ptr) {
    if ((*ptr)[1] == '-')
      error_flag = change_long_flag(*ptr, flags);
    else
      error_flag = change_short_flag(*ptr, flags);
    if (error_flag) {
      break;
    }
    ptr++;
  }
  return error_flag;
}

int change_long_flag(char *flag, Flags *flags) {
  int not_found = 1;
  struct {
    const char *name;
    void (*set)(Flags *f);
  } options[] = {{"--number-nonblank", set_b_flag},
                 {"--number", set_n_flag},
                 {"--squeeze-blank", set_s_flag},
                 {"\0", NULL}};

  for (int i = 0; strcmp(options[i].name, "\0") != 0; i++) {
    if (strcmp(flag, options[i].name) == 0) {
      options[i].set(flags);
      not_found = 0;
      break;
    }
  }
  if (not_found) printf("s21_cat: unrecognized option '%s'\n", flag);
  return not_found;
}

int change_short_flag(char *flag, Flags *flags) {
  int not_found = 1;
  struct {
    const char name;
    void (*set)(Flags *f);
  } options[] = {{'b', set_b_flag}, {'n', set_n_flag}, {'s', set_s_flag},
                 {'e', set_e_flag}, {'E', set_E_flag}, {'t', set_t_flag},
                 {'T', set_T_flag}, {'v', set_v_flag}, {'\0', NULL}};

  for (int i = 1; i < (int)strlen(flag); i++) {
    for (int j = 0; options[j].name != '\0'; j++) {
      if (options[j].name == flag[i]) {
        options[j].set(flags);
        not_found = 0;
        break;
      }
    }
    if (not_found) {
      printf("s21_cat: invalid option -- '%c'\n", flag[i]);
      break;
    }
    if (i != (int)strlen(flag) - 1) not_found = 1;
  }
  return not_found;
}