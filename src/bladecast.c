/**************************************************************************
*      Mindor 1.0 is copyright 2002-2019 by Chris Cox                    *
*      Portions of the code were inspired by other works                 *
*      found around the internet.                                        *
*                                                                        *
*      Please follow all previous copyrights and licenses.               *
**************************************************************************/

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
#include <unistd.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
/* command procedures needed */
DECLARE_DO_FUN( do_bladecast        );

char * bladecast_spells[10][16];

void do_bladecast (CHAR_DATA * ch,char * argument)
{
	CHAR_DATA * vch;
	OBJ_DATA * blade;
    AFFECT_DATA af;
    AFFECT_DATA *paf;
	int sn, level, mana, d1, d2;

	/* Do they know the skill? */
	if (get_skill (ch, gsn_bladecast) == 0)
	{
		send_to_char ("You have no idea what you're doing.\n\r", ch);
		return;
	}

	/* Make sure we have a weapon, AND that said weapon is a sword.*/
	if ((blade = get_eq_char (ch, WEAR_WIELD)) == NULL
		|| get_weapon_sn (ch) != gsn_sword)
	{
		send_to_char ("You have to be wielding a sword to bladecast.\n\r", ch);
		return;
	}

	/* Now make sure this blade hasn't already been used recently. */
    for (paf = blade->affected; paf != NULL; paf = paf->next)
	{
		if (paf->type == gsn_bladecast)
		{
			send_to_char ("This weapon still needs to recover from "
						  "bladecasting.\n\r", ch);
			return;
		}
	}

	/* Next we need a target. */
	if (argument == '\0')
	{
		/* If we're fighting it will target itself.*/
		if ((vch = ch->fighting) == NULL)
		{
			send_to_char ("Who are you trying to unleash the blade's "
						  "wrath upon?\n\r", ch);
			return;
		}
	}
	/* Otherwise...  We need to supply a valid target. */
	else if ((vch = get_char_room (ch, argument)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	/* And another failsafe... */
	else if (vch == ch)
	{
		send_to_char ("Stop hittin' yourself!\n\r", ch);
		return;
	}

	/* Spell level is averaged from user's level and sword's diceroll.*/
	level = (ch->level + dice (blade->value[1], blade->value[2])) / 2;

	/* Grab the skill number from the table above.*/
	d1 = UMIN(9, blade->value[1]);
	d2 = UMIN(15, blade->value[2]);
	
	/* This will hopefully catch any non-existant spells. */
	if ((sn = skill_lookup (bladecast_spells[d1][d2])) == -1
		|| skill_table[sn].spell_fun == NULL)
	{
		send_to_char ("This blade seems at peace.\n\r", ch);
		return;
	}

	/* Average of 1/2 spell's actual mana cost and bladecast cost. */
	mana = ((skill_table[sn].min_mana / 2) + skill_table[gsn_bladecast].min_mana) / 2;

	/* Make sure we have enough mana. */
	if (ch->mana < mana)
	{
		send_to_char ("You don't have enough mana.\n\r", ch);
		return;
	}

	/* Success rate drops as mana gets lower. */
	if (number_percent () > get_skill (ch, gsn_bladecast) * 
							(ch->mana * 100 / ch->max_mana) / 100)
	{
		ch->mana -= mana / 5;
		send_to_char ("You failed to draw out the blade's wrath.\n\r", ch);
		return;
	}

	/* Cast the spell... */
	obj_cast_spell (sn, level, ch, vch, blade);
	WAIT_STATE (ch, skill_table[gsn_bladecast].beats);

	/* See if we draw THE NUMBER. */
	if (number_percent () == 42)
		/* If we did, this blade is toast. */
		extract_obj (blade);
	else
	{
		/* Cooldown effect to keep it from being overused. */
		af.where = TO_OBJECT;
		af.type = gsn_bladecast;
		af.level = 0;
		af.duration = 2;
		af.modifier = 0;
		af.location = APPLY_NONE;
		affect_to_obj (blade, &af);
	}
	return;
}

char * bladecast_spells[10][16] =
{
	{"magic missile", "magic missile", "magic missile", "magic missile",
	 "magic missile", "magic missile", "magic missile", "magic missile",
	 "magic missile", "magic missile", "magic missile", "magic missile",
	 "magic missile", "magic missile", "magic missile", "magic missile"},

	/*1d...*/
	{"magic missile",
		/*...d1*/	"cause light",
		/*...d2*/	"slow",
		/*...d3*/	"energy drain",
		/*...d4*/	"magic missile",
		/*...d5*/	"chill touch",
		/*...d6*/	"blindness",
		/*...d7*/	"shocking grasp",
		/*...d8*/	"faerie fire",
		/*...d9*/	"cause light",
		/*...d10*/	"magic missile",
		/*...d11*/	"burning hands",
		/*...d12*/	"weaken",
		/*...d13*/	"dispel good",
		/*...d14*/	"dispel evil",
		/*...d15*/	"change sex"
	},
	/*2d...*/
	{"magic missile",
		/*...d1*/	"cause light",
		/*...d2*/	"blindness",
		/*...d3*/	"poison",
		/*...d4*/	"acid blast",
		/*...d5*/	"slow",
		/*...d6*/	"cause light",
		/*...d7*/	"chill touch",
		/*...d8*/	"chain lightning",
		/*...d9*/	"faerie fire",
		/*...d10*/	"lightning bolt",
		/*...d11*/	"fireball",
		/*...d12*/	"shocking grasp",
		/*...d13*/	"magic missile",
		/*...d14*/	"change sex",
		/*...d15*/	"energy drain"
	},
	/*3d...*/
	{"magic missile",
		/*...d1*/	"earthquake",
		/*...d2*/	"gas breath",
		/*...d3*/	"burning hands",
		/*...d4*/	"gas breath",
		/*...d5*/	"weaken",
		/*...d6*/	"chill touch",
		/*...d7*/	"slow",
		/*...d8*/	"ray of truth",
		/*...d9*/	"cause serious",
		/*...d10*/	"poison",
		/*...d11*/	"lightning bolt",
		/*...d12*/	"shocking grasp",
		/*...d13*/	"heat metal",
		/*...d14*/	"poison",
		/*...d15*/	"acid blast"
	},
	/*4d...*/
	{"magic missile",
		/*...d1*/	"dispel magic",
		/*...d2*/	"cause critical",
		/*...d3*/	"cause serious",
		/*...d4*/	"heat metal",
		/*...d5*/	"dispel good",
		/*...d6*/	"dispel evil",
		/*...d7*/	"cause serious",
		/*...d8*/	"fireball",
		/*...d9*/	"chain lightning",
		/*...d10*/	"colour spray",
		/*...d11*/	"heat metal",
		/*...d12*/	"chill touch",
		/*...d13*/	"lightning bolt",
		/*...d14*/	"poison",
		/*...d15*/	"holy word"
	},
	/*5d...*/
	{"magic missile",
		/*...d1*/	"shocking grasp",
		/*...d2*/	"flamestrike",
		/*...d3*/	"fireball",
		/*...d4*/	"cause serious",
		/*...d5*/	"cause critical",
		/*...d6*/	"earthquake",
		/*...d7*/	"blindness",
		/*...d8*/	"flamestrike",
		/*...d9*/	"heat metal",
		/*...d10*/	"ray of truth",
		/*...d11*/	"faerie fire",
		/*...d12*/	"colour spray",
		/*...d13*/	"curse",
		/*...d14*/	"weaken",
		/*...d15*/	"lightning bolt"
	},
	/*6d...*/
	{"magic missile",
		/*...d1*/	"harm",
		/*...d2*/	"fireball",
		/*...d3*/	"acid breath",
		/*...d4*/	"slow",
		/*...d5*/	"frost breath",
		/*...d6*/	"burning hands",
		/*...d7*/	"cause critical",
		/*...d8*/	"ray of truth",
		/*...d9*/	"fire breath",
		/*...d10*/	"energy drain",
		/*...d11*/	"flamestrike",
		/*...d12*/	"lightning breath",
		/*...d13*/	"harm",
		/*...d14*/	"acid blast",
		/*...d15*/	"earthquake"
	},
	/*7d...*/
	{"magic missile",
		/*...d1*/	"acid breath",
		/*...d2*/	"flamestrike",
		/*...d3*/	"fire breath",
		/*...d4*/	"change sex",
		/*...d5*/	"weaken",
		/*...d6*/	"frost breath",
		/*...d7*/	"cause critical",
		/*...d8*/	"lightning breath",
		/*...d9*/	"ray of truth",
		/*...d10*/	"dispel magic",
		/*...d11*/	"harm",
		/*...d12*/	"demonfire",
		/*...d13*/	"curse",
		/*...d14*/	"earthquake",
		/*...d15*/	"holy word"
	},
	/*8d...*/
	{"magic missile",
		/*...d1*/	"burning hands",
		/*...d2*/	"lightning breath",
		/*...d3*/	"acid blast",
		/*...d4*/	"gas breath",
		/*...d5*/	"dispel good",
		/*...d6*/	"dispel evil",
		/*...d7*/	"acid breath",
		/*...d8*/	"dispel magic",
		/*...d9*/	"faerie fire",
		/*...d10*/	"curse",
		/*...d11*/	"frost breath",
		/*...d12*/	"chain lightning",
		/*...d13*/	"demonfire",
		/*...d14*/	"fire breath",
		/*...d15*/	"colour spray"
	},
	/*9d...*/
	{"magic missile",
		/*...d1*/	"demonfire",
		/*...d2*/	"blindness",
		/*...d3*/	"frost breath",
		/*...d4*/	"chain lightning",
		/*...d5*/	"fire breath",
		/*...d6*/	"demonfire",
		/*...d7*/	"acid breath",
		/*...d8*/	"dispel magic",
		/*...d9*/	"dispel good",
		/*...d10*/	"dispel evil",
		/*...d11*/	"harm",
		/*...d12*/	"curse",
		/*...d13*/	"holy word",
		/*...d14*/	"lightning breath",
		/*...d15*/	"magic missile"
	}
};

