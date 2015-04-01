/*
 * This module includes functions for moving the cursor
 */

#include "proto.h"

void go_down()
{
	int tmp = curbuf->visual_x;

	if (curbuf->curln != curbuf->lastln) {
		curbuf->curln = curbuf->curln->next;
		curbuf->x_pos = visual2real(curbuf->visual_x);
		curbuf->visual_x = real2visual(curbuf->x_pos);
		curbuf->y_pos++;
		if (curbuf->y_pos == (LINES - MAINWIN_OFFSET)) {
			scrol(DOWN);
			curbuf->y_pos--;
			curbuf->topln = curbuf->topln->next;
			print_line(curbuf->curln);
		}
		position_cursor(mainwin, curbuf->y_pos, curbuf->visual_x);
		curbuf->visual_x = tmp;
	}
}

void go_up()
{
	int tmp = curbuf->visual_x;

	if (curbuf->curln != curbuf->firstln) {
		curbuf->curln = curbuf->curln->prev;
		curbuf->x_pos = visual2real(curbuf->visual_x);
		curbuf->visual_x = real2visual(curbuf->x_pos);
		curbuf->y_pos--;
		if (curbuf->y_pos == -1) {
			scrol(UP);
			curbuf->y_pos++;
			curbuf->topln = curbuf->topln->prev;
			print_line(curbuf->curln);
		}
		position_cursor(mainwin, curbuf->y_pos, curbuf->visual_x);
		curbuf->visual_x = tmp;
	}
}

void go_right()
{
	char curln_ch = curbuf->curln->text[curbuf->x_pos];

	if (curln_ch != '\n' && curln_ch != '\0') {
		curbuf->x_pos++;
		curbuf->visual_x = real2visual(curbuf->x_pos);
		position_cursor(mainwin, curbuf->y_pos, curbuf->visual_x);
	}
}

void go_left()
{
	if (curbuf->x_pos > 0) {
		curbuf->x_pos--;
		curbuf->visual_x = real2visual(curbuf->x_pos);
		position_cursor(mainwin, curbuf->y_pos, curbuf->visual_x);
	}
}

void go_end()
{
	if (curbuf->curln->text[curbuf->curln->len - 1] == '\n') {
		curbuf->x_pos = curbuf->curln->len - 1;
	}
	else {
		curbuf->x_pos = curbuf->curln->len;
	}
	curbuf->visual_x = real2visual(curbuf->x_pos);
	position_cursor(mainwin, curbuf->y_pos, curbuf->visual_x);
}

void go_beg()
{
	curbuf->x_pos = 0;
	curbuf->visual_x = real2visual(curbuf->x_pos);
	position_cursor(mainwin, curbuf->y_pos, curbuf->visual_x);
}

