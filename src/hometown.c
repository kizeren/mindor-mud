/**************************************************************************
*      Mindor 1.0 is copyright 2002-2004 by Shaun Mcbride                *
*      Portions of the code were inspired by other works                 *
*      found around the internet.                                        *
*                                                                        *
*      Please follow all previous copyrights and licenses.               *
**************************************************************************/





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

DECLARE_DO_FUN(do_announce);


void do_ctell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH]; 
    DESCRIPTOR_DATA *d;
             

   /* First if for Nomad or No hometown */

   if (ch->hometown == 0)
    {
     send_to_char("You are an outcast, you can't use this feature.\n\r",ch);
     return;
    }
    

   /* This was set just like the NOCLAN since I suppose it is just like the clan channel */


    if ( argument[0] == '\0' )  
    {
      if (IS_SET(ch->comm,COMM_NOCLAN))
      {
        send_to_char("Clan channel is now ON\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOCLAN);
      }
      else
      {
        send_to_char("Clan channel is now OFF\n\r",ch);
        SET_BIT(ch->comm,COMM_NOCLAN);
      }
      return;
    }
  if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
         send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }    
        
       /* Have to remove the bit to enable talking */

        REMOVE_BIT(ch->comm,COMM_NOCLAN);
	if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	argument = makedrunk(argument,ch);
       
     /* This code will display which ever hometown they are in. *
      * SHouldn't have to touch this unless you find a bug.     *
      *  Kizeren 2003                                           */

      sprintf( buf, "You tell the city of %s, '{C%s{x'\n\r",hometown_table[ch->hometown].name, argument);
      send_to_char( buf, ch );
      sprintf( buf, "{y( %s Tell ){x %s%s: '{C%s{x'\n\r",hometown_table[ch->hometown].name, ch->name,ch->pcdata->last_name, argument);   
    for ( d = descriptor_list; d != NULL; d = d->next )
    {  
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             is_same_hometown(ch,d->character) &&
             !IS_SET(d->character->comm,COMM_NOCLAN) &&
             !IS_SET(d->character->comm,COMM_QUIET) )
        {
            send_to_char(buf,d->character);
        }
    }   
        
    return;
}


/* Kizeren 2003  Seperate who for cities.  Displaying ranks and such for city memebers. */

void do_whocity( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MSL];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
    buf[0] = '\0';
    output = new_buf();

    /* Can't use if not in a city and Nomad is not a city. */

    if (ch->hometown == 0)
    {
     send_to_char("You are an outcast, you can't use this channel.\n\r", ch);
     return;
    }


    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
        

        if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
            continue;
 
        wch   = ( d->original != NULL ) ? d->original : d->character;

	if (!can_see(ch,wch))
	    continue;

        if (!is_same_hometown(ch,d->character))
            continue;

        /* Here is where we format up the who list for cities.  */
 
	sprintf( buf, "%s%s%s%s is a %s of %s.\n\r", wch->pcdata->pretitle,
	    wch->short_descr, wch->pcdata->last_name,
	    IS_NPC(wch) ? "" : wch->pcdata->title, city_rank_table[wch->pcdata->city_ranks].rank, hometown_table[wch->hometown].name );
	add_buf(output,buf);
    }

    /* This just displays the city name at the bottom */

    sprintf( buf2, "\n\rCitizens in the city of %s.\n\r", hometown_table[ch->hometown].name );
    add_buf(output,buf2);
    page_to_char( buf_string(output), ch );
    free_buf(output);
    return;
}


/* Kizeren 2003  Found this neat snippet on kyndig I think it was.  Basically lets you do ranks in Clans. *
 * I ended up heavily modifiying it for use with in the city structures.                                  */


void do_grant( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

   /* Can't promote mobs */

    if ( IS_NPC(ch) )
    {
        send_to_char( "NPC's can not promote someone.\n\r",ch);
        return;
    }
   
    /* Only city elders and above including immortals can promote someone */

    if ( ( ch->pcdata->city_ranks < 8 ) && ( !IS_IMMORTAL(ch) ) )
    {
        send_to_char( "You must be a Mayor or an Elder of a city to promote someone.\n\r",ch);
        return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0' 
      || atoi(arg2) < 1 || atoi(arg2) > 10 )
    {
        send_to_char( "Syntax: grant <char> <rank 1-10>\n\r",ch);
        return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They must be present to be promoted.\n\r", ch );
        return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("You must be mad.\n\r",ch);
	return;
    }
    if (victim->name == ch->name)
    {
        send_to_char("You can't promote yourself!\n\r",ch);
        return;
    }
    if (victim->level >= MAX_LEVEL - 8)
    {
      send_to_char("You can't grant city ranks to gods!\n\r",ch);
      return;
    }
    if ( (victim->hometown != ch->hometown ) && (!IS_IMMORTAL(ch) ) )
    {
	send_to_char("You can not promote a player who is not in your hometown.\n\r",ch);
	return;
    }
    
    if ( !IS_SET(victim->act, PLR_MAYOR) && (IS_IMMORTAL(ch) && atoi(arg2)==10 ))
    {
	send_to_char("This player is not qualified to be mayor.\n\r",ch);
	return;
    }

    victim->pcdata->city_ranks = atoi(arg2);
    
    send_to_char( "Rank ceremony complete\n\r", ch);
    send_to_char( "Rank ceremony complete\n\r", victim);
    
    return;
}    


/* Kizeren 2003 Almost a clone from clead */

void do_mlead( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: mlead <char>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    
    if (!is_hometown(victim))
    {
        send_to_char("This person is not in a your city.\n\r",ch);
        return;
    }
 
    if (hometown_table[victim->hometown].name == NULL)
    {
        send_to_char("This person does not belong to a city.\n\r",ch);
	return;
    }

    if (IS_SET(victim->act, PLR_MAYOR))
    {
	sprintf(buf,"They are no longer Mayor of %s.\n\r",
	    capitalize(hometown_table[victim->hometown].name));
	send_to_char(buf,ch);
	sprintf(buf,"You are no longer Mayor of %s.\n\r",
	    capitalize(hometown_table[victim->hometown].name));
        send_to_char(buf,victim);

	REMOVE_BIT(victim->act, PLR_MAYOR);
    }
    else
    {
        sprintf(buf,"They are now Mayor of %s.\n\r",
            capitalize(hometown_table[victim->hometown].name));
        send_to_char(buf,ch);
        sprintf(buf,"You are now Mayor of %s.\n\r",
            capitalize(hometown_table[victim->hometown].name));
        send_to_char(buf,victim); 
	SET_BIT(victim->act, PLR_MAYOR);
    }
}

/* Kizeren 2003 My first attempt at the pretitle code from scartch. */

void do_pretitle(CHAR_DATA *ch, char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  argument = one_argument(argument, arg);

  if(argument[0] == '\0' || arg[0] == '\0' || !(victim =
get_char_world(ch, arg))) {
    send_to_char("Syntax: pretitle [victim] [arg]\n\r", ch);
    return;
}             



    if ( IS_NPC(victim) )
    {
        send_to_char("You must be mad.\n\r",ch);
        return;
    }

    /* if thier not in a city you can not pretitle them */

    if (!is_hometown(victim))
    {
        send_to_char("This person is not in a city.\n\r",ch);
        return;
    }

   /* no pretiteling your self */

  if (victim->name == ch->name)
  {
    send_to_char("You can't give youself a pretitle.",ch);
    return;
   }

   /* Immortals can't be pretitled by mortals */

  if (victim->level >= MAX_LEVEL - 8)
  {
   send_to_char("The gods do not need pretitles",ch);
   return;
  }   
    /* If thier not in your city, you can't pretitle them */

    if ( (victim->hometown != ch->hometown ) && (!IS_IMMORTAL(ch) ) )
    {
        send_to_char("You can not give a player who is not in your hometown a pretitle.\n\r",ch);
        return;
    }

    /* Only elder and above can do pretitles including immortals */
    if ( ( ch->pcdata->city_ranks < 8 ) && ( !IS_IMMORTAL(ch) ) )
    {
        send_to_char( "You must be a Mayor or an Elder of a city to give someone a pretitles.\n\r",ch);
        return;
    }     
  
  victim->pcdata->pretitle = str_dup(argument);
  sprintf(buf, "%s's pretitle is now %s\n\r", victim->name, victim->pcdata->pretitle);
  send_to_char(buf, ch);
  sprintf(buf, "Your pretitle is now %s\n\r", victim->pcdata->pretitle);
  send_to_char(buf,victim);
  return;
}

/* Kizeren 2003 Modified version of my pretitle code. */

void do_lastname(CHAR_DATA *ch, char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  argument = one_argument(argument, arg);

  if(argument[0] == '\0' || arg[0] == '\0' 
           || !(victim = get_char_world(ch, arg))) 
{
    send_to_char("Syntax: pretitle [victim] [arg]\n\r", ch);
    return;
}             


  if (victim->level >= MAX_LEVEL - 8)
  {
   send_to_char("The gods do not need last names",ch);
   return;
  }
    if ( IS_NPC(victim) )
    {
        send_to_char("You must be mad.\n\r",ch);
        return;
    }
  if (victim->name == ch->name)
  {
    send_to_char("You can't give youself a last name",ch);
    return;
   }
  if (victim->level >= MAX_LEVEL - 8)
  {
   send_to_char("The gods do not need last names",ch);
   return;
  }   
    if ( (victim->hometown != ch->hometown ) && (!IS_IMMORTAL(ch) ) )
    {
        send_to_char("You can not give a player who is not in your hometown a last name.\n\r",ch);
        return;
    }
    if ( ( ch->pcdata->city_ranks < 8 ) && ( !IS_IMMORTAL(ch) ) )
    {
        send_to_char( "You must be a Mayor or an Elder of a city to give someone a last name.\n\r",ch);
        return;
    }     
  
  victim->pcdata->last_name = str_dup(argument);
  sprintf(buf, "%s's last name is now %s\n\r", victim->name, victim->pcdata->last_name);
  send_to_char(buf, ch);
  sprintf(buf, "Your last name is now %s\n\r", victim->pcdata->last_name);
  send_to_char(buf,victim);
  return;
}




/* Kizeren 2003 Found a snippet on the internet. Didn't work. Ended up with this. *
 * After many hours of trying to figure out why the code no works.  I pretty much *
 * stripped the original code and based this off of it.  Not quite sure if they   *
 * are the same or not.  Lost the original code in a power outage one nite.       */



char * makedrunk (char *string, CHAR_DATA * ch)
{
/* This structure defines all changes for a character */
  struct struckdrunk drunk[] =
  {
    {3, 10,
     {"a", "a", "a", "A", "aa", "ah", "Ah", "ao", "aw", "oa", "ahhhh"}},
    {8, 5,
     {"b", "b", "b", "B", "B", "vb"}},
    {3, 5,
     {"c", "c", "C", "cj", "sj", "zj"}},
    {5, 2,
     {"d", "d", "D"}},
    {3, 3,
     {"e", "e", "eh", "E"}},
    {4, 5,
     {"f", "f", "ff", "fff", "fFf", "F"}},
    {8, 2,
     {"g", "g", "G"}},
    {9, 6,
     {"h", "h", "hh", "hhh", "Hhh", "HhH", "H"}},
    {7, 6,
     {"i", "i", "Iii", "ii", "iI", "Ii", "I"}},
    {9, 5,
     {"j", "j", "jj", "Jj", "jJ", "J"}},
    {7, 2,
     {"k", "k", "K"}},
    {3, 2,
     {"l", "l", "L"}},
    {5, 8,
     {"m", "m", "mm", "mmm", "mmmm", "mmmmm", "MmM", "mM", "M"}},
    {6, 6,
     {"n", "n", "nn", "Nn", "nnn", "nNn", "N"}},
    {3, 6,
     {"o", "o", "ooo", "ao", "aOoo", "Ooo", "ooOo"}},
    {3, 2,
     {"p", "p", "P"}},
    {5, 5,
     {"q", "q", "Q", "ku", "ququ", "kukeleku"}},
    {4, 2,
     {"r", "r", "R"}},
    {2, 5,
     {"s", "ss", "zzZzssZ", "ZSssS", "sSzzsss", "sSss"}},
    {5, 2,
     {"t", "t", "T"}},
    {3, 6,
     {"u", "u", "uh", "Uh", "Uhuhhuh", "uhU", "uhhu"}},
    {4, 2,
     {"v", "v", "V"}},
    {4, 2,
     {"w", "w", "W"}},
    {5, 6,
     {"x", "x", "X", "ks", "iks", "kz", "xz"}},
    {3, 2,
     {"y", "y", "Y"}},
    {2, 9,
     {"z", "z", "ZzzZz", "Zzz", "Zsszzsz", "szz", "sZZz", "ZSz", "zZ", 
"Z"}}
  };

  char buf[1024];
  char temp;
  int pos = 0;
  int drunklevel;
  int randomnum;

  /* Check how drunk a person is... */
  if (IS_NPC(ch))
        drunklevel = 0;
  else
        drunklevel = ch->pcdata->condition[COND_DRUNK];

  if (drunklevel > 0)
    {
      do
        {
          temp = toupper (*string);
          if ((temp >= 'A') && (temp <= 'Z'))
            {
              if (drunklevel > drunk[temp - 'A'].min_drunk_level)
                {
                  randomnum = number_range (0, drunk[temp - 
'A'].number_of_rep);
                  strcpy (&buf[pos], drunk[temp - 
'A'].replacement[randomnum]);
                  pos += strlen (drunk[temp - 
'A'].replacement[randomnum]);
                }
              else
                buf[pos++] = *string;
            }
          else
            {
              if ((temp >= '0') && (temp <= '9'))
                {
                  temp = '0' + number_range (0, 9);
                  buf[pos++] = temp;
                }
              else
                buf[pos++] = *string;
            }
        }
      while (*string++);
      buf[pos] = '\0';          /* Mark end of the string... */
      strcpy(string, buf);
      return(string);
    }
  return (string);
}


/* Kizeren 2003  Very simple bounty code I wrote.  Was board. Had a need to kill mortals */


void do_bounty( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
	
    
	if ( arg1[0] == '\0' || arg2[0] == '\0' )
        {
	     send_to_char( "Place a bounty on who's head?\n\rSyntax: Bounty<victim> <amount>\n\r", ch );
             return;
        }
	
        if ( ( victim = get_char_world( ch, arg1 ) ) == NULL)
	{
  	   send_to_char( "They are currently not logged in!", ch );
	   return;
        }
  
      if (IS_NPC(victim))
      {
	send_to_char( "You cannot put a bounty on NPCs!", ch );
	return;
      }

	if ( is_number( arg2 ) )
        {
	int amount;
	amount   = atoi(arg2);
        if (ch->platinum < amount)
        {
		send_to_char( "You don't have that much platinum!", ch );
		return;
        }
	ch->platinum -= amount;
	victim->pcdata->bounty +=amount;
	sprintf( buf, "You have placed a %d platinum bounty on %s{g.\n\r%s now has a bounty of %d platinu.",
	amount,victim->name,victim->name,victim->pcdata->bounty );
	send_to_char(buf,ch);
	return;
	}
}


/* Kizeren 2003  Had to come up with a way to show mortals current bounties in the game *
 * again a very simple method                                                           */


void do_blist( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
   char buf2[MSL];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
    buf[0] = '\0';
    output = new_buf();
            

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
 
    
        if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
            continue;
    
        wch   = ( d->original != NULL ) ? d->original : d->character;
    
        if (!can_see(ch,wch))
            continue;

        if (wch->pcdata->bounty == 0 )
             continue;
       
        sprintf( buf, "%s has a bounty of %d gold.\n\r",
            wch->short_descr, wch->pcdata->bounty);
        add_buf(output,buf);
   }        
    sprintf( buf2, "\n\rPlayers with bounties.\n\r" );
    add_buf(output,buf2);
     page_to_char( buf_string(output), ch );
    free_buf(output);
    return;
}
   
void do_banish( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );

    if (IS_NPC(ch))
    {
	return;
    }

   
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: banish <char>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    if (IS_NPC(victim))
    {
	send_to_char( "You can't banish NPC's!\n\r", ch);
	return;
    }

    if ( victim->hometown != ch->hometown)
    {
     send_to_char( "The are not the same hometown as you.\n\r",ch);
     return;
    }
    if (!IS_SET(ch->act,PLR_MAYOR) && (!IS_IMMORTAL(ch)))
    {
    send_to_char("You must be a mayor to banish someone.\n\r",ch);
    return;
    }
    if ( victim == ch )
    {
	send_to_char( "You're stuck...only a god can help you now!\n\r",ch);
	return;
    }
    send_to_char("They are no longer in your hometown.\n\r",ch);
    sprintf(buf,"You have been banished from %s!\n\r",hometown_table[victim->hometown].name);
    send_to_char(buf,victim);
    victim->hometown = 0;
}

void do_citizen( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );

    if (IS_NPC(ch))
    {
	return;
    }

   
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: citizen <char>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    if (IS_NPC(victim))
    {
	send_to_char( "You can't grant citizenship to NPC's!\n\r", ch);
	return;
    }

    if ( victim->hometown == ch->hometown)
    {
     send_to_char( "The are the same hometown as you already.\n\r",ch);
     return;
    }
    if (!IS_SET(ch->act,PLR_MAYOR) && (!IS_IMMORTAL(ch)))
    {
    send_to_char("You must be a mayor to grant someone citizenship.\n\r",ch);
    return;
    }
    if ( victim == ch )
    {
	send_to_char( "You're stuck...only a god can help you now!\n\r",ch);
	return;
    }
    send_to_char("They are now a citizen of your hometown.\n\r",ch);
    sprintf(buf,"You have been accepted as a citizen of %s!\n\r",hometown_table[ch->hometown].name);
    send_to_char(buf,victim);
    victim->hometown = ch->hometown;
}


/***************************
New trade command
Used to trade quest/train/practice points.
Still in the works.   08062003
**************************/

void do_make(CHAR_DATA *ch, char *argument)
{
 
   char arg [MIL];
   char arg1 [MIL];
   OBJ_DATA *token;
   int amount;
 

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );
    amount = atoi(arg1);
    token = create_object(get_obj_index(OBJ_VNUM_QUEST),0);

   if (IS_NPC(ch))
      {
         stc("Yea okay like you are worth anything to start with!\n\r",ch);
         return;
      }


    if ( arg[0] == '\0' || arg1[0] == '\0' )
    {
        send_to_char( "Syntax: make <quest/train/prac> <number>\n\r",ch);
        return;
    }
   

    if ( is_number(arg1))
    {

    if ( !str_prefix( arg, "quest" ) )
    {
      if (ch->qps < amount)
      {
        stc("You don't have enough quest points to do that.\n\r",ch);
        return;
      }
      token = create_object( get_obj_index( OBJ_VNUM_QUEST ), 0);
      token->value[0] = amount;
      ch->qps -= amount;
    }
    
    if ( !str_prefix( arg, "train" ) )
    {
      if (ch->train < amount)
        {
         stc("You don't have enough trains to do that.\n\r",ch);
         return;
         }
      token = create_object( get_obj_index( OBJ_VNUM_TRAIN ), 0 );
      token->value[0] = amount;
      ch->train -= amount;
    }
   
    if ( !str_prefix( arg, "practice" ) )
    {
       if (ch->practice < amount)
       {
        stc("You don't have enough practices to do that.\n\r",ch);
        return;
        }
       token = create_object( get_obj_index( OBJ_VNUM_PRAC ), 0 );
       token->value[0] = amount;
       ch->practice -= amount;
     }
    }
     obj_to_char( token ,ch );
     stc("Your token has been made\n\r",ch);
     return;
}
