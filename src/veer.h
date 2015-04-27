#ifndef VEER_H
#define VEER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* header files that are only used throughout the program */
#include <stdlib.h>
#include <assert.h>
#include <curses.h>

/* VEER version */
#define VERSION "0.0.1"

typedef enum Curwin {
	MAINWIN,
	BOTTWIN
} Curwin;

typedef enum Direction {
	UP = -1, 
	DOWN = 1
} Direction;

typedef enum Response {
	YES = 1,
	NO = 0,
	CANCEL = -1,
	ILLEGAL = -2
} Response;

/* Global structures */

/* typedef struct Line Line; */
typedef struct Line {
	char *text;
	size_t len;
	size_t memsize;
	size_t line_no;
	struct Line *prev;
	struct Line *next;
} Line;

typedef struct Buffer {
	int id;
	char *path;
	Line *firstln;
	Line *lastln;
	/* culine is the line where the cursor is */
	Line *curln;
	Line *topln;
	int x_pos;
	int y_pos;
	int visual_x;
	bool modified;
	struct Buffer *prev;
	struct Buffer *next;
} Buffer; /* Buffer is only an alias not an instance */

/* Macros */
#define BUFFER_SIZE 	80
#define STATBAR_HEIGHT 	1
#define BOTTWIN_HEIGHT 	2
#define MAINWIN_OFFSET 	(STATBAR_HEIGHT + BOTTWIN_HEIGHT)

/* Functions with associated keys */
#define CNTRL(CH) ((CH) - 64)

#define DO_EXIT		CNTRL('X')	
#define DO_SAVE		CNTRL('S')

#define DO_PREV_BUF	544
#define DO_NEXT_BUF	559

#define CARRIAGE_RET	13
#define ESCAPE			27

#endif

