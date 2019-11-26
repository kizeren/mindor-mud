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
		#include <sys/timeb.h>
	#else
		#include <sys/time.h>
	#endif
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

/* globals from db.c for load_notes */
#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif
extern FILE *                  fpArea;
extern char                    strArea[MAX_INPUT_LENGTH];
extern char		       boot_buf[MAX_STRING_LENGTH];

/* local procedures */
void load_thread(char *name, NOTE_DATA **list, int type, time_t free_time);
void parse_note(CHAR_DATA *ch, char *argument, int type);
bool hide_note(CHAR_DATA *ch, NOTE_DATA *pnote);

NOTE_DATA *note_list;
NOTE_DATA *idea_list;
NOTE_DATA *penalty_list;
NOTE_DATA *news_list;
NOTE_DATA *changes_list;
NOTE_DATA *weddings_list;
NOTE_DATA *city_list;
NOTE_DATA *family_list;
NOTE_DATA *personal_list;

int count_spool(CHAR_DATA *ch, NOTE_DATA *spool)
{
    int count = 0;
    NOTE_DATA *pnote;

    for (pnote = spool; pnote != NULL; pnote = pnote->next)
	if (!hide_note(ch,pnote))
	    count++;

    return count;
}

void do_unread(CHAR_DATA *ch)
{
    char buf[MAX_STRING_LENGTH];
    int count;
    bool found = FALSE;

    if (IS_NPC(ch))
	return; 

send_to_char("\n\r",ch);
send_to_char("{1------------------------------------------{x\n\r",ch);
    if ((count = count_spool(ch,weddings_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"{3There %s {#%d{3 new wedding {#announcement%s waiting.{x\n\r",
	    count > 1 ? "are" : "is",count, count > 1 ? "{#s" : "");
	send_to_char(buf,ch); 
    }
    else
        send_to_char("{3You have {#no{3 unread {#wedding announcements{3.{x\n\r",ch);
    if ((count = count_spool(ch,news_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"{3There %s {#%d{3 new {#news article%s{3 waiting.{x\n\r",
	    count > 1 ? "are" : "is",count, count > 1 ? "{#s" : "");
	send_to_char(buf,ch);
    }
        else
        send_to_char("{3You have {#no{3 unread {#news articles{3.{x\n\r",ch);
	if ((count = count_spool(ch,changes_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"{3There %s {#%d{3 {#change%s{3 waiting to be read.{x\n\r",
	    count > 1 ? "are" : "is", count, count > 1 ? "{#s" : "");
        send_to_char(buf,ch);
    }
       else
        send_to_char("{3You have {#no{3 unread {#changes{3.{x\n\r",ch);    
if ((count = count_spool(ch,note_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"{3You have {#%d{3 new {#note%s{3 waiting.{x\n\r",
	    count, count > 1 ? "{#s" : "");
	send_to_char(buf,ch);
    }
       else
        send_to_char("{3You have {#no{3 unread {#notes{3.{x\n\r",ch); 
   if ((count = count_spool(ch,idea_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"{3You have {#%d{3 unread {#idea%s{3 to peruse.{x\n\r",
	    count, count > 1 ? "{#s" : "");
	send_to_char(buf,ch);
    }
        else
        send_to_char("{3You have {#no{3 unread {#ideas{3.{x\n\r",ch);
   if ((count = count_spool(ch,city_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"{3You have {#%d{3 unread {#city notes%s{3 to peruse.{x\n\r",
	    count, count > 1 ? "{#s" : "");
	send_to_char(buf,ch);
    }
        else
        send_to_char("{3You have {#no{3 unread {#city notes{3.{x\n\r",ch);
   if ((count = count_spool(ch,personal_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"{3You have {#%d{3 unread {#personal notes%s{3 to peruse.{x\n\r",
	    count, count > 1 ? "{#s" : "");
	send_to_char(buf,ch);
    }
        else
        send_to_char("{3You have {#no{3 unread {#personal notes{3.{x\n\r",ch);
   if ((count = count_spool(ch,family_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"{3You have {#%d{3 unread {#family notes%s{3 to peruse.{x\n\r",
	    count, count > 1 ? "{#s" : "");
	send_to_char(buf,ch);
    }
        else
        send_to_char("{3You have {#no{3 unread {#family notes{3.{x\n\r",ch);




    if (IS_TRUSTED(ch,SQUIRE) && (count = count_spool(ch,penalty_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"{#%d %s {3been added.{x\n\r",
	    count, count > 1 ? "penalties have" : "penalty has");
	send_to_char(buf,ch);
    }
send_to_char("{1------------------------------------------{x\n\r",ch);
  }
void do_city(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_CITY);
}
void do_personal(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_PERSONAL);
}
void do_family(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_FAMILY);
}
void do_note(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_NOTE);
}

void do_idea(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_IDEA);
}

void do_penalty(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_PENALTY);
}

void do_news(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_NEWS);
}

void do_weddings(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_WEDDINGS);
}

void do_changes(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_CHANGES);
}

void save_notes(int type)
{
    FILE *fp;
    char *name;
    NOTE_DATA *pnote;

    switch (type)
    {
	default:
	    return;
	case NOTE_NOTE:
	    name = NOTE_FILE;
	    pnote = note_list;
	    break;
	case NOTE_IDEA:
	    name = IDEA_FILE;
	    pnote = idea_list;
	    break;
	case NOTE_PENALTY:
	    name = PENALTY_FILE;
	    pnote = penalty_list;
	    break;
	case NOTE_NEWS:
	    name = NEWS_FILE;
	    pnote = news_list;
	    break;
	case NOTE_CHANGES:
	    name = CHANGES_FILE;
	    pnote = changes_list;
	    break;
	case NOTE_WEDDINGS:
	    name = WEDDINGS_FILE;
	    pnote = weddings_list;
	    break;
	case NOTE_CITY:
	    name = CITY_FILE;
	    pnote = city_list;
	    break;
	case NOTE_FAMILY:
	    name = FAMILY_FILE;
	    pnote = family_list;
	    break;
	case NOTE_PERSONAL:
	    name = PERSONAL_FILE;
	    pnote = personal_list;
	    break;
    }

    CLOSE_RESERVE();
    if ( ( fp = fopen( name, "w" ) ) == NULL )
    {
	perror( name );
    }
    else
    {
	for ( ; pnote != NULL; pnote = pnote->next )
	{
	    fprintf( fp, "Sender  %s~\n", pnote->sender);
	    fprintf( fp, "Date    %s~\n", pnote->date);
	    fprintf( fp, "Stamp   %ld\n", pnote->date_stamp);
	    fprintf( fp, "To      %s~\n", pnote->to_list);
	    fprintf( fp, "Subject %s~\n", pnote->subject);
	    fprintf( fp, "Text\n%s~\n",   pnote->text);
	}
	fclose( fp );
	OPEN_RESERVE();
   	return;
    }
}
void load_notes(void)
{
    load_thread(NOTE_FILE,&note_list, NOTE_NOTE, 14*24*60*60);
    strcat(boot_buf,"ortality, th");
    load_thread(IDEA_FILE,&idea_list, NOTE_IDEA, 28*24*60*60);
    strcat(boot_buf,"ey called up");
    load_thread(PENALTY_FILE,&penalty_list, NOTE_PENALTY, 0);
    strcat(boot_buf,"on themsel");
    load_thread(NEWS_FILE,&news_list, NOTE_NEWS, 0);
    strcat(boot_buf,"ves\n\r  the a");
    load_thread(WEDDINGS_FILE,&weddings_list, NOTE_WEDDINGS, 0);
    strcat(boot_buf,"nger of th");
    load_thread(CHANGES_FILE,&changes_list,NOTE_CHANGES, 0);
         load_thread(CITY_FILE,&city_list,NOTE_CITY, 0);
    load_thread(FAMILY_FILE,&family_list,NOTE_FAMILY, 0);
    load_thread(PERSONAL_FILE,&personal_list,NOTE_PERSONAL, 0);
}

void load_thread(char *name, NOTE_DATA **list, int type, time_t free_time)
{
    FILE *fp;
    NOTE_DATA *pnotelast;
 
    if ( ( fp = fopen( name, "r" ) ) == NULL )
	return;
	 
    pnotelast = NULL;
    for ( ; ; )
    {
	NOTE_DATA *pnote;
	char letter;
	 
	do
	{
	    letter = getc( fp );
            if ( feof(fp) )
            {
                fclose( fp );
                return;
            }
        }
        while ( isspace(letter) );
        ungetc( letter, fp );
 
        pnote           = alloc_perm( sizeof(*pnote) );
 
        if ( str_cmp( fread_word( fp ), "sender" ) )
            break;
        pnote->sender   = fread_string( fp );
 
        if ( str_cmp( fread_word( fp ), "date" ) )
            break;
        pnote->date     = fread_string( fp );
 
        if ( str_cmp( fread_word( fp ), "stamp" ) )
            break;
        pnote->date_stamp = fread_number(fp);
 
        if ( str_cmp( fread_word( fp ), "to" ) )
            break;
        pnote->to_list  = fread_string( fp );
 
        if ( str_cmp( fread_word( fp ), "subject" ) )
            break;
        pnote->subject  = fread_string( fp );
 
        if ( str_cmp( fread_word( fp ), "text" ) )
            break;
        pnote->text     = fread_string( fp );
 
        if (free_time && pnote->date_stamp < current_time - free_time)
        {
	    free_note(pnote);
            continue;
        }

	pnote->type = type;
 
        if (*list == NULL)
            *list           = pnote;
        else
            pnotelast->next     = pnote;
 
        pnotelast       = pnote;
    }
 
    strcpy( strArea, NOTE_FILE );
    fpArea = fp;
    bug( "Load_notes: bad key word.", 0 );
    exit( 1 );
    return;
}

void append_note(NOTE_DATA *pnote)
{
    FILE *fp;
    char *name;
    NOTE_DATA **list;
    NOTE_DATA *last;

    switch(pnote->type)
    {
	default:
	    return;
	case NOTE_NOTE:
	    name = NOTE_FILE;
	    list = &note_list;
	    break;
	case NOTE_IDEA:
	    name = IDEA_FILE;
	    list = &idea_list;
	    break;
	case NOTE_PENALTY:
	    name = PENALTY_FILE;
	    list = &penalty_list;
	    break;
	case NOTE_NEWS:
	     name = NEWS_FILE;
	     list = &news_list;
	     break;
	case NOTE_CHANGES:
	     name = CHANGES_FILE;
	     list = &changes_list;
	     break;
	case NOTE_WEDDINGS:
	     name = WEDDINGS_FILE;
	     list = &weddings_list;
	     break;
	case NOTE_FAMILY:
	     name = FAMILY_FILE;
	     list = &family_list;
	     break;
	case NOTE_PERSONAL:
	     name = PERSONAL_FILE;
	     list = &personal_list;
	     break;
	case NOTE_CITY:
	     name = CITY_FILE;
	     list = &city_list;
	     break;
    }

    if (*list == NULL)
	*list = pnote;
    else
    {
	for ( last = *list; last->next != NULL; last = last->next);
	last->next = pnote;
    }

    CLOSE_RESERVE();
    if ( ( fp = fopen(name, "a" ) ) == NULL )
    {
        perror(name);
    }
    else
    {
        fprintf( fp, "Sender  %s~\n", pnote->sender);
        fprintf( fp, "Date    %s~\n", pnote->date);
        fprintf( fp, "Stamp   %ld\n", pnote->date_stamp);
        fprintf( fp, "To      %s~\n", pnote->to_list);
        fprintf( fp, "Subject %s~\n", pnote->subject);
        fprintf( fp, "Text\n%s~\n", pnote->text);
        fclose( fp );
    }
    OPEN_RESERVE();
}

bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( !str_cmp( ch->name, pnote->sender ) )
	return TRUE;

    if ( is_name( "all", pnote->to_list ) )
	return TRUE;

    if ( IS_IMMORTAL(ch) && is_name( "immortal", pnote->to_list ) )
	return TRUE;

    if (ch->clan && is_name(clan_table[ch->clan].name,pnote->to_list))
	return TRUE;
    if (ch->hometown && is_name(hometown_table[ch->hometown].name,pnote->to_list))
	return TRUE;
    if ( is_exact_name( ch->name, pnote->to_list ) )
	return TRUE;

    return FALSE;
}



void note_attach( CHAR_DATA *ch, int type )
{
    NOTE_DATA *pnote;

    if ( ch->pnote != NULL )
	return;

    pnote = new_note();

    pnote->next		= NULL;
    pnote->sender	= str_dup( ch->short_descr );
    pnote->date		= str_dup( "" );
    pnote->to_list	= str_dup( "" );
    pnote->subject	= str_dup( "" );
    pnote->text		= str_dup( "" );
    pnote->type		= type;
    ch->pnote		= pnote;
    return;
}



void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote, bool delete)
{
    char to_new[MAX_INPUT_LENGTH];
    char to_one[MAX_INPUT_LENGTH];
    NOTE_DATA *prev;
    NOTE_DATA **list;
    char *to_list;

    if (!delete)
    {
	/* make a new list */
        to_new[0]	= '\0';
        to_list	= pnote->to_list;
        while ( *to_list != '\0' )
        {
    	    to_list	= one_argument( to_list, to_one );
    	    if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
	    {
	        strcat( to_new, " " );
	        strcat( to_new, to_one );
	    }
        }
        /* Just a simple recipient removal? */
       if ( str_cmp( ch->name, pnote->sender ) && to_new[0] != '\0' )
       {
	   free_string( pnote->to_list );
	   pnote->to_list = str_dup( to_new + 1 );
	   return;
       }
    }
    /* nuke the whole note */

    switch(pnote->type)
    {
	default:
	    return;
	case NOTE_NOTE:
	    list = &note_list;
	    break;
	case NOTE_IDEA:
	    list = &idea_list;
	    break;
	case NOTE_PENALTY:
	    list = &penalty_list;
	    break;
	case NOTE_NEWS:
	    list = &news_list;
	    break;
	case NOTE_CHANGES:
	    list = &changes_list;
	    break;
	case NOTE_WEDDINGS:
	    list = &weddings_list;
	    break;
	case NOTE_CITY:
	    list = &city_list;
	    break;
	case NOTE_PERSONAL:
	    list = &personal_list;
	    break;
	case NOTE_FAMILY:
	    list = &family_list;
	    break;

    }

    /*
     * Remove note from linked list.
     */
    if ( pnote == *list )
    {
	*list = pnote->next;
    }
    else
    {
	for ( prev = *list; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == pnote )
		break;
	}

	if ( prev == NULL )
	{
	    bug( "Note_remove: pnote not found.", 0 );
	    return;
	}

	prev->next = pnote->next;
    }

    save_notes(pnote->type);
    free_note(pnote);
    return;
}

bool hide_note (CHAR_DATA *ch, NOTE_DATA *pnote)
{
    time_t last_read;

    if (IS_NPC(ch))
	return TRUE;

    switch (pnote->type)
    {
	default:
	    return TRUE;
	case NOTE_NOTE:
	    last_read = ch->pcdata->last_note;
	    break;
	case NOTE_IDEA:
	    last_read = ch->pcdata->last_idea;
	    break;
	case NOTE_PENALTY:
	    last_read = ch->pcdata->last_penalty;
	    break;
	case NOTE_NEWS:
	    last_read = ch->pcdata->last_news;
	    break;
	case NOTE_CHANGES:
	    last_read = ch->pcdata->last_changes;
	    break;
	case NOTE_WEDDINGS:
	    last_read = ch->pcdata->last_weddings;
	    break;
	case NOTE_FAMILY:
	    last_read = ch->pcdata->last_family;
	    break;
	case NOTE_PERSONAL:
	    last_read = ch->pcdata->last_personal;
	    break;
	case NOTE_CITY:
	    last_read = ch->pcdata->last_city;
	    break;
    }
    
    if (pnote->date_stamp <= last_read)
	return TRUE;

    if (!str_cmp(ch->name,pnote->sender))
	return TRUE;

    if (!is_note_to(ch,pnote))
	return TRUE;

    return FALSE;
}

void update_read(CHAR_DATA *ch, NOTE_DATA *pnote)
{
    time_t stamp;

    if (IS_NPC(ch))
	return;

    stamp = pnote->date_stamp;

    switch (pnote->type)
    {
        default:
            return;
        case NOTE_NOTE:
	    ch->pcdata->last_note = UMAX(ch->pcdata->last_note,stamp);
            break;
        case NOTE_IDEA:
	    ch->pcdata->last_idea = UMAX(ch->pcdata->last_idea,stamp);
            break;
        case NOTE_PENALTY:
	    ch->pcdata->last_penalty = UMAX(ch->pcdata->last_penalty,stamp);
            break;
        case NOTE_NEWS:
	    ch->pcdata->last_news = UMAX(ch->pcdata->last_news,stamp);
            break;
        case NOTE_CHANGES:
	    ch->pcdata->last_changes = UMAX(ch->pcdata->last_changes,stamp);
            break;
	case NOTE_WEDDINGS:
	    ch->pcdata->last_weddings = UMAX(ch->pcdata->last_weddings,stamp);
	    break;
        case NOTE_CITY:
	    ch->pcdata->last_city = UMAX(ch->pcdata->last_city,stamp);
            break;
        case NOTE_PERSONAL:
	    ch->pcdata->last_personal = UMAX(ch->pcdata->last_personal,stamp);
            break;
	case NOTE_FAMILY:
	    ch->pcdata->last_family = UMAX(ch->pcdata->last_family,stamp);
	    break;
    }
}

void parse_note( CHAR_DATA *ch, char *argument, int type )
{
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    NOTE_DATA *pnote;
    NOTE_DATA **list;
    char *list_name;
    int vnum;
    int anum;

    if ( IS_NPC(ch) )
	return;

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
    && (!IS_IMMORTAL(ch)))
    {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
    }

    switch(type)
    {
	default:
	    return;
        case NOTE_NOTE:
            list = &note_list;
	    list_name = "notes";
            break;
        case NOTE_IDEA:
            list = &idea_list;
	    list_name = "ideas";
            break;
        case NOTE_PENALTY:
            list = &penalty_list;
	    list_name = "penalties";
            break;
        case NOTE_NEWS:
            list = &news_list;
	    list_name = "news";
            break;
        case NOTE_CHANGES:
            list = &changes_list;
	    list_name = "changes";
            break;
	case NOTE_WEDDINGS:
	    list = &weddings_list;
	    list_name = "wedding announcements";
	    break;
        case NOTE_FAMILY:
            list = &family_list;
	    list_name = "family notes";
            break;
        case NOTE_PERSONAL:
            list = &personal_list;
	    list_name = "personal notes";
            break;
	case NOTE_CITY:
	    list = &city_list;
	    list_name = "city notes";
	    break;
    }

    argument = one_argument( argument, arg );
    smash_tilde( argument );

    if ( arg[0] == '\0' || !str_prefix( arg, "read" ) )
    {
        bool fAll;
 
        if ( !str_cmp( argument, "all" ) )
        {
            fAll = TRUE;
            anum = 0;
        }
 
        else if ( argument[0] == '\0' || !str_prefix(argument, "next"))
        /* read next unread note */
        {
            vnum = 0;
            for ( pnote = *list; pnote != NULL; pnote = pnote->next)
            {
                if (!hide_note(ch,pnote))
                {
                      sprintf( buf, "{3Note[{#%d{3]\n\r{3From: {#%s\n\r{3To: {#%s\n\r{3Subject: {#%s\n\r{3Date Sent: {#%s\n\r",
                        vnum,
                        pnote->sender,
                        pnote->to_list,
                        pnote->subject,
                        pnote->date);
                    send_to_char("{1===================={!====================={1===================={x\n\r",ch);
		    send_to_char( buf, ch );
                    send_to_char("{1====================={!====================={1===================={x\n\r",ch);
		    page_to_char( pnote->text, ch );  
                    send_to_char("{1--------------------{!---------------------{1--------------------{x\n\r",ch);
                    update_read(ch,pnote);
       		return;
                }
                else if (is_note_to(ch,pnote))
                    vnum++;
            }    
	  do_unread(ch);
	return;
        }
 
        else if ( is_number( argument ) )
        {
            fAll = FALSE;
            anum = atoi( argument );
        }
        else
        {
            send_to_char( "Read which number?\n\r", ch );
            return;
        }
 
        vnum = 0;
        for ( pnote = *list; pnote != NULL; pnote = pnote->next )
        {
            if ( is_note_to( ch, pnote ) && ( vnum++ == anum || fAll ) )
            {
            sprintf( buf, "{3Note[{#%d{3]\n\r{3From: {#%s\n\r{3To: {#%s\n\r{3Subject: {#%s\n\r{3Date Sent: {#%s\n\r",
                 vnum-1,
                 pnote->sender,
                 pnote->to_list,
                 pnote->subject,
                 pnote->date);
                 send_to_char("{1===================={!====================={1===================={x\n\r",ch);
                 send_to_char( buf, ch );
                 send_to_char("{1====================={!====================={1===================={x\n\r",ch);
                 page_to_char( pnote->text, ch );
                 send_to_char("{1--------------------{!---------------------{1--------------------{x\n\r",ch);
                 update_read(ch,pnote);
                return;
            }
        }
 
	sprintf(buf,"There aren't that many %s.\n\r",list_name);
	send_to_char(buf,ch);
        return;
    }

    if ( !str_prefix( arg, "list" ) )
    {
	BUFFER *output;
	vnum = 0;
 	output = new_buf();
	for ( pnote = *list; pnote != NULL; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) )
	    {
		sprintf( buf, "{1[{3%3d%s{1]{#%s{3:{# %s\n\r",
		    vnum, hide_note(ch,pnote) ? " " : "N", 
		    pnote->sender, pnote->subject );
		add_buf(output,buf);
		vnum++;
	    }
	}
	if (!vnum)
	{
	    switch(type)
	    {
		case NOTE_NOTE:	
		    sprintf( buf, "There are no notes for you.\n\r");
		    break;
		case NOTE_IDEA:
		    sprintf( buf, "There are no ideas for you.\n\r");
		    break;
		case NOTE_PENALTY:
		    sprintf( buf, "There are no penalties for you.\n\r");
		    break;
		case NOTE_NEWS:
		    sprintf( buf, "There are no news for you.\n\r");
		    break;
		case NOTE_CHANGES:
		    sprintf( buf, "There are no changes for you.\n\r");
		    break;
		case NOTE_WEDDINGS:
		    sprintf( buf, "There are no wedding announcements for you.\n\r");
		    break;
	    }
	    add_buf(output,buf);
	}
	send_to_char("{1-------------------------------------------{x\n\r",ch);
	page_to_char( buf_string(output), ch );
	send_to_char("{1-------------------------------------------{x\n\r",ch);
	free_buf(output);
	return;
    }

    if ( !str_prefix( arg, "remove" ) )
    {
        if ( !is_number( argument ) )
        {
            send_to_char( "Note remove which number?\n\r", ch );
            return;
        }
 
        anum = atoi( argument );
        vnum = 0;
        for ( pnote = *list; pnote != NULL; pnote = pnote->next )
        {
            if ( is_note_to( ch, pnote ) && vnum++ == anum )
            {
                note_remove( ch, pnote, FALSE );
                send_to_char( "Ok.\n\r", ch );
                return;
            }
        }
 
	sprintf(buf,"There aren't that many %s.",list_name);
	send_to_char(buf,ch);
        return;
    }
 
    /*****************Maji2003************************************/
  	if ( !str_prefix( arg, "edit" ))
	{
		if ( ch->pnote == NULL )
		{
			send_to_char( "You have no note in progress.\n\r", ch );
			return;
		}

		if (ch->pnote->type != type)
		{
			send_to_char( "You aren't working on that kind of note.\n\r", ch );
			return;
		}
		send_to_char( "Editing the text on your note:\n\r", ch );
		string_append(ch, &ch->pnote->text);
		return;
	}
/****************************************************************/

	 if ( !str_prefix( arg, "delete" ) && get_trust(ch) >= MAX_LEVEL - 1)
    {
        if ( !is_number( argument ) )
        {
            send_to_char( "Note delete which number?\n\r", ch );
            return;
        }
 
        anum = atoi( argument );
        vnum = 0;
        for ( pnote = *list; pnote != NULL; pnote = pnote->next )
        {
            if ( is_note_to( ch, pnote ) && vnum++ == anum )
            {
                note_remove( ch, pnote,TRUE );
                send_to_char( "Ok.\n\r", ch );
                return;
            }
        }

 	sprintf(buf,"There aren't that many %s.",list_name);
	send_to_char(buf,ch);
        return;
    }

    if (!str_prefix(arg,"catchup"))
    {
	switch(type)
	{
	    case NOTE_NOTE:	
		ch->pcdata->last_note = current_time;
		break;
	    case NOTE_IDEA:
		ch->pcdata->last_idea = current_time;
		break;
	    case NOTE_PENALTY:
		ch->pcdata->last_penalty = current_time;
		break;
	    case NOTE_NEWS:
		ch->pcdata->last_news = current_time;
		break;
	    case NOTE_CHANGES:
		ch->pcdata->last_changes = current_time;
		break;
	    case NOTE_WEDDINGS:
		ch->pcdata->last_weddings = current_time;
		break;
	    case NOTE_CITY:
		ch->pcdata->last_city = current_time;
		break;
	    case NOTE_PERSONAL:
		ch->pcdata->last_personal = current_time;
		break;
	    case NOTE_FAMILY:
		ch->pcdata->last_family = current_time;
		break;
	}
	return;
    }

    /* below this point only certain people can edit notes */
    if ((type == NOTE_NEWS && !IS_TRUSTED(ch,KNIGHT))
    ||  (type == NOTE_CHANGES && !IS_TRUSTED(ch,IMMORTAL))
    ||  (type == NOTE_WEDDINGS && !ch->wedpost && !IS_IMMORTAL(ch)))
    {
	sprintf(buf,"You aren't authorized to write %s.",list_name);
	send_to_char(buf,ch);
	return;
    }

    if ( !str_cmp( arg, "+" ) )
    {
	note_attach( ch,type );
	if (ch->pnote->type != type)
	{
	    send_to_char(
		"You already have a different note in progress.\n\r",ch);
	    return;
	}

	if (strlen(ch->pnote->text)+strlen(argument) >= 4096)
	{
	    send_to_char( "Note too long.\n\r", ch );
	    return;
	}

 	buffer = new_buf();

	add_buf(buffer,ch->pnote->text);
	add_buf(buffer,argument);
	add_buf(buffer,"\n\r");
	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf_string(buffer) );
	free_buf(buffer);
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if (!str_cmp(arg,"-"))
    {
 	int len;
	bool found = FALSE;

	note_attach(ch,type);
        if (ch->pnote->type != type)
        {
            send_to_char(
                "You already have a different note in progress.\n\r",ch);
            return;
        }

	if (ch->pnote->text == NULL || ch->pnote->text[0] == '\0')
	{
	    send_to_char("No lines left to remove.\n\r",ch);
	    return;
	}

	strcpy(buf,ch->pnote->text);

	for (len = strlen(buf); len > 0; len--)
 	{
	    if (buf[len] == '\r')
	    {
		if (!found)  /* back it up */
		{
		    if (len > 0)
			len--;
		    found = TRUE;
		}
		else /* found the second one */
		{
		    buf[len + 1] = '\0';
		    free_string(ch->pnote->text);
		    ch->pnote->text = str_dup(buf);
		    return;
		}
	    }
	}
	buf[0] = '\0';
	free_string(ch->pnote->text);
	ch->pnote->text = str_dup(buf);
	return;
    }

    if ( !str_prefix( arg, "subject" ) )
    {
	note_attach( ch,type );
        if (ch->pnote->type != type)
        {
            send_to_char(
                "You already have a different note in progress.\n\r",ch);
            return;
        }

	free_string( ch->pnote->subject );
	ch->pnote->subject = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "to" ) )
    {
	note_attach( ch,type );
        if (ch->pnote->type != type)
        {
            send_to_char(
                "You already have a different note in progress.\n\r",ch);
            return;
        }
	free_string( ch->pnote->to_list );
	ch->pnote->to_list = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "clear" ) )
    {
	if ( ch->pnote != NULL )
	{
	    free_note(ch->pnote);
	    ch->pnote = NULL;
	}

	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "show" ) )
    {
	if ( ch->pnote == NULL )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

	if (ch->pnote->type != type)
	{
	    send_to_char("You aren't working on that kind of note.\n\r",ch);
	    return;
	}

	sprintf( buf, "%s: %s\n\rTo: %s\n\r",
	    ch->pnote->sender,
	    ch->pnote->subject,
	    ch->pnote->to_list
	    );
	send_to_char( buf, ch );
	send_to_char( ch->pnote->text, ch );
	return;
    }

    if ( !str_prefix( arg, "post" ) || !str_prefix(arg, "send"))
    {
	char *strtime;
        DESCRIPTOR_DATA *d;
	if ( ch->pnote == NULL )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

        if (ch->pnote->type != type)
        {
            send_to_char("You aren't working on that kind of note.\n\r",ch);
            return;
        }

	if (!str_cmp(ch->pnote->to_list,""))
	{
	    send_to_char(
		"You need to provide a recipient (name, all, or immortal).\n\r",
		ch);
	    return;
	}

	if (!str_cmp(ch->pnote->subject,""))
	{
	    send_to_char("You need to provide a subject.\n\r",ch);
	    return;
	}

	ch->pnote->next			= NULL;
	strtime				= ctime( &current_time );
	strtime[strlen(strtime)-1]	= '\0';
	ch->pnote->date			= str_dup( strtime );
	ch->pnote->date_stamp		= current_time;
       
       sprintf(buf,"Check your %s. [Sender: %s Subject: %s]\n\r",list_name,ch->name,ch->pnote->subject);

           for(d=descriptor_list;d;d = d->next)
       {
           if (d->connected == CON_PLAYING && is_note_to(d->character,ch->pnote))
              {
                 stc(buf,d->character);
              }
       }

	append_note(ch->pnote);
	ch->pnote = NULL;
	return;
    }

    send_to_char( "You can't do that.\n\r", ch );
    return;
}

void note_remove_quiet( NOTE_DATA *pnote)
{
    NOTE_DATA *prev;
    NOTE_DATA **list;


    switch(pnote->type)
    {
	default:
	    return;
	case NOTE_NOTE:
	    list = &note_list;
	    break;
	case NOTE_IDEA:
	    list = &idea_list;
	    break;
	case NOTE_PENALTY:
	    list = &penalty_list;
	    break;
	case NOTE_NEWS:
	    list = &news_list;
	    break;
	case NOTE_CHANGES:
	    list = &changes_list;
	    break;
	case NOTE_WEDDINGS:
	    list = &weddings_list;
	    break;
	case NOTE_PERSONAL:
	    list = &personal_list;
	    break;
	case NOTE_CITY:
	    list = &city_list;
	    break;
	case NOTE_FAMILY:
	    list = &family_list;
	    break;
    }

    /*
     * Remove note from linked list.
     */
    if ( pnote == *list )
    {
	*list = pnote->next;
    }
    else
    {
	for ( prev = *list; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == pnote )
		break;
	}

	if ( prev == NULL )
	{
	    bug( "Note_remove: pnote not found.", 0 );
	    return;
	}

	prev->next = pnote->next;
    }

    save_notes(pnote->type);
    free_note(pnote);
    return;
}

void expire_notes ( void )
{
    NOTE_DATA *pnote;
    NOTE_DATA **list;
    long diff;
    int note_num;

    note_num = 0;
    list = &note_list;
    for ( pnote = *list; pnote != NULL; pnote = pnote->next ) 
    {
        diff = (long)current_time - (long)pnote->date_stamp;
 	if (diff > 864000) {
	    note_num++;
	}
    }
    for ( ; note_num > 0; note_num--)
    {
	pnote = *list;
	if (pnote != NULL)
	{
	    note_remove_quiet(pnote);
	}
    }
    note_num = 0;
    list = &idea_list; 
    for ( pnote = *list; pnote != NULL; pnote = pnote->next ) 
    {
        diff = (long)current_time - (long)pnote->date_stamp;
 	if (diff > 864000) {
	    note_num++;
	}
    }
    for ( ; note_num > 0; note_num--)
    {
	pnote = *list;
	if (pnote != NULL)
	{
	    note_remove_quiet(pnote);
	}
    }

    return;
}


