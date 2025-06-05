#include "s21_grep.h"

int main(int argc, char **argv) {
  int flag_error = 0;
  Flags grep_flags = {0};
  char **flag_list = get_flags(argc, argv);
  if (flag_list) {
    flag_error = change_flags(flag_list, &grep_flags);
  } else if (!flag_list && check_flags(argc, argv))
    flag_error = 2;

  if ((argc == 2 && flag_list) || argc < 2) flag_error = 5;
  if (!flag_error && !check_templates(argc, argv, &grep_flags)) flag_error = 5;
  if (!flag_error && !t_comparison(argc, argv, &grep_flags)) flag_error = 5;

  char **templates = NULL;
  if (!flag_error) {
    templates = get_templates(argc, argv, &grep_flags);
    if (!templates && check_templates(argc, argv, &grep_flags)) flag_error = 3;
  }

  char **files_list = NULL;
  if (!flag_error) {
    files_list = get_files(argc, argv, &grep_flags);
    if (!files_list && check_files(argc, argv, &grep_flags)) flag_error = 4;
  }

  if (!flag_error && templates) {
    if (files_list) {
      char **file_ptr = files_list;
      for (int i = 0; files_list[i] != NULL; i++) {
        if (i > 0) set_solo_flag(&grep_flags);
      }
      while (*file_ptr) {
        work_with_file(&grep_flags, templates, *file_ptr);
        file_ptr++;
      }
    } else {
      reader(stdin, &grep_flags, templates, "(STDIN)");
    }
  }
  check_error(flag_error);
  free_memory(flag_list, templates, files_list);
  return 0;
}

void free_memory(char **str1, char **str2, char **str3) {
  if (str1) free_space_char(str1, listlen(str1));
  if (str2) free_space_char(str2, listlen(str2));
  if (str3) free_space_char(str3, listlen(str3));
}

void check_error(int error) {
  if (error == 2)
    printf("s21_grep: Get flags memory error!\n");
  else if (error == 3)
    printf("s21_grep: Get templates memory error!\n");
  else if (error == 4)
    printf("s21_grep: Get files memory error!\n");
  else if (error == 5)
    printf("s21_grep Usage: ./s21_grep [OPTION]... PATTERN [FILE]...\n");
}

int check_files(int argc, char **argv, Flags *grep_flags) {
  int flag = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-' &&
        !is_template(argv[i], argv[i - 1], i, grep_flags)) {
      flag = 1;
      break;
    }
  }
  return flag;
}

int check_templates(int argc, char **argv, Flags *grep_flags) {
  int flag = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-' && is_template(argv[i], argv[i - 1], i, grep_flags)) {
      flag = 1;
      break;
    }
  }
  return flag;
}

void work_with_file(Flags *grep_flags, char **templates, char *file_ptr) {
  FILE *file = fopen(file_ptr, "r");
  if (file) {
    reader(file, grep_flags, templates, file_ptr);
    fclose(file);
  } else
    printf("s21_grep: %s: No such file or directory\n", file_ptr);
}

void reader(FILE *file, Flags *grep_flags, char **templates, char *file_name) {
  char line[MAX_STR];
  int line_num = 0, match_count = 0;
  while (fgets(line, sizeof(line), file)) {
    line_num++;
    int match = get_match(grep_flags, line, templates);
    if ((match && !grep_flags->v_flag) || (!match && grep_flags->v_flag)) {
      match_count++;
      if (grep_flags->l_flag) {
        printf("%s\n", file_name);
        break;
      }
      if (grep_flags->c_flag) continue;
      if (grep_flags->solo_flag) printf("%s:", file_name);
      if (grep_flags->n_flag) printf("%d:", line_num);
      fputs(line, stdout);
      if (strchr(line, '\n') == NULL) putchar('\n');
    }
  }
  if (!grep_flags->l_flag && grep_flags->c_flag) {
    if (grep_flags->solo_flag) printf("%s:", file_name);
    printf("%d\n", match_count);
  }
}

int get_match(Flags *grep_flags, char *line, char **templates) {
  int match = 0;
  regex_t regex;
  char **ptr = templates;
  int cflags = REG_EXTENDED | (grep_flags->i_flag ? REG_ICASE : 0);

  while (*ptr) {
    if (regcomp(&regex, *ptr, cflags) != 0) {
      printf("Failed to compile regex: %s\n", *ptr);
      break;
    }
    match = regexec(&regex, line, 0, NULL, 0) == 0;
    regfree(&regex);
    if (match) break;
    ptr++;
  }
  return match;
}

int is_template(char *arg1, char *arg2, int item, Flags *grep_flags) {
  int flag = 0;
  if (!grep_flags->e_flag) {
    if (item == 1 && arg1[0] != '-')
      flag = 1;
    else if (item == 2 && arg2[0] == '-' && arg1[0] != '-')
      flag = 1;
  }
  for (int i = 1; grep_flags->e_flag && i < (int)strlen(arg2) && !flag; i++) {
    if (arg2[0] == '-') {
      if (arg2[i] == 'e') {
        flag = 1;
        break;
      }
    } else
      break;
  }
  return flag;
}

char **get_flags(int argc, char **argv) {
  int error = 0;
  int count = MIN_LIST, flags_count = 0;
  char **flags_list = malloc(count * sizeof(char *));
  if (flags_list) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-') continue;
      flags_list[flags_count] = malloc(strlen(argv[i]) + 1);
      if (!flags_list[flags_count]) {
        free_space_char(flags_list, count);
        error = 1;
      }
      if (!error) {
        strcpy(flags_list[flags_count], argv[i]);
        flags_count++;
      }

      if (!error && flags_count == count - 1) {
        flags_list = upgrade_space_char(flags_list, &count);
        if (!flags_list) error = 1;
      }
      if (error) break;
    }
  } else
    error = 1;

  if (error) {
    flags_list = NULL;
  } else {
    if (flags_count > 0)
      flags_list = add_null(flags_list, flags_count, count);
    else {
      free(flags_list, count);
      flags_list = NULL;
    }
  }
  return flags_list;
}

char **get_templates(int argc, char **argv, Flags *grep_flags) {
  int error = 0;
  int count = MIN_LIST, templ_count = 0;
  char **templ_list = malloc(count * sizeof(char *));
  if (templ_list) {
    if (!grep_flags->e_flag) {
      int first_temple_index = (argv[1][0] == '-') ? 2 : 1;
      templ_list[templ_count] = malloc(strlen(argv[first_temple_index]) + 1);
      if (!templ_list[templ_count]) {
        free_space_char(templ_list, count);
        error = 1;
      } else {
        strcpy(templ_list[templ_count], argv[first_temple_index]);
        templ_count += 1;
      }
    }
    for (int i = 1; i < argc && grep_flags->e_flag; i++) {
      if (!is_template(argv[i], argv[i - 1], i, grep_flags)) continue;
      templ_list[templ_count] = malloc(strlen(argv[i]) + 1);
      if (!templ_list[templ_count]) {
        free_space_char(templ_list, count);
        error = 1;
      }
      if (!error) {
        strcpy(templ_list[templ_count], argv[i]);
        templ_count++;
      }
      if (!error && templ_count == count - 1) {
        templ_list = upgrade_space_char(templ_list, &count);
        if (!templ_list) error = 1;
      }
      if (error) break;
    }
  } else
    error = 1;

  if (error)
    templ_list = NULL;
  else
    templ_list = add_null(templ_list, templ_count, count);
  return templ_list;
}

char **get_files(int argc, char **argv, Flags *grep_flags) {
  int error = 0;
  int count = MIN_LIST, files_count = 0;
  char **files = malloc(count * sizeof(char *));
  if (files) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-' || is_template(argv[i], argv[i - 1], i, grep_flags))
        continue;
      files[files_count] = malloc(strlen(argv[i]) + 1);
      if (!files[files_count]) {
        free_space_char(files, count);
        error = 1;
      }
      if (!error) {
        strcpy(files[files_count], argv[i]);
        files_count++;
      }

      if (!error && files_count == count - 1) {
        files = upgrade_space_char(files, &count);
        if (!files) error = 1;
      }
      if (error) break;
    }
  } else
    error = 1;

  if (error) {
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

int t_comparison(int argc, char **argv, Flags *grep_flags) {
  int t_count = 0, e_count = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      e_count += eq(argv[i]);
      continue;
    }
    if (!grep_flags->e_flag && (i == 1 || (i == 2 && e_count == 0))) {
      e_count += 1;
      t_count += 1;
      continue;
    }
    if (argv[i - 1][0] == '-' && eq(argv[i - 1])) {
      t_count += 1;
    }
  }
  return t_count == e_count;
}

int eq(char *str) {
  int flag = 0;
  for (int i = 1; i < (int)strlen(str); i++) {
    if (str[i] == 'e') {
      flag = 1;
      break;
    }
  }
  return flag;
}

int change_flags(char **flag_list, Flags *grep_flags) {
  int error = 0;
  char **ptr = flag_list;

  while (*ptr) {
    error = ewf(*ptr, grep_flags);
    if (error) break;
    ptr++;
  }
  return error;
}

int ewf(char *str, Flags *grep_flags) {
  int error = 1;
  struct {
    char name;
    void (*set)(Flags *f);
  } options[] = {{'e', set_e_flag}, {'i', set_i_flag}, {'v', set_v_flag},
                 {'c', set_c_flag}, {'l', set_l_flag}, {'n', set_n_flag},
                 {'\0', NULL}};
  for (int i = 1; i < (int)strlen(str); i++) {
    for (int j = 0; options[j].name != '\0'; j++) {
      if (options[j].name == str[i]) {
        options[j].set(grep_flags);
        error = 0;
        continue;
      }
    }
    if (error) {
      if ((str)[1] == '-')
        printf("s21_grep: unrecognized option '%s'\n", str);
      else
        printf("grep: invalid option -- '%c'\n", str[i]);
      break;
    }
    if (i != (int)strlen(str) - 1) error = 1;
  }

  return error;
}