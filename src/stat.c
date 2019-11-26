/********************************************************************************
 * Stat List code copyright 1999-2001                                           *
 * Markanth : markanth@spaceservices.net                                        *
 * Devil's Lament : spaceservices.net port 3778                                 *
 * Web Page : http://spaceservices.net/~markanth/                               *
 *                                                                              *
 * All I ask in return is that you give me credit on your mud somewhere         *
 * or email me if you use it.                                                   *
 ********************************************************************************/


/**************************************************************************
*      Mindor 1.0 is copyright 2002-2019 by Shaun Mcbride                *
*      Portions of the code were inspired by other works                 *
*      found around the internet.                                        *
*                                                                        *
*      Please follow all previous copyrights and licenses.               *
**************************************************************************/

/* Again heavily modified to work with Mindor.         */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
STAT_DATA *stat_list;
long TopGameStat[MAX_GAMESTAT];

void show_game_stats(CHAR_DATA * ch, int type);

void save_statlist(void)
{
    STAT_DATA *pstat;
    FILE *fp;
    bool found = FALSE;
    int i;

    CLOSE_RESERVE();
    if ((fp = fopen(STAT_FILE, "w")) == NULL)
    {
	perror(STAT_FILE);
    }

    fprintf(fp, "TOPSTATS %d ", MAX_GAMESTAT);
    for (i = 0; i < MAX_GAMESTAT; i++)
	fprintf(fp, "%ld ", TopGameStat[i]);
    fprintf(fp, "\n");
    for (pstat = stat_list; pstat != NULL; pstat = pstat->next)
    {
	found = TRUE;
	fprintf(fp, "%s ", pstat->name);
	for (i = 0; i < MAX_GAMESTAT; i++)
	    fprintf(fp, "%ld ", pstat->gamestat[i]);
	fprintf(fp, "\n");
    }
    fclose(fp);
    OPEN_RESERVE();
    if (!found)
	unlink(STAT_FILE);
}

void load_statlist(void)
{
    FILE *fp;
    STAT_DATA *stat_last;
    int i,
     maxStat = 0;

    if ((fp = fopen(STAT_FILE, "r")) == NULL)
	return;

    if (!str_cmp(fread_word(fp), "TOPSTATS"));
    {
	maxStat = fread_number(fp);
	for (i = 0; i < maxStat; i++)
	    TopGameStat[i] = fread_number(fp);
    }

    stat_last = NULL;
    for (;;)
    {
	STAT_DATA *pstat;

	if (feof(fp))
	{
	    fclose(fp);
	    return;
	}

	pstat = new_stat_data();
	pstat->name = str_dup(fread_word(fp));
	for (i = 0; i < maxStat; i++)
	    pstat->gamestat[i] = fread_number(fp);
	fread_to_eol(fp);

	if (stat_list == NULL)
	    stat_list = pstat;
	else
	    stat_last->next = pstat;
	stat_last = pstat;
    }
}

void update_statlist(CHAR_DATA * ch, bool delete)
{
    STAT_DATA *prev;
    STAT_DATA *curr;
    int i;

    if (IS_NPC(ch) || IS_IMMORTAL(ch))
	return;

    prev = NULL;

    for (curr = stat_list; curr != NULL; prev = curr, curr = curr->next)
    {
	if (!str_cmp(ch->name, curr->name))
	{
	    if (prev == NULL)
		stat_list = stat_list->next;
	    else
		prev->next = curr->next;

	    free_stat_data(curr);
	    save_statlist();
	}
    }
    if (delete)
    {
	return;
    }

    curr = new_stat_data();
    curr->name = str_dup(ch->name);
    for (i = 0; i < MAX_GAMESTAT; i++)
    {
	curr->gamestat[i] = ch->pcdata->gamestat[i];

	if (curr->gamestat[i] > TopGameStat[i])
	    TopGameStat[i] = curr->gamestat[i];
    }

    curr->next = stat_list;
    stat_list = curr;
    save_statlist();
    return;
}

void do_showstats(CHAR_DATA * ch, char *argument)
{
    int option;
    char buf[MSL],
     arg[MIL];

    argument = one_argument(argument, arg);

    if (IS_NULLSTR(arg))
    {
	send_to_char("      {ROPTIONS AVAILABLE:{x\n\r", ch);
	/* belar 
	send_to_char("      {G0{x - Game Stats on Devils Lament (gstats)\n\r", ch);
	*/
	send_to_char("      {G1{x - Ranking of Player Killers (pkills)\n\r", ch);
	send_to_char("      {G2{x - Ranking of Player Deaths (pdeaths)\n\r", ch);
	send_to_char("      {G3{x - Ranking of Mob Kills (mkills)\n\r", ch);
	send_to_char("      {G4{x - Ranking of Mob Deaths (mdeaths)\n\r", ch);
	send_to_char("      {G5{x - Ranking of Quests Completed (qcomplete)\n\r", ch);
	send_to_char("      {G6{x - Ranking of Quests Failed (qfail)\n\r", ch);
	send_to_char("      {G7{x - Ranking of Quest Points Earned (qpoints)\n\r", ch);
        if(IS_IMMORTAL(ch))
	send_to_char("      {Gdelete <name>{x - deletes from statlist\n\r", ch);
	return;
    }
    option = atoi(arg);

    if (!str_cmp(arg, "delete") && IS_IMMORTAL(ch))
    {
	STAT_DATA *prev = NULL;
	STAT_DATA *curr = NULL;
	bool found = FALSE;

	for (curr = stat_list; curr != NULL; prev = curr, curr = curr->next)
	{
	    if (!str_cmp(argument, curr->name))
	    {
		if (prev == NULL)
		    stat_list = stat_list->next;
		else
		    prev->next = curr->next;

		free_stat_data(curr);
		save_statlist();
		found = TRUE;
	    }
	}
	if (!found)
	{
	    sprintf( buf, "Error deleting %s.\n\r", argument);
	    send_to_char(buf, ch );
	}
    } else if (option == 1 || !str_prefix(arg, "pkills"))
	show_game_stats(ch, PK_KILLS);
    else if (option == 3 || !str_prefix(arg, "mkills"))
	show_game_stats(ch, MOB_KILLS);
    else if (option == 2 || !str_prefix(arg, "pdeaths"))
	show_game_stats(ch, PK_DEATHS);
    else if (option == 4 || !str_prefix(arg, "mdeaths"))
	show_game_stats(ch, MOB_DEATHS);
    else if (option == 5 || !str_prefix(arg, "qcomplete"))
	show_game_stats(ch, QUESTS_COMPLETE);
    else if (option == 6 || !str_prefix(arg, "qfail"))
	show_game_stats(ch, QUESTS_FAIL);
    else if (option == 7 || !str_prefix(arg, "qpoints"))
	show_game_stats(ch, QUESTS_POINTS);
    /* belar
    else
	do_statlist(ch, "");
    */
    return;
}

void show_game_stats(CHAR_DATA * ch, int type)
{
    STAT_DATA *curr;
    BUFFER *output;
    char buf[MSL];
    long top;
    int count,
     pos;
    bool found = FALSE;
    const char *stat_name[MAX_GAMESTAT] = {
	"PLAYER KILLERS",
	"MOB KILLERS",
	"PK DEATHS",
	"MOB DEATHS",
	"QUESTS COMPLETED",
	"QUESTS FAILED",
	"QUEST POINTS"
    };

    output = new_buf();
    count = 0;
    pos = 0;
    sprintf(buf, "{CRANKING OF %s{x", stat_name[type]);
    add_buf(output, buf);
    add_buf(output, "\n\r");
    for (top = TopGameStat[type]; top > 0; top--)
    {
	for (curr = stat_list; curr != NULL; curr = curr->next)
	{

	    if (curr->gamestat[type] == top)
	    {
		if (count++ > 49)
		    break;
		sprintf(buf, "{G%2d{w){W %-20s {w[{R%8ld{W]{x    ", count, curr->name, curr->gamestat[type]);
                add_buf(output, buf);
                if (++pos % 2 == 0)
		{
		    add_buf(output, "\n\r");
		    pos = 0;
		}
		found = TRUE;
	    }
	}
    }

    if (!found)
	add_buf(output, "\n\rNo one found yet.\n\r");
    else if (pos % 2 != 0)
	add_buf(output, "\n\r");
    page_to_char(buf_string(output), ch);
    free_buf(output);
    return;
}
