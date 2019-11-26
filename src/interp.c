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
*   ROM 2.4 is copyright 1993-1995 Russ Taylor             
*
*   ROM has been brought to you by the ROM consortium          *
*       Russ Taylor (rtaylor@pacinfo.com)                  *
*       Gabrielle Taylor (gtaylor@pacinfo.com)             *
*       Brian Moore (rom@rom.efn.org)                  *
*   By using this code, you have agreed to follow the terms of the     *
*   ROM license, in the file Rom24/doc/rom.license             *
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
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"


bool    check_social    args( ( CHAR_DATA *ch, char *command,
                char *argument ) );



/*
 * Command logging types.
 */
#define LOG_NORMAL  0
#define LOG_ALWAYS  1
#define LOG_NEVER   2



/*
 * Log-all switch.
 */
bool                fLogAll     = FALSE;

bool                global_quest    = FALSE;
void do_function (CHAR_DATA *ch, DO_FUN *do_fun, char *argument)
{
    char *command_string;

    /* copy the string */
    command_string = str_dup(argument);

    /* dispatch the command */
    (*do_fun) (ch, command_string);

    /* free the string */
    free_string(command_string);
}

/*
 * Command table.
 */
const   struct  cmd_type    cmd_table   [] =
{
    /*
     * Common movement commands.
     */
    { "north",      do_north,   POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "east",       do_east,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "south",      do_south,   POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "west",       do_west,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "up",     do_up,      POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "down",       do_down,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "ne",       do_northeast,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
   { "se",       do_southeast,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
   { "sw",       do_southwest,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
   { "nw",       do_northwest,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "northeast",       do_northeast,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "southeast",       do_southeast,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "southwest",       do_southwest,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "northwest",       do_northwest,    POS_STANDING,    0,  1,  LOG_NEVER, 1,6 },
    { "in",     do_inside,    POS_STANDING,  0, 1, LOG_NEVER, 1, 6 },
    { "inside",     do_inside,    POS_STANDING,  0, 1, LOG_NEVER, 1, 6 },
    { "out",     do_outside,    POS_STANDING,  0, 1, LOG_NEVER, 1, 6 },
    { "outside",     do_outside,    POS_STANDING,  0, 1, LOG_NEVER, 1, 6 },

    { "petname",       do_petname,      POS_SLEEPING,    2,  1,  LOG_NEVER, 1,7 },
    /*

     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
/*    { "gmessage", do_gmessage, POS_SLEEPING, IM, 1, LOG_NORMAL, 1, 2}, */
    { "tcolor", do_tcolor, POS_SLEEPING, 0, 1, LOG_NORMAL, 1, 2 },
    { "newbiezone", do_newbiezone, POS_RESTING,  0,  1,  LOG_NORMAL, 1, 2 },
    { "look",       do_look,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "askout",       do_askout,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "propose",       do_propose,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "divorce",       do_divorce,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "breakup",       do_breakup,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "noconsent",       do_noconsent,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "blist",      do_blist,   POS_DEAD,        0, 1,  LOG_NORMAL,  1,2 },
    { "cast",       do_cast,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,2 },
    { "banish",       do_banish,     POS_RESTING,     0, 1, LOG_NORMAL, 1, 1},
    { "citizen",      do_citizen, POS_RESTING, 0,1, LOG_NORMAL, 1, 1},
    { "fish",           do_fish,        POS_RESTING,     0,  1,  LOG_NORMAL, 1,0 },
    { "consent",       do_consent,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "clearwed",       do_clearwed,    POS_RESTING,     IM,  1,  LOG_NORMAL, 1,2 },
    { "pretitle",    do_pretitle,    POS_DEAD,      10, 1,  LOG_NORMAL, 1, 
1 },
    { "shop",     do_shop,          POS_DEAD,       1, 1, LOG_ALWAYS,  1, 1},
    { "lastname",   do_lastname,     POS_DEAD,      10, 1,  LOG_NORMAL,  1, 1 },
    { "at",             do_at,          POS_DEAD,       L6,  1,  LOG_NORMAL, 1,0 },
    { "grant",         do_grant,        POS_SLEEPING,  10,  1,  LOG_NORMAL, 1, 1},
    { "wcity",           do_whocity,        POS_RESTING,    0,  1,  LOG_NORMAL, 1,1 },
    { "ooc",            do_ooc,         POS_SLEEPING,    1,  1,  LOG_NORMAL, 1,0 },
    { "buy",        do_buy,     POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "channels",       do_channels,    POS_DEAD,        1,  1,  LOG_NORMAL, 1,3 },
    { "exits",      do_exits,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,6 },
    { "get",        do_get,     POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "map",        do_map,     POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "dice",        do_dice,     POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "goto",           do_goto,        POS_DEAD,       IM,  1,  LOG_NORMAL, 1,0 },
    { "group",          do_group,       POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "bounty",     do_bounty,  POS_DEAD,    0, 1,  LOG_NORMAL, 1, 1 },
    { "guild",      do_guild,   POS_DEAD,   L2,  1,  LOG_ALWAYS, 1,0 },
    { "hit",        do_kill,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 0,6},
    { "inventory",  do_inventory,   POS_DEAD,    0,  1,  LOG_NORMAL, 1,2 },
    { "kill",       do_kill,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "ctell",         do_ctell,        POS_DEAD,        0,  1,  LOG_NORMAL, 1, 1},
    { "clan",       do_clantalk,    POS_SLEEPING,    2,  1,  LOG_NORMAL, 1,3 },
    { "music",          do_music,       POS_SLEEPING,    2,  1,  LOG_NORMAL, 1,3}, 
    { "mock",       do_mock,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,7 },
    { "order",      do_order,   POS_RESTING,     2,  1,  LOG_NORMAL, 1,7 },
    { "practice",       do_practice,    POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "rest",       do_rest,    POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "sit",        do_sit,     POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "sockets",        do_sockets, POS_DEAD,       IM,  1,  LOG_NORMAL, 1,0 },
    { "stand",      do_stand,   POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "tell",       do_tell,    POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,3 },
    { "unlock",         do_unlock,      POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "wield",      do_wear,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "wizhelp",    do_wizhelp, POS_DEAD,    0,  2,  LOG_NORMAL, 1,0 },
    { "study",          do_study,       POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "withdraw",           do_withdraw,        POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "balance",        do_balance,     POS_SLEEPING,   0, 1, LOG_NORMAL, 1,2 },
    { "deposit",       do_deposit,     POS_SLEEPING,   0,  1, LOG_NORMAL, 1,2 },
{"showstats", do_showstats, POS_SLEEPING, 0, 1, LOG_NORMAL, 1, 1}, 

    { "beep",       do_beep,        POS_SLEEPING,   20,  1,  LOG_NORMAL, 1,0 },

    /*
     * Informational commands.
     */
    { "affects",    do_affects, POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "areas",      do_areas,   POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "bug",        do_bug,     POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "changes",    do_changes, POS_DEAD,    0,  1,  LOG_NORMAL, 1,3 },
    { "commands",   do_commands,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "compare",    do_compare, POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "consider",   do_consider,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,7 },
    { "count",      do_count,   POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "credits",    do_credits, POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "equipment",  do_equipment,   POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "examine",    do_examine, POS_RESTING,     0,  1,  LOG_NORMAL, 1,4 },
/*  { "groups",     do_groups,  POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,4 }, */
    { "?",      do_help,    POS_DEAD,    0,  1,  LOG_NORMAL, 0,4 },
    { "help",       do_help,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "idea",       do_idea,    POS_DEAD,    1,  1,  LOG_NORMAL, 1,4 },
    { "groupinfo",      do_groups,      POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,4 },
    { "info",           do_announce,    POS_SLEEPING,    IM,  1,  LOG_NORMAL, 1,4 },   
    { "motd",       do_motd,    POS_DEAD,        0,  1,  LOG_NORMAL, 1,4 },
    { "news",       do_news,    POS_DEAD,    1,  1,  LOG_NORMAL, 1,4 },
    { "peek",       do_peek,    POS_STANDING,    0,  1,  LOG_NORMAL, 1,4 },
    { "read",       do_read,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,4 },
    { "report",     do_report,  POS_RESTING,     0,  1,  LOG_NORMAL, 1,4 },
    { "rules",      do_rules,   POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "score",      do_score,   POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "scan",       do_scan,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,4 },
    { "skills",     do_skills,  POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "socials",    do_socials, POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
/*    { "show",     do_show,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },*/
    { "spells",     do_spells,  POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "story",      do_story,   POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "time",       do_time,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "typo",       do_typo,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "weather",    do_weather, POS_RESTING,     0,  1,  LOG_NORMAL, 1,4 },
    { "who",        do_who,     POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "whois",      do_whois,   POS_DEAD,    0,  1,  LOG_NORMAL, 1,4 },
    { "wizlist",    do_wizlist, POS_DEAD,        0,  1,  LOG_NORMAL, 1,4 },
    { "worth",      do_worth,   POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,4 },
    { "slist",          do_slist,       POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,4 },
    { "clanlist",       do_clanlist,    POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,4 },

    /*
     * Configuration commands.
     */
    { "alia",       do_alia,    POS_DEAD,    0,  1,  LOG_NORMAL, 0,1 },
    { "alias",      do_alias,   POS_DEAD,    0,  1,  LOG_NORMAL, 1,1 },
    { "make",       do_make,   POS_RESTING, 0, 1, LOG_NORMAL,  1 },
    { "mudftp",     do_mudftp,  POS_DEAD,    0,  1,  LOG_NORMAL, 1 },
    { "autolist",   do_autolist,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,1 },
    { "autoassist", do_autoassist,  POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "autoexit",   do_autoexit,    POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "autogold",   do_autogold,    POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "autoloot",   do_autoloot,    POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "autosac",    do_autosac, POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "autosplit",  do_autosplit,   POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "autostore",  do_autostore,   POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "autopeek",   do_autopeek,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,1 },
    { "brief",      do_brief,   POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
/*  { "channels",   do_channels,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,1 }, */
    { "colour",         do_colour,      POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "color",          do_colour,      POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "combine",    do_combine, POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "compact",    do_compact, POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "description",    do_description, POS_DEAD,    0,  1,  LOG_NORMAL, 1,1 },
    { "long",       do_long,    POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "nofollow",   do_nofollow,    POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "noloot",     do_noloot,  POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "meet",  do_meet,  POS_DEAD,  0, 1,   LOG_NORMAL, 1, 1},
    { "nosummon",   do_nosummon,    POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "notran",     do_notran,  POS_DEAD,    0,  1,  LOG_NORMAL, 1,1 },
    { "outfit",     do_outfit,  POS_RESTING,     0,  1,  LOG_NORMAL, 1,1 },
    { "password",   do_password,    POS_DEAD,    0,  1,  LOG_NEVER,  1,1 },
    { "prompt",     do_prompt,  POS_DEAD,        0,  1,  LOG_NORMAL, 1,1 },
    { "scroll",     do_scroll,  POS_DEAD,    0,  1,  LOG_NORMAL, 1,1 },
    { "title",      do_title,   POS_DEAD,    0,  1,  LOG_NORMAL, 1,1 },
    { "unalias",    do_unalias, POS_DEAD,    0,  1,  LOG_NORMAL, 1,1 },
    { "wimpy",      do_wimpy,   POS_DEAD,    0,  1,  LOG_NORMAL, 1,1 },

    /*
     * Communication commands.
     */
    { "afk",        do_afk,     POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,3 },
    { "ansi",       do_colour,  POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,3 },
    { "deaf",       do_deaf,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,3 },
    { "emote",      do_emote,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,3 },
    { "pmote",      do_pmote,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,3 },
    { "=",      do_cgossip, POS_SLEEPING,    2,  1,  LOG_NORMAL, 0,3 },
    { "cgossip",    do_cgossip, POS_SLEEPING,    2,  1,  LOG_NORMAL, 1,3 },
    { "-",      do_qgossip, POS_SLEEPING,    2,  1,  LOG_NORMAL, 0,3 },
    { "qgossip",    do_qgossip, POS_SLEEPING,    2,  1,  LOG_NORMAL, 1,3 },
    { ",",      do_emote,   POS_RESTING,     2,  1,  LOG_NORMAL, 0,3 },
    { "grats",      do_grats,   POS_SLEEPING,    2,  1,  LOG_NORMAL, 1,3 },
    { "gtell",      do_gtell,   POS_DEAD,    2,  1,  LOG_NORMAL, 1,3 },
    { ";",      do_gtell,   POS_DEAD,    2,  1,  LOG_NORMAL, 0,3},
/*  { "music",      do_music,   POS_SLEEPING,    2,  1,  LOG_NORMAL, 1,3 }, */
    { "note",       do_note,    POS_SLEEPING,    1,  1,  LOG_NORMAL, 1,3 },
    { "personal",   do_personal, POS_SLEEPING, 1, 1, LOG_NORMAL, 1,3 },
    { "city",    do_city,   POS_SLEEPING, 1, 1, LOG_NORMAL, 1, 3 },
    { "family",  do_family, POS_SLEEPING, 1, 1, LOG_NORMAL, 1, 3 },
    { "pose",       do_pose,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,3 },
    { "quest",      do_quest,   POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,3 },
    { "quiet",      do_quiet,   POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,3 },
    { "reply",      do_reply,   POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,3 },
    { "replay",     do_replay,  POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,3 },
    { "say",        do_say,     POS_RESTING,     0,  1,  LOG_NORMAL, 1,3 },
    { "'",      do_say,     POS_RESTING,     0,  1,  LOG_NORMAL, 0,3 },
    { "shout",      do_shout,   POS_DEAD,     0,  1,  LOG_NORMAL, 1,3 },
    { ".",     do_shout,   POS_DEAD,  0, 1, LOG_NORMAL, 1, 3 },
    { "unread",     do_unread,  POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,3 },
    { "weddings",   do_weddings,    POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,3 },
    { "yell",       do_yell,    POS_RESTING,     2,  1,  LOG_NORMAL, 1,3 },
    { "pk",             do_pk,          POS_DEAD,        10,  1,  LOG_NORMAL, 1,3 },

    /*
     * Object manipulation commands.
     */
    { "brandish",   do_brandish,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "close",      do_close,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "drink",      do_drink,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "drop",       do_drop,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "eat",        do_eat,     POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "envenom",    do_envenom, POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "fill",       do_fill,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "give",       do_give,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "heal",       do_heal,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 }, 
    { "repent",     do_repent,  POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "curse",      do_curse,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "hold",       do_wear,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "list",       do_list,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "lock",       do_lock,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "open",       do_open,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "pick",       do_pick,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5},
    { "pour",       do_pour,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "put",        do_put,     POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "quaff",      do_quaff,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "recite",     do_recite,  POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "remove",     do_remove,  POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "sell",       do_sell,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "second",     do_second,  POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "take",       do_get,     POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "sacrifice",  do_sacrifice,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "junk",           do_sacrifice,   POS_RESTING,     0,  1,  LOG_NORMAL, 0,5 },
    { "tap",        do_sacrifice,   POS_RESTING,     0,  1,  LOG_NORMAL, 0,5 },   
/*  { "unlock",     do_unlock,  POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 }, */
    { "value",      do_value,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "wear",       do_wear,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    { "zap",        do_zap,     POS_RESTING,     0,  1,  LOG_NORMAL, 1,5 },
    {  "home",       do_home,        POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,5 },

    /*
     * Combat commands.
     */
    { "backstab",   do_backstab,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "bash",       do_bash,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "bs",     do_backstab,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 0,7 },
    { "berserk",    do_berserk, POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "circle",     do_circle,  POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "feed",       do_feed,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "dirt",       do_dirt,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "disarm",     do_disarm,  POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "flee",       do_flee,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "gouge",      do_gouge,   POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "kick",       do_kick,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "murde",      do_murde,   POS_FIGHTING,    0,  1,  LOG_NORMAL, 0,7 },
    { "murder",     do_murder,  POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "rescue",     do_rescue,  POS_FIGHTING,    0,  1,  LOG_NORMAL, 0,7 },
    { "surrender",      do_surrender,   POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },
    { "trip",       do_trip,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,7 },

    /*
     * Mob command interpreter (placed here for faster scan...)
     */
    { "mob",        do_mob,     POS_DEAD,    0,  LOG_NEVER,  0 },    
    
    /*
     * Miscellaneous commands.
     */
    { "enter",      do_enter,   POS_STANDING,    0,  1,  LOG_NORMAL, 1,6 },
    { "follow",     do_follow,  POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "convert",    do_convert, POS_RESTING,     0,  1, LOG_NORMAL,  1,2 },
    { "gain",       do_gain,    POS_STANDING,    0,  1,  LOG_NORMAL, 1,2 },
    { "go",     do_enter,   POS_STANDING,    0,  1,  LOG_NORMAL, 1,6 },
/*  { "group",      do_group,   POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,4 }, */
    { "groups",     do_groups,  POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,4 },
    { "hide",       do_hide,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "member",     do_member,  POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "play",       do_play,    POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
/*  { "practice",   do_practice,    POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 }, */
    { "qui",        do_qui,     POS_DEAD,    0,  1,  LOG_NORMAL, 0,2 },
    { "quit",       do_delayquit,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,2 },
    { "recall",     do_summit,  POS_FIGHTING,    0,  1,  LOG_NORMAL, 1,6 },
    { "bladecast",  do_bladecast,  POS_FIGHTING,    0,  1,  LOG_NORMAL, 0,6 },
    { "rent",       do_rent,    POS_DEAD,    0,  1,  LOG_NORMAL, 0,2 },
    { "save",       do_save,    POS_DEAD,    0,  1,  LOG_NORMAL, 1,2 },
    { "sleep",      do_sleep,   POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "sneak",      do_sneak,   POS_STANDING,    0,  1,  LOG_NORMAL, 1,2 },
    { "split",      do_split,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "steal",      do_steal,   POS_STANDING,    0,  1,  LOG_NORMAL, 1,2 },
    { "train",      do_train,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "track",      do_track,   POS_STANDING,    0,  1,  LOG_NORMAL, 1,2 },
    { "visible",    do_visible, POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "wake",       do_wake,    POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "where",      do_where,   POS_RESTING,     0,  1,  LOG_NORMAL, 1,2 },
    { "donate",     do_donate,  POS_STANDING,    5,  1,  LOG_NORMAL, 1,2 },
    { "cdonate",    do_cdonate, POS_STANDING,    5,  1,  LOG_NORMAL, 1,2 },
    { "class",      do_class,   POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "forge",      do_forge,   POS_SLEEPING,    0,  1,  LOG_NORMAL, 0,2 },
    { "forget",     do_forget,  POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "remembe",    do_remembe, POS_SLEEPING,    0,  1,  LOG_NORMAL, 0,2 },
    { "remember",   do_remember,    POS_SLEEPING,    0,  1,  LOG_NORMAL, 1,2 },
    { "voodoo",     do_voodoo,  POS_STANDING,   20,  1,  LOG_NORMAL, 1,2 },


    /*
     * Immortal commands.
     */
    { "advance",    do_advance, POS_DEAD,   L1,  1,  LOG_ALWAYS, 1 },
    { "arealinks",      do_arealinks,   POS_DEAD,       IM,  1, LOG_ALWAYS, 1 },
    { "mlead",         do_mlead,        POS_DEAD,       IM,  1,  LOG_ALWAYS, 1, 0 },
    { "dump",       do_dump,    POS_DEAD,   ML,  1,  LOG_ALWAYS, 0 },
    { "trust",      do_trust,   POS_DEAD,   ML,  1,  LOG_ALWAYS, 1 },
    { "violate",    do_violate, POS_DEAD,   ML,  1,  LOG_ALWAYS, 1 },
    { "rebirth",      do_remort,     POS_DEAD,      IM,   1, LOG_ALWAYS, 1 },
    { "allow",      do_allow,   POS_DEAD,   L3,  1,  LOG_ALWAYS, 1 },
    { "ban",        do_ban,     POS_DEAD,   L3,  1,  LOG_ALWAYS, 1 },
    { "deny",       do_deny,    POS_DEAD,   L3,  1,  LOG_ALWAYS, 1 },
    { "disconnect", do_disconnect,  POS_DEAD,   L4,  1,  LOG_ALWAYS, 1 },
    { "dupe",       do_dupe,    POS_DEAD,   L1,  1,  LOG_NORMAL, 1 },
    { "finger",     do_finger,  POS_DEAD,    0,  1,  LOG_NORMAL, 1 },
    { "confiscate", do_confiscate, POS_DEAD, IM, 1, LOG_ALWAYS, 1 },
    { "flag",       do_flag,    POS_DEAD,   L5,  1,  LOG_ALWAYS, 1 },
    { "avatar",    do_avatar,  POS_DEAD,   L7,  1, LOG_ALWAYS, 1 },
    { "freeze",     do_freeze,  POS_DEAD,   L6,  1,  LOG_ALWAYS, 1 },
    { "permban",    do_permban, POS_DEAD,   L1,  1,  LOG_ALWAYS, 1 },
    { "protect",    do_protect, POS_DEAD,   L1,  1,  LOG_ALWAYS, 1 },
    { "reboo",      do_reboo,   POS_DEAD,   L1,  1,  LOG_NORMAL, 0 },
    { "reboot",     do_reboot,  POS_DEAD,   L1,  1,  LOG_ALWAYS, 1 },
    { "ftick",      do_ftick,   POS_DEAD,   ML,  1,  LOG_ALWAYS, 1 },
    { "set",        do_set,     POS_DEAD,   L2,  1,  LOG_NORMAL, 1 },
    { "shutdow",    do_shutdow, POS_DEAD,   ML,  1,  LOG_NORMAL, 0 },
    { "shutdown",   do_shutdown,    POS_DEAD,   ML,  1,  LOG_ALWAYS, 1 },
/*  { "sockets",    do_sockets, POS_DEAD,   IM,  1,  LOG_NORMAL, 1 }, */
    { "pload",      do_pload,  POS_DEAD,    ML,  1,  LOG_ALWAYS, 1 },
    { "punload",    do_punload,POS_DEAD,   ML,   1,  LOG_ALWAYS, 1 },
    { "wedpost",    do_wedpost, POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "wizlock",    do_wizlock, POS_DEAD,   L2,  1,  LOG_ALWAYS, 1 },
    { "fvlist",     do_fvlist,  POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "force",      do_force,   POS_DEAD,   L5,  1,  LOG_NORMAL, 1 },
    { "load",       do_load,    POS_DEAD,   L5,  1,  LOG_NORMAL, 1 },
    { "newlock",    do_newlock, POS_DEAD,   L1,  1,  LOG_ALWAYS, 1 },
    { "nochannels", do_nochannels,  POS_DEAD,   L7,  1,  LOG_ALWAYS, 1 },
    { "noemote",    do_noemote, POS_DEAD,   L5,  1,  LOG_ALWAYS, 1 },
    { "norestore",  do_norestore,   POS_DEAD,   L3,  1,  LOG_ALWAYS, 1 },
    { "noshout",    do_noshout, POS_DEAD,   L5,  1,  LOG_ALWAYS, 1 },
    { "notell",     do_notell,  POS_DEAD,   L5,  1,  LOG_ALWAYS, 1 },

    { "pecho",      do_pecho,   POS_DEAD,   L8,  1,  LOG_NORMAL, 1 }, 
    { "purge",      do_purge,   POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "restore",    do_restore, POS_DEAD,   L5,  1,  LOG_NORMAL, 1 },
    { "sla",        do_sla,     POS_DEAD,   L3,  1,  LOG_NORMAL, 0 },
    { "slay",       do_slay,    POS_DEAD,   L3,  1,  LOG_NORMAL, 1 },
    { "teleport",   do_transfer,    POS_DEAD,   L6,  1,  LOG_NORMAL, 1 },   
    { "transfer",       do_transfer,    POS_DEAD,   L6,  1,  LOG_NORMAL, 1 },
    { "corner",     do_corner,  POS_DEAD,   IM,  1,  LOG_ALWAYS, 1 },
    { "pardon",     do_pardon,  POS_DEAD,   L7,  1,  LOG_NORMAL, 1 },
    { "scatter",        do_scatter,     POS_DEAD,       L7,  1,  LOG_NORMAL, 1 },

    { "reject",  do_reject,  POS_DEAD,  0,  1,  LOG_NORMAL,  1, 2},
/*  { "at",     do_at,      POS_DEAD,   L6,  1,  LOG_NORMAL, 1 }, */
    { "poofin",     do_bamfin,  POS_DEAD,   L8,  1,  LOG_NORMAL, 1 },
    { "poofout",    do_bamfout, POS_DEAD,   L8,  1,  LOG_NORMAL, 1 },
    { "gecho",      do_echo,    POS_DEAD,   L4,  1,  LOG_NORMAL, 1 },
/*  { "goto",       do_goto,    POS_DEAD,   L8,  1,  LOG_NORMAL, 1 }, */
    { "ghost",      do_ghost,   POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "holylight",  do_holylight,   POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "incognito",  do_incognito,   POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "invis",      do_invis,   POS_DEAD,   IM,  1,  LOG_NORMAL, 0 },
    { "log",        do_log,     POS_DEAD,   L1,  1,  LOG_ALWAYS, 1 },
    { "memory",     do_memory,  POS_DEAD,   ML,  1,  LOG_NORMAL, 1 },
    { "mwhere",     do_mwhere,  POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "owhere",     do_owhere,  POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "peace",      do_peace,   POS_DEAD,   L6,  1,  LOG_NORMAL, 1 },
    { "allpeace",   do_allpeace,    POS_DEAD,   L1,  1,  LOG_NORMAL, 1 },
    { "penalty",    do_penalty, POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "echo",       do_recho,   POS_DEAD,   L8,  1,  LOG_NORMAL, 1 },
    { "return",         do_return,      POS_DEAD,       L7,  1,  LOG_NORMAL, 1 },
    { "snoop",      do_snoop,   POS_DEAD,   L4,  1,  LOG_ALWAYS, 1 },
    { "stat",       do_stat,    POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "string",     do_string,  POS_DEAD,   L6,  1,  LOG_NORMAL, 1 },
    { "switch",     do_switch,  POS_DEAD,   L7,  1,  LOG_NORMAL, 1 },
    { "wizinvis",   do_invis,   POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "wecho",      do_wecho,   POS_DEAD,   ML,  1,  LOG_NORMAL, 1 },
    { "vnum",       do_vnum,    POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "zecho",      do_zecho,   POS_DEAD,   L4,  1,  LOG_NORMAL, 1 },
    { "clead",      do_clead,   POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },

    { "clone",      do_clone,   POS_DEAD,   L6,  1,  LOG_NORMAL, 1 },
    { "wiznet",     do_wiznet,  POS_DEAD,    0,  2,  LOG_NORMAL, 1 },
    { "immtalk",    do_immtalk, POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "imptalk",    do_imptalk, POS_DEAD,   ML,  1,  LOG_NORMAL, 1 },
    { "imotd",          do_imotd,       POS_DEAD,       IM,  1,  LOG_NORMAL, 1 },
    { ":",      do_immtalk, POS_DEAD,   IM,  1,  LOG_NORMAL, 0 },
    { "smote",      do_smote,   POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "prefi",      do_prefi,   POS_DEAD,   IM,  1,  LOG_NORMAL, 0 },
    { "prefix",     do_prefix,  POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "mpoint",     do_mpoint,  POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "mquest",     do_mquest,  POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "randclan",   do_randclan,    POS_DEAD,   ML,  1,  LOG_NORMAL, 1 },
    { "sign",       do_sign,    POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "immkiss",    do_immkiss, POS_DEAD,   L5,  1,  LOG_NORMAL, 1 },
    { "squire",     do_squire,  POS_DEAD,   ML,  1,  LOG_ALWAYS, 1 },
    { "knight",     do_knight,  POS_DEAD,   ML,  1,  LOG_ALWAYS, 1 },
    { "recover",    do_recover, POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "gset",       do_gset,    POS_DEAD,   IM,  1,  LOG_NORMAL, 1 },
    { "wizslap",    do_wizslap, POS_DEAD,   L5,  1,  LOG_NORMAL, 1 },
    { "wipe",       do_wipe,    POS_DEAD,   L2,  1,  LOG_ALWAYS, 1 },
    { "mpdump",     do_mpdump,  POS_DEAD,   ML,  1,  LOG_NEVER, 1 },
    { "mpstat",     do_mpstat,  POS_DEAD,       IM,  1,  LOG_NEVER, 1 },
    { "award",          do_award,       POS_DEAD,       IM,  1,  LOG_ALWAYS, 1 },
    { "cname",          do_cname,       POS_DEAD,       IM,  1,  LOG_NORMAL, 1 },
    { "spellup",        do_spellup,     POS_DEAD,        0,  1,  LOG_NORMAL, 2 },
    { "rename",         do_rename,      POS_DEAD,       L5,  1,   LOG_ALWAYS, 1 },
    { "check",          do_check,       POS_DEAD,       IM,  1,   LOG_ALWAYS, 1 },
    { "extraexp",       do_doublexp,    POS_DEAD,       IM,  1,   LOG_ALWAYS, 1 },
{"doubleqp", do_doubleqp, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
{ "version", do_version, POS_DEAD, 0, 1, LOG_NORMAL, 1, 1 },
    /*    
     * OLC   
     */
    { "edit",     do_olc,   POS_DEAD,   L5, 1,  LOG_NORMAL, 1,0 },
    { "asave",  do_asave,   POS_DEAD,   IM, 1,  LOG_NORMAL, 1,0 },    
    {"alist",  do_alist,    POS_DEAD,   IM, 1,  LOG_NORMAL, 1,0 },    
    {"resets", do_resets,   POS_DEAD,   IM, 1,  LOG_NORMAL, 1,0 },    
    {"redit",  do_redit,    POS_DEAD,   IM, 1,   LOG_NORMAL, 1,0 },    
    {"medit",  do_medit,    POS_DEAD,   IM, 1,   LOG_NORMAL, 1,0 },    
    {"aedit",  do_aedit,    POS_DEAD,   IM, 1,  LOG_NORMAL, 1,0 },    
    {"oedit",  do_oedit,    POS_DEAD,   IM, 1,  LOG_NORMAL, 1,0 },    
    {"mpedit",  do_mpedit,  POS_DEAD,   IM, 1,  LOG_NORMAL, 1,0 },
       { "hedit",       do_hedit,   POS_DEAD,    L5, 1, LOG_NORMAL, 1,0 },
    {"sedit",   do_sedit,   POS_DEAD,   L3, 1, LOG_NORMAL,  1, 0 },
    /*
     * Commands needed last on list
     */
    { "restring",   do_restring,    POS_STANDING,    0,  1,  LOG_NORMAL, 1,5 },
    { "delet",      do_delet,   POS_DEAD,    0,  1,  LOG_NORMAL, 0,5 },
    { "delete",     do_delete,  POS_STANDING,    0,  1,  LOG_NORMAL, 1,5 },
    { "remo",       do_rerol,   POS_DEAD,    0,  2,  LOG_NORMAL, 0,5 },
    { "remort",     do_reroll,  POS_STANDING,    0,  2,  LOG_NORMAL, 1,5 },
    { "lore",       do_lore,    POS_RESTING, 0, 1, LOG_NORMAL, 1, 1 },
   /*
    * New Commmands
    */
    { "wiziname",  do_wiziname,  POS_DEAD, IM, 1, LOG_NORMAL, 1, 0 },

    /*
     * End of list.
     */
    { "",       0,      POS_DEAD,    0,  1,  LOG_NORMAL, 0 }
};




/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    bool found;

    /*
     * Placed here, so we don't have to worry about tildes, period.
     * RW
     */
    smash_tilde( argument );

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
    argument++;
    if ( argument[0] == '\0' )
    return;

    /*
     * No hiding.
     */
    REMOVE_BIT( ch->affected_by, AFF_HIDE );

    /*
     * Fix for kyndig timed events. Basically if you had a command that 
     * took a long time to complete and the (ch) was lucky enough to get
     * a command like quit to work, then the game would actually dupe 
     * thier descriptor in a sense when they logged back in then all hell 
     * would break out.
     * Not by any means fool proof but gets the job done.
     */

    if ( !IS_NPC(ch) && IS_SET(ch->comm,COMM_NOCOMMANDS) )
    {
    stc("You can't do anything yet.\n\r",ch);
    return;
    }


   /*
    * To unset the AFK command.
    */
    if ( !IS_NPC(ch) && IS_SET(ch->comm,COMM_AFK) )
       {
       stc("You are no longer in AFK status.\n\r",ch);
       REMOVE_BIT(ch->comm,COMM_AFK);
       }


    /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE) )
    {
    send_to_char( "You're totally frozen!\n\r", ch );
    return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
    command[0] = argument[0];
    command[1] = '\0';
    argument++;
    while ( isspace(*argument) )
        argument++;
    }
    else
    {
    argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
    if ( command[0] == cmd_table[cmd].name[0]
    &&   !str_prefix( command, cmd_table[cmd].name )
    &&   cmd_table[cmd].level <= trust )
    {
        if (cmd_table[cmd].tier == 1)
        {
        found = TRUE;
        break;
        } else if (ch->class >= MAX_CLASS/2)
        {
        found = TRUE;
        break;
        } else if (ch->level >= LEVEL_HERO)
        {
        found = TRUE;
        break;
        }
    }
    }

    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
    strcpy( logline, "" );

    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
    ||   fLogAll
    ||   (cmd_table[cmd].log == LOG_ALWAYS
    &&   ch->level != MAX_LEVEL ) )
    {
    sprintf( log_buf, "Log %s: %s", ch->name, logline );
    wiznet(log_buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
    log_string( log_buf );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
    write_to_buffer( ch->desc->snoop_by, "% ",    2 );
    write_to_buffer( ch->desc->snoop_by, logline, 0 );
    write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if ( !found )
    {
    /*
     * Look for command in socials table.
     */
    if ( !check_social( ch, command, argument ) )
        send_to_char( "Huh?\n\r", ch );
    return;
    }

    /*
     * Character not in position for command?
    */
    if ( ch->position < cmd_table[cmd].position )
    {
        switch( ch->position )
        {
        case POS_DEAD:
            send_to_char( "Lie still; you are DEAD.\n\r", ch );
            break;
            
        case POS_MORTAL:
        case POS_INCAP:
            send_to_char( "You are hurt far too bad for that.\n\r", ch );
            break;
            
        case POS_STUNNED:
            send_to_char( "You are too stunned to do that.\n\r", ch );
            break;
            
        case POS_SLEEPING:
            send_to_char( "In your dreams, or what?\n\r", ch );
            break;
            
        case POS_RESTING:
            send_to_char( "Nah... You feel too relaxed...\n\r", ch);
            break;
            
        case POS_SITTING:
            send_to_char( "Better stand up first.\n\r",ch);
            break;
            
        case POS_FIGHTING:
            send_to_char( "No way!  You are still fighting!\n\r", ch);
            break;
            
        }
        return;
    }

    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun) ( ch, argument );

    tail_chain( );
    return;
}



bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;

    found  = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
    if ( command[0] == social_table[cmd].name[0]
    &&   !str_prefix( command, social_table[cmd].name ) )
    {
        found = TRUE;
        break;
    }
    }

    if ( !found )
    return FALSE;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
    send_to_char( "You are anti-social!\n\r", ch );
    return TRUE;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
    send_to_char( "Lie still; you are DEAD.\n\r", ch );
    return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
    send_to_char( "You are hurt far too bad for that.\n\r", ch );
    return TRUE;

    case POS_STUNNED:
    send_to_char( "You are too stunned to do that.\n\r", ch );
    return TRUE;

    case POS_SLEEPING:
    /*
     * I just know this is the path to a 12" 'if' statement.  :(
     * But two players asked for it already!  -- Furey
     */
    if ( !str_cmp( social_table[cmd].name, "snore" ) )
        break;
    send_to_char( "In your dreams, or what?\n\r", ch );
    return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
    act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
    act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    }
    else if ( victim == ch )
    {
    act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM    );
    act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
    }
    else
    {
    act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );
    act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
    act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    );

    if ( !IS_NPC(ch) && IS_NPC(victim)
    &&   !IS_AFFECTED(victim, AFF_CHARM)
    &&   IS_AWAKE(victim) 
    &&   victim->desc == NULL)
    {
        switch ( number_bits( 4 ) )
        {
        case 0:

        case 1: case 2: case 3: case 4:
        case 5: case 6: case 7: case 8:
        act( social_table[cmd].others_found,
            victim, NULL, ch, TO_NOTVICT );
        act( social_table[cmd].char_found,
            victim, NULL, ch, TO_CHAR    );
        act( social_table[cmd].vict_found,
            victim, NULL, ch, TO_VICT    );
        break;

        case 9: case 10: case 11: case 12:
        act( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
        act( "You slap $N.",  victim, NULL, ch, TO_CHAR    );
        act( "$n slaps you.", victim, NULL, ch, TO_VICT    );
        break;
        }
    }
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number ( char *arg )
{
 
    if ( *arg == '\0' )
        return FALSE;
 
    if ( *arg == '+' || *arg == '-' )
        arg++;
 
    for ( ; *arg != '\0'; arg++ )
    {
        if ( !isdigit( *arg ) )
            return FALSE;
    }
 
    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;
    
    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
    if ( *pdot == '.' )
    {
        *pdot = '\0';
        number = atoi( argument );
        *pdot = '.';
        strcpy( arg, pdot+1 );
        return number;
    }
    }

    strcpy( arg, argument );
    return 1;
}

/* 
 * Given a string like 14*foo, return 14 and 'foo'
*/
int mult_argument(char *argument, char *arg)
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
        if ( *pdot == '*' )
        {
            *pdot = '\0';
            number = atoi( argument );
            *pdot = '*';
            strcpy( arg, pdot+1 );
            return number;
        }
    }
 
    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
    argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
    cEnd = *argument++;

    while ( *argument != '\0' )
    {
    if ( *argument == cEnd )
    {
        argument++;
        break;
    }
    *arg_first = LOWER(*argument);
    arg_first++;
    argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
    argument++;

    return argument;
}

void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
    int category;
 
    col = 0;

 /* I had to categorize the immortal commands, but not print them to mortals, 
  * so immortal commands are category 0, and I started the loop with category
  * 1 commands. There's probably a better way to do this, but hey, it works!
  * I also have the commands that I felt were most important in the higher 
  * categories, which print out last. The more useless commands are in lower 
  * categories so they might scroll off the screen.
  */
    for( category = 1; category < 8; category++ )
    {
      switch(category)
      {
      case 1:
          send_to_char("{W*** {MConfiguration Commands {W***{x\n\r", ch);      
          break;                            
      case 2:                           
          send_to_char("{W*** {YCommon Commands {W***{x\n\r", ch);       
          break;                                 
      case 3:
          send_to_char("{W*** {GCommunication Commands {W***{x\n\r", ch);
          break;
      case 4:
          send_to_char("{W*** {BInformational Commands {W***{x\n\r", ch);
          break;
      case 5:
          send_to_char("{W*** {CObject Manipulation Commands {W***{x\n\r", ch);
          break;
      case 6:
          send_to_char("{W*** Movement Commands ***{x\n\r", ch);
          break;
      case 7:
          send_to_char("{W*** {YCombat Commands {W***{x\n\r", ch);
          break;
      }
       for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
       {
           if ( cmd_table[cmd].level <  LEVEL_HERO
           &&   cmd_table[cmd].level <= get_trust( ch ) 
       &&   cmd_table[cmd].show
       &&   cmd_table[cmd].cat == category) /* <--- this ensures the category equals the current category loop value */
       {
           sprintf( buf, "   %-12s", cmd_table[cmd].name );
           send_to_char( buf, ch );
           if ( ++col % 5 == 0 )
          send_to_char( "\n\r", ch );
       }
       }
       if (col % 5 != 0 )
       {
       send_to_char( "\n\r", ch );
       col = 0;
       }
    }
 
    if ( col % 5 != 0 )
    send_to_char( "\n\r", ch );
    return;
}

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
    int clevel;
    col = 0;

    for( clevel = LEVEL_HERO + 1; clevel < MAX_LEVEL + 1; clevel++ ) 
    {
        for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    { 
            if ( cmd_table[cmd].level >= LEVEL_HERO
            &&   cmd_table[cmd].level <= get_trust( ch ) 
            &&   cmd_table[cmd].show
        &&   cmd_table[cmd].level == clevel)
        {
            sprintf( buf, "{c[{x%-3d{c] %-12s{x", cmd_table[cmd].level, cmd_table[cmd].name );
            send_to_char( buf, ch );
            if ( ++col % 5 == 0 )
            send_to_char( "\n\r", ch );
        }
    }
    }
 
    if ( col % 5 != 0 )
    send_to_char( "\n\r", ch );
    return;
}

/* check the syntax of the speewalk command */
bool check_speedwalk( char *s )
{
   bool found_char = FALSE;
   while ( *s )
   {
      if (   !strchr( "nsewudNSEWUD ", *s )
          && !isdigit( *s ) )
         return FALSE;

      if ( !isdigit( *s ) )
         found_char = TRUE;
      s++;
   }
   return found_char;
}




