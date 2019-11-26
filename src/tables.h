
/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
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







/* HOMETOWN STUFF */

struct hometown_bank
{
       int money;
};

struct  hometown_type

{

        char    *name;

        int     recall;

        int     school;
        
        char    *who_name;
  
        int     death;
 
        int     donation;
};




/* game tables */


struct clan_type
{
    char 	*name;
    char 	*who_name;
    sh_int 	hall;
    sh_int 	entrance;
    sh_int 	pit;
    bool	independent; /* true for loners */
    bool	pkill; /* true for pkilling clans */
    char	*exname;
};




extern	const	struct	clan_type	clan_table[MAX_CLAN];


struct position_type
{
    char *name;
    char *short_name;
};

extern	const	struct	position_type	position_table[];

struct sex_type
{
    char *name;
};

extern	const	struct	sex_type	sex_table[];



struct size_type
{
    char *name;
};
extern	const	struct	size_type	size_table[];

/* flag tables */


struct flag_type
{
    char *name;
    int bit;
    bool settable;
};

extern	const	struct	flag_type	act_flags[];
extern  const   struct  flag_type       act2_flags[];
extern	const	struct	flag_type	plr_flags[];
extern  const   struct  flag_type       plr2_flags[];
extern	const	struct	flag_type	affect_flags[];
extern	const	struct	flag_type	shield_flags[];
extern	const	struct	flag_type	off_flags[];
extern	const	struct	flag_type	imm_flags[];
extern	const	struct	flag_type	form_flags[];
extern	const	struct	flag_type	part_flags[];
extern	const	struct	flag_type	comm_flags[];
extern	const	struct	flag_type	item_extra[];
extern	const	struct	flag_type	item_wear[];
extern	const	struct	flag_type	item_weapon[];
extern	const	struct	flag_type	extra_flags[];
extern	const	struct	flag_type	wear_flags[];
extern	const	struct	flag_type	weapon_flags[];
extern	const	struct	flag_type	container_flags[];
extern	const	struct	flag_type	portal_flags[];
extern	const	struct	flag_type	room_flags[];
extern	const	struct	flag_type	area_room[];
extern	const	struct	flag_type	exit_flags[];
extern 	const	struct  flag_type	mprog_flags[];
/* bit.c */

extern const struct flag_type   area_flags[];
extern const struct flag_type   sex_flags[];
extern const struct flag_type   exit_flags[];
extern const struct flag_type   door_resets[];
extern const struct flag_type   room_flags[];
extern const struct flag_type   sector_flags[];
extern const struct flag_type   affect_room_flags[];
extern const struct flag_type   type_flags[];
extern const struct flag_type   extra_flags[];
extern const struct flag_type   wear_flags[];
extern const struct flag_type   act_flags[];
extern const struct flag_type   act2_flags[];
extern const struct flag_type   affect_flags[];
extern const struct flag_type   apply_flags[];
extern const struct flag_type   wear_loc_strings[];
extern const struct flag_type   wear_loc_flags[];
extern const struct flag_type   container_flags[];

/* ROM OLC: */

extern const struct flag_type   form_flags[];
extern const struct flag_type   part_flags[];
extern const struct flag_type   ac_type[];
extern const struct flag_type   size_flags[];
extern const struct flag_type   off_flags[];
extern const struct flag_type   imm_flags[];
extern const struct flag_type   res_flags[];
extern const struct flag_type   vuln_flags[];
extern const struct flag_type   position_flags[];
extern const struct flag_type   weapon_class[];
extern const struct flag_type   weapon_type2[];
extern const struct flag_type   furniture_flags[];
extern  const   struct  flag_type   apply_types[];
extern const  struct  flag_type    can_eat_flags[];



extern struct  hometown_type   hometown_table  [MAX_HOMETOWN];

int     get_hometown    args( ( char *argument ) );




struct city_titles
{
    char *rank;
    int extra;
};

extern  const   struct  city_titles     city_rank_table[];


