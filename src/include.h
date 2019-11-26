

/**************************************************************************
*      Mindor 1.0 is copyright 2002-2019 by Shaun Mcbride                *
*      Portions of the code were inspired by other works                 *
*      found around the internet.                                        *
*                                                                        *
*      Please follow all previous copyrights and licenses.               *
**************************************************************************/


/* This is what I include in nearly every file.
 *
 */

#if defined( macintosh )
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <errno.h>		
#include <unistd.h>		
#include <sys/time.h>
#endif
#include <ctype.h>		
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* #include "unchecked.h" */ /* If you are using gcc w/bounds-checking */
#include "merc.h"

#define MIL  MAX_INPUT_LENGTH
#define MSL  MAX_STRING_LENGTH

