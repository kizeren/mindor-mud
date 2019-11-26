
/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/


/**************************************************************************
*      Mindor 1.0 is copyright 2002-2004 by Shaun Mcbride                *
*      Portions of the code were inspired by other works                 *
*      found around the internet.                                        *
*                                                                        *
*      Please follow all previous copyrights and licenses.               *
**************************************************************************/


/*
 *  This is a specially edited version of remort.c   Editing done by
 *  Ryouga and Jeff for This Old Mud.
 *  Taylored to Mindor in late 2003 by Kizeren
 */

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

/* command procedures needed */
DECLARE_DO_FUN(do_inform);
DECLARE_DO_FUN(do_recall);
DECLARE_DO_FUN(do_announce);



void do_remort( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MAX_INPUT_LENGTH];
    int sn = 0;
    sh_int wear_loc;
    OBJ_DATA *obj,*obj_next;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Remort whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if (IS_NPC(victim))
    {
	send_to_char("You are going to rebirth a mob?!?!\n\r",ch);
	return;
    }

    if (ch == victim)
    {
	send_to_char( "You cannot rebirth yourself silly.\n\r",ch);
	return;
    }
    
      
    if (victim->level < LEVEL_HERO - 10)
    {
	sprintf(buf, "You can only rebirth players from level %d to 101.\n\r",
	    LEVEL_HERO - 10 );
	send_to_char(buf,ch);
	return;
    }

    if (victim->pcdata->remorts >= 10)
    {
	send_to_char("This char has already been rebirthed too many times.\n\r",ch);
	return;
    }

    victim->pcdata->remorts += 1;
    sprintf(buf,
	"Having conquered that which has been set before you, you demand that\n\r"
	"the immortals buff you up a little more, maybe with eq or a nifty set \n\r"
	"command. Well it seems your ego has gotten you into some trouble, as\n\r"
	"your mind becomes instantly filled with a loud thunder and images of death.\n\r"
	"Maybe today was the wrong time to annoy the immortals...\n\r");
    send_to_char(buf,victim);


 
    for (wear_loc = WEAR_LIGHT; wear_loc <= MAX_WEAR; wear_loc++)
    {
	if ((obj = get_eq_char(victim,wear_loc)) == NULL)
	    continue;
	unequip_char(victim,obj);
    }

    for (obj = victim->carrying; obj != NULL; obj = obj_next)
    {
	obj_next = obj->next_content;
	extract_obj(obj);
    }

    victim->pcdata->perm_hit /= 3;
    victim->pcdata->perm_mana /= 3;
    victim->pcdata->perm_move /= 3;

    victim->max_hit = victim->pcdata->perm_hit;
    victim->max_mana = victim->pcdata->perm_mana;
    victim->max_move = victim->pcdata->perm_move;
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    victim->level = 10;
    victim->pcdata->points -= 7;   
    victim->exp = exp_per_level(victim,victim->pcdata->points) * victim->level;
    victim->pcdata->learned[sn] = 1; 
    do_recall(victim,"");
    do_announce(victim, "has been reborn!");  

    return;
}


     
