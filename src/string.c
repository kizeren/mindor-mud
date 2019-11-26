/***************************************************************************
 *  File: string.c                                                         *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/


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

DECLARE_DO_FUN(do_immtalk);
DECLARE_DO_FUN(do_ooc);
DECLARE_DO_FUN(do_say);
DECLARE_DO_FUN(do_ooc);
DECLARE_DO_FUN(do_shout);

/*****************************************************************************
 Name:		string_edit
 Purpose:	Clears string and puts player into editing mode.
 Called by:	none
 ****************************************************************************/
/* ftp.c */
void string_edit( CHAR_DATA *ch, char **pString )
{
    send_to_char( "-========- Entering EDIT Mode -=========-\n\r", ch );
    send_to_char( "    Type .h on a new line for help\n\r", ch );
    send_to_char( " Terminate with a ~ or @ on a blank line.\n\r", ch );
    send_to_char( "-=======================================-\n\r", ch );

    if ( *pString == NULL )
    {
        *pString = str_dup( "" );
    }
    else
    {
        **pString = '\0';
    }

    ch->desc->pString = pString;

    return;
}


bool ftp_push(DESCRIPTOR_DATA *); /* ftp.c */
/*****************************************************************************
 Name:		string_append
 Purpose:	Puts player into append mode for given string.
 Called by:	(many)olc_act.c
 ****************************************************************************/
void string_append( CHAR_DATA *ch, char **pString )
{
    send_to_char( "-=======- Entering APPEND Mode -========-\n\r", ch );
    send_to_char( "    Type .h on a new line for help\n\r", ch );
    send_to_char( " Terminate with a ~ or @ on a blank line.\n\r", ch );
    send_to_char( "-=======================================-\n\r", ch );

    if ( *pString == NULL )
    {
        *pString = str_dup( "" );
    }
    if (IS_SET(ch->comm, COMM_MUDFTP))
    {
        ch->desc->pString = pString;
        if (ftp_push(ch->desc)) /* ftp: PUSH mode */
            send_to_char("Editing string via mudFTP push connection. Use @ to abort.\n\r",ch);
        else /* PULL mode */
        {
            char buf[MAX_STRING_LENGTH];
            sprintf(buf,"Sending mudFTP request. If your client does not support mudFTP, abort this\n\r"
                       "edit (type @ on a blank line), turn off mudFTP, and try again.\n\r"
                       "\ntmp/%lu%c\n", (unsigned long) pString, 230);
            send_to_char(buf,ch);
        }
    }
    else
    {
        send_to_char( *pString, ch );
         
        if ( *(*pString + strlen( *pString ) - 1) != '\r' )
            send_to_char( "\n\r", ch );    

    ch->desc->pString = pString;
}   
  return;
}



/*****************************************************************************
 Name:		string_replace
 Purpose:	Substitutes one string for another.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char * string_replace( char * orig, char * old, char * new )
{
    char xbuf[MAX_STRING_LENGTH];
    int i;

    xbuf[0] = '\0';
    strcpy( xbuf, orig );
    if ( strstr( orig, old ) != NULL )
    {
        i = strlen( orig ) - strlen( strstr( orig, old ) );
        xbuf[i] = '\0';
        strcat( xbuf, new );
        strcat( xbuf, &orig[i+strlen( old )] );
        free_string( orig );
    }

    return str_dup( xbuf );
}

/*****************************************************************************
 Name:		string_replace2
 Purpose:	Replaces a line of text.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char   *string_replace2 (CHAR_DATA * ch, char *orig, int line, char *new)
{
	char   *rdesc;
	char    xbuf[MAX_STRING_LENGTH];
	int     current_line = 1;
	int     i;
	bool    fReplaced = FALSE;

	xbuf[0] = '\0';
	strcpy (xbuf, orig);

	i = 0;

	for (rdesc = orig; *rdesc; rdesc++)
	{
		if (current_line == line && !fReplaced)
		{
			xbuf[i] = '\0';

			if (*new)
				strcat (xbuf, new);
			strcat (xbuf, "\n\r");
			fReplaced = TRUE;
		}

		if (current_line == line + 1)
		{
			strcat (xbuf, &orig[i]);
			free_string (orig);

			send_to_char ("Line replaced.\n\r", ch);

			return str_dup (xbuf);
		}

		i++;

		if (*rdesc == '\r')
			current_line++;
	}

	if (current_line - 1 != line)
	{
		send_to_char ("That line does not exist.\n\r", ch);
		return str_dup (xbuf);
	}

	free_string (orig);
	send_to_char ("Line replaced.\n\r", ch);

	return str_dup (xbuf);
}

/*****************************************************************************
 Name:		string_insertline
 Purpose:	Inserts a line, blank or containing text.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char   *string_insertline (CHAR_DATA * ch, char *orig, int line, char *addstring)
{
	char   *rdesc;
	char    xbuf[MAX_STRING_LENGTH];
	int     current_line = 1;
	int     i;

	xbuf[0] = '\0';
	strcpy (xbuf, orig);

	i = 0;

	for (rdesc = orig; *rdesc; rdesc++)
	{
		if (current_line == line)
			break;

		i++;

		if (*rdesc == '\r')
			current_line++;
	}

	if (!*rdesc)
	{
		send_to_char ("That line does not exist.\n\r", ch);
		return str_dup (xbuf);
	}

	xbuf[i] = '\0';

	if (*addstring)
		strcat (xbuf, addstring);
	strcat (xbuf, "\n\r");

	strcat (xbuf, &orig[i]);
	free_string (orig);

	send_to_char ("Line inserted.\n\r", ch);

	return str_dup (xbuf);
}


/*****************************************************************************
 Name:		string_deleteline
 Purpose:	Deletes a specified line of the string.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char   *string_deleteline (char *orig, int line)
{
	char   *rdesc;
	char    xbuf[MAX_STRING_LENGTH];
	int     current_line = 1;
	int     i = 0;

	xbuf[0] = '\0';

	for (rdesc = orig; *rdesc; rdesc++)
	{
		if (current_line != line)
		{
			xbuf[i] = *rdesc;
			i++;
		}

		if (*rdesc == '\r')
			current_line++;
	}

	free_string (orig);
	xbuf[i] = 0;

	return str_dup (xbuf);
}
/*****************************************************************************
 Name:		string_add
 Purpose:	Interpreter for string editing.
 Called by:	game_loop_xxxx(comm.c).
 ****************************************************************************/
void string_add( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    /*
     * Thanks to James Seng
     */
    smash_tilde( argument );

    if (IS_SET(ch->comm, COMM_MUDFTP) && str_cmp(argument, "@"))
    {
        send_to_char ("Type @  to manually abort FTP mode.\n\r"
                      "If mudFTP is not supported by your client, abort this edit, and disable mudFTP.\n\r", ch);
        return;
    }
    if ( *argument == '.' )
    {
        char arg1 [MAX_INPUT_LENGTH];
        char arg2 [MAX_INPUT_LENGTH];
        char arg3 [MAX_INPUT_LENGTH];
	char text [MAX_INPUT_LENGTH];
        
	argument = one_argument( argument, arg1 );
	sprintf(text, argument);        
	argument = first_arg( argument, arg2, FALSE );
        argument = first_arg( argument, arg3, FALSE );

        if ( !str_cmp( arg1, ".c" ) )
        {
            send_to_char( "String cleared.\n\r", ch );
            **ch->desc->pString = '\0';
            return;
        }

if (!str_cmp (arg1, ".rl"))
		{
			if (arg2[0] == '\0' || !is_number (arg2))
			{
				send_to_char (
				   "usage:  .rl <line> <text>\n\r", ch);
				return;
			}

			smash_tilde (arg3);	/* Just to be sure -- Hugin */
			*ch->desc->pString =
			   string_replace2 (ch, *ch->desc->pString, atoi (arg2), arg3);
			return;
		}


		if (!str_cmp (arg1, ".li"))
		{
			if (arg2[0] == '\0' || !is_number (arg2))
			{
				send_to_char (
				   "usage:  .i <line> {text}\n\r", ch);
				return;
			}

			smash_tilde (arg3);	/* Just to be sure -- Hugin */
			*ch->desc->pString =
			   string_insertline (ch, *ch->desc->pString, atoi (arg2), arg3);
			return;
		}


		if (!str_cmp (arg1, ".ld"))
		{
			if (arg2[0] == '\0' || !is_number (arg2))
			{
				send_to_char (
				   "usage:  .d <line>\n\r", ch);
				return;
			}

			*ch->desc->pString =
			   string_deleteline (*ch->desc->pString, atoi (arg2));
			sprintf (buf, "Line %d deleted.\n\r", atoi (arg2));
			send_to_char (buf, ch);
			return;
		}




        if ( !str_cmp( arg1, ".s" ) )
        {
            send_to_char( "String so far:\n\r", ch );
            send_to_char( *ch->desc->pString, ch );
            return;
        }

        if ( !str_cmp( arg1, ".r" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char(
                    "usage:  .r \"old string\" \"new string\"\n\r", ch );
                return;
            }

	    smash_tilde( arg3 );   /* Just to be sure -- Hugin */
            *ch->desc->pString =
            string_replace( *ch->desc->pString, arg2, arg3 );
            sprintf( buf, "'%s' replaced with '%s'.\n\r", arg2, arg3 );
            send_to_char( buf, ch );
            return;
        }

        if ( !str_cmp( arg1, ".f" ) )
        {
            *ch->desc->pString = format_string( *ch->desc->pString );
            send_to_char( "String formatted.\n\r", ch );
            return;
        }
		 if ( !str_cmp( arg1, ".i" ) )
        {
			do_immtalk(ch, text);
            return;
        }
          if ( !str_cmp( arg1, ".y" ) )
        {
                        do_say(ch, text);
            return;
        }
         if ( !str_cmp( arg1, ".q" ) )
        {
                        do_ooc(ch, text);
            return;
        }
        if ( !str_cmp( arg1, ".z" ) )
        {
                     do_shout(ch, text);
           return;
        }
        

	if ( !str_cmp( arg1, ".h" ) )
        {
            
            send_to_char( "{B.r{b '{Bold{b' '{Bnew{b'{w    -{W replace a substring \n\r", ch );
            send_to_char( "                     {w (requires '', \"\") \n\r", ch );
            send_to_char( "{B.h{b               {w -{W get help (this info)\n\r", ch );
            send_to_char( "{B.s{b               {w -{W show string so far  \n\r", ch );
            send_to_char( "{B.f{b               {w -{W (word wrap) string  \n\r", ch );
            send_to_char( "{B.c{b               {w -{W clear string so far \n\r", ch );
	    send_to_char( "{B.i{b               {w -{W Immtalk (Immortal Only)\n\r", ch );
            send_to_char( "{B.z{b               {w -{W Shout\n\r",ch);
            send_to_char( "{B.q{b               {w -{W Thunder Channel (IMM Only)         \n\r", ch );
            send_to_char( "{B.y{b               {w -{W To use Say         \n\r", ch );
	    send_to_char ("{B.ld{b <{Bline{b>       {w -{W deletes a line      \n\r", ch);
	    send_to_char ("{B.li{b <{Bline{b> <{Btext{b>{w -{W inserts a line	  \n\r", ch);
	    send_to_char ("{B.rl{b <{Bline{b> <{Btext{b>{w -{W replaces a line{x   \n\r", ch);
	
	
		send_to_char( "{B@{w                - end string          {x\n\r", ch );
            return;
        }
            

        send_to_char( "SEdit:  Invalid dot command.\n\r", ch );
        return;
    }

    if ( *argument == '~' || *argument == '@' )
    {
        ch->desc->pString = NULL;
        return;
    }

    strcpy( buf, *ch->desc->pString );

    /*
     * Truncate strings to MAX_STRING_LENGTH.
     * --------------------------------------
     */
    if ( strlen( buf ) + strlen( argument ) >= ( MAX_STRING_LENGTH - 4 ) )
    {
        send_to_char( "String too long, last line skipped.\n\r", ch );

	/* Force character out of editing mode. */
        ch->desc->pString = NULL;
        return;
    }

    /*
     * Ensure no tilde's inside string.
     * --------------------------------
     */
    smash_tilde( argument );

    strcat( buf, argument );
    strcat( buf, "\n\r" );
    free_string( *ch->desc->pString );
    *ch->desc->pString = str_dup( buf );
    return;
}



/*
 * Thanks to Kalgen for the new procedure (no more bug!)
 * Original wordwrap() written by Surreality.
 */
/*****************************************************************************
 Name:		format_string
 Purpose:	Special string formating and word-wrapping.
 Called by:	string_add(string.c) (many)olc_act.c
 ****************************************************************************/
char *format_string( char *oldstring /*, bool fSpace */)
{
  char xbuf[MAX_STRING_LENGTH];
  char xbuf2[MAX_STRING_LENGTH];
  char *rdesc;
  int i=0;
  bool cap=TRUE;
  
  xbuf[0]=xbuf2[0]=0;
  
  i=0;
  
  for (rdesc = oldstring; *rdesc; rdesc++)
  {
    if (*rdesc=='\n')
    {
      if (xbuf[i-1] != ' ')
      {
        xbuf[i]=' ';
        i++;
      }
    }
    else if (*rdesc=='\r') ;
    else if (*rdesc==' ')
    {
      if (xbuf[i-1] != ' ')
      {
        xbuf[i]=' ';
        i++;
      }
    }
    else if (*rdesc==')')
    {
      if (xbuf[i-1]==' ' && xbuf[i-2]==' ' && 
          (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!'))
      {
        xbuf[i-2]=*rdesc;
        xbuf[i-1]=' ';
        xbuf[i]=' ';
        i++;
      }
      else
      {
        xbuf[i]=*rdesc;
        i++;
      }
    }
    else if (*rdesc=='.' || *rdesc=='?' || *rdesc=='!') {
      if (xbuf[i-1]==' ' && xbuf[i-2]==' ' && 
          (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!')) {
        xbuf[i-2]=*rdesc;
        if (*(rdesc+1) != '\"')
        {
          xbuf[i-1]=' ';
          xbuf[i]=' ';
          i++;
        }
        else
        {
          xbuf[i-1]='\"';
          xbuf[i]=' ';
          xbuf[i+1]=' ';
          i+=2;
          rdesc++;
        }
      }
      else
      {
        xbuf[i]=*rdesc;
        if (*(rdesc+1) != '\"')
        {
          xbuf[i+1]=' ';
          xbuf[i+2]=' ';
          i += 3;
        }
        else
        {
          xbuf[i+1]='\"';
          xbuf[i+2]=' ';
          xbuf[i+3]=' ';
          i += 4;
          rdesc++;
        }
      }
      cap = TRUE;
    }
    else
    {
      xbuf[i]=*rdesc;
      if ( cap )
        {
          cap = FALSE;
          xbuf[i] = UPPER( xbuf[i] );
        }
      i++;
    }
  }
  xbuf[i]=0;
  strcpy(xbuf2,xbuf);
  
  rdesc=xbuf2;
  
  xbuf[0]=0;
  
  for ( ; ; )
  {
    for (i=0; i<77; i++)
    {
      if (!*(rdesc+i)) break;
    }
    if (i<77)
    {
      break;
    }
    for (i=(xbuf[0]?76:73) ; i ; i--)
    {
      if (*(rdesc+i)==' ') break;
    }
    if (i)
    {
      *(rdesc+i)=0;
      strcat(xbuf,rdesc);
      strcat(xbuf,"\n\r");
      rdesc += i+1;
      while (*rdesc == ' ') rdesc++;
    }
    else
    {
      bug ("No spaces", 0);
      *(rdesc+75)=0;
      strcat(xbuf,rdesc);
      strcat(xbuf,"-\n\r");
      rdesc += 76;
    }
  }
  while (*(rdesc+i) && (*(rdesc+i)==' '||
                        *(rdesc+i)=='\n'||
                        *(rdesc+i)=='\r'))
    i--;
  *(rdesc+i+1)=0;
  strcat(xbuf,rdesc);
  if (xbuf[strlen(xbuf)-2] != '\n')
    strcat(xbuf,"\n\r");

  free_string(oldstring);
  return(str_dup(xbuf));
}



/*
 * Used above in string_add.  Because this function does not
 * modify case if fCase is FALSE and because it understands
 * parenthesis, it would probably make a nice replacement
 * for one_argument.
 */
/*****************************************************************************
 Name:		first_arg
 Purpose:	Pick off one argument from a string and return the rest.
 		Understands quates, parenthesis (barring ) ('s) and
 		percentages.
 Called by:	string_add(string.c)
 ****************************************************************************/
char *first_arg( char *argument, char *arg_first, bool fCase )
{
    char cEnd;

    while ( *argument == ' ' )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"'
      || *argument == '%'  || *argument == '(' )
    {
        if ( *argument == '(' )
        {
            cEnd = ')';
            argument++;
        }
        else cEnd = *argument++;
    }

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
    if ( fCase ) *arg_first = LOWER(*argument);
            else *arg_first = *argument;
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( *argument == ' ' )
	argument++;

    return argument;
}




/*
 * Used in olc_act.c for aedit_builders.
 */
char * string_unpad( char * argument )
{
    char buf[MAX_STRING_LENGTH];
    char *s;

    s = argument;

    while ( *s == ' ' )
        s++;

    strcpy( buf, s );
    s = buf;

    if ( *s != '\0' )
    {
        while ( *s != '\0' )
            s++;
        s--;

        while( *s == ' ' )
            s--;
        s++;
        *s = '\0';
    }

    free_string( argument );
    return str_dup( buf );
}



/*
 * Same as capitalize but changes the pointer's data.
 * Used in olc_act.c in aedit_builder.
 */
char * string_proper( char * argument )
{
    char *s;

    s = argument;

    while ( *s != '\0' )
    {
        if ( *s != ' ' )
        {
            *s = UPPER(*s);
            while ( *s != ' ' && *s != '\0' )
                s++;
        }
        else
        {
            s++;
        }
    }

    return argument;
}
