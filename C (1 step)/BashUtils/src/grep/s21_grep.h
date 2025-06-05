#ifndef S21_GREP_H
#define S21_GREP_H

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s21_common.h"

#define MIN_LIST 5
#define MAX_STR 4096

typedef struct {
  int e_flag;
  int i_flag;
  int v_flag;
  int c_flag;
  int l_flag;
  int n_flag;
  int solo_flag;
} Flags;

void free_memory(char **str1, char **str2, char **str3);
void check_error(int error);
int check_files(int argc, char **argv, Flags *grep_flags);
int check_templates(int argc, char **argv, Flags *grep_flags);

void work_with_file(Flags *grep_flags, char **templates, char *file_ptr);
void reader(FILE *file, Flags *grep_flags, char **templates, char *file_name);
int get_match(Flags *grep_flags, char *line, char **templates);

int is_template(char *arg1, char *arg2, int item, Flags *grep_flags);
char **get_flags(int argc, char **argv);
char **get_templates(int argc, char **argv, Flags *grep_flags);
char **get_files(int argc, char **argv, Flags *grep_flags);
int t_comparison(int argc, char **argv, Flags *grep_flags);
int eq(char *str);

int change_flags(char **flag_list, Flags *grep_flags);
int ewf(char *str, Flags *grep_flags);

static inline void set_e_flag(Flags *f) { f->e_flag = 1; }

static inline void set_i_flag(Flags *f) { f->i_flag = 1; }

static inline void set_v_flag(Flags *f) { f->v_flag = 1; }

static inline void set_c_flag(Flags *f) { f->c_flag = 1; }

static inline void set_l_flag(Flags *f) { f->l_flag = 1; }

static inline void set_n_flag(Flags *f) { f->n_flag = 1; }

static inline void set_solo_flag(Flags *f) { f->solo_flag = 1; }

#endif