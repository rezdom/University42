#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s21_common.h"

#define MAX_STR 4096
#define MIN_STR_LIST 5

typedef struct {
  int b_flag;
  int e_flag;
  int n_flag;
  int s_flag;
  int t_flag;
  int v_flag;

} Flags;

void free_memory(char **str1, char **str2);
void check_error(int error);
void files_work(char *file_name, Flags *cat_flags);
int check_files(int argc, char **argv);
void reader(FILE *file, Flags *cat_flags);
void non_visible_print(char *line, Flags *cat_flags);
void flag_v_work(char c);
int is_empty(char *str);
char **get_files(int argc, char **argv);
char **get_flags_list(int argc, char **argv);
int change_flags(char **flags_list, Flags *flags);
int change_long_flag(char *flag, Flags *flags);
int change_short_flag(char *flag, Flags *flags);

static inline void set_b_flag(Flags *f) { f->b_flag = 1; }

static inline void set_n_flag(Flags *f) { f->n_flag = 1; }

static inline void set_s_flag(Flags *f) { f->s_flag = 1; }

static inline void set_e_flag(Flags *f) {
  f->e_flag = 1;
  f->v_flag = 1;
}
static inline void set_E_flag(Flags *f) { f->e_flag = 1; }

static inline void set_t_flag(Flags *f) {
  f->t_flag = 1;
  f->v_flag = 1;
}
static inline void set_T_flag(Flags *f) { f->t_flag = 1; }

static inline void set_v_flag(Flags *f) { f->v_flag = 1; }
#endif