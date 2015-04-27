
#include "proto.h"
#include <string.h>


/*
 * Insert the character c into the curln line where the cursor is located.
 *
 * The new string consists of three parts: xcy
 * strlen(x) == curbuf->x_pos
 * strlen(y) == curbuf->curln->len - curbuf->x_pos
 */
void insert_char(const char c)
{
	/* +1 for the new character and +1 for '\0' */
	if (curbuf->curln->len < curbuf->curln->memsize) {
		memmove(curbuf->curln->text + curbuf->x_pos + 1, 
				curbuf->curln->text + curbuf->x_pos, 
				curbuf->curln->len - curbuf->x_pos + 1);
		curbuf->curln->text[curbuf->x_pos] = c;
		curbuf->curln->len += 1;
	}
	else {
		char *tmp;

		tmp = charalloc(curbuf->curln->memsize * 2);
		strncpy(tmp, curbuf->curln->text, curbuf->x_pos);
		tmp[curbuf->x_pos] = c;
		strcpy(tmp + curbuf->x_pos + 1, curbuf->curln->text + curbuf->x_pos);
		curbuf->curln->len += 1;
		curbuf->curln->memsize *= 2;

		free(curbuf->curln->text);
		curbuf->curln->text = tmp;
	}
	curbuf->x_pos++;
	curbuf->visual_x = real2visual(curbuf->x_pos);
	print_line(curbuf->curln);
	buffer_modified(TRUE);
}

/*
 * Handle the enter key
 *
 * The curln string looks like this: x|y
 * After this procedure, it looks like: x
 *                                     |y
 * strlen(x) = curbuf->x_pos
 * strlen(y) = curbuf->curln->len - curbuf->x_pos
 */
void do_enter()
{
	/* char *tmp; */
	/* size_t len; */
	Line *line;

	line = new_line();
	/* +1 for '\0' */
	assert(curbuf->curln->memsize > 0);
	line->text = charalloc(curbuf->curln->memsize);
	line->memsize = curbuf->curln->memsize;
	/* Copy the second half (y) to line->text */
	strcpy(line->text, curbuf->curln->text + curbuf->x_pos);
	line->len = curbuf->curln->len - curbuf->x_pos;
	
	/* Cut the current line to its new length */
	/* curbuf->curln->text = charrealloc(curbuf->curln->text, curbuf->x_pos + 2); */
	curbuf->curln->text[curbuf->x_pos] = '\n';
	curbuf->curln->text[curbuf->x_pos + 1] = '\0';
	/* +1 for '\n' */
	curbuf->curln->len = (size_t)curbuf->x_pos + 1;

	insert_line(curbuf->curln, line);

	print_buffer(curbuf->curln);
	go_down();
	/* Place the cursor at the beginnig of the new line */
	curbuf->x_pos = 0;
	curbuf->visual_x = real2visual(curbuf->x_pos);
	position_cursor(mainwin, curbuf->y_pos, curbuf->visual_x);

	buffer_modified(TRUE);
	delete_line(line);
}

/*
 * Handle the backspace key
 *
 * The new string consists of two parts: x
 *                                      |y
 *
 * After this procedure, it looks like: x|y
 * strlen(x) == curbuf->x_pos
 * strlen(y) == curbuf->curln->len - curbuf->x_pos
 */
void do_backspace()
{
	if (curbuf->x_pos != 0) {
		memmove(curbuf->curln->text + curbuf->x_pos - 1, 
				curbuf->curln->text + curbuf->x_pos,
				curbuf->curln->len - curbuf->x_pos + 1);
		curbuf->x_pos--;
		curbuf->visual_x = real2visual(curbuf->x_pos);
		clear_line(mainwin, curbuf->y_pos);
		print_line(curbuf->curln);
		buffer_modified(TRUE);
	}
	else if (curbuf->x_pos == 0 && curbuf->curln->prev != NULL) {
		go_up();
		curbuf->x_pos = curbuf->curln->len - 1;
		curbuf->visual_x = real2visual(curbuf->x_pos);
		position_cursor(mainwin, curbuf->y_pos, curbuf->visual_x);

		if (curbuf->curln->len + curbuf->curln->next->len < curbuf->curln->memsize) {
			memmove(curbuf->curln->text + curbuf->curln->len - 1,
					curbuf->curln->next->text, 
					curbuf->curln->next->len + 1);
		}
		else {
			curbuf->curln->text = charrealloc(curbuf->curln->text, 
					curbuf->curln->memsize + curbuf->curln->next->memsize);
			curbuf->curln->memsize = curbuf->curln->memsize + curbuf->curln->next->memsize;

			strcpy(curbuf->curln->text + curbuf->curln->len - 1, 
					curbuf->curln->next->text);
		}
		curbuf->curln->len += curbuf->curln->next->len - 1;
		erase_line(curbuf->curln->next);
		print_buffer(curbuf->curln);
		buffer_modified(TRUE);
	}
}

