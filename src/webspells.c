/**************************************************************************
*      Mindor 1.0 is copyright 2002-2004 by Shaun Mcbride                *
*      Portions of the code were inspired by other works                 *
*      found around the internet.                                        *
*                                                                        *
*      Please follow all previous copyrights and licenses.               *
*      Only instance I didn't follow the rules.                          *
*      Clandestine link don't work.  Removed by Kizeren.                 *
**************************************************************************/





/* This file allows you to output spells and skills to your web page.
   Do not remove the copyright statement and link to the Clandestine
   page included in the HTML output. */
/* To implement this code, simply make a call to spells_html_update
   somewhere in your code.  You can execute every time by placing it
   in db.c, or you can create a command for it in interp.c and
   execute only when you wish. */
/* The replacement of Clandestine-specific terms is left as an
   exercise for the user.  You will need to change the path to the
   file, possibly modify the code for religion spells if you have
   no religion/clan spell system or operate significantly differently,
   and modify the text appropriately for your mud.  In the religion
   spell section I have specifically filtered the table for the only
   three levels with spells (207,222,225); if you grant spells at
   different levels you should modify or delete that filter */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
 
#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif

/* We can't have spaces in HTML links so this function simply replaces
   spaces in a string with underscore characters.  It also makes the
   link entirely lower case for consistency */
void under_line(char *under_lined, char *spaced_out )
{
  char * point;

#ifdef DEBUG
        Debug ("under_line");
#endif
  strcpy(under_lined,spaced_out);
  for (point = under_lined; *point; point++)
    {
	if (*point == ' ')
	{
	  *point = '_';
	}
	else 
	{
	  *point = LOWER(*point);
	}
    }
  return;
} 

/* In order to differentiate "immortal spells" we must check whether
   the min level for all classes is an IMM level.  This is a UMIN
   across all classes of skill_level for spell number sn. */
int min_class_level( int sn )
{
    int min_so_far = MAX_LEVEL;
    int iClass;
#ifdef DEBUG
        Debug ("min_class_level");
#endif
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
        if (skill_table[sn].skill_level[iClass] < min_so_far)
		min_so_far = skill_table[sn].skill_level[iClass];
	}
  return min_so_far;
} 

void spells_html_update (void)
{

  FILE *fp; 
  int gn;
  int sn;
  int nn;
  int displayed[MAX_SKILL];
  int past_default = TRUE;
  char buf[MAX_STRING_LENGTH]; 
  char buf2[MAX_STRING_LENGTH]; 
  char buf3[MAX_STRING_LENGTH]; 
  
#ifdef DEBUG
        Debug ("spells_html_update");
#endif

  /* I have an "other" section for some odd spells not addressed in the
     spell groups, religion, or immortal sections.  I use a variable
     to note whether a spell has been displayed in another group in
     order to keep track of ones to be displayed.  This initializes
     that variable */
  for (nn = 0; nn < MAX_SKILL; nn++)
  {
    displayed[nn] = FALSE;
  }

  /* Change this to the absolute path for the html file in which you
     want the output to appear.*/
  CLOSE_RESERVE();
  if ( (fp = fopen("/home/mcbride/public_html/webspells1.html", "w") ) == NULL)
  {
     bug( "spells.html: fopen", 0 );
     perror( "webspells.html" );
  }
  else
  {
  /* Your header information, of course, goes here. */
  fprintf(fp, "<html>\n");
  fprintf(fp, "<head>\n");
  fprintf(fp, "<title>\n");
  fprintf(fp, "Skills and Spells on Mindor\n");
  fprintf(fp, "</title>\n");
  fprintf(fp, "<BODY TEXT=\"#C0C0C0\" BGCOLOR=\"#000000\" LINK=\"#00FFFF\"\n");
  fprintf(fp, "VLINK=\"#FFFFFF\" ALINK=\"#008080\" BACKGROUND=\"dragbkgd.gif\">\n");
  fprintf(fp, "<h1><center>Spells and Skills on Mindor</center></h1>\n");

           
	/* For the first part of the page, we will go through all the
	   spell groups.  This code assumes the following unique
         characteristics in the Clandestine code, which you may have
	   to change depending on the organization of your groups:
		* The first groups are "<class> basics" starting with
		  "rom basics".  All skills in the basic groups are
		  skills (i.e., no spell groups)
		* The next groups are "<class> default" starting with
		  "mage default"
		* Finally, other skill/spell groups are listed, starting
		  with "weaponsmaster".	None of these have spell groups
		  as part of their group.*/
	for (gn = 0; gn < MAX_GROUP; gn++)
	{
	    if (group_table[gn].name == NULL)
		break;

		/* Key to start table. This should be first BASIC spell group */
	    if (!strcmp(group_table[gn].name,"rom basics"))
		{
		  fprintf(fp, "<center><H2>Basic Skills</H2></center>\n");
		  fprintf(fp, "<center>All players receive the <I>basic</I> skills for their class.</center><br>\n");
		  fprintf(fp, "<CENTER><TABLE BORDER=1>\n");
		  fprintf(fp, "<TR ALIGN=LEFT VALIGN=CENTER>\n");
		  fprintf(fp, "<TD>Basic Group</TD>\n");
		  fprintf(fp, "<TD>Skills Included</TD></TR>\n");
		}

		/* Key to end table/start new table. This should be first DEFAULT 
											spell group */
	    if (!strcmp(group_table[gn].name,"mage default"))
		{
		  /* Third column only prints out if past_default is false.
		  This column contains spell groups for default spell groups */
		  past_default = FALSE;
		  fprintf(fp, "</TABLE></CENTER>\n");
		  fprintf(fp, "<center><H2>Default Skill/Spell Groups</H2></center>\n");
		  fprintf(fp, "<center>Players receive the <I>default</I> skills and spell groups if they bypass customization.</center><br>\n");
		  fprintf(fp, "<CENTER><TABLE BORDER=1>\n");
		  fprintf(fp, "<TR ALIGN=LEFT VALIGN=CENTER>\n");
		  fprintf(fp, "<TD>Default Group</TD>\n");
		  fprintf(fp, "<TD>Skills Included</TD>\n");
		  fprintf(fp, "<TD>Skill/Spell Groups Included</TD></TR>\n");
		}

		/* Key to end table/start new table. This should be first group
			after the defaults */
	    if (!strcmp(group_table[gn].name,"weaponsmaster"))
		{
		  past_default = TRUE;
		  fprintf(fp, "</TABLE></CENTER>\n");
		  fprintf(fp, "<center><H2>Other Skill/Spell Groups</H2></center>\n");
		  fprintf(fp, "<center>Players may gain these skills and spells.</center><br>\n");
		  fprintf(fp, "<CENTER><TABLE BORDER=1>\n");
		  fprintf(fp, "<TR ALIGN=LEFT VALIGN=CENTER>\n");
		  fprintf(fp, "<TD>Skill/Spell Group</TD>\n");
		  fprintf(fp, "<TD>Skills/Spells Included</TD></TR>\n");
		}

	    /* Create anchor for the spell group */
	    under_line(buf3,group_table[gn].name);
	    fprintf(fp,"<TR><TD><A NAME=\"%s\">%s</A></TD><TD>",
			buf3,
			group_table[gn].name);
	    /* Print the skills in the group */	    
	    buf[0] = '\0';  
	    for ( sn = 0; sn < MAX_IN_GROUP; sn++)
	    {
	        if (group_table[gn].spells[sn] == NULL)
	            break;
		  nn = skill_lookup(group_table[gn].spells[sn]);
		  if (nn != -1) 
			/* not a skill, must be spell group */
		  {
			sprintf(buf2,"%s, ",group_table[gn].spells[sn]);
			strcat(buf, buf2);
		    /* If not a spell group, check off the spell as
			 displayed */
			displayed[nn] = TRUE;
		  }
	    }
	    if ((buf[0] != '\0') && (strlen(buf) > 2))
		buf[(strlen(buf)-2)] = '\0'; /* remove last ", " */
	    else
		strcpy(buf, "None");

	    /* This third column only prints out for the default
		 groups. */
	    if (!past_default)
	    {
	      fprintf(fp, "%s</TD><TD>\n", buf);

		/* Print the spell groups in the group */
	      buf[0] = '\0';  
	      for ( sn = 0; sn < MAX_IN_GROUP; sn++)
	      {
	        if (group_table[gn].spells[sn] == NULL)
	            break;
		  nn = skill_lookup(group_table[gn].spells[sn]);
		  if (nn == -1) 
			/* not a skill, must be spell group */
		  {
			under_line(buf3,group_table[gn].spells[sn]);
			sprintf(buf2,"<A HREF=\"#%s\">%s</A>, ",
				buf3,
				group_table[gn].spells[sn]);
			strcat(buf, buf2);
		  }
		  else
		  {
		    /* If not a spell group, check off the spell as
			 displayed */
		    displayed[nn] = TRUE;
		  }
	      }
	      if ((buf[0] != '\0') && (strlen(buf) > 2))
		  buf[(strlen(buf)-2)] = '\0'; /* remove last comma/space */
	      else
		  strcpy(buf, "None");
	    }
	    fprintf(fp, "%s</TD></TR>\n", buf);
	}
  fprintf(fp, "</TABLE></CENTER>\n");

  
  /* OK we've been keeping track of displayed skills and spells.  Now
     it's time to print the ones we haven't shown.  This could in
     theory be organized but I'm lazy so I'll just lump them all
     together. */
  fprintf(fp, "<center><H2>Other Skills and Spells</H2></center>\n");
  fprintf(fp, "<center>The following skills and spells are available to various mortals.</center><br>\n");
  fprintf(fp, "<CENTER><TABLE BORDER=1>\n");
  fprintf(fp, "<TR ALIGN=LEFT VALIGN=CENTER>\n");
  fprintf(fp, "<TD>\n");
    buf[0] = '\0';  
    /* Go through spell table and.... */
    for (sn = 1; sn < MAX_SKILL; sn++)
    {
      if (skill_table[sn].name == NULL)
        break;
	/* Display any spell not done which is not an IMM spell. */
	if (!displayed[sn] && (min_class_level(sn) < HERO) )
      {
	  sprintf(buf2,"%s, ", skill_table[sn].name);
	  strcat(buf, buf2);
	}	
    }
    if ((buf[0] != '\0') && (strlen(buf) > 2))
	  buf[(strlen(buf)-2)] = '\0'; /* remove last comma/space */
    else
	  strcpy(buf, "None");
    fprintf(fp, "%s</TD></TR>\n", buf);
  fprintf(fp, "</TABLE></CENTER>\n");

  /* What haven't we covered yet?  The IMM spells, of course.
     We just go through the list and identify spells by level.
     I have arbitrarily grabbed only the three levels (207,222,225)
     that have imm spells on my MUD, you may need to change this. */
  fprintf(fp, "<center><H2>Immortal Skills and Spells</H2></center>\n");
  fprintf(fp, "<center>The following skills and spells are available to various immortals.\n");
	/* Spells stay at MAX_LEVEL until we're ready to use them. */
  fprintf(fp, "This list includes some spells under development.</center><br>\n");
  fprintf(fp, "<CENTER><TABLE BORDER=1>\n");
  fprintf(fp, "<TR ALIGN=LEFT VALIGN=CENTER>\n");
  /* Column headers */
  fprintf(fp, "<TD>Level</TD>\n");
  fprintf(fp, "<TD>Spells</TD></TR>\n");

  /* Yeah, AVATARs aren't really IMMs, but they have no skill/spell
	gains at their level anyway, so who cares. */
  for (nn = (HERO); nn <= MAX_LEVEL; nn++ )
  {
	/* I don't want a bunch of empty columns so I skip 'em */
    if ((nn != 207) && (nn != 222) && (nn != 225))
	continue;
    fprintf(fp, "<TR><TD>%d</TD><TD>\n", nn);
    buf[0] = '\0';  
    for (sn = 1; sn < MAX_SKILL; sn++)
    {
      if (skill_table[sn].name == NULL)
        break;
      if (min_class_level(sn) == nn)
      {
	  sprintf(buf2,"%s, ", skill_table[sn].name);
	  strcat(buf, buf2);
	}	
    }
    if ((buf[0] != '\0') && (strlen(buf) > 2))
	  buf[(strlen(buf)-2)] = '\0'; /* remove last comma/space */
    else
	  strcpy(buf, "None");
    fprintf(fp, "%s</TD></TR>\n", buf);
  }
  fprintf(fp, "</TABLE></CENTER>\n");

  fprintf(fp, "<font face=\"Times New Roman\"><center>\n");
  sprintf(buf, "This file last updated at %s Eastern Time.\n", ((char *) ctime( &current_time )));
  fprintf(fp, buf);
  fprintf(fp, "</center></font>\n");

  fprintf(fp, "<br><br>\n");


  fprintf(fp, "</body>\n");
  fprintf(fp, "</html>\n");
  fclose( fp ); 
  OPEN_RESERVE();
  } /*end if */ 
  
  return;
}/* end function */



