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
 **************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1995 Russ Taylor							   *
*	ROM has been brought to you by the ROM consortium					   *
*	    Russ Taylor (rtaylor@pacinfo.com)								   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)							   *
*	    Brian Moore (rom@rom.efn.org)									   *
*	By using this code, you have agreed to follow the terms of the		   *
*	ROM license, in the file Rom24/doc/rom.license						   *
***************************************************************************/

/***************************************************************************
*       ROT 1.4 is copyright 1996-1997 by Russ Walsh                       *
*       By using this code, you have agreed to follow the terms of the     *
*       ROT license, in the file doc/rot.license                           *
***************************************************************************/

/**************************************************************************
*      Mindor 1.0 is copyright 2002-2019 by Shaun Mcbride                *
*      Portions of the code were inspired by other works                 *
*      found around the internet.                                        *
*                                                                        *
*      Please follow all previous copyrights and licenses.               *
**************************************************************************/

/* Modified version of the healer code to produce a mob that spells people up */


#if defined(macintosh)
	#include <types.h>
	#include <time.h>
#else
	#include <sys/types.h>
	#if defined(WIN32)
		#include <sys/timeb.h>
	#else
		#include <sys/time.h>
	#endif
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "magic.h"

void do_spellup(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *mob;
    char arg[MAX_INPUT_LENGTH];
    int cost,sn;
    SPELL_FUN *spell;
    char *words;	

    /* check for healer */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act, ACT_IS_SPELLUP) )
            break;
    }
 
    if ( mob == NULL )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }

    one_argument(argument,arg);

    if (arg[0] == '\0')
    {
        /* display price list */
	act("$N says '{aI offer the following spells:{x'",ch,NULL,mob,TO_CHAR);
	send_to_char("  Fly                           10 gold\n\r",ch);
	send_to_char("  Iceshield                     15 gold\n\r",ch);
	send_to_char("  Fireshield                    25 gold\n\r",ch);
	send_to_char("  Shockshield           	      50 gold\n\r",ch);
	send_to_char("  Giant Strength                20 gold\n\r",ch);
	send_to_char("  Frenzy                        15 gold\n\r",ch);
	send_to_char("  Haste                  	      25 gold\n\r",ch); 
	send_to_char("  Invis              	      50 gold\n\r",ch);
	send_to_char("  Sanc                           5 gold\n\r",ch);
	send_to_char(" Type spellup <type> to get spells cast.\n\r",ch);
	return;
    }

    if (!str_prefix(arg,"fly"))
    {
        spell = spell_fly;
	sn    = skill_lookup("fly");
	words = "Iasfd";
	 cost  = 1000;
    }

    else if (!str_prefix(arg,"iceshield"))
    {
	spell = spell_iceshield;
	sn    = skill_lookup("iceshield");
	words = "judicandus gzfuajg";
	cost  = 1600;
    }

    else if (!str_prefix(arg,"fireshield"))
    {
	spell = spell_fireshield;
	sn    = skill_lookup("fireshield");
	words = "judicandus qfuhuqar";
	cost  = 2500;
    }

    else if (!str_prefix(arg,"shockshield"))
    {
	spell = spell_shockshield;
	sn = skill_lookup("shockshield");
	words = "pzar";
	cost  = 5000;
    }

    else if (!str_prefix(arg,"giant strength"))
    {
	spell = spell_giant_strength;
	sn    = skill_lookup("giant strength");
      	words = "judicandus noselacri";		
        cost  = 2000;
    }

    else if (!str_prefix(arg,"frenzy"))
    {
	spell = spell_frenzy;
	sn    = skill_lookup("frenzy");
	words = "judicandus eugzagz";
	cost = 1500;
    }

    else if (!str_prefix(arg,"haste"))
    {
	spell = spell_haste;
	sn    = skill_lookup("haste");
	words = "judicandus sausabru";
	cost  = 2500;
    }
	
    else if (!str_prefix(arg,"invis"))
    {
	spell = spell_invis; 
	sn    = skill_lookup("invis");
	words = "candussido judifgz";
	cost  = 5000;
    }

    else if (!str_prefix(arg,"sanc"))
    {
        spell = spell_sanctuary;
        sn = skill_lookup("sanctuary");
        words = "feois wefjoi";
        cost = 1000;
    }

	
    else if (!str_prefix(arg,"refresh") || !str_prefix(arg,"moves"))
    {
	spell =  spell_refresh;
	sn    = skill_lookup("refresh");
	words = "candusima"; 
	cost  = 500;
    }

    else 
    {
	act("$N says '{aType 'spellup' for a list of spells.{x'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    if (cost > ((ch->platinum * 10000) + (ch->gold * 100) + ch->silver))
    {
	act("$N says '{aYou do not have enough gold for my services.{x'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    WAIT_STATE(ch,PULSE_VIOLENCE);

    deduct_cost(ch,cost,VALUE_SILVER);
    act("$n utters the words '{a$T{x'.",mob,NULL,words,TO_ROOM);
  
    if (spell == NULL)  /* restore mana trap...kinda hackish */
    {
	ch->mana += dice(2,8) + mob->level / 3;
	ch->mana = UMIN(ch->mana,ch->max_mana);
	send_to_char("A warm {Yglow{x passes through you.\n\r",ch);
	return;
     }

     if (sn == -1)
	return;
    
     spell(sn,mob->level,mob,ch,TARGET_CHAR);
}
