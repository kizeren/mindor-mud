/*--------------------------------------------------------------------------
              .88b  d88. db    db d8888b.   .d888b. db   dD
              88'YbdP`88 88    88 88  `8D   VP  `8D 88 ,8P'
              88  88  88 88    88 88   88      odD' 88,8P
              88  88  88 88    88 88   88    .88'   88`8b
              88  88  88 88b  d88 88  .8D   j88.    88 `88.
              YP  YP  YP ~Y8888P' Y8888D'   888888D YP   YD
This material is copyrighted (c) Thomas J Whiting (twhiting6@home.com). 
Usage of this material  means that you have read and agree to all of the 
licenses in the ../licenses directory. None of these licenses may ever be 
removed. In addition, these headers may never be removed.
----------------------------------------------------------------------------
A LOT of time has gone into this code by a LOT of people. Not just on
this individual code, but on all of the codebases this even takes a piece
of. I hope that you find this code in some way useful and you decide to
contribute a small bit to it. There's still a lot of work yet to do.
---------------------------------------------------------------------------*/
/*
	mud relationships.. Ahh, the only thing for heros to do is
	play house. Right?? Well, not necessarily so. They should 
	have more to do than this, but this is in for 'em now.
*/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
 
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>                                                         
#include <time.h>
 
#include "merc.h"
#include "interp.h"
#include "recycle.h"  
#include "tables.h"
#include "relationship.h"
void do_askout( CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MSL];
    CHAR_DATA *victim;
    argument = one_argument( argument, arg1 );


    if ( arg1[0] == '\0' )
    {
        send_to_char( "Who would you like to ask out on a date?\n\r",ch);
        return;
    }
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }
   if (IS_NPC(victim))
   {
 stc("Umm, Please, can we void mob dates! They get rather ugly when they die\n\r",ch);		
       return;
   }
   if (ch->pcdata->spouse !=NULL)
   {
   	sprintf(buf, "I don't think that %s would like that very much\n\r",ch->pcdata->spouse);
   stc(buf, ch);
   	return;
   }
else
{
act( "You ask $M out on a date.", ch, NULL,victim, TO_CHAR );
act( "$n asks you on a date..\n\rTo accept, type {rconsent{x $n, to decline, type {rnoconsent{x $n.", ch, NULL, victim, TO_VICT); 
act( "$n asks $N out on a date.", ch, NULL, victim, TO_NOTVICT );

ch->pcdata->tspouse = victim->name;
victim->pcdata->tspouse = ch->name;
ch->pcdata->predating = TRUE;
victim->pcdata->predating = TRUE;

return;
}
}
   
void do_propose( CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int dtime;
    dtime = ch->pcdata->rtime /7 ;
    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Who would you like to propose to?\n\r",ch);
        return;
    }
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
     {
        send_to_char( "That person isn't in the room.\n\r", ch );
        return;
    }
if (IS_NPC(victim))
    {
      stc("That mob doesn't love you enough.\n\r",ch);
      return;
     }

if (!ch->pcdata->dating)
	{
	stc("Try actually going out on a date with someone first dumbass\n\r",ch);
	return;
	}

if (!ISDATING(ch, victim))

	{
	
	stc("That's not your spouse.. OOOOH, wait till I tell on you!!\n\r",ch);
	return;
	}

if (!victim->pcdata->dating)
	{
	stc("Hmmm, They're not dating anyone!\n\r",ch);
	return;
	}
else
if (dtime < 20)
{
stc("Why not get to know the person a bit first\n\r",ch);
stc("You MUST be dating 20 weeks before you can propose\n\r",ch);
return;
}
else
    if ( victim->desc == NULL )
    {
        act("$N seems to have misplaced $S link...try again later.",
            ch,NULL,victim,TO_CHAR);
        return;
    }

else
{
ch->pcdata->tspouse = victim->name;
victim->pcdata->tspouse = ch->name;
do_save(ch, "");
do_save(victim, "");
ch->pcdata->preengaged = TRUE;
victim->pcdata->preengaged = TRUE;
act( "You ask $M to marry you", ch, NULL,victim, TO_CHAR );
act( "$n gets on $m knees before you, a ring in $m hand and asks for your hand in marriage .\n\rTo accept, type {rconsent{x $n, to decline, type {rnoconsent{x $n.", ch, NULL, victim, TO_VICT); 
act( "$n gets down one $m knees before $N and asks $M hand in marriage.", ch, NULL, victim, TO_NOTVICT );
}
}
void do_marry(CHAR_DATA *ch, char *argument)
{ 
	char arg1[MIL], arg2[MIL];
	char buf[MSL];
	CHAR_DATA *spouse1;
	CHAR_DATA *spouse2;
	int dtime=0;
	argument = one_argument (argument, arg1);
	argument = one_argument(argument, arg2);
/* 
 Before we can do the deed, there's a few recursive checks we gotta go
 through.. Okay, QUITE a few, but at least this way we KNOW that these 
 two actually want to get married.. Shouldn't be at this point without
 knowing that they want to get married, but, still.. This is the last 
 and final point for bailout.. So, give them the benefit of the doubt
 */
 
	if (!IS_IMMORTAL (ch))
	{
		stc("You've not got the marrying powers\n\r",ch);	
		return;
	}
	if (arg1[0]== '\0' || arg2[0] == '\0')
	{
		stc("Syntax: Marry <person1> <person2>\n\r",ch);
		return;
	}

    if ( ( spouse1 = get_char_room( ch, arg1 ) ) == NULL )
    {
sprintf(buf,"But %s isn't in the room!!\n\r", arg1);
stc(buf,ch);
        return;
    }	
        if ( ( spouse2 = get_char_room( ch, arg2 ) ) == NULL )
    {
sprintf(buf,"But %s isn't in the room!!\n\r", arg2);
stc(buf,ch);
        return;
    }
	
dtime = spouse1->pcdata->rtime/7;
if (dtime < 50)
		{
		stc ("They MUST be dating 50 weeks before they can get married\n\r",ch);
		return;
		}
dtime = spouse2->pcdata->rtime/7;
if (dtime < 50)
		{
		stc ("They MUST be dating 50 weeks before they can get married\n\r",ch);
		return;
		}	

if (!ISDATING(spouse1, spouse2))
{
	stc("They must actually be dating before they can be married\n\r",ch);
	return;
}
if (!ISDATING(spouse2, spouse1))
{
	stc("They must actually be dating before they can be married\n\r",ch);
	return;
}
if (!spouse1->pcdata->engaged)
{
	sprintf(buf, "%s isn't engaged, though", spouse1->name);
	stc(buf,ch);
	return;
}
if (!spouse2->pcdata->engaged)
{
	sprintf(buf, "%s isn't engaged, though", spouse2->name);
	stc(buf,ch);
	return;
}
/* 
Okay, we made it through all the nasties, now time to do the deed..
Why not make this a global announcement when it's done as well, ehh?
*/
else
{
	stc ("You pronounce them Man and Wife ??\n\r",ch);
	spouse1->pcdata->married = TRUE;
	spouse1->pcdata->engaged = FALSE;
	spouse2->pcdata->married = TRUE;
	spouse2->pcdata->engaged = FALSE;
	
	
	send_to_char( "You are now officially married. Congratulations\n\r", spouse1);
        send_to_char( "You are now officially married. Congratulations\n\r", spouse2);
	sprintf(buf, "{R%s{x and {R%s{x have finally been married. Everyone congratulate them.\n\r", spouse1->name, spouse2->name);
	do_echo(ch, buf);
       do_save(spouse1, "");
        do_save(spouse2,"");
	return;
	
}



}
void do_divorce(CHAR_DATA *ch, char *argument)
{ 
	char arg1[MIL], arg2[MIL];
	char buf[MSL];
	CHAR_DATA *spouse1;
	CHAR_DATA *spouse2;
	argument = one_argument (argument, arg1);
	argument = one_argument(argument, arg2);
/* 
 Before we can do the deed, there's a few recursive checks we gotta go
 through.. Okay, QUITE a few, but at least this way we KNOW that these 
 two actually want to get divorced.. Shouldn't be at this point without
 knowing that they want to get married, but, still.. This is the last 
 and final point for bailout.. So, give them the benefit of the doubt
 */
 
	if (!IS_IMMORTAL (ch))
	{
		stc("You've not got the divorcing powers\n\r",ch);	
		return;
	}
	if (arg1[0]== '\0' || arg2[0] == '\0')
	{
		stc("Syntax: Divorce <person1> <person2>\n\r",ch);
		return;
	}

    if ( ( spouse1 = get_char_room( ch, arg1 ) ) == NULL )
    {
sprintf(buf,"But %s isn't in the room!!\n\r", arg1);
stc(buf,ch);
        return;
    }	
        if ( ( spouse2 = get_char_room( ch, arg2 ) ) == NULL )
    {
sprintf(buf,"But %s isn't in the room!!\n\r", arg2);
stc(buf,ch);
        return;
    }

if (!ISDATING(spouse1, spouse2))
{
	stc("They must actually be married before they can be divorced dumbass.\n\r",ch);
	return;
}
if (!ISDATING(spouse2, spouse1))
{
	stc("They must actually be dating before they can be divorced dumbass\n\r",ch);
	return;
}
if (!spouse1->pcdata->married)
{
	sprintf(buf, "%s isn't married, though", spouse1->name);
	stc(buf,ch);
	return;
}
if (!spouse2->pcdata->married)
{
	sprintf(buf, "%s isn't married, though", spouse2->name);
	stc(buf,ch);
	return;
}
/* 
Okay, we made it through all the nasties, now time to do the deed..
Why not make this a global announcement when it's done as well, ehh?
*/
else
{
	stc ("You pronounce them divorced ??\n\r",ch);
	spouse1->pcdata->married = FALSE;
	spouse1->pcdata->engaged = FALSE;
	spouse2->pcdata->married = FALSE;
	spouse2->pcdata->engaged = FALSE;
	spouse2->pcdata->spouse	 = NULL;
	spouse1->pcdata->spouse  = NULL;
	spouse1->pcdata->rtime = 0;
	spouse2->pcdata->rtime = 0;
	send_to_char( "You are now officially divorced.\n\r", spouse1);
        send_to_char( "You are now officially divorced.\n\r", spouse2);
        do_save(spouse1, "");
        do_save(spouse2,"");
		return;
	
}



	}



void do_consent( CHAR_DATA *ch, char *argument )
{

char buf[MSL];
CHAR_DATA *victim;

   if (IS_NPC(ch))
    return;
 if(argument[0]=='\0')
  {
  send_to_char("Give consent to who??\n\r",ch);
  return;
  }
 if((victim = get_char_room(ch,argument)) == NULL)
  {
  send_to_char("But they aren't here!!\n\r",ch);
  return;
  }
   if (IS_NPC(victim))
    return;


 if (!ISTDATING(ch,victim))
    {
sprintf(buf,"%s hasn't asked for your consent to do anything\n\r",victim->name);
stc(buf,ch);
free_string(buf);
     return;
    }

    else
    {
        
if (ch->pcdata->predating)
{
sprintf (buf, "You accept %s's invitation\n\r", ch->pcdata->tspouse);
stc(buf,ch);
sprintf(buf, "%s has accepted your invitation\n\r",ch->name);
stc(buf,victim);
victim->pcdata->spouse = ch->name;
ch->pcdata->spouse =  victim->name;
victim->pcdata->tspouse = NULL;
ch->pcdata->tspouse =  NULL;
victim->pcdata->dating  = TRUE;
ch->pcdata->dating 	 = TRUE;
victim->pcdata->predating  = FALSE;
ch->pcdata->predating 	 = FALSE;
do_save(ch, "");
do_save(victim, "");
return;
}

    
if (ch->pcdata->preengaged)
{
sprintf (buf, "You accept %s's proposal\n\r", ch->pcdata->tspouse);
stc(buf,ch);
sprintf(buf, "%s has accepted your proposal\n\r",ch->name);
stc(buf,victim);
victim->pcdata->dating  = FALSE;
ch->pcdata->dating 	 = FALSE;
victim->pcdata->predating  = FALSE;
ch->pcdata->predating 	 = FALSE;
ch->pcdata->preengaged	 = FALSE;
victim->pcdata->preengaged = FALSE;
ch->pcdata->engaged = 	TRUE;
victim->pcdata->engaged = TRUE;
do_save(ch, "");
do_save(victim, "");
return;
}
}
}
void do_noconsent( CHAR_DATA *ch, char *argument )
{

CHAR_DATA *victim;
char buf[MSL];

   if (IS_NPC(ch))
    return;
 if(argument[0]=='\0')
  {
  send_to_char("Decline who??\n\r",ch);
  return;
  }
 if((victim = get_char_world(ch,argument)) == NULL)
  {
  send_to_char("They aren't logged in!\n\r",ch);
  return;
  }
 if (!ISTDATING(ch,victim))
    {
sprintf(buf,"%s hasn't asked for your consent to do anything\n\r",buf);
stc(buf,ch);
     return;
    }

    
if (ch->pcdata->predating)
{
sprintf (buf, "You decline %s's invitation\n\r", ch->pcdata->tspouse);
stc(buf,ch);
sprintf(buf, "%s has declined your invitation\n\r",ch->name);
stc(buf,victim);
victim->pcdata->tspouse = NULL;
ch->pcdata->tspouse =  NULL;
victim->pcdata->spouse = NULL;
ch->pcdata->spouse =  NULL;
victim->pcdata->dating  = FALSE;
ch->pcdata->dating 	 = FALSE;
victim->pcdata->predating  = FALSE;
ch->pcdata->predating 	 = FALSE;
do_save(ch, NULL);
do_save(victim, NULL);
return;
}
    
if (ch->pcdata->preengaged)
{
sprintf (buf, "You decline %s's proposal.\n\r", ch->pcdata->tspouse);
stc(buf,ch);
sprintf(buf, "%s has declined your proposal.\n\r",ch->name);
stc(buf,victim);

ch->pcdata->dating 	 = TRUE;
victim->pcdata->dating 	 = TRUE;
victim->pcdata->spouse = ch->name;
ch->pcdata->spouse =  victim->name;
victim->pcdata->tspouse = NULL;
ch->pcdata->tspouse =  NULL;
victim->pcdata->engaged  = FALSE;
ch->pcdata->engaged 	 = FALSE;
victim->pcdata->preengaged  = FALSE;
ch->pcdata->preengaged 	 = FALSE;
do_save(ch, NULL);
do_save(victim, NULL);
return;
}
}



void spouse_update( void )
{
    DESCRIPTOR_DATA *d;
    DESCRIPTOR_DATA *b;
    CHAR_DATA *ch;
    CHAR_DATA *victim;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->character != NULL && d->connected == CON_PLAYING)
        {
        ch = d->character; 
          for ( b = descriptor_list; b != NULL; b = b->next )
          {

        victim = b->original ? b->original : b->character; 

        if (b->connected == CON_PLAYING &&
            victim != ch &&
            victim->pcdata->spouse == ch->name)

        {
        ch->pcdata->rtime ++ ;
        victim->pcdata->rtime ++ ;
        }
          }
        }
    }
return;
}

void do_breakup (CHAR_DATA *ch, char *argument)
{
CHAR_DATA *victim;
char buf[MSL];

   if (IS_NPC(ch))
    return;
 if(argument[0]=='\0')
  {
  send_to_char("Breakup with who??\n\r",ch);
  return;
  }
 if((victim = get_char_world(ch,argument)) == NULL)
  {
  send_to_char("They aren't logged in!\n\r",ch);
  return;
  }
 if (!ISDATING(ch,victim))
	{
	stc("You're not in a relationship with them\n\r",ch);
	return;
	}

if (ch->pcdata->married)
{
	stc("Nope, gotta get an official divorce.. Sorry\n\r",ch);
	return;
}
    
if (ch->pcdata->dating)
{
sprintf (buf, "You break up with %s\n\r", ch->pcdata->spouse);
stc(buf,ch);
sprintf(buf, "%s has broken up with you\n\r",ch->name);
stc(buf,victim);
victim->pcdata->tspouse = NULL;
ch->pcdata->tspouse =  NULL;
victim->pcdata->spouse = NULL;
ch->pcdata->spouse =  NULL;
victim->pcdata->dating  = FALSE;
ch->pcdata->dating 	 = FALSE;
victim->pcdata->predating  = FALSE;
ch->pcdata->predating 	 = FALSE;
victim->pcdata->rtime  = 0;
ch->pcdata->rtime  = 0;
return;
}
    
if (ch->pcdata->engaged)
{
sprintf (buf, "You break up with %s\n\r", ch->pcdata->spouse);
stc(buf,ch);
sprintf(buf, "%s has broken up with you\n\r",ch->name);
stc(buf,victim);
victim->pcdata->tspouse = NULL;
ch->pcdata->tspouse =  NULL;
victim->pcdata->spouse = NULL;
ch->pcdata->spouse =  NULL;
victim->pcdata->engaged  = FALSE;
ch->pcdata->engaged 	 = FALSE;
victim->pcdata->preengaged  = FALSE;
ch->pcdata->preengaged 	 = FALSE;
victim->pcdata->rtime  = 0;
ch->pcdata->rtime  = 0;
return;
}

}



void do_clearwed( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
        
    one_argument( argument, arg );
              
    if ( arg[0] == '\0' )
    {
        send_to_char( "Who gets the lucky priviledge ", ch );
        return;
    }
        
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if ( get_trust( victim ) > get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }
victim->pcdata->spouse = NULL;
victim->pcdata->rtime = 0;
victim->pcdata->dating = FALSE;
victim->pcdata->engaged = FALSE;
victim->pcdata->married = FALSE;
victim->pcdata->predating = FALSE;
victim->pcdata->preengaged = FALSE;
victim->pcdata->premarried = FALSE;

stc("make it so\n\r",ch);
stc("You are no longer in a relationship with NULL-- Don't you feel so unloved??\n\r",victim);
    return;
}
