/*	File:			room_map.c
 *	Descrtiption:
 *		This file contains my function to
 *		create an asciimap of the room you are
 *		in. It shows all 6 standard exits on
 *		the map. The output looks something
 *		like:
 *
 *		.-----------------.
 *		|                 |
 *		| xxxxxxxxxxxxxxx | { ALL } Thri     The City of Vandagard
 *		| xxxxxxxxxxxxxxx | The Temple Altar
 *		| xxxx       xxxx |
 *		| xxxx            |
 *		| xxxx       xxxx |
 *		| xxxxxxxxxxxxxxx |
 *		| xxxxxxxxxxxxxxx |
 *		|                 |
 *		'-----------------'  [Room 113]
 *
 *		Each room sector is defined with two
 *		varabiles in the switch statement.
 *		sp = The sector pixel (defines what those xxx's show as)
 *		     the charaters should be about 20 across or so.
 *		co = The color of the room. (bright cyan for water,
 *		     grey for mountians etc)
 *
 * 	Known Bugs:	1. Sometimes the mapper will get confused, and
 *			and output complete crap or 1/2 a room.
 *			2. It would be too much of a pain to add
 *			   nw/sw/ne/nw exits for me ;) If you do,
 *			   send it to me, id love to have it.
 *			3. Doesnt support secret exits (they arent
 *			   really secret ;)
 *			4. The way it places the charaters is crappy,
 *			   the First charater HAS to be something, other wise
 *			   it looks really bad. Its a good idea to have no spaces.
 *
 *	How to use it:	Well Ihave both a wilderness map and this map,
 *				so in do_look, i did an if ROOM_IS_WILD
 *				do_wilderness, else do_room_map
 *
 *	Conditions of Use:	I request and require if you are going
 *				to use this in a mud that will have
 *				players, or in any sort of released
 *				code base, you email or AIM me telling
 *				where its going.
 *				I also request a helpfile under the topic
 *				_room_map (yes thats underscore_room_map)
 *				with my contact info, and this snips info.
 *				Also with this information below mine:
 *				Orginal Idea by Bela of Underworld Dreams.
 *				http://dreams.daestroke.com
 *
 *				Also you are required to Enjoy life. ;)
 *				Also i require you to follow all previous
 *				licences.
 *
 *	-Thri
 *	AIM: CalibanL
 *	ICQ: 5335308
 *	Email: cyhawk@comcast.net
 *
 *
 *
 */


#if defined(macintosh)
#include <types.h>
#else
        #include <sys/types.h>
        #if defined(WIN32)
                #include <time.h>
        #else
                #include <sys/time.h>
        #endif
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "build.h"
#include "version.h"

void
do_map (CHAR_DATA * ch, char *argument)
{
  EXIT_DATA *pexit_north;
  EXIT_DATA *pexit_south;
  EXIT_DATA *pexit_east;
  EXIT_DATA *pexit_west;
  EXIT_DATA *pexit_up;
  EXIT_DATA *pexit_down;

  pexit_north = ch->in_room->exit[DIR_NORTH];
  pexit_south = ch->in_room->exit[DIR_SOUTH];
  pexit_east = ch->in_room->exit[DIR_EAST];
  pexit_west = ch->in_room->exit[DIR_WEST];
  pexit_up = ch->in_room->exit[DIR_UP];
  pexit_down = ch->in_room->exit[DIR_DOWN];
  char buf[MSL];
  char buf2[MSL];
  char buf3[MSL];
  char buf4[MSL];
  char buf5[MSL];
  char buf6[MSL];
  char buf7[MSL];

  char sp[MSL];
  char co[MSL];

  switch (ch->in_room->sector_type)
    {
    default:
    case SECT_INSIDE:
      sprintf (sp, "xxxxxxxxxxxxxxxxxx");
      sprintf (co, "{g");
      break;

    case SECT_CITY:
      sprintf (sp, "xxxxxxxxxxxxxxxxxx");
      sprintf (co, "{G");
      break;

    case SECT_FIELD:
      sprintf (sp, "i1Il1lII11i1l1ll1l");
      sprintf (co, "{w");
      break;

    case SECT_FOREST:
      sprintf (sp, "oO*p*O*p*8*O*8p*8*O");
      sprintf (co, "{g");
      break;

    case SECT_HILLS:
      sprintf (sp, "@mMm@MmM@MmM@mMM@Mm");
      sprintf (co, "{w");
      break;

    case SECT_MOUNTAIN:
      sprintf (sp, "/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\");
      sprintf (co, "{D");
      break;

    case SECT_WATER_SWIM:
      sprintf (sp, "~oOo~oOo~oOo~oOo~oOo~oOo");
      sprintf (co, "{C");
      break;

    case SECT_WATER_NOSWIM:
      sprintf (sp, "~oOo~oOo~oOo~oOo~oOo~oOo");
      sprintf (co, "{B");
      break;

    case SECT_AIR:
      sprintf (sp, "()@()@()@()@()@()@()@()");
      sprintf (co, "{D");
      break;

    case SECT_DESERT:
      sprintf (sp, "*_-_&_)-*_&)_*^*-)*-");
      sprintf (co, "{Y");
      break;

    }



  send_to_char (".-----------------.\n\r", ch);
  send_to_char ("|                 |\n\r", ch);

  if ((ch->in_room->exit[DIR_NORTH] != NULL)
      && (ch->in_room->exit[DIR_UP] == NULL)
      && (IS_SET (pexit_north->exit_info, EX_CLOSED)))
    {
      sprintf (buf, "|%s %.6s   %.6s {x|\n\r{x", co, sp, sp);
      sprintf (buf2, "|%s %.6s---%.6s {x|\n\r{x", co, sp, sp);
      sprintf (buf3, "|%s %.4s       %.4s {x|     \n\r{x", co, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_NORTH] != NULL)
	   && (ch->in_room->exit[DIR_UP] != NULL)
	   && (IS_SET (pexit_north->exit_info, EX_CLOSED))
	   && (IS_SET (pexit_up->exit_info, EX_CLOSED)))
    {
      sprintf (buf, "|%s %.6s   %.2s   %.1s {x|\n\r{x", co, sp,
	       sp, sp);
      sprintf (buf2, "|%s %.6s---%.1s---%.2s {x|\n\r{x", co, sp, sp, sp);
      sprintf (buf3, "|%s %.4s        %.3s {x|     \n\r{x", co, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_NORTH] != NULL)
	   && (ch->in_room->exit[DIR_UP] != NULL)
	   && (IS_SET (pexit_north->exit_info, EX_CLOSED)))
    {
      sprintf (buf, "|%s %.6s   %.2s   %.1s {x|\n\r{x", co, sp,
	       sp, sp);
      sprintf (buf2, "|%s %.6s---%.1s   %.2s {x|\n\r{x", co, sp, sp, sp);
      sprintf (buf3, "|%s %.4s        %.3s {x|     \n\r{x", co, sp, sp);
    }


  else if ((ch->in_room->exit[DIR_NORTH] != NULL)
	   && (ch->in_room->exit[DIR_UP] != NULL)
	   && (IS_SET (pexit_up->exit_info, EX_CLOSED)))
    {
      sprintf (buf, "|%s %.6s   %.2s   %.1s {x|\n\r{x", co, sp,
	       sp, sp);
      sprintf (buf2, "|%s %.6s   %.1s---%.2s {x|\n\r{x", co, sp, sp, sp);
      sprintf (buf3, "|%s %.4s        %.3s {x|     \n\r{x", co, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_NORTH] != NULL)
	   && (ch->in_room->exit[DIR_UP] != NULL)
	   && (IS_SET (pexit_north->exit_info, EX_CLOSED))
	   && (IS_SET (pexit_up->exit_info, EX_CLOSED)))
    {
      sprintf (buf, "|%s %.6s   %.2s   %.1s {x|\n\r{x", co, sp,
	       sp, sp);
      sprintf (buf2, "|%s %.6s---%.1s---%.2s {x|\n\r{x", co, sp, sp, sp);
      sprintf (buf3, "|%s %.4s        %.3s {x|     \n\r{x", co, sp, sp);
    }


  else if ((ch->in_room->exit[DIR_NORTH] == NULL)
	   && (ch->in_room->exit[DIR_UP] != NULL)
	   && (IS_SET (pexit_up->exit_info, EX_CLOSED)))
    {
      sprintf (buf, "|%s %.11s   %.1s {x|\n\r{x", co, sp, sp);
      sprintf (buf2, "|%s %.10s---%.2s {x|\n\r{x", co, sp, sp);
      sprintf (buf3, "|%s %.4s        %.3s {x|     \n\r{x", co, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_NORTH] == NULL)
	   && (ch->in_room->exit[DIR_UP] != NULL))
    {
      sprintf (buf, "|%s %.11s   %.1s {x|\n\r{x", co, sp, sp);
      sprintf (buf2, "|%s %.10s   %.2s {x|\n\r{x", co, sp, sp);
      sprintf (buf3, "|%s %.4s        %.3s {x|     \n\r{x", co, sp, sp);
    }
  else if ((ch->in_room->exit[DIR_NORTH] != NULL)
	   && (ch->in_room->exit[DIR_UP] == NULL))
    {
      sprintf (buf, "|%s %.6s   %.6s {x|\n\r{x", co, sp, sp);
      sprintf (buf2, "|%s %.6s   %.6s {x|\n\r{x", co, sp, sp);
      sprintf (buf3, "|%s %.4s       %.4s {x|     \n\r{x", co, sp, sp);
    }


  else if ((ch->in_room->exit[DIR_NORTH] != NULL)
	   && (ch->in_room->exit[DIR_UP] != NULL))
    {
      sprintf (buf, "|%s %.6s   %.2s   %.1s {x|\n\r{x", co, sp,
	       sp, sp);
      sprintf (buf2, "|%s %.6s   %.1s   %.2s {x|\n\r{x", co, sp, sp, sp);
      sprintf (buf3, "|%s %.4s        %.3s {x|     \n\r{x", co, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_NORTH] == NULL)
	   && (ch->in_room->exit[DIR_UP] == NULL))
    {
      sprintf (buf, "|%s %.15s {x|\n\r{x", co, sp);
      sprintf (buf2, "|%s %.15s {x|\n\r{x", co, sp);
      sprintf (buf3, "|%s %.4s       %.4s {x|     \n\r{x", co, sp, sp);
    }
  else
    {
      sprintf (buf, "|%s xxxxxxxxxxxxxxx {x|     \n\r{x", co);
      sprintf (buf2, "|%s xxxx Bugged xxx {x|     \n\r{x", co);
      sprintf (buf3, "|%s xxxxx exit xxxx {x|     \n\r{x", co);
    }




  if ((ch->in_room->exit[DIR_EAST] != NULL)
      && (ch->in_room->exit[DIR_WEST] != NULL)
      && (IS_SET (pexit_east->exit_info, EX_CLOSED))
      && (IS_SET (pexit_west->exit_info, EX_CLOSED)))
    {
      sprintf (buf4, "|%s   |         |   {x|\n\r", co);
    }


  else if ((ch->in_room->exit[DIR_EAST] != NULL)
	   && (ch->in_room->exit[DIR_WEST] != NULL)
	   && (IS_SET (pexit_east->exit_info, EX_CLOSED)))
    {
      sprintf (buf4, "|%s             |   {x|\n\r", co);
    }

  else if ((ch->in_room->exit[DIR_EAST] != NULL)
	   && (ch->in_room->exit[DIR_WEST] != NULL)
	   && (IS_SET (pexit_west->exit_info, EX_CLOSED)))
    {
      sprintf (buf4, "|%s   |             {x|\n\r", co);
    }


  else if ((ch->in_room->exit[DIR_EAST] == NULL)
	   && (ch->in_room->exit[DIR_WEST] != NULL)
	   && (IS_SET (pexit_west->exit_info, EX_CLOSED)))
    {
      sprintf (buf4, "|%s   |        %.4s {x|\n\r", co, sp);
    }


  else if ((ch->in_room->exit[DIR_EAST] != NULL)
	   && (ch->in_room->exit[DIR_WEST] == NULL)
	   && (IS_SET (pexit_east->exit_info, EX_CLOSED)))
    {
      sprintf (buf4, "|%s %.4s        |   {x|\n\r", co, sp);
    }

  else if ((ch->in_room->exit[DIR_EAST] != NULL)
	   && (ch->in_room->exit[DIR_WEST] == NULL))
    {
      sprintf (buf4, "|%s %.4s            {x|\n\r", co, sp);
    }

  else if ((ch->in_room->exit[DIR_EAST] == NULL)
	   && (ch->in_room->exit[DIR_WEST] != NULL))
    {
      sprintf (buf4, "|%s            %.4s {x|\n\r", co, sp);
    }

  else if ((ch->in_room->exit[DIR_EAST] != NULL)
	   && (ch->in_room->exit[DIR_WEST] != NULL))
    {
      sprintf (buf4, "|%s                 {x|\n\r", co);
    }

  else if ((ch->in_room->exit[DIR_EAST] == NULL)
	   && (ch->in_room->exit[DIR_WEST] == NULL))
    {
      sprintf (buf4, "|%s %.4s       %.4s {x|\n\r", co, sp, sp);
    }

  else
    {
      sprintf (buf4, "|%s x Bugged exit x {x|     \n\r{x", co);
    }


  if ((ch->in_room->exit[DIR_SOUTH] != NULL)
      && (ch->in_room->exit[DIR_DOWN] == NULL)
      && (IS_SET (pexit_south->exit_info, EX_CLOSED)))
    {
      sprintf (buf5, "|%s %.4s       %.4s {x|     \n\r{x", co, sp, sp);
      sprintf (buf6, "|%s %.6s---%.6s {x|     \n\r{x", co, sp, sp);
      sprintf (buf7, "|%s %.6s   %.6s {x|     \n\r{x", co, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_SOUTH] != NULL)
	   && (ch->in_room->exit[DIR_DOWN] != NULL)
	   && (IS_SET (pexit_south->exit_info, EX_CLOSED))
	   && (IS_SET (pexit_down->exit_info, EX_CLOSED)))
    {
      sprintf (buf5, "|%s %.3s        %.4s {x|     \n\r{x", co, sp, sp);
      sprintf (buf6, "|%s %.2s---%.1s---%.6s {x|     \n\r{x", co, sp, sp, sp);
      sprintf (buf7, "|%s %.1s   %.2s   %.6s {x|     \n\r{x", co, sp, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_SOUTH] != NULL)
	   && (ch->in_room->exit[DIR_DOWN] != NULL)
	   && (IS_SET (pexit_south->exit_info, EX_CLOSED)))
    {
      sprintf (buf5, "|%s %.3s        %.4s {x|     \n\r{x", co, sp, sp);
      sprintf (buf6, "|%s %.2s   %.1s---%.6s {x|     \n\r{x", co, sp, sp, sp);
      sprintf (buf7, "|%s %.1s   %.2s   %.6s {x|     \n\r{x", co, sp, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_SOUTH] == NULL)
	   && (ch->in_room->exit[DIR_DOWN] != NULL)
	   && (IS_SET (pexit_down->exit_info, EX_CLOSED)))
    {
      sprintf (buf5, "|%s %.3s        %.4s {x|     \n\r{x", co, sp, sp);
      sprintf (buf6, "|%s %.2s---%.10s {x|     \n\r{x", co, sp, sp);
      sprintf (buf7, "|%s %.1s   %.11s {x|     \n\r{x", co, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_SOUTH] != NULL)
	   && (ch->in_room->exit[DIR_DOWN] != NULL)
	   && (IS_SET (pexit_south->exit_info, EX_CLOSED))
	   && (IS_SET (pexit_down->exit_info, EX_CLOSED)))
    {
      sprintf (buf5, "|%s %.3s        %.4s {x|     \n\r{x", co, sp, sp);
      sprintf (buf6, "|%s %.2s---%.1s---%.6s {x|     \n\r{x", co, sp, sp, sp);
      sprintf (buf7, "|%s %.1s   %.2s   %.6s {x|     \n\r{x", co, sp, sp, sp);
    }


  else if ((ch->in_room->exit[DIR_SOUTH] != NULL)
	   && (ch->in_room->exit[DIR_DOWN] != NULL)
	   && (IS_SET (pexit_down->exit_info, EX_CLOSED)))
    {
      sprintf (buf5, "|%s %.3s        %.4s {x|     \n\r{x", co, sp, sp);
      sprintf (buf6, "|%s %.2s---%.1s   %.6s {x|     \n\r{x", co, sp, sp, sp);
      sprintf (buf7, "|%s %.1s   %.2s   %.6s {x|     \n\r{x", co, sp, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_SOUTH] != NULL)
	   && (ch->in_room->exit[DIR_DOWN] != NULL))
    {
      sprintf (buf5, "|%s %.3s        %.4s {x|     \n\r{x", co, sp, sp);
      sprintf (buf6, "|%s %.2s   %.1s   %.6s {x|     \n\r{x", co, sp, sp, sp);
      sprintf (buf7, "|%s %.1s   %.2s   %.6s {x|     \n\r{x", co, sp, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_SOUTH] != NULL)
	   && (ch->in_room->exit[DIR_DOWN] == NULL))
    {
      sprintf (buf5, "|%s %.4s       %.4s {x|     \n\r{x", co, sp, sp);
      sprintf (buf6, "|%s %.6s   %.6s {x|     \n\r{x", co, sp, sp);
      sprintf (buf7, "|%s %.6s   %.6s {x|     \n\r{x", co, sp, sp);
    }


  else if ((ch->in_room->exit[DIR_SOUTH] == NULL)
	   && (ch->in_room->exit[DIR_DOWN] != NULL))
    {
      sprintf (buf5, "|%s %.3s        %.4s {x|     \n\r{x", co, sp, sp);
      sprintf (buf6, "|%s %.2s   %.10s {x|     \n\r{x", co, sp, sp);
      sprintf (buf7, "|%s %.1s   %.11s {x|     \n\r{x", co, sp, sp);
    }

  else if ((ch->in_room->exit[DIR_SOUTH] == NULL)
	   && (ch->in_room->exit[DIR_DOWN] == NULL))
    {
      sprintf (buf5, "|%s %.4s       %.4s {x|     \n\r{x", co, sp, sp);
      sprintf (buf6, "|%s %.15s {x|     \n\r{x", co, sp);
      sprintf (buf7, "|%s %.15s {x|     \n\r{x", co, sp);
    }

  else
    {
      sprintf (buf5, "|%s xxxxxxxxxxxxxxx {x|     \n\r{x", co);
      sprintf (buf6, "|%s xxxx Bugged xxx {x|     \n\r{x", co);
      sprintf (buf7, "|%s xxxxx exit xxxx {x|     \n\r{x", co);
    }


  send_to_char (buf, ch);
  send_to_char (buf2, ch);
  send_to_char (buf3, ch);
  send_to_char (buf4, ch);
  send_to_char (buf5, ch);
  send_to_char (buf6, ch);
  send_to_char (buf7, ch);

  send_to_char ("|                 |\n\r", ch);
  send_to_char ("'-----------------'\n\r", ch);

}
