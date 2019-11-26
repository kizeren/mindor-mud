/**************************************************************************
*      Mindor 1.0 is copyright 2002-2004 by Shaun Mcbride                *
*      Portions of the code were inspired by other works                 *
*      found around the internet.                                        *
*                                                                        *
*      Please follow all previous copyrights and licenses.               *
*      Another snippet I found on the internet.                          *
*      As of 2004 it still doesn't function.                             *
*      Feel free to obtain original snippet and retry                    *
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
#include "recycle.h"
#include "interp.h"
#include "tables.h"

/* Global calls */
RESET_DATA *new_reset_data args ( ( void ) );
SHOP_DATA	*new_shop		args ( ( void ) );
void add_reset args ( ( ROOM_INDEX_DATA *room, RESET_DATA *shop_reset, int index ) );
void free_reset_data args ( ( RESET_DATA *pReset ) );
/* Local calls */
void fwrite_shops args( ( CHAR_DATA *ch, CHAR_DATA *shop_mob ) );
void fread_shops  args( ( CHAR_DATA *ch ) );
void shop_buy      args( ( CHAR_DATA *ch, char *argument ) );
void shop_open     args( ( CHAR_DATA *ch ) );
void shop_close    args( ( CHAR_DATA *ch, char *argument ) );
void shop_describe args( ( CHAR_DATA *ch ) );
void shop_provide  args( ( CHAR_DATA *ch, char *argument, int amount ) );
void shop_collect  args( ( CHAR_DATA *ch ) );
void shop_update   args( ( CHAR_DATA *mob ) );
void    fread_obj   args( ( CHAR_DATA *ch,  FILE *fp ) );
int shop_cost = 50000;
ROOM_INDEX_DATA *shop;
AREA_DATA *shop_area;
RESET_DATA *shop_reset;

void do_shop( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
        char buf[MSL];
	shop = ch->in_room;
	shop_area = ch->in_room->area;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( IS_NPC(ch)
	|| ch == NULL
	|| ch->desc == NULL )
		return;

	if ( arg1[0] == '\0'
	|| !str_prefix ( arg1, "help" )
	|| !str_prefix ( arg1, "list" ) )
	{
		stc("\n\r{r-----------------------------------------------------------{x\n\r",ch);
		stc("{wShop Buy{W:      {r<{Rshop buy (confirm){r>{x\n\r",ch);
		stc("               {wPurchase a shop and place it in the current room.{x\n\r",ch);
		sprintf(buf,"               {wCost is %d gold.{x\n\r",shop_cost);
                stc(buf,ch);
		stc("{wShop Open{W:     {r<{Rshop open{r>{x\n\r",ch);
		stc("               {wIf the MUD crashes, this command will reopen your shop.{x\n\r",ch);
		stc("{wShop Close{W:    {r<{Rshop close (confirm){r>{x\n\r",ch);
		stc("               {wSell the current shop for half of the purchase{x\n\r",ch);
		stc("               {wcost. Will release the shop mobile and all items{x\n\r",ch);
		stc("               {win the mobile's inventory.{x\n\r",ch);
		stc("{wShop Describe{W: {r<{Rshop describe{r>{x\n\r",ch);
		stc("               {wWhile standing in the shop, describe the shop using{x\n\r",ch);
		stc("               {wthe OLC editor. Careful thought should go into the{x\n\r",ch);
		stc("               {wdescription.{x\n\r",ch);
		stc("{wShop Provide{W:  {r<{Rshop provide item_name amount{r>{x\n\r",ch);
		stc("               {wProvide the shop mobile with item_name. The item must{x\n\r",ch);
		stc("               {whave a positive net worth and be held in the inventory.{x\n\r",ch);
		stc("               {wThe amount must be a positive numeric value greater than{x\n\r",ch);
		stc("               {w0 and less than 32000.{x\n\r",ch);
		stc("{wShop Collect{W:  {r<{Rshop collect{r>{x\n\r",ch);
		stc("               {wCollect monies from the shop mobile. The mobile will always{x\n\r",ch);
		stc("               {wretain at least 100 silver coins for making change and to{x\n\r",ch);
		stc("               {wcover basic business needs.{x\n\r",ch);
		stc("{wShop Save{W: {r<{Rshop save{r>{x\n\r",ch);
		stc("               {wForces the data to be written to file. Testing only.{x\n\r",ch);
		stc("{r-----------------------------------------------------------{x\n\r",ch);
		return;
	}

	shop = ch->in_room;
	shop_area = ch->in_room->area;

	if ( shop == NULL || shop_area == NULL )
		return;

	if ( !str_prefix( arg1, "save" ) )
	{
		/*fwrite_shops(ch, arg2);*/
	}
	else if ( !str_prefix( arg1, "buy" ) )
	{
		shop_buy(ch, arg2);
	}
	else if ( !str_prefix( arg1, "open" ) )
	{
		/*shop_open(ch);*/
                stc("This feature is still underconstruction.",ch);
                return;
	}
	else if ( !str_prefix( arg1, "close" )
	|| !str_prefix( arg1, "sell" ) )
	{
		shop_close(ch, arg2);
	}
	else if ( !str_prefix( arg1, "describe" ) )
	{
		shop_describe(ch);
	}
	else if ( !str_prefix( arg1, "provide" ) )
	{
		shop_provide(ch, arg2, atoi(arg3));
	}
	else if ( !str_prefix( arg1, "collect" ) )
	{
		shop_collect(ch);
	}
	else if ( !str_prefix( arg1, "test" ) )
	{
		sprintf(buf, "ShopV: %d ShopM: %d\n\r", ch->pcdata->shop_vnum, ch->pcdata->shopmob_vnum );
                stc(buf,ch);
		return;
	}
	else if ( !str_prefix( arg1, "reset" ) )
	{
		ch->in_room->owner = NULL;
		ch->pcdata->shop_vnum = 0;
		ch->pcdata->shopmob_vnum = 0;
		return;
	}
	else
		do_function( ch, &do_shop, "" );
	return;
}
CHAR_DATA * make_shop_mob( CHAR_DATA *ch )
{
	CHAR_DATA *shop_mob;
	int i;

	shop_mob = create_mobile( get_mob_index( shop->vnum ) );
        shop_mob->pIndexData->vnum   = ch->in_room->vnum;
    shop_mob->group	            = 0;
	/* sex */
   	shop_mob->sex		        = ch->sex;
   	shop_mob->pIndexData->sex		        = ch->sex;
	/* race */
    shop_mob->race		        = ch->race;
    shop_mob->pIndexData->race		        = ch->race;
	/* armor class */
    for (i = 0; i < 4; i++)
   		shop_mob->armor[i]		= 1;
    for (i = 0; i < 4; i++)
   		shop_mob->pIndexData->ac[i]		= 1;
	/* damage dice */
    shop_mob->damage[DICE_NUMBER]=51;
   	shop_mob->damage[DICE_TYPE]  = 51;
    shop_mob->damage[DICE_BONUS] = 51;
    shop_mob->pIndexData->damage[DICE_NUMBER]=51;
   	shop_mob->pIndexData->damage[DICE_TYPE]  = 51;
    shop_mob->pIndexData->damage[DICE_BONUS] = 51;
    shop_mob->damroll	        = 100;
	/* level */
   	shop_mob->level	            = 51;
   	shop_mob->pIndexData->level	            = 51;
	/* hitroll */
   	shop_mob->hit		        = 500;
   	shop_mob->pIndexData->hitroll	        = 500;
	/* hit points */
    shop_mob->pIndexData->hit[DICE_NUMBER] = 1;
    shop_mob->pIndexData->hit[DICE_TYPE]   = 499;
    shop_mob->pIndexData->hit[DICE_BONUS]  = 1;
    shop_mob->max_hit	        = 500;
	/* mana points */
   	shop_mob->mana		        = 500;
    shop_mob->pIndexData->mana[DICE_NUMBER] = 1;
    shop_mob->pIndexData->mana[DICE_TYPE]   = 499;
    shop_mob->pIndexData->mana[DICE_BONUS]  = 1;
    shop_mob->max_mana	        = 500;
	/* movement points */
   	shop_mob->move		        = 500;
    shop_mob->max_move	        = 500;
	/* wealth - THIS NEEDS TO BE READ FROM PFILE OR OTHER EXTERNAL FILE!!! */
   	shop_mob->gold	    	    = 1;
    shop_mob->silver	        = 1;
   	shop_mob->pIndexData->wealth	    	    = 1;
   	shop_mob->imm_flags  	    = 0;
    shop_mob->res_flags  	    = 0;
   	shop_mob->vuln_flags   	    = 0;
   	shop_mob->pIndexData->imm_flags  	    = 0;
    shop_mob->pIndexData->res_flags  	    = 0;
   	shop_mob->pIndexData->vuln_flags   	    = 0;
   	shop_mob->act               += ACT_SENTINEL;

   	shop_mob->pIndexData->act               += ACT_SENTINEL;

    shop_mob->position	        = POS_STANDING;
   	shop_mob->start_pos	        = POS_STANDING;
    shop_mob->pIndexData->default_pos        = POS_STANDING;
   	shop_mob->pIndexData->start_pos	        = POS_STANDING;
   	shop_mob->saving_throw	    = 0;
    shop_mob->alignment	        = 0;
    shop_mob->pIndexData->alignment	        = 0;
   	shop_mob->form		        = race_table[shop_mob->race].form;
   	shop_mob->pIndexData->form		        = race_table[shop_mob->race].form;
    shop_mob->parts	            = race_table[shop_mob->race].parts;
    shop_mob->pIndexData->parts	            = race_table[shop_mob->race].parts;
   	shop_mob->size		        = SIZE_MEDIUM;
   	shop_mob->pIndexData->size		        = SIZE_MEDIUM;
	shop_mob->dam_type	        = attack_lookup("pound");
	shop_mob->pIndexData->dam_type	        = attack_lookup("pound");

	free_string( shop_mob->name );
    shop_mob->name              = str_dup( "shop mobile shopkeeper" );
	free_string( shop_mob->name );
    shop_mob->pIndexData->player_name              = str_dup( "shop mobile shopkeeper" );

    free_string( shop_mob->short_descr );
    shop_mob->short_descr       = str_dup( "The shopkeeper" );
    free_string( shop_mob->short_descr );
    shop_mob->pIndexData->short_descr       = str_dup( "The shopkeeper" );

    free_string( shop_mob->long_descr );
    shop_mob->long_descr        = str_dup( "The shopkeeper stands patiently nearby.\n\r" );
    free_string( shop_mob->long_descr );
    shop_mob->pIndexData->long_descr        = str_dup( "The shopkeeper stands patiently nearby.\n\r" );

   	if ( !shop_mob->pIndexData->pShop )
	{

		shop_mob->pIndexData->pShop		= new_shop();
		if ( !shop_first )
        	shop_first	= shop_mob->pIndexData->pShop;
		if ( shop_last )
			shop_last->next	= shop_mob->pIndexData->pShop;
		shop_last		= shop_mob->pIndexData->pShop;
		shop_mob->pIndexData->pShop->keeper	= shop_mob->pIndexData->vnum;
		shop_mob->pIndexData->pShop->open_hour = 0;
		shop_mob->pIndexData->pShop->close_hour = 24;
		shop_mob->pIndexData->pShop->profit_buy  = 100;
		shop_mob->pIndexData->pShop->profit_sell = 100;
		/*shop_mob->pIndexData->pShop->buy_type = 0;*/
	}

	shop_reset               = new_reset_data();
	shop_reset->command      = 'M';
	shop_reset->arg1         = shop_mob->pIndexData->vnum;
	shop_reset->arg2         = 1;
	shop_reset->arg3         = shop->vnum;
	shop_reset->arg4         = 1;
	add_reset( shop, shop_reset, 0 );

    ch->pcdata->shopmob_vnum = shop_mob->pIndexData->vnum;
    ch->pcdata->shop_vnum = shop->vnum;

	char_to_room( shop_mob, shop );
    return shop_mob;
}

void shop_open ( CHAR_DATA *ch )
{
	/* reopen a shop in case of MUD crash.
	 * if the MUD never crashes, this is not
	 * necessary, but the option is here just
	 * in case.
   * This is just a buffer command for now.
   * Later it should perform preemptive error
   * checking before read.
	 */
	if ( !IS_SET( shop->room_flags,ROOM_HOME ) )
	{
		stc("But this is not a shop!\n\r",ch);
		return;
	}
/*
  if ( ch->pcdata->shop_vnum == 0 || ch->pcdata->shop_vnum == 0 )
	{
		stc("But you do not own a shop!\n\r",ch);
		return;
	}
*/
  if ( !is_room_owner ( ch, shop ) )
	{
		stc("But you do not own this shop!\n\r",ch);
		return;
	}
  fread_shops( ch );
  return;
}
void shop_buy ( CHAR_DATA *ch, char *argument )
{
	/* create shop from blank room.
	 * create blank mobile like faire mobs and place into resets
	 * Each mobile should be unique to the room, ie. 100 rooms
	 * equals 100 different mobs.
	 * assign ownership of room and mob to pfile
	 * flag room as safe
	 * flag area as changed
	 */

	char buf[MSL];
	buf[0] = '\0';

	if ( !IS_SET ( shop->room_flags, ROOM_HOME ) )
	{
		stc("This isn't a shop.\n\r",ch);
		return;
	}
	/* Shops are free during testing
	if ( ch->gold < 50000 )
	{
		stc("You cant afford it.\n\r",ch);
		return;
	}*/
	if ( ch->pcdata->shop_vnum != 0 )
	{
		stc("You already have a shop.\n\r",ch);
		return;
	}
	if ( str_cmp( argument, "confirm" ) )
	{
		stc("Confirm expenditure of 50,000 gold?\n\r",ch);
		stc("Type Shop Buy Confirm\n\r",ch);
		return;
	}


  ch->pcdata->shop_vnum = ch->in_room->vnum;
  ch->pcdata->shopmob_vnum = ch->in_room->vnum;

    free_string ( shop->owner );
    shop->owner = str_dup ( ch->name );
    /* Shops are free during testing */
    /*ch->gold -= 50000;*/
    free_string ( shop->name );
    sprintf ( buf, "%s's Shop", ch->name );
    shop->name = str_dup ( buf );

	/* flag the room safe it its not */
    if ( !IS_SET(shop->room_flags,ROOM_SAFE) )
    	SET_BIT( shop->room_flags,ROOM_SAFE );

    make_shop_mob( ch ); 
    stc("Congratulations on purchasing your new shop!\n\r",ch);
    stc("You are now 50,000 gold coins lighter and the owner\n\r",ch);
    stc("of the room in which you now stand! Be sure to describe\n\r",ch);
    stc("your shop and provide merchandise for it.\n\r",ch);
	return;
}

extern int unlink( );

void shop_close ( CHAR_DATA *ch, char *argument )
{
	/* check for comfirmation
	 * remove mobile and items
	 * credit the char for half of item value
	 * strip resets
	 * remove room and mobile vnums from pfile
	 * flag area as changed
	 */
	char shop_file[MIL];
	RESET_DATA *current_reset;
	RESET_DATA *next_reset;
  CHAR_DATA *shop_mob;
	sprintf( shop_file, "%s%s", SHOPS_DIR, capitalize( ch->name ) );

	if ( !is_room_owner ( ch, shop ) )
	{
		stc("But you do not own this shop!\n\r",ch);
		return;
	}
	if ( argument[0] == '\0' )
	{
		stc("This command allows you to sell your shop.\n\r",ch);
		stc("You will no longer own your shop once it is sold,\n\r",ch);
		stc("and you will be compensated half the cost of the shop.\n\r",ch);
		stc("You must be standing in the shop which you own.\n\r",ch);
		stc("\n\rSyntax: Shop (close) (confirm)\n\r",ch);
		return;
	}
	else if ( !strcmp ( argument, "confirm" ) )
	{
	    free_string ( shop->owner );
	    shop->owner = str_dup ( "" );
	    /* Shops are free during testing */
	    /*ch->gold += 25000;*/

	    ch->pcdata->shopmob_vnum = 0;
	    ch->pcdata->shop_vnum = 0;
	    free_string ( shop->name );
	    shop->name = str_dup ( "An Empty Building" );
	    free_string ( shop->description );
	    shop->description = str_dup ( "" );

	    /* remove safe flags if set */
	    if ( IS_SET(shop->room_flags,ROOM_SAFE) )
	    	REMOVE_BIT( shop->room_flags,ROOM_SAFE );

	    if ( !ch->in_room->reset_first )
	    {
			return;
		}
		for( current_reset = ch->in_room->reset_first; current_reset;
		    current_reset = next_reset)
		{
		   next_reset = current_reset->next;
		   free_reset_data(current_reset);
		}

		ch->in_room->reset_first = NULL;

    for ( shop_mob = ch->in_room->people; shop_mob != NULL; shop_mob = shop_mob->next_in_room )
    {
      if ( IS_NPC( shop_mob )
      && shop_mob->pIndexData
      && shop_mob->pIndexData->vnum == ch->pcdata->shopmob_vnum )
      {
        extract_char( shop_mob, TRUE );
        break;
      }
    }
    
    ch->pcdata->shop_vnum = 0;
    ch->pcdata->shopmob_vnum = 0;

		stc("Your shop has been sold and you are now 25,000 gold coins richer!\n\r",ch);
        SET_BIT(shop_area->area_flags, AREA_CHANGED);

	/* unlink the data file from the shops directory */




	unlink( shop_file );

        return;
	}
	else
	{
		stc("This command allows you to sell your shop.\n\r",ch);
		stc("You will no longer own your shop once it is sold,\n\r",ch);
		stc("and you will be compensated half the cost of the shop.\n\r",ch);
		stc("You must be standing in the shop which you own.\n\r",ch);
		stc("\n\rSyntax: Shop (close) (confirm)\n\r",ch);
		return;
	}
	return;
}
void shop_describe ( CHAR_DATA *ch )
{
	/* show help on using the editor
	 * place character into editor
	 * save room desc and flag as changed
	 */
	if ( !is_room_owner ( ch, shop ) )
	{
		stc("But you do not own this shop!\n\r",ch);
		return;
	}
	else
	{
		stc("This command allows you to describe your shop.\n\r",ch);
		stc("If you currently own this shop, you will be placed into.\n\r",ch);
		stc("the OLC editor. Be warned that while in the OLC editor,\n\r",ch);
		stc("you are only allowed to type the description. If you are\n\r",ch);
		stc("unsure or hesitant about this, please note the Staff,\n\r",ch);
		stc("or better, discuss the how-to's with a Builder.\n\r",ch);
		stc("Syntax: Shop (describe)\n\r",ch);
    	string_append( ch, &shop->description );
        SET_BIT(shop_area->area_flags, AREA_CHANGED);
		return;
	}
	return;
}
void shop_provide ( CHAR_DATA *ch, char *argument, int amount )
{
	/* check for valid item
	 * check for valid mobile
	 * fix item cost according to amount
	 * add item to resets
	 * flag area as changed
	 * inform next copyover item will be available
	 */
	CHAR_DATA *shop_mob;
	OBJ_DATA *item;
         char buf[MSL];
	if ( argument[0] == '\0' || amount <= 0 || amount > 32000 )
	{
		stc("{wShop Provide{W:  {r<{Rshop provide item_name amount{r>{x\n\r",ch);
		stc("               {wProvide the shop mobile with item_name. The item must{x\n\r",ch);
		stc("               {whave a positive net worth and be held in the inventory.{x\n\r",ch);
		stc("               {wThe amount must be a positive numeric value greater than{x\n\r",ch);
		stc("               {w0 and less than 32000.{x\n\r",ch);
		return;
	}
    if ( ( item = get_obj_carry( ch, argument ) ) == NULL )
    {
		stc( "Check your pockets: You do not have that item.\n\r", ch );
		return;
    }
    /* do some error checking against bad merchandise */
    if ( item->item_type == ITEM_LIGHT && item->value[0] <= 0 )
    {
		stc( "That light will be useless soon and wouldn't make good merchandise.\n\r", ch );
		return;
    }
    if ( item->item_type == ITEM_TRASH )
    {
		stc( "Why would anyone want to buy trash?\n\r", ch );
		return;
    }
    if ( item->item_type == ITEM_ROOM_KEY )
    {
		stc( "Selling room keys probably isn't good business.\n\r", ch );
		return;
    }


    if ( item->item_type == ITEM_CONTAINER )
    {
		stc( "Sorry, you cant sell containers this way.\n\r", ch );
		return;
    }
    if ( !CAN_WEAR(item, ITEM_TAKE) )
    {
		stc( "How do you propose to sell something that cant be picked up?\n\r", ch );
		return;
    }
    /* ok, that's enough checking */
    item->cost = amount;

	if ( !IS_SET( shop->room_flags,ROOM_HOME ) )
	{
		stc("But this is not a shop!\n\r",ch);
		return;
	}
	if ( ch->pcdata->shop_vnum == 0 || ch->pcdata->shop_vnum == 0 )
	{
		stc("But you do not own a shop!\n\r",ch);
		return;
	}
	if ( !is_room_owner ( ch, shop ) )
	{
		stc("But you do not own this shop!\n\r",ch);
		return;
	}
	for (shop_mob = ch->in_room->people; shop_mob != NULL; shop_mob = shop_mob->next_in_room )
	{
  	if ( shop_mob == NULL )
	  {
  		stc( "No shopkeeper found here!\n\r", ch );
	  	return;
  	}
		if ( IS_NPC(shop_mob) && shop_mob->pIndexData->vnum == ch->pcdata->shopmob_vnum )
			break;
	}
	sprintf(buf, "You confer with %s for a bit.\n\r", shop_mob->short_descr );
        send_to_char(buf,ch);
	act( "$n confers with $N for a bit.", ch,NULL,shop_mob->short_descr,TO_ROOM );
	obj_from_char( item);
	obj_to_char( item, shop_mob );
	fwrite_shops( ch, shop_mob );
	return;
}
void shop_collect ( CHAR_DATA *ch )
{
	/* check for valid mobile
	 * check for mobile wealth over 100
	 * deduct mob and add wealth over 100 to character
	 */
	if ( !is_room_owner ( ch, shop ) )
	{
		stc("But you do not own this shop!\n\r",ch);
		return;
	}
	return;
}
#define MAX_NEST    100
static  OBJ_DATA *  rgObjNest   [MAX_NEST];
void fread_shops( CHAR_DATA *ch )
{
	FILE *fp;
	char name[24];
  CHAR_DATA * shop_mob;
  bool shopMob = FALSE;
      char strsave[MIL];
   bool found;
  sprintf( name, "%s%s", SHOPS_DIR, capitalize( ch->name ) );

	if ( ch == NULL || ch->pcdata == NULL || ch->desc == NULL )
  {
		bug("NULL CH trying to write pc_shop data!", 0 );
		return;
  }

	if ( ch->pcdata->shop_vnum == 0 || ch->pcdata->shopmob_vnum == 0 )
	{
		stc("But you do not own a shop!\n\r",ch);
		return;
	}

  for ( shop_mob = ch->in_room->people; shop_mob != NULL; shop_mob = shop_mob->next_in_room )
  {
    if ( IS_NPC( shop_mob )
    && shop_mob->pIndexData
    && shop_mob->pIndexData->vnum == ch->pcdata->shopmob_vnum )
    {
      shopMob = TRUE;
      break;
    }
  }
  if ( !shopMob )
  {
    stc( "No shop mob found here.\n\r", ch );
    return;
  }

   if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
    int iNest;

    for ( iNest = 0; iNest < MAX_NEST; iNest++ )
        rgObjNest[iNest] = NULL;

    found = TRUE;
    for ( ; ; )
    {
        char letter;
        char *word;

        letter = fread_letter( fp );
        if ( letter == '*' )
        {
        fread_to_eol( fp );
        continue;
        }

        if ( letter != '#' )
        {
        bug( "Load_char_obj: # not found.", 0 );
        break;
        }

        word = fread_word( fp );

         if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( obj_to_room, fp );
        else if ( !str_cmp( word, "O"      ) ) fread_obj  ( obj_to_room, fp );

        else if ( !str_cmp( word, "END"    ) ) break;
        else
        {
        bug( "Load_char_obj: bad section.", 0 );
        break;
        }
    }
    fclose( fp );
    }
 
   return;

  
}


/*
 * Write an object and its contents.
 */
void fwrite_sobj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest )
{
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if ( obj->next_content != NULL )
    fwrite_sobj( ch, obj->next_content, fp, iNest );

    /*
     * Castrate storage characters.
     */
    if ( (ch->level < obj->level - 2 && obj->level > 19
    && (obj->item_type != ITEM_CONTAINER
    &&  obj->item_type != ITEM_PIT))
    ||   obj->item_type == ITEM_KEY
    ||   (obj->item_type == ITEM_MAP && !obj->value[0]))
    return;

    fprintf( fp, "#OBJECT\n" );
    fprintf( fp, "Vnum %d\n",   obj->pIndexData->vnum        );
    if (!obj->pIndexData->new_format)
    fprintf( fp, "Oldstyle\n");
    if (obj->enchanted)
    fprintf( fp,"Enchanted\n");
    fprintf( fp, "Nest %d\n",   iNest            );

    /* these data are only used if they do not match the defaults */

    if ( obj->name != obj->pIndexData->name)
        fprintf( fp, "Name %s~\n",  obj->name            );
    if ( obj->short_descr != obj->pIndexData->short_descr)
        fprintf( fp, "ShD  %s~\n",  obj->short_descr         );
    if ( obj->description != obj->pIndexData->description)
        fprintf( fp, "Desc %s~\n",  obj->description         );
    if ( obj->extra_flags != obj->pIndexData->extra_flags)
        fprintf( fp, "ExtF %d\n",   obj->extra_flags         );
    if ( obj->wear_flags != obj->pIndexData->wear_flags)
        fprintf( fp, "WeaF %d\n",   obj->wear_flags          );
    if ( obj->item_type != obj->pIndexData->item_type)
        fprintf( fp, "Ityp %d\n",   obj->item_type           );
    if ( obj->weight != obj->pIndexData->weight)
        fprintf( fp, "Wt   %d\n",   obj->weight          );
    if ( obj->condition != obj->pIndexData->condition)
    fprintf( fp, "Cond %d\n",   obj->condition           );

    /* variable data */

    fprintf( fp, "Wear %d\n",   obj->wear_loc                );
    if (obj->level != obj->pIndexData->level)
        fprintf( fp, "Lev  %d\n",   obj->level           );
    if (obj->timer != 0)
        fprintf( fp, "Time %d\n",   obj->timer       );
    fprintf( fp, "Cost %d\n",   obj->cost            );
    if (obj->value[0] != obj->pIndexData->value[0]
    ||  obj->value[1] != obj->pIndexData->value[1]
    ||  obj->value[2] != obj->pIndexData->value[2]
    ||  obj->value[3] != obj->pIndexData->value[3]
    ||  obj->value[4] != obj->pIndexData->value[4]) 
        fprintf( fp, "Val  %d %d %d %d %d\n",
        obj->value[0], obj->value[1], obj->value[2], obj->value[3],
        obj->value[4]        );

    switch ( obj->item_type )
    {
    case ITEM_POTION:
    case ITEM_SCROLL:
    if ( obj->value[1] > 0 )
    {
        fprintf( fp, "Spell 1 '%s'\n", 
        skill_table[obj->value[1]].name );
    }

    if ( obj->value[2] > 0 )
    {
        fprintf( fp, "Spell 2 '%s'\n", 
        skill_table[obj->value[2]].name );
    }

    if ( obj->value[3] > 0 )
    {
        fprintf( fp, "Spell 3 '%s'\n", 
        skill_table[obj->value[3]].name );
    }

    break;

    case ITEM_PILL:
    case ITEM_STAFF:
    case ITEM_WAND:
    if ( obj->value[3] > 0 )
    {
        fprintf( fp, "Spell 3 '%s'\n", 
        skill_table[obj->value[3]].name );
    }

    break;
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
    if (paf->type < 0 || paf->type >= MAX_SKILL)
        continue;
        fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
            skill_table[paf->type].name,
            paf->where,
            paf->level,
            paf->duration,
            paf->modifier,
            paf->location,
            paf->bitvector
            );
    }

    for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
    {
    fprintf( fp, "ExDe %s~ %s~\n",
        ed->keyword, ed->description );
    }

    fprintf( fp, "End\n\n" );


    return;
}



void fwrite_shops( CHAR_DATA *ch, CHAR_DATA *shop_mob )
{
	FILE *fp;
	char name[24];

    sprintf( name, "%s%s", SHOPS_DIR, capitalize( ch->name ) );

	if ( ch == NULL || ch->pcdata == NULL || ch->desc == NULL )
    {
		bug("NULL CH trying to write pc_shop data!", 0 );
		return;
    }
	if ( shop_mob == NULL )
    {
		bug("NULL SHOP_MOB trying to write pc_shop data!", 0 );
		return;
    }

    if ( ( fp = fopen( name, "w" ) ) == NULL )
    {
		bug("Could not open %s for write access!", name );
		return;
    }
    else
    {
/*      
	    fprintf( fp, "Char    %s~\n", ch->name);
	    fprintf( fp, "MVnum   %d\n", shop_mob->pIndexData->vnum);
*/
		if ( shop_mob->carrying != NULL )
		    fwrite_sobj( shop_mob, shop_mob->carrying, fp, 0 );
    fprintf(fp, "#END" );
		fclose( fp );
	   	return;
    }
	return;
}
