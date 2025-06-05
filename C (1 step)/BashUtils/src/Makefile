CC = gcc
CFLAGS = -Wall -Werror -Wextra -Icommon -std=c11

SRC_CAT = common/s21_common.c cat/s21_cat.c
SRC_GREP = common/s21_common.c grep/s21_grep.c
OBJ_CAT = $(SRC_CAT:.c=.o)
OBJ_GREP = $(SRC_GREP:.c=.o)
TARGET_CAT = cat/s21_cat
TARGET_GREP = grep/s21_grep

.PHONY: all clean rebuild s21_cat s21_grep clean_all

all: s21_cat s21_grep

s21_cat: $(TARGET_CAT)

s21_grep: $(TARGET_GREP)

$(TARGET_CAT): $(OBJ_CAT)
	$(CC) -o $@ $^

$(TARGET_GREP): $(OBJ_GREP)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean_all:
	rm -rf $(OBJ_CAT) $(OBJ_GREP) $(TARGET_GREP) $(TARGET_CAT)

clean:
	rm -rf $(OBJ_CAT) $(OBJ_GREP)

rebuild: clean_all all