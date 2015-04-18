
#include <string.h>
#include <stdarg.h>
#include "proto.h"

/*
 * A structure definition is private to a source file unless placed 
 * in a shared header file.
 */
typedef struct Answer {
	char *text;
	size_t len;
	int x_pos;
	int y_pos;
	int x_margin;
} Answer;
static Answer answer;

static void answer_init();
static void mvprint_msg_prompt(const char *msg, int y);
static void print_answer_prompt(const char *ans);
static void insert_char_prompt(const char c);
static void go_right_prompt();
static void go_left_prompt();
static void do_backspace_prompt();
static void do_escape();
static int do_input_prompt();

/*
 * Prompt user with a Yes/No/Cancel question
 * Return the response
 */
Response prompt_ync(const char *question, ...)
{
	char input;
	bool sc, ak;
	Response res = ILLEGAL;
	va_list ap;
	char buffer[256];

	curwin = BOTTWIN;

	va_start (ap, question);
	vsnprintf (buffer, 256, question, ap);
	va_end (ap);

	clear_win(bottwin);
	mvprint_msg_prompt(buffer, 0);
	mvprint_msg_prompt("[Y]es, [N]o, [C]ancel: ", 1);

	do {
		switch (input = (char)get_input(bottwin, &sc, &ak)) {
		case 'y':
		case 'Y':
			res = YES;
			break;
		case 'n':
		case 'N':
			res = NO;
			break;
		case 'c':
		case 'C':
			res = CANCEL;
			break;
		case ERR:
			mvprint_msg_prompt(buffer, 0);
			mvprint_msg_prompt("[Y]es, [N]o, [C]ancel: ", 1);
		default:
			res = ILLEGAL;
		}
	} while (res == ILLEGAL);

	clear_win(bottwin);
	curwin = MAINWIN;
	switch_win(curwin);

	return res;
}

/*
 * Prompt user to input string
 * Return the user input string
 */
char *prompt_str(const char *msg, ...)
{
	va_list ap;
	char buffer[256];
	int retval;

	va_start (ap, msg);
	vsnprintf (buffer, 256, msg, ap);
	va_end (ap);

	answer_init();
	
	clear_win(bottwin);
	answer.x_margin = strlen(msg);
	mvprint_msg_prompt(buffer, 0);
	mvprint_msg_prompt("(Press ESCAPE to cancel)", 1);
	position_cursor(bottwin, 0, answer.x_margin);

	while ((retval = do_input_prompt()) != FALSE) {
		if (retval == ERR) {
			mvprint_msg_prompt(buffer, 0);
			mvprint_msg_prompt("(Press ESCAPE to cancel)", 1);
			print_answer_prompt(answer.text);
		}
	}

	clear_win(bottwin);
	switch_win(MAINWIN);

	return answer.text;
}

/*
 * Print a message on bottwin
 */
void print_msg_prompt(const char *msg, ...)
{
	va_list ap;
	char buffer[256];

	clear_win(bottwin);

	va_start (ap, msg);
	vsnprintf (buffer, 256, msg, ap);
	waddstr(bottwin, buffer);
	va_end (ap);

	wnoutrefresh(bottwin);
	switch_win(MAINWIN);
}

static void mvprint_msg_prompt(const char *msg, int y)
{
	mvwaddstr(bottwin, y, 0, msg);
	wnoutrefresh(bottwin);
}

static void print_answer_prompt(const char *ans)
{
	mvwaddstr(bottwin, 0, answer.x_margin, ans);
	wmove(bottwin, 0, answer.x_margin + answer.x_pos);
	wnoutrefresh(bottwin);
}

static void insert_char_prompt(const char c)
{
	char *tmp;

	/* +1 for the new character and +1 for '\0' */
	tmp = charalloc(answer.len + 2);
	strncpy(tmp, answer.text, answer.x_pos);
	tmp[answer.x_pos] = c;
	strcpy(tmp + answer.x_pos + 1, answer.text + answer.x_pos);
	answer.len++;

	free(answer.text);
	answer.text = tmp;

	answer.x_pos++;
	print_answer_prompt(answer.text);
}

static void go_right_prompt()
{
	if (answer.text[answer.x_pos] != '\0') {
		answer.x_pos++;
		wmove(bottwin, answer.y_pos, answer.x_margin + answer.x_pos);
		wnoutrefresh(bottwin);
	}
}

static void go_left_prompt()
{
	if (answer.x_pos > 0) {
		answer.x_pos--;
		wmove(bottwin, answer.y_pos, answer.x_margin + answer.x_pos);
		wnoutrefresh(bottwin);
	}
}

static void do_backspace_prompt()
{
	if (answer.x_pos != 0) {
		char *tmp;

		tmp = charalloc(answer.len);
		strncpy(tmp, answer.text, answer.x_pos - 1);
		strcpy(tmp + answer.x_pos - 1, answer.text + answer.x_pos);
		answer.len--;

		free(answer.text);
		answer.text = tmp;

		answer.x_pos--;
		wmove(bottwin, 0, answer.x_margin);
		wclrtoeol(bottwin);
		print_answer_prompt(answer.text);
	}
}

static void do_escape()
{
	clear_win(bottwin);
	free(answer.text);
	answer.text = NULL;
	switch_win(MAINWIN);
}

static int do_input_prompt()
{
	int input;
	int retval = TRUE;
	bool short_cut, action_key;

	input = get_input(bottwin, &short_cut, &action_key);

	if (short_cut == FALSE && action_key == FALSE) {
		insert_char_prompt(input);
	}
	else if (action_key == TRUE) {
		switch (input) {
		case KEY_RIGHT:
			go_right_prompt();
			break;
		case KEY_LEFT:
			go_left_prompt();
			break;
		case KEY_HOME:
			go_beg();
			break;
		case KEY_END:
			go_end();
			break;
		case CARRIAGE_RET:
			retval = FALSE;
			break;
		case ESCAPE:
			do_escape();
			retval = FALSE;
			break;
		case KEY_BACKSPACE:
			do_backspace_prompt();
			break;
		case ERR:
			retval = ERR;
			break;
		default:
			break;
		}
	}
	return retval;
}

static void answer_init()
{
	answer.text = charalloc(BUFFER_SIZE);
	answer.text[0] = '\0';
	answer.len = 0;
	answer.x_pos = 0;
	answer.y_pos = 0;
	answer.x_margin = 0;
}

