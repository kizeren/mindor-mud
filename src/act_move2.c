/**************************************************************************
*      Mindor 1.0 is copyright 2002-2004 by Shaun Mcbride                *
*      Portions of the code were inspired by other works                 *
*      found around the internet.                                        *
*                                                                        *
*      Please follow all previous copyrights and licenses.               *
**************************************************************************/




#if defined(macintosh)
#include <types.h>
#include <time.h>
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
#include "merc.h"
#include "tables.h"

DECLARE_DO_FUN(do_look);

void do_meet1(CHAR_DATA *ch, char *arguement)
{
char_from_room(ch);
char_to_room(ch, get_room_index(ROOM_VNUM_VOID));
do_look( ch, "auto" );
return;
}

void do_meet2(CHAR_DATA *ch, char *arguement)
{
char_from_room(ch);
char_to_room(ch, get_room_index(ROOM_VNUM_MEET));
do_look( ch, "auto" );
return;
}

void do_meet(CHAR_DATA *ch)
 
{

CHAR_DATA *victim;

if (ch->level >= 75)
{
stc("You must walk there.\n\r",ch);
return;
}

if (IS_NPC(ch))
{
send_to_char("Only players can goto the gathering place.\n\r",ch);
return;
}
if (ch->in_room->vnum == 4)
  {
  stc("You are already there dummy!",ch);
  return;
  }
if ( ( victim = ch->fighting ) != NULL )
   {
   stc("You are fighting NO WAY!",ch);
   return;
   }
         if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
         {
         stc("I think you are a little to drunk to do that.\n\rClear you head first then the gods may let you.\n\r",ch);
         return;
         }
    if ( ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE))
    &&   (ch->level <= HERO) )
    {
	act( "$G has forsaken you.", ch, NULL, NULL, TO_CHAR );
	return;
    }

WAIT_STATE( ch, PULSE_EVENT * 14 );

wait_act(3,"$n bows $s head and prays for transporation to the gathering place.\n\r", ch, 0, 0, 
    TO_ROOM);
wait_act(3,"You bow your head and pray for transportation to the gathering place.\n\r", ch, 0, 0,
    TO_CHAR);

wait_act(8,"$n has left.\n\r",ch ,0 ,0, TO_ROOM);

wait_function(ch, 8, do_meet1, "");

wait_function(ch, 13, do_meet2, "");
wait_act(14,"You have arrived and feeling a little dizzy.\n\r",ch,0,0,TO_CHAR);
wait_act(14,"$n has arrived and is ready to have $s say.\n\r",ch,0,0,TO_ROOM);
}

void do_newbiezone(CHAR_DATA *ch, char *arguement)
{

if (ch->level >= 5)
    {
      stc("Sorry but your above the ranks of a newbie.\n\r",ch);
      return;
    }
else 
{

char_from_room(ch);
char_to_room(ch, get_room_index(ROOM_VNUM_NEWBIEZONE));
do_look( ch, "auto" );
return;
}
}



