/*
 * This module contains functions for window I/O
 */
#include "proto.h"

/*
 * Get input from user and determine if the input is a short cut (CTRL+char),
 * an action key like arrow keys or a printable character.
 * Return the input and set short_cut and action_key flags.
 */
int get_input(WINDOW *win, bool *short_cut, bool *action_key)
{
	int input;

	/* A single call for all the screen update */
	doupdate();

	/* Using blocking mode */
	input = wgetch(mainwin);

	/* Printable character */
	/*
	 * Exception:
	 * Horizontal tab = 9
	 */
	if ((32 <= input && input <= 126) || input == 9) {
		*short_cut = FALSE;
		*action_key = FALSE;
	}
	/* Short cut (CTRL+char) */
	else if ((0 <= input && input <= 31) && input != 13 && input != 27) {
		*short_cut = TRUE;
		*action_key = FALSE;
	}
	else if (input == 544 || input == 559) {
		*short_cut = TRUE;
		*action_key = FALSE;
	}
	/* Action key */
	/*
	 * Exceptions:
	 * Carriage return = 13
	 * Escape = 27
	 * Delete = 127
	 */
	else if (input == 13 || input == 127 || input == 27) {
		*short_cut = FALSE;
		*action_key = TRUE;
	}
	else {
		*short_cut = FALSE;
		*action_key = TRUE;
	}
	
	return input;
}

/*
 * Display buffer starting from the topln
 */
void display_buffer()
{
	int tmp;

	tmp = curbuf->y_pos;
	curbuf->y_pos = 0;
	print_buffer(curbuf->topln);
	curbuf->y_pos = tmp;
	position_cursor(mainwin, curbuf->y_pos, curbuf->visual_x);
	update_statbar();
}

/*
 * print buffer on the virtual screen starting from beg line. First,we move 
 * the cursor to the beginning of the line and at the end, we place the cursor
 * at its original position.
 */
void print_buffer(Line *beg)
{
	Line *it;
	size_t i;

	wmove(mainwin, curbuf->y_pos, 0);
	wclrtobot(mainwin);
	for (it = beg, i = curbuf->y_pos; it != NULL && 
			i < (LINES - MAINWIN_OFFSET); it = it->next, i++) {
		waddstr(mainwin, it->text);
	}
	wmove(mainwin, curbuf->y_pos, curbuf->visual_x);
	wnoutrefresh(mainwin);
}

/*
 * Position the cursor at (0, 0)
 */
void reset_cursor()
{
	wmove(mainwin, 0, 0);
	wnoutrefresh(mainwin);
}

/*
 * Position the cursor at (y, x)
 */
void position_cursor(WINDOW *win, int y, int x)
{
	wmove(win, y, x);
	update_statbar();
	wnoutrefresh(win);
}

/*
 * Clear the line at y line
 */
void clear_line(WINDOW *win, int y)
{
	wmove(win, y, 0);
	wclrtoeol(win);
}

/*
 * Clear the window pointed by win
 */
void clear_win(WINDOW *win)
{
	wmove(win, 0, 0);
	wclrtobot(win);
	wnoutrefresh(win);
}

/*
 * Scroll mainwin dir-ward
 */
void scrol(Direction dir)
{
	scrollok(mainwin, TRUE);
	wscrl(mainwin, dir);
	scrollok(mainwin, FALSE);
}

/*
 * print the line pointed by line.
 */
void print_line(Line *line)
{
	wmove(mainwin, curbuf->y_pos, 0);
	waddstr(mainwin, line->text);
	wmove(mainwin, curbuf->y_pos, curbuf->visual_x);

	wnoutrefresh(mainwin);
}

/*
 * Paint the background of statbar
 */
void paint_statbar()
{
	int i;
	
	wmove(statbar, 0, 0);
	wclrtoeol(statbar);
	for (i = 0; i < COLS; i++) {
		waddch(statbar, ' ');
	}
}

/*
 * Update and print the status bar
 */
void update_statbar()
{
	const char *buffer_path;
	const char *buffer_state;

	buffer_path = (curbuf->path != NULL) ? curbuf->path : "[Untitled]";
	buffer_state = curbuf->modified ? "[+]" : "   ";

	wattron(statbar, A_REVERSE);

	paint_statbar();
	mvwprintw(statbar, 0, 0, "%s %s %d-%d", 
			buffer_path, buffer_state, curbuf->curln->line_no,
			curbuf->visual_x + 1);

	wattroff(statbar, A_REVERSE);

	wnoutrefresh(statbar);
	switch_win(MAINWIN);
}

void do_prev_buf()
{
	if (curbuf != firstbuf) {
		curbuf = curbuf->prev;
		display_buffer();
	}
}

void do_next_buf()
{
	if (curbuf != lastbuf) {
		curbuf = curbuf->next;
		display_buffer();
	}
}

void clear_allwin()
{
	clear_win(mainwin);
	clear_win(statbar);
	clear_win(bottwin);
}

void switch_win(Curwin cur)
{
	if (cur == MAINWIN)
		wnoutrefresh(mainwin);
	else if (cur == BOTTWIN)
		wnoutrefresh(bottwin);
}

