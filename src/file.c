/*
 * This module contains functions for file and buffer manipulation
 */

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "proto.h"

/*
 * Open a new buffer according to path.
 */
void open_buffer(const char *path)
{
	FILE *fs;

	/* No path was given. Just create a blank buffer.*/
	if (path == NULL) {
		push_back_buffer(NULL);
		push_back_line("", 0);
	} 
	/* A path was given */
	else {
		fs = open_file(path, "r");
		if (fs != NULL) {
			push_back_buffer(path);
			read_into_buffer(fs);
		} 
		/*
		 * File does not exist, opening a new buffer with the given path.
		 * Later when saving the buffer, we check if path is valid or not.
		 */
		else {
			/* new_buffer(path); */
			push_back_buffer(path);
			push_back_line("", 0);
		}
	}
	curbuf = firstbuf;
}

/*
 * Initialize buffer (should only be called from within push_back_buffer()).
 */
Buffer *new_buffer()
{
	Buffer *buf;

	buf = malloc(sizeof(Buffer));
	if (buf == NULL) {
		fprintf(stderr, "%s: malloc failed\n", __func__);
		exit(EXIT_FAILURE);
	}

	if (lastbuf != NULL) {
		buf->id = lastbuf->id + 1;
	} else {
		buf->id = 0;
	}
	buf->path = NULL;
	buf->firstln = NULL;
	buf->lastln = NULL;
	buf->curln = NULL;
	buf->topln = NULL;
	buf->x_pos = 0;
	buf->y_pos = 0;
	buf->visual_x = 0;
	buf->modified = FALSE;
	buf->prev = NULL;
	buf->prev = NULL;

	return buf;
}

void push_back_buffer(const char *path)
{
	Buffer *nbuf;

	nbuf = new_buffer();
	if (path != NULL) {
		nbuf->path = charalloc(strlen(path) + 1);
		strcpy(nbuf->path, path);
	}

	/* If not the first buffer, link the last buffer to the new buffer */
	if (lastbuf != NULL) {
		nbuf->prev = lastbuf;
		nbuf->next = NULL;
		lastbuf->next = nbuf;
		/* nbuf->id = lastbuf->id + 1; */
	}
	/* If the first buffer, link the first buffer to the new buffer */
	else if (firstbuf == NULL) {
		nbuf->prev = NULL;
		nbuf->next = NULL;
		firstbuf = nbuf;
	}
	/* Make the new buffer the last buffer */
	lastbuf = nbuf;
	/* Make current buffer the last buffer */
	curbuf = nbuf;
}

/*
 * Check if path is a valid path or not.
 * Return a filestream if path is valid or null otherwise.
 */
FILE *open_file(const char *path, const char *mode)
{
	struct stat filestat;
	FILE *fs = NULL;
	const char *strerr = NULL;

	assert(path != NULL);

	/*
	 * No permissions are required on the file itself to be stat-ed.
	 * First we check if path is valid or not.
	 */
	if (stat(path, &filestat) == 0) {
		if (S_ISDIR(filestat.st_mode)) {
			strerr = "'%s' is directory";
			goto error_handling;
		}
		else if (S_ISCHR(filestat.st_mode)) {
			strerr = "'%s' is character device";
		}
		else if (S_ISBLK(filestat.st_mode)) {
			strerr = "'%s' is block device";
			goto error_handling;
		}
		else if (S_ISFIFO(filestat.st_mode)) {
			strerr = "'%s' is FIFO";
			goto error_handling;
		}
		else if (S_ISLNK(filestat.st_mode)) {
			strerr = "'%s' is symbolic link";
			goto error_handling;
		}
		else if (S_ISSOCK(filestat.st_mode)) {
			strerr = "'%s' is socket";
			goto error_handling;
		}
		else if (S_ISREG(filestat.st_mode)) {
			fs = fopen(path, mode);
			if (fs == NULL) {
				error = errno;
				goto error_handling;
			}
			if (strcmp(mode, "w") == 0) {
				switch (prompt_ync("`%s' already exists, overwrite?", path)) {
				case YES:
					break;
				case NO:
					return NULL;
				case CANCEL:
					return NULL;
				default:
					return NULL;
				}
			}
			return fs;
		}
	}
	else {
		if (strcmp(mode, "r") == 0) {
			return NULL;
		}
		else if (strcmp(mode, "w") == 0) {
			fs = fopen(path, mode);
			if (fs == NULL) {
				error = errno;
				goto error_handling;
			}
			return fs;
		}
	}

error_handling:
	if (strerr == NULL) {
		strerr = "'%s': %s";
		print_msg_prompt(strerr, path, strerror(error));
	}
	else {
		print_msg_prompt(strerr, path);
	}

	return fs;
}

/*
 * Read in the entire file associated with fs
 */
void read_into_buffer(FILE *fs)
{
	int ch;
	int i = 0;
	char *buf;
	size_t buffer_mem = BUFFER_SIZE;

	assert(fs != NULL);

	buf = charalloc(BUFFER_SIZE);
	buf[0] = '\0';

	/* Read the whole file into the buffer */
	while ((ch = fgetc(fs)) != EOF) {
		/* First we check if there is enough room for storing characters */
		if (i == (buffer_mem - 1)) {
			buffer_mem += BUFFER_SIZE;
			buf = charrealloc(buf, buffer_mem);
		}

		/* Store ch and null-terminate buf */
		buf[i++] = (char)ch;
		buf[i] = '\0';

		/* A new line */
		if ((char)ch == '\n') {
			push_back_line(buf, i);
			memset(buf, 0, i + 1);
			buffer_mem = BUFFER_SIZE;
			i = 0;
		}
	}
	/*
	 * If we have an empty file or if we only have one line and 
	 * it does not have '\n' at its end, handle it. It also handles
	 * the lastln line of the file.
	 */
	if (curbuf->firstln == NULL || buf[0] != '\0') {
		push_back_line(buf, i);
	}
	
	free(buf);
}

/*
 * Initialize line (should only be called from within push_back_line()).
 */
Line *new_line()
{
	Line *line;

	line = malloc(sizeof(Line));

	line->text = NULL;
	line->len = 0;
	line->line_no = 0;
	line->next = NULL;
	line->prev = NULL;

	return line;
}

void delete_line(Line *line)
{
	free(line->text);
	free(line);
}

/*
 * Make a new line with text of len characters. Push the new line at 
 * the back of the linked list.
 */
void push_back_line(const char *text, size_t len)
{
	Line *nline;

	assert(text != NULL);

	nline = new_line();
	nline->text = charalloc(len + 1);
	strcpy(nline->text, text);
	nline->len = len;

	/* If not the first line, link the last line to the new line */
	if (curbuf->lastln != NULL) {
		nline->prev = curbuf->lastln;
		nline->next = NULL;
		curbuf->lastln->next = nline;

		/* nline->line_no = curbuf->lastln->line_no + 1; */
	}
	/* If the first line, link the first line to the new line */
	else if (curbuf->firstln == NULL) {
		nline->prev = NULL;
		nline->next = NULL;
		curbuf->firstln = nline;
		curbuf->topln = nline;
		/* Make the current line line the first line */
		curbuf->curln = nline;

		/* nline->line_no = 1; */
	}
	/* Make the new line the last line */
	curbuf->lastln = nline;
}

/*
 * Make a new line with text of len characters and insert it after the line 
 * pointed by ptr. If ptr points to the last line, call push_back_line() instead.
 * We do not advance curbuf->curln to point to the new line.
 */
void insert_line(Line *ptr, const char *text, size_t len)
{
	Line *nline;

	if (ptr == curbuf->lastln) {
		push_back_line(text, len);
	}
	else {
		nline = new_line();
		nline->text = charalloc(len + 1);
		strcpy(nline->text, text);
		nline->len = len;

		nline->prev = ptr;
		nline->next = ptr->next;

		ptr->next->prev = nline;
		ptr->next = nline;
	}
}

/*
 * Delete the line pointed by line, then make curln pointer point to 
 * the *next* line.
 */
void erase_line(Line *ptr)
{
	assert(ptr != NULL);

	/* If a middle line */
	if (ptr->next != NULL) {
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
	}
	/* If the last line */
	else if (ptr == curbuf->lastln){
		curbuf->lastln = ptr->prev;
		ptr->prev->next = NULL;
	}

	free(ptr->text);
	free(ptr);
}

/*
 * Set modified flag if buffer is modified
 */
void buffer_modified(bool modified)
{
	curbuf->modified = modified ? TRUE : FALSE;
	update_statbar();
}

/*
 * Save buffer to disk.
 */
void save_buffer()
{
	int i;
	FILE *fs;
	Line *it;

	if (curbuf->path == NULL) {
		curbuf->path = prompt_str("File name to save: ");
		if (curbuf->path == NULL)
			return;
	}

	fs = open_file(curbuf->path, "w");
	if (fs == NULL) {
		curbuf->path = NULL;
		return;
	}
	for (it = curbuf->firstln; it != NULL; it = it->next) {
		for (i = 0; it->text[i] != '\0'; i++)
			fputc(it->text[i], fs);
	}
	fclose(fs);

	buffer_modified(FALSE);
}

