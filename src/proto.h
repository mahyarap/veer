/*
 * Function prototypes and extern variables are kept here
 */

#ifndef PROTO_H
#define PROTO_H

#include "veer.h"

/* Global variables */
extern WINDOW *mainwin;
extern WINDOW *statbar;
extern WINDOW *bottwin;
extern Curwin curwin;

extern Buffer *firstbuf;
extern Buffer *lastbuf;
extern Buffer *curbuf;

extern int error;

/* Functions prototypes */

/* veer.c */
void do_input();
void finish();
void init_terminal();
void init_window();
void help();

/* file.c */
Buffer *new_buffer();
void push_back_buffer(const char *path);
void do_prev_buf();
void do_next_buf();
Line *new_line();
void delete_line(Line *line);
FILE *open_file(const char *path, const char *mode);
void open_buffer(const char* path);
void push_back_line(const Line *line);
void insert_line(Line *ptr, const Line *line);
void read_into_buffer(FILE* fs);
void save_buffer();
void erase_line();
void buffer_modified(bool modified);

/* move.c */
void go_up();
void go_down();
void go_left();
void go_right();
void go_beg();
void go_end();

/* utils.c */
int visual2real(const int visualx);
int real2visual(const int realx);
char *charalloc(size_t size);
char *charrealloc(char *ptr, size_t size);
char *file_name(const char *path);

/* winio.c */
int get_input(WINDOW *win, bool *short_cut, bool *action_key);
void clear_line(WINDOW *win, int y);
void print_buffer(Line *beg);
void scrol(Direction dir);
void print_line(Line *line);
void update_statbar();
void position_cursor(WINDOW *win, int y, int x);
void reset_cursor();
void display_buffer();
void clear_win(WINDOW *win);
void clear_allwin();
void switch_win(Curwin cur);

/* text.c */
void do_enter();
void do_backspace();
void insert_char(const char c);

/* prompt.c */
Response prompt_ync(const char *question, ...);
char *prompt_str(const char *msg, ...);
void print_msg_prompt(const char *msg, ...);

#endif

