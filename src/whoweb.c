/*
 *  This Code originally created by Matthew Peck, aka Valatar
 *              sauron@digital-genesis.com
 *  Modified by Nebseni of Clandestine MUD
 *              http://clandestine.mudnet.net
 *
 *  Created so that I wouldn't have to make a CGI script that would update
 *  the page everytime you check.  All you have to do to use this is email 
 *  me and let me know that you are.  Include your mud address and webpage 
 *  if you don't mind.  You have to know a bit of HTML to 
 *  make the changes necessary for this to work for your mud, but other 
 *  than that, its all self contained.  This code is compatible with Lope's
 *  Colour codes, if you use some other system, you'll need to change 
 *  get_who_data to fit.  If there are any problems, email me and let me know.
 *
 *  To get this to work, save this as a .c file.  Then add a line in update.c
 *  that declares the function, and another that calls it.  I added it to 
 *  the pulse_music part, so it updates about every 5 seconds.  You can add
 *  it elsewhere if you like.  The declaration looks like this:
 *                  void    who_html_update args( ( void ) );
 *  Then the function call looks like this:
 *                  who_html_update();
 *  Update the makefile to include this code, then compile and you'll be all set.
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "lookup.h"
#include "tables.h"
 
#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif

int html_colour args (( char type, char *string ));
void html_colourconv args (( char *buffer, const char *txt, CHAR_DATA *ch ));

void who_html_update (void)
{

	/* this code assumes 45-character titles and max color switches (485 bytes).
	  if title length is increased the buf sizes must be increased */
  FILE *fp; 
  DESCRIPTOR_DATA *d;
  char buf[2*MAX_INPUT_LENGTH]; 
  char buf2[2*MAX_INPUT_LENGTH];
#ifdef DEBUG
        Debug ("who_html_update");
#endif

  buf[0] = '\0';
  buf2[0] = '\0';
  
  CLOSE_RESERVE();
  if ( (fp = fopen("/home/mcbride/online1.html", "w") ) == NULL)
     /* change the directory above to the absolute directory and filename
      * of the page you are going to make.  IMPORTANT:  The file needs to 
      * exist before you attempt to run this. 
      *         --Valatar
      */

  {
     bug( "online.html: fopen", 0 );
     perror( "online.html" );
  }
  else
  {
  fprintf(fp, "<html>\n");
  fprintf(fp, "<head>\n");
  fprintf(fp, "<title>\n");
  fprintf(fp, "Players currently on Mindor\n");
/* INSERT YOUR MUD NAME THERE AND THREE LINES DOWN */

  fprintf(fp, "</title>\n");
  fprintf(fp, "<BODY  TEXT=""#C0C0C0"" BGCOLOR=""#000000"" LINK=""#00FFFF""\n");
  fprintf(fp, "VLINK=""#FFFFFF"" ALINK=""#008080"">\n");
 

  fprintf(fp, "<left><TABLE BORDER=0 BGCOLOR=""#000000"" >\n");
  fprintf(fp, "\n");
  
    
  for ( d = descriptor_list; d != NULL ; d = d->next )
  {
    CHAR_DATA *wch;
    char class[5];
    
    if ( d->connected != CON_PLAYING)
        continue;
    wch   = ( d->original != NULL ) ? d->original : d->character;
    class[0] = '\0';

    if (!(wch->invis_level > LEVEL_HERO))
    {

	

      buf2[0] = '\0';
     
      sprintf(buf2, "{W<<%d %s %s>> %s%s{W<br>",wch->level,race_table[wch->race].name,class_table[wch->class].who_name, wch->short_descr, (IS_NPC(wch)? "" : wch->pcdata->title));
	html_colourconv( buf, buf2, wch );
      fprintf(fp, buf);
      fprintf(fp, "\n");
 
    }  /*end if */
  }    /*end for */
  
  fprintf(fp, "</TABLE></CENTER>\n");
  fprintf(fp, "<P><A HREF=""./"" TARGET=""_top""><Font Size=+1>\n");
  fprintf(fp, "Return to main page</A> </P></Font>\n");

  fprintf(fp, "</body>\n");
  fprintf(fp, "</html>\n");
  fclose( fp ); 
  OPEN_RESERVE();
  } /*end if */ 
  
  return;
}/* end function */


int html_colour( char type, char *string )
{
    char	code[ 25 ];
    char	*p = '\0';

#ifdef DEBUG
        Debug ("html_colour");
#endif
        
    switch( type )
    {
	default:
	case '\0':
	    code[0] = '\0';
	    break;
	case ' ':
	    sprintf( code, " " );
	    break;
	case 'x':
	    sprintf( code, "<font color=""#006400"">" );
	    break;
	case 'b':
	    sprintf( code, "<font color=""#00008B"">" );
	    break;
	case 'c':
	    sprintf( code, "<font color=""#008B8B"">" );
	    break;
	case 'g':
	    sprintf( code, "<font color=""#006400"">" );
	    break;
	case 'm':
	    sprintf( code, "<font color=""#8B008B"">" );
	    break;
	case 'r':
	    sprintf( code, "<font color=""#8B0000"">" );
	    break;
	case 'w':
	    sprintf( code, "<font color=""#808080"">" );
	    break;
	case 'y':
	    sprintf( code, "<font color=""#808000"">" );
	    break;
	case 'B':
	    sprintf( code, "<font color=""#0000FF"">" );
	    break;
	case 'C':
	    sprintf( code, "<font color=""#OOFFFF"">" );
	    break;
	case 'G':
	    sprintf( code, "<font color=""#00FF00"">" );
	    break;
	case 'M':
	    sprintf( code, "<font color=""#FF00FF"">" );
	    break;
	case 'R':
	    sprintf( code, "<font color=""#FF0000"">" );
	    break;
	case 'W':
	    sprintf( code, "<font color=""#FFFFFF"">" );
	    break;
	case 'Y':
	    sprintf( code, "<font color=""#FFFF00"">" );
	    break;
	case 'D':
	    sprintf( code, "<font color=""#636363"">" );
	    break;
	case '{':
	    sprintf( code, "{" );
	    break;
    }

    p = code;
    while( *p != '\0' )
    {
	*string = *p++;
	*++string = '\0';
    }

    return( strlen( code ) );
}

void html_colourconv( char *buffer, const char *txt, CHAR_DATA *ch )
{
    const	char	*point;
		int	skip = 0;

#ifdef DEBUG
        Debug ("html_colourconv");
#endif
        
    for( point = txt ; *point ; point++ )
    {
	if( *point == '{' )
	{
	    point++;
	    if( *point == '\0' )
		point--;
	    else
	      skip = html_colour( *point, buffer );
	    while( skip-- > 0 )
		++buffer;
	    continue;
	}

	*buffer = *point;
	*++buffer = '\0';
    }			
    *buffer = '\0';
    return;
}



