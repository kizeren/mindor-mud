
/***************************************************************************
 *  As the Wheel Weaves based on ROM 2.4. Original code by Dalsor, Caxandra,
 *	and Zy of AWW. See changes.log for a list of changes from the original
 *	ROM code. Credits for code created by other authors have been left
 *	intact at the head of each function.
 ***************************************************************************/


/**************************************************************************
*      Mindor 1.0 is copyright 2002-2004 by Shaun Mcbride                *
*      Portions of the code were inspired by other works                 *
*      found around the internet.                                        *
*                                                                        *
*      Please follow all previous copyrights and licenses.               *
**************************************************************************/



#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "recycle.h"
#include "interp.h"

#define stc   send_to_char

/* Thanks to Kyndig and Sandi Fallon for assisting with arguments - Dalsor.
 * At the time I started on furnishings, I was about 12 hours straight into
 * the code and very brain dead. Thanks to them for pointing out my mistakes.
 */

/* Homes uses aspects of OLC, and it should be installed, or the necessary
 * componants installed in order for this to work
 */

/* Damn, this is a lot of defines :(
 * Each one of these vnums will need to be created. If this many items
 * aren't desired, removed the uneccesary ones and comment out the code
 */

RESET_DATA *new_reset_data args ( ( void ) );
void add_reset args ( ( ROOM_INDEX_DATA *room, RESET_DATA *loc_reset, int index ) );
void free_reset_data args ( ( RESET_DATA *pReset ) );
void home_buy ( CHAR_DATA *ch );
void home_sell ( CHAR_DATA *ch, char *argument );
void home_describe ( CHAR_DATA *ch );

void do_home ( CHAR_DATA *ch, char *argument )
{

    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MSL];

	loc = ch->in_room;
	loc_area = ch->in_room->area;
	buf[0] = '\0';

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( IS_NPC(ch)
	|| ch == NULL )
		return;

	if ( arg1[0] == '\0' || !strcmp ( arg1, "list" ) )
	{
		stc("What about a home?\n\r",ch);
		stc("\n\rSyntax: Home (buy, sell, furnish, describe)\n\r",ch);
		stc("\n\rExtended:\n\rHome (buy, sell, furnish, describe)\n\r",ch);
		stc("Buy      (purchase a home in the room you are standing in)\n\r",ch);
		stc("Sell     (confirm - does not prompt for confirmation!)\n\r",ch);
		stc("Furnish  (allows purchases of items)\n\r",ch);
		stc("Describe (describe the room - uses the OLC editor)\n\r",ch);
		return;
	}
/*****************Disable MIndor Apartments Only Code, Maji 2003******	
	if ( !is_name( ch->in_room->area->file_name, "mhotel.are" ) )
	{
		stc("You aren't in Mindor Apartments.\n\r",ch);
		return;
	}
**********************************************************************/
	if ( !IS_SET ( loc->room_flags, ROOM_HOME ) )
	{
		stc("You cannot buy a home in this room.\n\r",ch);
		return;
	}

	/* Find out what the argument is, if any */
	if      ( !strcmp ( arg1, "buy" ) )        home_buy ( ch );
	else if ( !strcmp ( arg1, "sell" ) )       home_sell ( ch, arg2 );
	else if ( !strcmp ( arg1, "describe" ) )   home_describe ( ch );
	
	else
	{
		stc("No such Home Command. See HELP HOME for more informatio.\n\r",ch);
		return;
	}
	return;
}

void home_buy ( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
	char buf[MSL];

	loc = ch->in_room;
	loc_area = ch->in_room->area;
	if ( loc->owner[0] == '\0' )
	{
		if ( ch->platinum < 250 )
		{
		stc("This command allows you to buy a home.\n\r",ch);
		stc("You must be standing in the room to buy.\n\r",ch);
		stc("You start with a blank, untitled room that is void of\n\r",ch);
		stc("furnishings and light. A single door allows entrance.\n\r",ch);
		stc("A home costs 250 PLATINUM. You must be carrying the coins.\n\r",ch);
		stc("\n\rSyntax: Home buy\n\r",ch);
		return;
		}
		else
		{
	    free_string ( loc->owner );
	    loc->owner = str_dup ( ch->name );
	    ch->platinum -= 250;
	    free_string ( loc->name );
	    sprintf ( buf, "%s's Home", ch->name );
	    loc->name = str_dup ( buf );
	    stc("Congratulations on purchasing your new home!\n\r",ch);
	    stc("You are now 250 platinum coins lighter and the owner\n\r",ch);
	    stc("of the room in which you now stand! Be sure to describe\n\r",ch);
	    stc("your home and purchase furnishings for it.\n\r",ch);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
		}
	}
	else
	{
		stc("This home is already owned.\n\r",ch);
		return;
	}
	return;
}

void home_sell ( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
	RESET_DATA *current_reset;
	RESET_DATA *next_reset;
    OBJ_DATA *furn;
    OBJ_DATA *furn_next;

	loc = ch->in_room;
	loc_area = ch->in_room->area;

	if ( !is_room_owner ( ch, loc ) )
	{
		stc("But you do not own this room!\n\r",ch);
		return;
	}
	if ( argument[0] == '\0' )
	{
		stc("This command allows you to sell your home.\n\r",ch);
		stc("You will no longer own your home once it is sold,\n\r",ch);
		stc("and you will be compensated half the cost of the home,\n\r",ch);
		stc("not including items you have purchased.\n\r",ch);
		stc("You must be standing in the room which you own.\n\r",ch);
		stc("\n\rSyntax: Home (sell) (confirm)\n\r",ch);
		return;
	}
	else if ( !strcmp ( argument, "confirm" ) )
	{
	    free_string ( loc->owner );
	    loc->owner = str_dup ( "" );
	    ch->platinum += 125;
	    free_string ( loc->name );
	    loc->name = str_dup ( "An Abandoned Home" );

	    if ( !ch->in_room->reset_first )
	    {
			return;
		}
		/* Thanks to Edwin and Kender for the help with killing
		 * resets. Pointed out some real problems with my handling
		 * of it. Thanks a bunch guys.
		 * This is the exact way Kender put it, and it works great!
		 */
		for( current_reset = ch->in_room->reset_first; current_reset;
		    current_reset = next_reset)
		{
		   next_reset = current_reset->next;
		   free_reset_data(current_reset);
		}

		ch->in_room->reset_first = NULL;

		for ( furn = ch->in_room->contents; furn != NULL; furn = furn_next )
		{
		    furn_next = furn->next_content;
	        extract_obj( furn );
		}
		stc("Your home has been sold and you are now 125 platinum coins richer!\n\r",ch);
        SET_BIT(loc_area->area_flags, AREA_CHANGED);
        return;
	}
	else
	{
		stc("This command allows you to sell your home.\n\r",ch);
		stc("You will no longer own your home once it is sold,\n\r",ch);
		stc("and you will be compensated half the cost of the home,\n\r",ch);
		stc("not including items you have purchased.\n\r",ch);
		stc("You must be standing in the room which you own.\n\r",ch);
		stc("\n\rSyntax: Home (sell) (confirm)\n\r",ch);
		return;
	}
}

void home_describe ( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;

	loc = ch->in_room;
	loc_area = ch->in_room->area;

	if ( !is_room_owner ( ch, loc ) )
	{
		stc("But you do not own this room!\n\r",ch);
		return;
	}
	else
	{
		stc("This command allows you to describe your home.\n\r",ch);
		stc("You should not describe items that are in the room,\n\r",ch);
		stc("rather allowing the furnishing of the home to do that.\n\r",ch);
		stc("If you currently own this room, you will be placed into.\n\r",ch);
		stc("the room editor. Be warned that while in the room editor,\n\r",ch);
		stc("you are only allowed to type the description. If you are\n\r",ch);
		stc("unsure or hesitant about this, please note the Immortals,\n\r",ch);
		stc("or better, discuss the how-to's with a Builder.\n\r",ch);
		stc("Syntax: Home (describe)\n\r",ch);
    	string_append( ch, &loc->description );
        SET_BIT(loc_area->area_flags, AREA_CHANGED);
		return;
	}

}
