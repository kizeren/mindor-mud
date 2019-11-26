/****************************************************************************

this is just a test

*  Automated Quest code written by Vassago of MOONGATE, moongate.ams.com    *
*  4000. Copyright (c) 1996 Ryan Addams, All Rights Reserved. Use of this   *
*  code is allowed provided you add a credit line to the effect of:         *
*  "Quest Code (c) 1996 Ryan Addams" to your logon screen with the rest     *
*  of the standard diku/rom credits. If you use this or a modified version  *
*  of this code, let me know via email: moongate@moongate.ams.com. Further  *
*  updates will be posted to the rom mailing list. If you'd like to get     *
*  the latest version of quest.c, please send a request to the above add-   *
*  ress. Quest Code v2.03. Please do not remove this notice from this file. *
****************************************************************************/
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
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <sys/signal.h>
#include <sys/resource.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h> /* OLC -- for close read write etc */
#include <stdarg.h> /* printf_to_char */
#include <signal.h>
#include "merc.h"
#define OD OBJ_DATA
OD *    get_obj_keeper  args( (CHAR_DATA *ch,CHAR_DATA *keeper,char *argument));
#undef OD
DECLARE_DO_FUN( do_say );
void	quest_buy	args( (CHAR_DATA *ch, CHAR_DATA *qm, char *argument) );
void	quest_list	args( (CHAR_DATA *ch, CHAR_DATA *qm, char *argument) );
void	quest_sell	args( (CHAR_DATA *ch, CHAR_DATA *qm, char *argument) );
int get_cost    args((CHAR_DATA *qm, CHAR_DATA *obj, bool fReplace ));
/* Object vnums for object quest 'tokens'. In Moongate, the tokens are
   things like 'the Shield of Moongate', 'the Sceptre of Moongate'. These
   items are worthless and have the rot-death flag, as they are placed
   into the world when a player receives an object quest. */

#define QUEST_OBJQUEST1 3201
#define QUEST_OBJQUEST2 3202
#define QUEST_OBJQUEST3 3203
#define QUEST_OBJQUEST4 3204
#define QUEST_OBJQUEST5 3206
#define QUEST_OBJQUEST6 3207
#define QUEST_OBJQUEST7 3208
#define QUEST_OBJQUEST8 3209
#define QUEST_OBJQUEST9 3210
#define QUEST_OBJQUEST10 3211

/* Local functions */

void generate_quest     args(( CHAR_DATA *ch, CHAR_DATA *questman ));
void quest_update       args(( void ));
bool quest_level_diff   args(( int clevel, int mlevel));
bool chance             args(( int num ));
ROOM_INDEX_DATA         *find_location( CHAR_DATA *ch, char *arg );

/* CHANCE function. I use this everywhere in my code, very handy :> */

bool chance(int num)
{
    if (number_range(1,100) <= num) return TRUE;
    else return FALSE;
}

/* The main quest function */

void do_quest(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *questman;
    OBJ_DATA *obj=NULL, *obj_next;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0')
    {
        send_to_char("QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY.\n\r",ch);
        send_to_char("For more information, type 'HELP QUEST'.\n\r",ch);
        return;
    }
    if (!strcmp(arg1, "info"))
    {
        if (IS_SET(ch->act2, PLR2_QUESTOR))
        {
            if (ch->questmob == -1 && ch->questgiver->short_descr != NULL)
            {
                sprintf(buf, "You're not quite done yet!\n\rGet back to %s so he can give you some points for your efforts!\n\r",ch->questgiver->short_descr);
                send_to_char(buf, ch);
            }
            else if (ch->questobj > 0)
            {
                questinfoobj = get_obj_index(ch->questobj);
                if (questinfoobj != NULL)
                {
                    sprintf(buf, "You must find %s to finish your quest!\n\r",questinfoobj->name);
                    send_to_char(buf, ch);
                }
                else send_to_char("You aren't currently on a quest.\n\r",ch);
                return;
            }
            else if (ch->questmob > 0)
            {
                questinfo = get_mob_index(ch->questmob);
                if (questinfo != NULL)
                {
                    sprintf(buf, "Find the %s and bring back their head to finish your quest!\n\r",questinfo->short_descr);
                    send_to_char(buf, ch);
                }
                else send_to_char("You aren't currently on a quest.\n\r",ch);
                return;
            }
        }
        else
            send_to_char("You aren't currently on a quest.\n\r",ch);
        return;
    }
    if (!strcmp(arg1, "points"))
    {
        sprintf(buf, "You have have been favored with  %d quest points.\n\r",ch->qps);
        send_to_char(buf, ch);
        return;
    }
    else if (!strcmp(arg1, "time"))
    {
        if (!IS_SET(ch->act2, PLR2_QUESTOR))
        {
            send_to_char("You aren't currently on a quest.\n\r",ch);
            if (ch->nextquest > 1)
            {
                sprintf(buf, "There are still %d minutes remaining until you can go on another quest.\n\r",ch->nextquest);
                send_to_char(buf, ch);
            }
            else if (ch->nextquest == 1)
            {
                sprintf(buf, "There is less than a minute remaining until you can go on another quest. Be patient!\n\r");
                send_to_char(buf, ch);
            }
        }
        else if (ch->countdown > 0)
        {
            sprintf(buf, "Time left for current quest: %d\n\r",ch->countdown);
            send_to_char(buf, ch);
        }
        return;
    }

/* Checks for a character in the room with spec_questmaster set. This special
   procedure must be defined in special.c. You could instead use an
   ACT_QUESTMASTER flag instead of a special procedure. */

    for ( questman = ch->in_room->people; questman != NULL; 
questman = questman->next_in_room )
    {
        if (!IS_NPC(questman)) continue;
        if (questman->spec_fun == spec_lookup( "spec_questmaster" )) break;
    }

    if (questman == NULL || questman->spec_fun != spec_lookup( "spec_questmaster" ))
    {
        send_to_char("You can't do that here.\n\r",ch);
        return;
    }

    if ( questman->fighting != NULL)
    {
        send_to_char("Wait until the fighting stops.\n\r",ch);
        return;
    }

    ch->questgiver = questman;


/* add this to do_quest, after the questmaster is found.
 * qm = the questmaster mob.  If there is already a list
 * section in the quest code, remove it and replace it with this.
 */

    if( !str_prefix( arg1, "list" ) )
    {
	quest_list(ch,questman,argument);
	return;
    }

    if( !str_prefix( arg1, "buy" ) )
    {
	quest_buy(ch,questman,arg2);
	return;
    }
    if( !str_prefix( arg1, "sell" ) )
    {
	quest_sell(ch,questman,arg2);
	return;
    }



else if (!strcmp(arg1, "quit")) /* Quit code by Ubermon 11/20/98 */
    {
		if (!IS_SET(ch->act2,PLR2_QUESTOR))
		{
	    send_to_char("You aren't currently on a quest.\n\r",ch);
		return;
		}
		sprintf(buf,"You admit defeat and give up on your quest.  You may quest again in 1 minutes.\n\r");
		send_to_char(buf, ch);
	    REMOVE_BIT(ch->act2,PLR2_QUESTOR);
	    ch->questgiver = NULL;
	    ch->countdown = 0;
	    ch->questmob = 0;
		ch->questobj = 0;
	    ch->nextquest = 1;
		return;
	}

    else if (!strcmp(arg1, "request"))
    {
        act( "$n asks $N for a quest.", ch, NULL, questman, TO_ROOM);
        act ("You ask $N for a quest.",ch, NULL, questman, TO_CHAR);
        if (IS_SET(ch->act2, PLR2_QUESTOR))
        {
            sprintf(buf, "But you're already on a quest!");
            do_say(questman, buf);
            return;
        }
        if (ch->nextquest > 0)
        {
            sprintf(buf, "Ok you have proven your braveness now you're just being a hog  %s, let someone else have a chance.",ch->name);
            do_say(questman, buf);
            sprintf(buf, "Come back later.");
            do_say(questman, buf);
            return;
        }

        sprintf(buf, "Thank you, brave %s!",ch->name);
        do_say(questman, buf);
        ch->questmob = 0;
        ch->questobj = 0;

        generate_quest(ch, questman);

        if (ch->questmob > 0 || ch->questobj > 0)
        {
            ch->countdown = number_range(10,30);
            ch->pcdata->qtime = ch->countdown;
            SET_BIT(ch->act2, PLR2_QUESTOR);
            sprintf(buf, "You have %d minutes to complete this quest.",ch->countdown);
            do_say(questman, buf);
            sprintf(buf, "May you go with the power of the gods!");
            do_say(questman, buf);
        }
        return;
    }
    else if (!strcmp(arg1, "complete"))
    {
        act( "$n informs $N $e has completed $s quest.", ch, NULL, questman, TO_ROOM);
        act ("You inform $N you have completed $s quest.",ch, NULL, questman, TO_CHAR);
        if (ch->questgiver != questman)
        {
            sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
            do_say(questman,buf);
            return;
        }

        if (IS_SET(ch->act2, PLR2_QUESTOR))
        {
            if (ch->questmob == -1 && ch->countdown > 0)
            {
                int reward, pointreward, pracreward, expreward;
                reward = number_range(10,100);
                expreward = number_range(250,1000);
                pointreward = number_range(50,250);
                sprintf(buf, "Congratulations on completing your quest!");
                do_say(questman,buf);
                sprintf(buf,"As a reward, I am giving you %d quest points, and %d platinum.",pointreward,reward);
                do_say(questman,buf);
if  (!IS_HERO(ch))
{
sprintf(buf,"I shall also reward  you with %d experience.",expreward);
                do_say(questman,buf);
}
                if (chance(10))
                {
                    pracreward = number_range(5,25);
                    sprintf(buf, "You gain %d practices!\n\r",pracreward);
                    send_to_char(buf, ch);
                    ch->practice += pracreward;
                }

                REMOVE_BIT(ch->act2, PLR2_QUESTOR);
                ch->questgiver = NULL;
                ch->countdown = 0;
                ch->questmob = 0;
                ch->questobj = 0;
                ch->nextquest = 10;
                ch->pcdata->gamestat[QUESTS_COMPLETE]++;

                ch->platinum += reward;
                ch->qps += pointreward;
                ch->pcdata->gamestat[QUESTS_POINTS] += pointreward;
		if(!IS_HERO(ch))
	{
                gain_exp( ch, expreward );

	}

                return;
            }
            else if (ch->questobj > 0 && ch->countdown > 0)
            {
                bool obj_found = FALSE;

                for (obj = ch->carrying; obj != NULL; obj= obj_next)
                {
                    obj_next = obj->next_content;

                    if (obj != NULL && obj->pIndexData->vnum == ch->questobj)
                    {
                        obj_found = TRUE;
                        break;
                    }
                }
                if (obj_found == TRUE)
                {
                    int reward, pointreward, pracreward, expreward;

                    reward = number_range(50,2000);
	           expreward = number_range(100,500);
	            pointreward = number_range(50,175);

                    act("You hand $p to $N.",ch, obj, questman, TO_CHAR);
                    act("$n hands $p to $N.",ch, obj, questman, TO_ROOM);

                    sprintf(buf, "Congratulations on completing your quest!");
                    do_say(questman,buf);
                    sprintf(buf,"As a reward, I am giving you %d quest points, and %d gold.",pointreward,reward);
                    do_say(questman,buf);
if  (!IS_HERO(ch))
{
sprintf(buf,"I shall also reward  you with %d experience.",expreward);
                do_say(questman,buf);
}
                    if (chance(10))
                    {
                        pracreward = number_range(1,6);
                        sprintf(buf, "You gain %d practices!\n\r",pracreward);
                        send_to_char(buf, ch);
                        ch->practice += pracreward;
                    }

                    REMOVE_BIT(ch->act2, PLR2_QUESTOR);
                    ch->questgiver = NULL;
                    ch->countdown = 0;
                    ch->questmob = 0;
                    ch->questobj = 0;
                ch->nextquest = 10;

                    ch->gold += reward;
                    ch->qps += pointreward;
                    extract_obj(obj);
		if(!IS_HERO(ch))
	{
                gain_exp( ch, expreward );
	}

                    return;
                }
                else
                {
                    sprintf(buf, "You haven't completed the quest yet, but there is still time!");
                    do_say(questman, buf);
                    return;
                }
                return;
            }
            else if ((ch->questmob > 0 || ch->questobj > 0) && ch->countdown > 0)
            {
                sprintf(buf, "You haven't completed the quest yet, but there is still time!");
                do_say(questman, buf);
                return;
            }
        }
        if (ch->nextquest > 0)
            sprintf(buf,"But you didn't complete your quest in time!");
        else sprintf(buf, "You have to REQUEST a quest first, %s.",ch->name);
        do_say(questman, buf);
        return;
    }

    send_to_char("QUEST commands: QUIT POINTS INFO TIME REQUEST COMPLETE  LIST BUY SELL.\n\r",ch);
    send_to_char("For more information, type 'HELP QUEST'.\n\r",ch);
    return;
}

void generate_quest(CHAR_DATA *ch, CHAR_DATA *questman)
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *questitem;
    char buf [MAX_STRING_LENGTH];
   
    /*  Randomly selects a mob from the world mob list. If you don't
        want a mob to be selected, make sure it is immune to summon.
        Or, you could add a new mob flag called ACT_NOQUEST. The mob
        is selected for both mob and obj quests, even tho in the obj
        quest the mob is not used. This is done to assure the level
        of difficulty for the area isn't too great for the player. */
    for (victim = char_list; victim != NULL; victim = victim->next)
    {
        if (!IS_NPC(victim)) continue;

            if(!IS_SET(victim->imm_flags, IMM_SUMMON)
            && victim->pIndexData != NULL
            && victim->pIndexData->pShop == NULL
            && !IS_SET(victim->act, ACT_PET)
            && !IS_SET(victim->act, ACT_GAIN)
            && !IS_SET(victim->act, ACT_TRAIN)
            && !IS_SET(victim->act, ACT_PRACTICE)
            && !IS_SET(victim->affected_by, AFF_CHARM)
            && chance(1)
            && !strstr(victim->in_room->area->builders,"Unlinked")) break;

    }
    if ( victim == NULL  )
    {
        do_say(questman, "I'm sorry, but I don't have any quests for you at this time.");
        do_say(questman, "Try again later.");
        ch->nextquest = 2;
        return;
    }

    if ( ( room = find_location( ch, victim->name ) ) == NULL )
    {
        sprintf(buf, "I'm sorry, but I don't have any quests for you at this time.");
        do_say(questman, buf);
        sprintf(buf, "Try again later.");
        do_say(questman, buf);
        ch->nextquest = 2;
        return;
    }

    /*  40% chance it will send the player on a 'recover item' quest. */

    if (chance(40))
    {
        int objvnum = 0;

        switch(number_range(0,4))
        {
            case 0:
            objvnum = QUEST_OBJQUEST1;
            break;

            case 1:
            objvnum = QUEST_OBJQUEST2;
            break;

            case 2:
            objvnum = QUEST_OBJQUEST3;
            break;

            case 3:
            objvnum = QUEST_OBJQUEST4;
            break;

            case 4:
            objvnum = QUEST_OBJQUEST5;
            break;

            case 5:
            objvnum = QUEST_OBJQUEST6;
            break;

            case 6:
            objvnum = QUEST_OBJQUEST7;
            break;

            case 7:
            objvnum = QUEST_OBJQUEST8;
            break;

            case 8:
            objvnum = QUEST_OBJQUEST9;
            break;

            case 9:
            objvnum = QUEST_OBJQUEST10;
            break;

        }

        questitem = create_object( get_obj_index(objvnum), ch->level );
        obj_to_room(questitem, room);
        ch->questobj = questitem->pIndexData->vnum;

        sprintf(buf, "Vile pilferers have stolen %s from the royal kitchen!",questitem->short_descr);
        do_say(questman, buf);
        do_say(questman, "My court Chef, with her magic mirror, has pinpointed its location.");

        /* I changed my area names so that they have just the name of the area
           and none of the level stuff. You may want to comment these next two
           lines. - Vassago */

        sprintf(buf, "Look in the general area of %s for %s!",room->area->name, room->name);
        do_say(questman, buf);
        return;
    }

    /* Quest to kill a mob */

    else
    {
    switch(number_range(0,1))
    {
        case 0:
        sprintf(buf, "An enemy of mine, %s, is making vile threats against the crown.",victim->short_descr);
        do_say(questman, buf);
        sprintf(buf, "This threat must be eliminated!");
        do_say(questman, buf);
        break;

        case 1:
        sprintf(buf, "%s has escaped from the dungeons!",victim->short_descr);
        do_say(questman, buf);
        sprintf(buf, "Since the escape, %s has murdered %d civillians!",victim->short_descr, number_range(2,20));
        do_say(questman, buf);
        do_say(questman,"The penalty for this crime is death, and you are to deliver the sentence!");
        break;
    }

    if (room->name != NULL)
    {
        sprintf(buf, "%s can be found in the general area of %s.", victim->short_descr,room->area->name);
        do_say(questman, buf);
    }
    ch->questmob = victim->pIndexData->vnum;
	SET_BIT(victim->act2, ACT2_QUESTMOB);
	victim->tmpquest = 6000;
		
    }
    return;
}

bool quest_level_diff(int clevel, int mlevel)
{
if (( mlevel - clevel <= 10) && (mlevel - clevel > 10))  
return TRUE;
    else return FALSE;
}
                
                
/* Called from update_handler() by pulse_area */

void quest_update(void)
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->character != NULL && d->connected == CON_PLAYING)
        {

        ch = d->character;

        if ((ch->nextquest > 0) && (ch->timer < 2))
        {
            ch->nextquest--;
            if (ch->nextquest == 0)
            {
                send_to_char("You may now quest again.\n\r",ch);
                return;
            }
        }
        else if (IS_SET(ch->act2,PLR2_QUESTOR))
        {
            if (--ch->countdown <= 0)
            {
                char buf [MAX_STRING_LENGTH];

                ch->nextquest = 5;
                sprintf(buf, "You have run out of time for your quest!\n\rYou may quest again in %d minutes.\n\r",ch->nextquest);
                send_to_char(buf, ch);
                REMOVE_BIT(ch->act2, PLR2_QUESTOR);
                ch->questgiver = NULL;
                ch->countdown = 0;
                ch->questmob = 0;
                ch->pcdata->gamestat[QUESTS_FAIL]++;
            }
            if (ch->countdown > 0 && ch->countdown < 5)
            {
                send_to_char("Better hurry, you're almost out of time for your quest!\n\r",ch);
                return;
            }
        }
        }
    }
    return;
}

void quest_buy( CHAR_DATA *ch,CHAR_DATA *qm, char *argument )
{
    char buf[MAX_STRING_LENGTH];
/*    int cost; */
    OBJ_DATA *obj, *t_obj;
    char arg[MAX_INPUT_LENGTH];
    int number, count = 1;

    /* get rid of buy in argument */

    number = mult_argument(argument,arg);
    obj  = get_obj_keeper( ch,qm, arg );


    if (obj == NULL)
    {
	send_to_char( "Buy what?\n\r", ch );
	return;
    }

    int cost = get_cost(obj, qm, TRUE);  
    if (number < 1)
    {
        act("$n tells you 'Get real!",qm,NULL,ch,TO_VICT);
        return;
    }

    if ( cost < 0 || !can_see_obj( ch, obj ) )
    {
        act( "$n tells you 'I don't sell that -- try 'list''.",
           qm, NULL, ch, TO_VICT );
        ch->reply = qm;
        return;
    }

    if ( (ch->qps) < cost * number )
    {
        if (number > 1)
            act("$n tells you 'You can't afford to buy that many.",
              qm,obj,ch,TO_VICT);
        else
            act( "$n tells you 'You can't afford to buy $p'.",
              qm, obj, ch, TO_VICT );
        ch->reply = qm;
        return;
    }

    if ( ch->level < obj->level )
    {
        act( "$n tells you 'You can't use $p yet'.",
          qm, obj, ch, TO_VICT );
        ch->reply = qm;
        return;
    }

    if (ch->carry_number +  number * get_obj_number(obj) > can_carry_n(ch))
    {
        send_to_char( "You can't carry that many items.\n\r", ch );
        return;
    }

    if ( ch->carry_weight + number * get_obj_weight(obj) > can_carry_w(ch))
    {
        send_to_char( "You can't carry that much weight.\n\r", ch );
        return;
    }

    if (number > 1)
    {
        sprintf(buf,"$n buys $p[%d].",number);
        act(buf,ch,obj,NULL,TO_ROOM);
        sprintf(buf,"You buy $p[%d] for %d questpoints.",number,cost * number);
        act(buf,ch,obj,NULL,TO_CHAR);
    }
    else
    {
        act( "$n buys $p.", ch, obj, NULL, TO_ROOM );
        sprintf(buf,"You buy $p for %d questpoints.",cost);
        act( buf, ch, obj, NULL, TO_CHAR );
    }

    ch->qps -= (cost * number);

    for (count = 0; count < number; count++)
    {
	t_obj = create_object( obj->pIndexData, obj->level );

        if (t_obj->timer > 0 && !IS_OBJ_STAT(t_obj,ITEM_HAD_TIMER))
            t_obj->timer = 0;
        REMOVE_BIT(t_obj->extra_flags,ITEM_HAD_TIMER);
        obj_to_char( t_obj, ch );
    }
}

void quest_sell( CHAR_DATA *ch, CHAR_DATA *qm, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cost;

    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    int number;

    number = mult_argument(argument,arg);
    obj  = get_obj_carry( ch,arg );

    if (obj == NULL)
    {
	send_to_char( "Sell what?\n\r", ch );
	return;
    }

    cost = obj->cost/2;
/* to prevent problems*/
    if (number < 1)
    {
        act("$n tells you 'Get real!",qm,NULL,ch,TO_VICT);
        return;
    }
/* to prevent selling more than one at a time*/
    if (number > 1)
    {
        act("$n tells you 'One object at a time please",qm,NULL,ch,TO_VICT);
        return;
    }
/* does it have a cost, or can the qm see it?*/
    if ( cost < 0 || !can_see_obj( qm, obj ) )
    {
        act( "$n tells you 'I can't see that that object'.",
           qm, NULL, ch, TO_VICT );
        ch->reply = qm;
        return;
    }
/*is it a quest object??*/
    if (!IS_SET(obj->extra_flags, ITEM_QUESTOBJ))
    {
      sprintf(buf, "But %s isn't a quest object!!\n\r", obj->short_descr);
      send_to_char(buf,ch);
      return;
     }
 sprintf (buf, "Since this is a used %s, I shall only give you half of the purchase price for it.\n\rYou  shall receive %d questpoints.\n\r",obj->short_descr, cost);
            do_say(qm, buf);
        sprintf(buf,"$n sells $p.");
        act(buf,ch,obj,NULL,TO_ROOM);
        sprintf(buf,"You sell $p for %d questpoints.",cost);
        act(buf,ch,obj,NULL,TO_CHAR);

ch->qps += cost;
obj_from_char(obj);
}

void quest_list( CHAR_DATA *ch, CHAR_DATA *qm, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int cost;
    bool found;

    found = FALSE;

    for ( obj = qm->carrying; obj; obj = obj->next_content )
    {
	if ( obj->wear_loc == WEAR_NONE
	 &&  can_see_obj( ch, obj )
	 && (cost = obj->cost ) > 0 )
	{
	    if ( !found )
	    {
		found = TRUE;
		send_to_char( "{c[{WLvl {RToken{c]{x Item{x\n\r", ch );
	    }

	    if (IS_OBJ_STAT(obj,ITEM_INVENTORY))
	    {
		sprintf(buf,"{c[{W%3d {R%5d{c] {x%s\n\r",obj->level,cost,obj->short_descr);
		send_to_char( buf, ch );
	    }
	}

    }
    if ( !found )
	send_to_char( "No items available at this time.\n\r", ch );
    else
    {
	sprintf(buf, "\n\r{wYou have {B%d {wquestpoints to spend.{x", ch->qps);
	act(buf,qm,NULL,ch,TO_VICT);
    }
    return;
}

