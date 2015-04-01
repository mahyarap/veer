/**************************************************************************
 *   veer.c                                                               *
 *                                                                        *
 *   Copyright (C) 2015, Mahyar Abbas Pour                                *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 3, or (at your option)  *
 *   any later version.                                                   *
 *                                                                        *
 *   This program is distributed in the hope that it will be useful, but  *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    *
 *   General Public License for more details.                             *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program; if not, write to the Free Software          *
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA            *
 *   02110-1301, USA.                                                     *
 *                                                                        *
 **************************************************************************/

#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <signal.h>

#include "proto.h"

/*
 * Print the usage of the program and exit.
 */
void usage()
{
	#define HELP "Usage: veer [OPTIONS] [FILES]\n\n\
Option		Meaning\n\
-h		Show this msg\n\
-v		Print version\n"

	printf(HELP);
	exit(EXIT_SUCCESS);
}

/*
 * Print version of the program and exit.
 */
void version()
{
	printf(" VEER version %s (compiled %s, %s)\n", VERSION,
			__TIME__, __DATE__);
	exit(EXIT_SUCCESS);
}

/*
 * Enable terminal generated signals i.e. signals that generated upon receipt of any
 * of the characters INTR, QUIT, SUSP, or DSUSP.
 */
void enable_signal()
{
	struct termios term;

	tcgetattr(0, &term);
	term.c_lflag |= ISIG;
	tcsetattr(0, TCSANOW, &term);
}

/*
 * Initialize curses. Put the terminal on raw mode and disable translation of 
 * carriage return to new line. Disable echoing characters on terminal and set 
 * the cursor visibility to normal.
 */
void init_terminal()
{
	if (initscr() == NULL)
		exit(EXIT_FAILURE);

	raw();
	nonl();
	noecho();
	curs_set(1);
	set_escdelay(250);
	/* enable_signal(); */
}

/*
 * Initialize the three windows of the program i.e. mainwin, statbar and bottwin.
 * Also enable the keypad for mainwin and bottwin.
 */
void init_window()
{
	/* newwin(int nlines, int ncols, int begin_y, int begin_x); */
	mainwin = newwin(LINES - BOTTWIN_HEIGHT, COLS, 0, 0);
	statbar = newwin(1, COLS, LINES - BOTTWIN_HEIGHT - 1, 0);
	bottwin = newwin(BOTTWIN_HEIGHT, COLS, LINES - BOTTWIN_HEIGHT, 0); 

	keypad(mainwin, TRUE);
	keypad(bottwin, TRUE);
}

void do_suspend(int signal)
{

}

/*
 * Initialize signal handling feature for various signals.
 */
void init_signal()
{
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = do_suspend;
	/* SIGTSTP: Stop typed at terminal (^Z) */
	sigaction(SIGTSTP, &act, NULL);
}

/*
 * Prepare for exit.
 */
void do_exit()
{
	Buffer *it;

	for (it = firstbuf; it != NULL; it = it->next) {
		if (it->modified) {
			switch (prompt_ync("Save modified buffer `%s`?", it->path)) {
			case YES:
				save_buffer();
				if (it->modified) {
					return;
				} else {
					break;
				}
			case NO:
				break;
			case CANCEL:
				return;
			/* Suppress compiler warning */
			default:
				break;
			}
		}
	}
	finish();
}

/*
 * Exit gracefully
 */
void finish()
{
	delwin(statbar);
	delwin(mainwin);
	endwin();
	exit(EXIT_SUCCESS);
}

/*
 * Handle various inputs. There are 3 different inputs.
 * 1 - Printable ASCII characters
 * 2 - Short cuts i.e. key combinations that perform an action
 * 3 - Action keys i.e. single keys that perform an action
 */
void do_input()
{
	int input;
	bool short_cut = FALSE;
	bool action_key = FALSE;
	
	input = get_input(mainwin, &short_cut, &action_key);

	/* We have a printable character */
	if (short_cut == FALSE && action_key == FALSE) {
		insert_char((char)input);
	}
	else if (short_cut == TRUE) {
		switch (input) {
		case DO_SAVE:
			save_buffer();
			break;
		case DO_EXIT:
			do_exit();
			break;
		case DO_PREV_BUF:
			do_prev_buf();
			break;
		case DO_NEXT_BUF:
			do_next_buf();
			break;
		}
	}
	else if (action_key == TRUE) {
		switch (input) {
		case KEY_DOWN:
			go_down();
			break;
		case KEY_RIGHT:
			go_right();
			break;
		case KEY_UP:
			go_up();
			break;
		case KEY_LEFT:
			go_left();
			break;
		case KEY_HOME:
			go_beg();
			break;
		case KEY_END:
			go_end();
			break;
		case CARRIAGE_RET:
			do_enter();
			break;
		case KEY_BACKSPACE:
			do_backspace();
			break;
		default:
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	char opt;
	bool opts_processed = FALSE;
	
	while ((opt = (char)getopt(argc, argv, "hv")) != -1) {
		switch (opt) {
		case 'h':
			usage();
			break;
		case 'v':
			version();
			break;
		default:
			usage();
		}
		opts_processed = TRUE;
	}

	/* Initializations */
	
	init_terminal();
	init_window();	

	/* End of initializations */

	/* Open buffer */
	if ((argc > 1) && (opts_processed == FALSE)) {
		int i;

		for (i = optind; i < argc; i++) {
			open_buffer(argv[i]);
		}
	}
	else if (argc == 1) {
		open_buffer(NULL);
	}

	/* Show buffer if it is not empty */
	if (curbuf != NULL) {
		display_buffer();
	}
	reset_cursor();

	/* The main loop of the program */
	while (TRUE) {
		do_input();
	}

	/* We should never get here! */
	assert(FALSE);
}

