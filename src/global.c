/*
 * Definitions of global variables go here
 */

#include "proto.h"

WINDOW *mainwin = NULL;
WINDOW *statbar = NULL;
WINDOW *bottwin = NULL;

Buffer *curbuf = NULL;
Buffer *firstbuf = NULL;
Buffer *lastbuf = NULL;

/* Valid error numbers are all nonzero; errno is never set to zero
 * by any system call or library function. */
int error = 0;

