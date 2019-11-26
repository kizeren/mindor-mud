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
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"

DECLARE_DO_FUN( do_help );

/** Function: do_petname
  * Descr   : renames pets, also allows changing of short/long and 
  *           descritpion variables.
  * Returns : (void)
  * Syntax  : petname ([name|short|long|desc]) (argument)
  * Written : v1.1 1/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
void do_petname(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char command[MAX_INPUT_LENGTH];

  /* smash all tilde's right away. */
  smash_tilde(argument);

  if ( ch->pet == NULL )
  {
    send_to_char("You dont have a pet!\n\r", ch);
    return;
  }

  if ( ch->in_room != ch->pet->in_room )
  {
    send_to_char("Kinda hard for your pet to learn his new name\n\r",ch);
    send_to_char("if he's not even with you! *boggle*\n\r", ch);
    return;
  }

  argument = one_argument(argument, command);


  if ( command[0] == '\0' ||  argument[0] == '\0' )
  {
    send_to_char(
    "\n\rsyntax: petname [name|short|long|desc] <argument>\n\r",ch);
    send_to_char( "\n\r  example: \"petname name fido\n\r", ch);
    send_to_char( "            \"petname short is hungry!\"\n\r",ch);
    send_to_char("\n\rTry using color in the descriptions!\n\r\n\r", ch);
    send_to_char(
    "See \"help petname\" and \"help color\" for more information.\n\r",ch);
    return;
  } /* syntax */

  if ( !str_prefix(command, "name") )
  {

     ch->pet->name = "A pet"; 
  } /* name */
  else

  if ( !str_prefix(command, "short") )
  {
    if ( argument[0] == '\0' ) return;
    free_string(ch->pet->short_descr);

    /* short description should not have an "\n\r" appended to it. */
    ch->pet->short_descr = str_dup(argument);

    sprintf(buf, "%s's short description set to: \n\r%s\n\r",
            ch->pet->name, ch->pet->short_descr);

    send_to_char(buf, ch);
  } /* short */
  else

  if ( !str_prefix(command, "long") )
  {
    if ( argument[0] == '\0' ) return;
    free_string(ch->pet->long_descr);

    /* long descr needs "\n\r" appended to it. */
    sprintf(buf, "%s\n\r", argument);
    ch->pet->long_descr = str_dup(buf);

    sprintf(buf, "%s's long description set to: \n\r%s\n\r",
            ch->pet->name, ch->pet->long_descr);
    send_to_char(buf, ch);

  } /* long */
  else

  if ( !str_prefix(command, "description") )
  {
#if defined (OldDesc)
    do_description(ch, argument);
#else
    if ( argument[0] == '\0' )
    {
        string_append( ch, &ch->pet->description );
        return;
    }
#endif
  } /* desc */

  else
    do_help(ch, "petname");

  return;

} /* end of do_petname */


