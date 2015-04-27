/*
 * This module includes auxiliary functions to perform various tasks
 */
#include "proto.h"
#include <errno.h>
#include <string.h>


/*
 * Convert visual x-coordinate to the index of its correspondent
 * character and return the index.
 */
int visual2real(const int visualx)
{
	int i = 0;
	int pos = 0;

	while (curbuf->curln->text[i] != '\0' && 
			curbuf->curln->text[i] != '\n') {
		if (curbuf->curln->text[i] == '\t')
			pos += 8 - pos % 8;
		else
			pos++;
		if (pos > visualx)
			break;
		i++;
	}
	return i;
}

/* Opposite of visual2real() */
int real2visual(const int realx)
{
	int i = 0;
	int pos = 0;

	while (i < realx && curbuf->curln->text[i] != '\0' && 
			curbuf->curln->text[i] != '\n') {
		if (curbuf->curln->text[i] == '\t')
			pos += 8 - pos % 8;
		else
			pos++;
		i++;
	}
	return pos;
}

char *charalloc(size_t size)
{
	char *ptr = NULL;

	ptr = malloc(sizeof(char) * size);
	if (ptr == NULL) {
		error = errno;
		fprintf(stderr, "%s: malloc() failed, %s", __func__, strerror(error));
		finish();
	}
	return ptr;
}

char *charrealloc(char *ptr, size_t size)
{
	ptr = realloc(ptr, sizeof(char) * size);
	if (ptr == NULL) {
		error =errno;
		fprintf(stderr, "%s: realloc() failed, %s", __func__, strerror(error));
		finish();
	}
	return ptr;
}

char *file_name(const char *path)
{
	char *name;

	assert(path != NULL);

	if ((name = strrchr(path, '/')) == NULL) {
		/* Suppress compiler warning */
		return (char *)path;
	}
	/* else */
	return (name + 1);
}

