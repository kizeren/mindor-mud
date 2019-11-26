<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0041)http://cland.dustbite.net/code/webraces.c -->
<HTML><HEAD>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<META content="MSHTML 6.00.2722.900" name=GENERATOR></HEAD>
<BODY><PRE>/* This file allows you to output races and classes to your web page.
   I put a significant amount of work into this function, coding and
   debugging for approximately 20 combined hours.  Do not remove the
   copyright statement and link to the Clandestine page included in
   the HTML output. */
/* To implement this code, simply make a call to races_html_update
   somewhere in your code.  You can execute every time by placing it
   in db.c, or you can create a command for it in interp.c and
   execute only when you wish. */
/* The replacement of Clandestine-specific terms is left as an
   exercise for the user.  You will need to change the path to the
   file, modify the raceclass_strip function for the format of your
   help files, and modify the text appropriately for your mud.  In
   addition, the number "5" is used as the max number of skills
   associated with a race.  If you have a different constant in your
   merc.h file you will need to change that portion of the code.
   Clandestine also has 10 affect variables (with 31 affects each)
   built on the same concept as the standard "AFF2" snippet.  If you
   do not have that many, selectively delete or modify the code
   appropriately. */

#if defined(macintosh)
#include &lt;types.h&gt;
#else
#include &lt;sys/types.h&gt;
#endif
#include &lt;ctype.h&gt;
#include &lt;stdio.h&gt;
#include &lt;string.h&gt;
#include &lt;time.h&gt;
#include &lt;malloc.h&gt;
#include "merc.h"
#include "recycle.h"
#include "tables.h"
 
#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif

/* The under_line() function from webspells.c is needed to create
   valid HTML links. */
void under_line args ((char *under_lined, char *spaced_out ));

/* And a reverse hack of the under_line() function turns a list
   of affects/immunities from "this_affect that_affect the_other_affect"
   to "this affect, that affect, the other affect, " */
void un_under_line(char *spaced_out, char *under_lined )
{
  char * point;
  char * space;

#ifdef DEBUG
        Debug ("un_under_line");
#endif
  spaced_out[0] = '\0';
  space = spaced_out;
  for (point = under_lined; *point; point++)
    {
	/* replace _ with a space */
	if (*point == '_')
	  *space++ = ' ';
	else
	/* and insert a comma where there already is a space */
	if (*point == ' ')
	{
	  *space++ = ',';
	  *space++ = ' ';
	}
	else
	/* otherwise just copy the character */
	{
	  *space++ = *point;
	}
    }
  /* We'll return with a comma at the end so that multiple lists
     can be appended together.  Remove after the last one is appended. */
  *space++ = ',';
  *space++ = ' ';
  *space = '\0';
  return;
} 

/* Clandestine's race and class helps have the description of the
   race or class in between the race/class name and the word VITALS,
   plus the framing characters - | * # and % .  This function strips
   out the description.  Other help files will obviously require
   different treatment. */
void raceclass_help_strip ( char *buffer, char *txt, const char *raceclass_name )
{
    const	char	*point;
    int i;

#ifdef DEBUG
        Debug ("raceclass_help_strip");
#endif
    /* Advance until raceclass_name is recognized */
    for( point = txt ; *point ; point++ )
    {
	if( !str_prefix( raceclass_name, point ))
	{
	  break;
	}
    }	
    /* loop exits with either *point == NULL (not found) or
	 *point at beginning of race string.  Strip out race_string
	 and then return until 'VITALS' */	
    if (*point == '\0')
	{
	  /* String not found, return empty string */
	  *buffer = '\0';
	  return;
	}
    /* String found.  Advance to the end of raceclass_name */
    for ( i = strlen(raceclass_name); i &gt; 0; i-- )
	{
	    point++;
	}		
    /* Now copy characters until "VITALS" is found.  Strip out
       framing characters */
    for( ; *point ; point++ )
    {
	/* Don't copy these characters (copy hyphen if not followed
	   by a space */
	if( (*point == '|') || 
	   ((*point == '-') &amp;&amp; (*point+1 == ' ')) ||
	    (*point == '#') || 
	    (*point == '%') || 
	    (*point == '*') )
	{
	    point++;
	    continue;
	}
	/* Don't copy VITALS, and exit loop when it's found */
	if( !str_prefix( "VITALS" , point ))
	{
	  break;
	}
	/* Copy everything else.  Advance buffer */
	*buffer = *point;
	*++buffer = '\0';
    }			
    /* We've either found VITALS or the end of the string.  In either
	 case, terminate the string and return */
    *buffer = '\0';
}

void races_html_update (void)
{

  FILE *fp; 
  int i;
  int j;
  int k;
  int gn;
  int sn;
  HELP_DATA *pHelp;
  BUFFER *output;
  char buf[4*MAX_STRING_LENGTH]; 
  char buf2[MAX_STRING_LENGTH]; 
  char buf3[MAX_STRING_LENGTH]; 
  
#ifdef DEBUG
        Debug ("races_html_update");
#endif

  CLOSE_RESERVE();
  /* You will have to change this to the absolut path of the
     file you want to use for your race/class page */
  if ( (fp = fopen("/usr/home/mcbride/raceclass.html", "w") ) == NULL)
  {
     bug( "raceclass.html: fopen", 0 );
     perror( "raceclass.html" );
  }
  else
  {
  /* Change header information for your MUD here */
  /* Don't forget to add your own fonts, backgrounds, etc. */
  fprintf(fp, "&lt;html&gt;\n");
  fprintf(fp, "&lt;head&gt;\n");
  fprintf(fp, "&lt;title&gt;\n");
  fprintf(fp, "&lt;style&gt;&lt;!--a:hover{color:red; font-weight:bold; }--&gt;&lt;/style&gt;\n");
  fprintf(fp, "Races and Classes on Clandestine\n");
  fprintf(fp, "&lt;/title&gt;\n");
  fprintf(fp, "&lt;CENTER&gt;&lt;P&gt;&lt;A NAME=\"raceclassmenu\"&gt;&lt;/A&gt;&lt;/P&gt;&lt;/CENTER&gt;\n");
  fprintf(fp, "&lt;BODY TEXT=\"#C0C0C0\" BGCOLOR=\"#000000\" LINK=\"#00FFFF\"\n");
  fprintf(fp, "VLINK=\"#FFFFFF\" ALINK=\"#008080\" BACKGROUND=\"dragbkgd.gif\"&gt;\n");
  fprintf(fp, "&lt;h1&gt;&lt;center&gt;Races and Classes on Clandestine&lt;/center&gt;&lt;/h1&gt;\n");

  /* Blah blah blah */
  fprintf(fp, "&lt;P&gt;The Realm of Clandestine is host to many unique races and classes\n");
  fprintf(fp, "of characters.  Players who reincarnate have the option of choosing\n");
  fprintf(fp, "from even more powerful races and classes.&lt;BR&gt;&lt;BR&gt;\n");
  fprintf(fp, "Each race has certain innate affects, immunities, resistances,\n");
  fprintf(fp, "vulnerabilities, and skills. Beyond this, each class offers a wide\n");
  fprintf(fp, "range of additional spell groups and skills.&lt;/P&gt;\n");

  /* Here are the links to the lists of races and classes.  Remove the
     references to reincarnation (remort) if you don't have it.*/
  fprintf(fp, "&lt;CENTER&gt;&lt;P&gt;&lt;FONT SIZE=+1&gt;&lt;A HREF=\"#MortalRaces\"&gt;Mortal Races\n");
  fprintf(fp, "&lt;/A&gt; - &lt;A HREF=\"#ReincRaces\"&gt;Reincarnate Races &lt;BR&gt;\n");
  fprintf(fp, "&lt;/A&gt;&lt;A HREF=\"#MortalClasses\"&gt;Mortal Classes\n");
  fprintf(fp, "&lt;/A&gt; - &lt;A HREF=\"#ReincClasses\"&gt;Reincarnate Classes &lt;/A&gt;&lt;/FONT&gt;&lt;BR&gt;\n");
  fprintf(fp, "&lt;/P&gt;&lt;/CENTER&gt;&lt;BR&gt;&lt;BR&gt;\n");

/*** DO NOT REMOVE THE FOLLOWING LINK.  INCLUSION OF THIS LINK IS
     REQUIRED FOR YOUR USE OF THIS FUNCTION.  You may change the
     target from "_top" to "_new" if you desire.***/
  fprintf(fp, "&lt;CENTER&gt;&lt;P&gt;Automated race/class web update function &amp;copy;1998 by Nebseni of \n");
  fprintf(fp, "&lt;A HREF=\"http://clandestine.mudnet.net\" target=\"_top\"&gt;\n");
  fprintf(fp, "Clandestine MUD&lt;/A&gt;&lt;/P&gt;&lt;/CENTER&gt;\n");
/*** DO NOT REMOVE THE PRECEEDING LINK.  INCLUSION OF THIS LINK IS
     REQUIRED FOR YOUR USE OF THIS FUNCTION.  ***/

/* Output a table of races linked to the detailed information to follow.
   If you don't have reincarnates remove that if statement. */
  fprintf(fp, "&lt;A NAME=\"MortalRaces\"&gt;\n");
  fprintf(fp, "&lt;hr&gt;&lt;h2&gt;&lt;center&gt;Mortal Races&lt;/center&gt;&lt;/h2&gt;&lt;/A&gt;\n");
  fprintf(fp, "&lt;CENTER&gt;&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
  fprintf(fp, "&lt;tr ALIGN=\"LEFT\" VALIGN=\"CENTER\"&gt;\n");
  j = 0;
  for ( i = 1; race_table[i].name != NULL; i++ )
	{
	    if (!race_table[i].pc_race)
		continue;
          if ( !race_table[i].reincarnate_race )
	    {
		under_line(buf3,race_table[i].name);
		sprintf(buf2,"&lt;td&gt;&lt;A HREF=\"#%s\"&gt;%s&lt;/A&gt;&lt;/td&gt;",
			buf3,
			race_table[i].name);
		fprintf(fp, buf2);
		if (j++ &gt; 3)
		{
		  fprintf(fp, "&lt;/tr&gt;&lt;tr&gt;\n");
		  j = 0;
		}
	    }
	}
  fprintf(fp, "&lt;/tr&gt;&lt;/TABLE&gt;&lt;/CENTER&gt;\n");

/* I strongly recommend many internal links within the document for
   easy navigation. */
  fprintf(fp, "&lt;FONT SIZE=0&gt;&lt;CENTER&gt;&lt;P&gt;&lt;A HREF=\"#ReincRaces\"&gt;Reincarnate Races\n");
  fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#MortalClasses\"&gt;Mortal Classes\n");
  fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincClasses\"&gt;Reincarnate Classes &lt;/A&gt;\n");
  fprintf(fp, "&lt;/br&gt;\n");
  fprintf(fp, "&lt;A HREF=\"#raceclassmenu\"&gt;\n");
  fprintf(fp, "Races and Classes of Clandestine&lt;/A&gt;&lt;/P&gt;&lt;/CENTER&gt;&lt;/FONT&gt;\n");

/* Output a table of races linked to the detailed information to follow.
   If you don't have reincarnates remove this section. */
  fprintf(fp, "&lt;A NAME=\"ReincRaces\"&gt;\n");
  fprintf(fp, "&lt;hr&gt;&lt;h2&gt;&lt;center&gt;Reincarnate Races&lt;/center&gt;&lt;/h2&gt;&lt;/A&gt;\n");
  fprintf(fp, "&lt;CENTER&gt;&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
  fprintf(fp, "&lt;tr ALIGN=\"LEFT\" VALIGN=\"CENTER\"&gt;\n");
  j = 0;
  for ( i = 1; race_table[i].name != NULL; i++ )
	{
	    if (!race_table[i].pc_race)
		continue;
          if ( race_table[i].reincarnate_race )
	    {
		under_line(buf3,race_table[i].name);
		sprintf(buf2,"&lt;td&gt;&lt;A HREF=\"#%s\"&gt;%s&lt;/A&gt;&lt;/td&gt;",
			buf3,
			race_table[i].name);
		fprintf(fp, buf2);
		if (j++ &gt; 3)
		{
		  fprintf(fp, "&lt;/tr&gt;&lt;tr&gt;\n");
		  j = 0;
		}
	    }
	}
  fprintf(fp, "&lt;/tr&gt;&lt;/TABLE&gt;&lt;/CENTER&gt;\n");  

/* I strongly recommend many internal links within the document for
   easy navigation. */
  fprintf(fp, "&lt;FONT SIZE=0&gt;&lt;CENTER&gt;&lt;P&gt;&lt;A HREF=\"#MortalRaces\"&gt;Mortal Races\n");
  fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#MortalClasses\"&gt;Mortal Classes\n");
  fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincClasses\"&gt;Reincarnate Classes &lt;/A&gt;\n");
  fprintf(fp, "&lt;/br&gt;\n");
  fprintf(fp, "&lt;A HREF=\"#raceclassmenu\"&gt;\n");
  fprintf(fp, "Races and Classes of Clandestine&lt;/A&gt;&lt;/P&gt;&lt;/CENTER&gt;&lt;/FONT&gt;\n");

  fprintf(fp, "&lt;A NAME=\"MortalClasses\"&gt;\n");
  fprintf(fp, "&lt;hr&gt;&lt;h2&gt;&lt;center&gt;Mortal Classes&lt;/center&gt;&lt;/h2&gt;&lt;/A&gt;\n");
  fprintf(fp, "&lt;CENTER&gt;&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
  fprintf(fp, "&lt;tr ALIGN=\"LEFT\" VALIGN=\"CENTER\"&gt;\n");
  j = 0;
  for ( i = 0; i &lt; MAX_CLASS; i++ )
	{
        if (!class_table[i].reincarnate_class )
		{	
		under_line(buf3,class_table[i].name);
		sprintf(buf2,"&lt;td&gt;&lt;A HREF=\"#%s\"&gt;%s&lt;/A&gt;&lt;/td&gt;",
			buf3,
			class_table[i].name);
		fprintf(fp, buf2);
		if (j++ &gt; 3)
		{
		  fprintf(fp, "&lt;/tr&gt;&lt;tr&gt;\n");
		  j = 0;
		}
		}
	}
  fprintf(fp, "&lt;/tr&gt;&lt;/TABLE&gt;&lt;/CENTER&gt;\n");

/* I strongly recommend many internal links within the document for
   easy navigation. */
  fprintf(fp, "&lt;FONT SIZE=0&gt;&lt;CENTER&gt;&lt;P&gt;&lt;A HREF=\"#MortalRaces\"&gt;Mortal Races\n");
  fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincRaces\"&gt;Reincarnate Races\n");
  fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincClasses\"&gt;Reincarnate Classes &lt;/A&gt;\n");
  fprintf(fp, "&lt;/br&gt;\n");
  fprintf(fp, "&lt;A HREF=\"#raceclassmenu\"&gt;\n");
  fprintf(fp, "Races and Classes of Clandestine&lt;/A&gt;&lt;/P&gt;&lt;/CENTER&gt;&lt;/FONT&gt;\n");

  fprintf(fp, "&lt;A NAME=\"ReincClasses\"&gt;\n");
  fprintf(fp, "&lt;hr&gt;&lt;h2&gt;&lt;center&gt;Reincarnate Classes&lt;/center&gt;&lt;/h2&gt;&lt;/A&gt;\n");
  fprintf(fp, "&lt;CENTER&gt;&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
  fprintf(fp, "&lt;tr ALIGN=\"LEFT\" VALIGN=\"CENTER\"&gt;\n");
  j = 0;
  for ( i = 0; i &lt; MAX_CLASS; i++ )
	{
        if (class_table[i].reincarnate_class )
		{	
		under_line(buf3,class_table[i].name);
		sprintf(buf2,"&lt;td&gt;&lt;A HREF=\"#%s\"&gt;%s&lt;/A&gt;&lt;/td&gt;",
			buf3,
			class_table[i].name);
		fprintf(fp, buf2);
		if (j++ &gt; 3)
		{
		  fprintf(fp, "&lt;/tr&gt;&lt;tr&gt;\n");
		  j = 0;
		}
		}
	}
  fprintf(fp, "&lt;/tr&gt;&lt;/TABLE&gt;&lt;/CENTER&gt;\n");

/* I strongly recommend many internal links within the document for
   easy navigation. */
  fprintf(fp, "&lt;FONT SIZE=0&gt;&lt;CENTER&gt;&lt;P&gt;&lt;A HREF=\"#MortalRaces\"&gt;Mortal Races\n");
  fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincRaces\"&gt;Reincarnate Races\n");
  fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#MortalClasses\"&gt;Mortal Classes &lt;/A&gt;\n");
  fprintf(fp, "&lt;/br&gt;\n");
  fprintf(fp, "&lt;A HREF=\"#raceclassmenu\"&gt;\n");
  fprintf(fp, "Races and Classes of Clandestine&lt;/A&gt;&lt;/P&gt;&lt;/CENTER&gt;&lt;/FONT&gt;\n");

/* OK the above stuff was easy... the below sections were a total pain
   in the you-know-what.  (Neck of course.)  If you can understand this
   code, you're probably doing better than most people!  The main loops
   in this section are identical to the ones that produced the lists
   earlier on, so if you modified them, you will need to perform 
   similar modifications here. */
/******BEGIN MORTAL RACES SECTION******/
  fprintf(fp, "&lt;hr&gt;&lt;h2&gt;&lt;center&gt;Mortal Races&lt;/center&gt;&lt;/h2&gt;&lt;/A&gt;&lt;hr&gt;\n");
  for ( i = 1; race_table[i].name != NULL; i++ )
	{
	    if (!race_table[i].pc_race)
		continue;
          if ( !race_table[i].reincarnate_race )
	    {
		/* OK we've decided to create a table for this race.
		   First we do the internal link stuff and set up the
 		   table formatting. */
		under_line(buf3,race_table[i].name);
		sprintf(buf2,"&lt;A NAME=\"%s\"&gt;&lt;br&gt;&lt;/A&gt;",
			buf3);
		fprintf(fp, buf2);
		fprintf(fp, "&lt;CENTER&gt;&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
		fprintf(fp, "&lt;tr ALIGN=\"LEFT\" VALIGN=\"CENTER\"&gt;&lt;td colspan=2&gt;\n");
		sprintf(buf2,"&lt;center&gt;&lt;h2&gt;%s&lt;/h2&gt;&lt;/center&gt;",
			race_table[i].name);
		fprintf(fp, buf2);

		/* Now the tricky part.  Let's use a hack of do_help here
		   and grab the information from the help file.  The below
		   loop and associated function took me a full 5 hours to
		   code and debug.  Yeah, I know you coding experts out
		   there could have done it better.  But did you feel as
		   good when you finally got it working?  I think not. 
		   Oh, by the way, those of you who are experts at
		   manipulating strings can probably find some ways to
		   save time here.  I'm no expert, and this works, so
		   to you I say "Thfpbbbbbt!" */
		/* let's store the help info here... */
		output = new_buf();
		/* find the right help item */
		for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp-&gt;next )
		{
		 /* we only want the first help keyword to match, otherwise
		    we get Half Dragon when we want Dragon, etc... */
     		 one_argument( pHelp-&gt;keyword, buf3 );
		 if ( (buf3[0] != '\0') &amp;&amp; !str_cmp( race_table[i].name,
			buf3 ) )
		  {		    
		    /* This is (hopefully) the right help file.  Let's
			 take the help text, pull out what we want, and
			 print it.*/
	 	    add_buf(output,pHelp-&gt;text);
		    strcpy(buf, buf_string(output));
		    raceclass_help_strip(buf2, buf, race_table[i].name);	    
		    free_buf(output);
	 	    fprintf(fp, "%s",buf2);
		    break;
		  }
		}
	
		/* OK the hard part's done.  Let's throw in more statistics
		   for this race.  If you don't have the AFF2 code in just
		   delete the aff# statements you don't need. */
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td colspan=2&gt;\n");
		/* Affects */
		fprintf(fp, "Affected by: \n");
		if (	race_table[i].aff || 
			race_table[i].aff2 || 
			race_table[i].aff3 || 
			race_table[i].aff4 || 
			race_table[i].aff5 || 
			race_table[i].aff6 || 
			race_table[i].aff7 || 
			race_table[i].aff8 || 
			race_table[i].aff9 || 
			race_table[i].aff10  )
		  {
		    buf[0] = '\0';
		    /* Each of these, if it returns, will have an extra
			 ", " at the end of it so you can just append them
			 all to each other.  Remove the ", " at the end. */
		    if (race_table[i].aff)
			{
			  un_under_line(buf2,affect_bit_name(race_table[i].aff));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff2)
			{
			  un_under_line(buf2,affect2_bit_name(race_table[i].aff2));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff3)
			{
			  un_under_line(buf2,affect3_bit_name(race_table[i].aff3));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff4)
			{
			  un_under_line(buf2,affect4_bit_name(race_table[i].aff4));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff5)
			{
			  un_under_line(buf2,affect5_bit_name(race_table[i].aff5));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff6)
			{
			  un_under_line(buf2,affect6_bit_name(race_table[i].aff6));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff7)
			{
			  un_under_line(buf2,affect7_bit_name(race_table[i].aff7));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff8)
			{
			  un_under_line(buf2,affect8_bit_name(race_table[i].aff8));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff9)
			{
			  un_under_line(buf2,affect9_bit_name(race_table[i].aff9));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff10)
			{
			  un_under_line(buf2,affect10_bit_name(race_table[i].aff10));
			  strcat(buf, buf2);
			}
		    /* Now kill the ending ", " */
		    if (strlen(buf) &gt; 2)
			buf[strlen(buf)-2] = '\0';
		    fprintf(fp, "%s\n\r", buf);
		  }
		else
		  {
		  fprintf(fp, "no permanent affects.\n\r");
		  }
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td colspan=2&gt;\n");
		/* Skills */
		fprintf(fp, "Skills: \n");
	      buf[0] = '\0';
		/* This is fun.  We have a different table for pc_races and
		   mob/pc combined races.  We need to find the matching one
		   to get the right table.  In case they didn't match because
		   you have a PC race in your normal table that's not in your
		   PC race table your k will point to some other data so
		   always check k before you use it.*/
		for (k = 0; k &lt; MAX_PC_RACE; k++)
		{
		  if (!str_cmp(pc_race_table[k].name,race_table[i].name))
		    break;
		}
		/* Remember to check k here.  And note the 5 in the j
		   loop.  I pulled that out of the merc.h definition
		   for pc_race_type.skills.  Don't know why we don't
		   use a constant there like everywhere else in the code.*/
		if (k &lt; MAX_PC_RACE)
		  for (j = 0; j &lt; 5; j++)
		  {
		    if (pc_race_table[k].skills[j] == NULL)
		 	break;
		    strcat(buf, pc_race_table[k].skills[j]);
		    strcat(buf, ", ");
		  }
		/* Let's get rid of the trailing ", " again */
		if (strlen(buf) &gt; 2)
		  buf[strlen(buf)-2] = '\0';
		else
		  sprintf(buf,"None. ");
		fprintf(fp, "%s\n\r", buf);
		/* I fudged for a while with table widths here.  It appears
		   Netscape makes the columns equal size (50%) regardless
		   of what I put for width here.  Your results may vary. */
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td WIDTH=\"40%%\"&gt;\n");
		fprintf(fp, "&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
		fprintf(fp, "&lt;tr ALIGN=\"CENTER\"&gt;&lt;td VALIGN=\"CENTER\"&gt;\n");
		/* Stats header */
		/* If you have other than the 5 standard stats you'll
		   need to change this a bit */
		fprintf(fp, "&lt;/td&gt;&lt;td&gt;Str&lt;/td&gt;&lt;td&gt;Int&lt;/td&gt;&lt;td&gt;Wis&lt;/td&gt;");
		fprintf(fp, "&lt;td&gt;Dex&lt;/td&gt;&lt;td&gt;Con\n");
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Base\n");
		/* Stats base */
		if (k &lt; MAX_PC_RACE)
	  	  for (j = 0; j &lt; MAX_STATS; j++)
		    fprintf(fp, "&lt;/td&gt;&lt;td&gt;%d\n", pc_race_table[k].stats[j] );
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Max\n");
		/* Stats max */
		if (k &lt; MAX_PC_RACE)
	  	  for (j = 0; j &lt; MAX_STATS; j++)
		    fprintf(fp, "&lt;/td&gt;&lt;td&gt;%d\n", pc_race_table[k].max_stats[j] );
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;\n");
		/* 100% minus 40% is what... */
		fprintf(fp, "&lt;/td&gt;&lt;td WIDTH=\"60%%\"&gt;\n");
		fprintf(fp, "&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
		fprintf(fp, "&lt;tr ALIGN=\"LEFT\" VALIGN=\"CENTER\"&gt;&lt;td&gt;\n");
		/* This stuff is the same as the affects, only easier */
		/* Immune */
		fprintf(fp, "Immune to: \n");
		if ( race_table[i].imm )
		  {
		    buf[0] = '\0';
		    if (race_table[i].imm)
			{
			  un_under_line(buf2,imm_bit_name(race_table[i].imm));
			  strcat(buf, buf2);
			}
		    if (strlen(buf) &gt; 2)
			buf[strlen(buf)-2] = '\0';
		    fprintf(fp, "%s\n\r", buf);
		  }
		else
		  {
		  fprintf(fp, "no immunities.\n\r");
		  }
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;\n");
		/* Resistant */
		fprintf(fp, "Resistant to: \n");
		if ( race_table[i].res )
		  {
		    buf[0] = '\0';
		    if (race_table[i].res)
			{
			  un_under_line(buf2,imm_bit_name(race_table[i].res));
			  strcat(buf, buf2);
			}
		    if (strlen(buf) &gt; 2)
			buf[strlen(buf)-2] = '\0';
		    fprintf(fp, "%s\n\r", buf);
		  }
		else
		  {
		  fprintf(fp, "no resistances.\n\r");
		  }
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;\n");
		/* Vulnerable */
		fprintf(fp, "Vulnerable to: \n");
		if ( race_table[i].vuln )
		  {
		    buf[0] = '\0';
		    if (race_table[i].vuln)
			{
			  un_under_line(buf2,imm_bit_name(race_table[i].vuln));
			  strcat(buf, buf2);
			}
		    if (strlen(buf) &gt; 2)
			buf[strlen(buf)-2] = '\0';
		    fprintf(fp, "%s\n\r", buf);
		  }
		else
		  {
		  fprintf(fp, "no vulnerabilities.\n\r");
		  }
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;\n");

		/* Hey, that's all I decided to put on my tables.  There
		   is more information available if you want to dig through
		   the type structure and add stuff. */
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;/TABLE&gt;&lt;/CENTER&gt;\n");

		/* And of course the links to the various other tables here
		   so your readers don't have to scroll forever. */
		fprintf(fp, "&lt;FONT SIZE=0&gt;&lt;CENTER&gt;&lt;P&gt;&lt;A HREF=\"#MortalRaces\"&gt;Mortal Races\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincRaces\"&gt;Reincarnate Races\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#MortalClasses\"&gt;Mortal Classes\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincClasses\"&gt;Reincarnate Classes &lt;/A&gt;\n");
		fprintf(fp, "&lt;br&gt;&lt;A HREF=\"#raceclassmenu\"&gt;\n");
		fprintf(fp, "Races and Classes of Clandestine&lt;/A&gt;&lt;/P&gt;&lt;/CENTER&gt;&lt;/FONT&gt;&lt;hr&gt;\n");
	    }
	}
/******END MORTAL RACES SECTION******/
/* The Reinc Race section is identical to the Mortal race section
   except for the main loop which has a few boolean variables reversed.
   Sure, I could have probably saved time with some sort of common
   function that both loops could call but it's so much easier to
   just cut and paste the whole thing here.  So take 10 points off
   my grade, I don't care! */
/******BEGIN REINC RACES SECTION******/
  fprintf(fp, "&lt;hr&gt;&lt;h2&gt;&lt;center&gt;Reincarnate Races&lt;/center&gt;&lt;/h2&gt;&lt;/A&gt;&lt;hr&gt;\n");
  for ( i = 1; race_table[i].name != NULL; i++ )
	{
	    if (!race_table[i].pc_race)
		continue;
	    /* Here's the only change from above.  Delete a "!"! */
          if ( race_table[i].reincarnate_race )
	    {
		/* OK we've decided to create a table for this race.
		   First we do the internal link stuff and set up the
 		   table formatting. */
		under_line(buf3,race_table[i].name);
		sprintf(buf2,"&lt;A NAME=\"%s\"&gt;&lt;br&gt;&lt;/A&gt;",
			buf3);
		fprintf(fp, buf2);
		fprintf(fp, "&lt;CENTER&gt;&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
		fprintf(fp, "&lt;tr ALIGN=\"LEFT\" VALIGN=\"CENTER\"&gt;&lt;td colspan=2&gt;\n");
		sprintf(buf2,"&lt;center&gt;&lt;h2&gt;%s&lt;/h2&gt;&lt;/center&gt;",
			race_table[i].name);
		fprintf(fp, buf2);

		/* Now the tricky part.  Let's use a hack of do_help here
		   and grab the information from the help file.  The below
		   loop and associated function took me a full 5 hours to
		   code and debug.  Yeah, I know you coding experts out
		   there could have done it better.  But did you feel as
		   good when you finally got it working?  I think not. 
		   Oh, by the way, those of you who are experts at
		   manipulating strings can probably find some ways to
		   save time here.  I'm no expert, and this works, so
		   to you I say "Thfpbbbbbt!" */
		/* let's store the help info here... */
		output = new_buf();
		/* find the right help item */
		for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp-&gt;next )
		{
		 /* we only want the first help keyword to match, otherwise
		    we get Half Dragon when we want Dragon, etc... */
     		 one_argument( pHelp-&gt;keyword, buf3 );
		 if ( (buf3[0] != '\0') &amp;&amp; !str_cmp( race_table[i].name,
			buf3 ) )
		  {		    
		    /* This is (hopefully) the right help file.  Let's
			 take the help text, pull out what we want, and
			 print it.*/
	 	    add_buf(output,pHelp-&gt;text);
		    strcpy(buf, buf_string(output));
		    raceclass_help_strip(buf2, buf, race_table[i].name);	    
		    free_buf(output);
	 	    fprintf(fp, "%s",buf2);
		    break;
		  }
		}
	
		/* OK the hard part's done.  Let's throw in more statistics
		   for this race.  If you don't have the AFF2 code in just
		   delete the aff# statements you don't need. */
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td colspan=2&gt;\n");
		/* Affects */
		fprintf(fp, "Affected by: \n");
		if (	race_table[i].aff || 
			race_table[i].aff2 || 
			race_table[i].aff3 || 
			race_table[i].aff4 || 
			race_table[i].aff5 || 
			race_table[i].aff6 || 
			race_table[i].aff7 || 
			race_table[i].aff8 || 
			race_table[i].aff9 || 
			race_table[i].aff10  )
		  {
		    buf[0] = '\0';
		    /* Each of these, if it returns, will have an extra
			 ", " at the end of it so you can just append them
			 all to each other.  Remove the ", " at the end. */
		    if (race_table[i].aff)
			{
			  un_under_line(buf2,affect_bit_name(race_table[i].aff));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff2)
			{
			  un_under_line(buf2,affect2_bit_name(race_table[i].aff2));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff3)
			{
			  un_under_line(buf2,affect3_bit_name(race_table[i].aff3));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff4)
			{
			  un_under_line(buf2,affect4_bit_name(race_table[i].aff4));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff5)
			{
			  un_under_line(buf2,affect5_bit_name(race_table[i].aff5));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff6)
			{
			  un_under_line(buf2,affect6_bit_name(race_table[i].aff6));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff7)
			{
			  un_under_line(buf2,affect7_bit_name(race_table[i].aff7));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff8)
			{
			  un_under_line(buf2,affect8_bit_name(race_table[i].aff8));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff9)
			{
			  un_under_line(buf2,affect9_bit_name(race_table[i].aff9));
			  strcat(buf, buf2);
			}
		    if (race_table[i].aff10)
			{
			  un_under_line(buf2,affect10_bit_name(race_table[i].aff10));
			  strcat(buf, buf2);
			}
		    /* Now kill the ending ", " */
		    if (strlen(buf) &gt; 2)
			buf[strlen(buf)-2] = '\0';
		    fprintf(fp, "%s\n\r", buf);
		  }
		else
		  {
		  fprintf(fp, "no permanent affects.\n\r");
		  }
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td colspan=2&gt;\n");
		/* Skills */
		fprintf(fp, "Skills: \n");
	      buf[0] = '\0';
		/* This is fun.  We have a different table for pc_races and
		   mob/pc combined races.  We need to find the matching one
		   to get the right table.  In case they didn't match because
		   you have a PC race in your normal table that's not in your
		   PC race table your k will point to some other data so
		   always check k before you use it.*/
		for (k = 0; k &lt; MAX_PC_RACE; k++)
		{
		  if (!str_cmp(pc_race_table[k].name,race_table[i].name))
		    break;
		}
		/* Remember to check k here.  And note the 5 in the j
		   loop.  I pulled that out of the merc.h definition
		   for pc_race_type.skills.  Don't know why we don't
		   use a constant there like everywhere else in the code.*/
		if (k &lt; MAX_PC_RACE)
		  for (j = 0; j &lt; 5; j++)
		  {
		    if (pc_race_table[k].skills[j] == NULL)
		 	break;
		    strcat(buf, pc_race_table[k].skills[j]);
		    strcat(buf, ", ");
		  }
		/* Let's get rid of the trailing ", " again */
		if (strlen(buf) &gt; 2)
		  buf[strlen(buf)-2] = '\0';
		else
		  sprintf(buf,"None. ");
		fprintf(fp, "%s\n\r", buf);
		/* I fudged for a while with table widths here.  It appears
		   Netscape makes the columns equal size (50%) regardless
		   of what I put for width here.  Your results may vary. */
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td WIDTH=\"40%%\"&gt;\n");
		fprintf(fp, "&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
		fprintf(fp, "&lt;tr ALIGN=\"CENTER\"&gt;&lt;td VALIGN=\"CENTER\"&gt;\n");
		/* Stats header */
		/* If you have other than the 5 standard stats you'll
		   need to change this a bit */
		fprintf(fp, "&lt;/td&gt;&lt;td&gt;Str&lt;/td&gt;&lt;td&gt;Int&lt;/td&gt;&lt;td&gt;Wis&lt;/td&gt;");
		fprintf(fp, "&lt;td&gt;Dex&lt;/td&gt;&lt;td&gt;Con\n");
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Base\n");
		/* Stats base */
		if (k &lt; MAX_PC_RACE)
	  	  for (j = 0; j &lt; MAX_STATS; j++)
		    fprintf(fp, "&lt;/td&gt;&lt;td&gt;%d\n", pc_race_table[k].stats[j] );
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Max\n");
		/* Stats max */
		if (k &lt; MAX_PC_RACE)
	  	  for (j = 0; j &lt; MAX_STATS; j++)
		    fprintf(fp, "&lt;/td&gt;&lt;td&gt;%d\n", pc_race_table[k].max_stats[j] );
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;\n");
		/* 100% minus 40% is what... */
		fprintf(fp, "&lt;/td&gt;&lt;td WIDTH=\"60%%\"&gt;\n");
		fprintf(fp, "&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
		fprintf(fp, "&lt;tr ALIGN=\"LEFT\" VALIGN=\"CENTER\"&gt;&lt;td&gt;\n");
		/* This stuff is the same as the affects, only easier */
		/* Immune */
		fprintf(fp, "Immune to: \n");
		if ( race_table[i].imm )
		  {
		    buf[0] = '\0';
		    if (race_table[i].imm)
			{
			  un_under_line(buf2,imm_bit_name(race_table[i].imm));
			  strcat(buf, buf2);
			}
		    if (strlen(buf) &gt; 2)
			buf[strlen(buf)-2] = '\0';
		    fprintf(fp, "%s\n\r", buf);
		  }
		else
		  {
		  fprintf(fp, "no immunities.\n\r");
		  }
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;\n");
		/* Resistant */
		fprintf(fp, "Resistant to: \n");
		if ( race_table[i].res )
		  {
		    buf[0] = '\0';
		    if (race_table[i].res)
			{
			  un_under_line(buf2,imm_bit_name(race_table[i].res));
			  strcat(buf, buf2);
			}
		    if (strlen(buf) &gt; 2)
			buf[strlen(buf)-2] = '\0';
		    fprintf(fp, "%s\n\r", buf);
		  }
		else
		  {
		  fprintf(fp, "no resistances.\n\r");
		  }
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;\n");
		/* Vulnerable */
		fprintf(fp, "Vulnerable to: \n");
		if ( race_table[i].vuln )
		  {
		    buf[0] = '\0';
		    if (race_table[i].vuln)
			{
			  un_under_line(buf2,imm_bit_name(race_table[i].vuln));
			  strcat(buf, buf2);
			}
		    if (strlen(buf) &gt; 2)
			buf[strlen(buf)-2] = '\0';
		    fprintf(fp, "%s\n\r", buf);
		  }
		else
		  {
		  fprintf(fp, "no vulnerabilities.\n\r");
		  }
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;\n");

		/* Hey, that's all I decided to put on my tables.  There
		   is more information available if you want to dig through
		   the type structure and add stuff. */
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;/TABLE&gt;&lt;/CENTER&gt;\n");

		/* And of course the links to the various other tables here
		   so your readers don't have to scroll forever. */
		fprintf(fp, "&lt;FONT SIZE=0&gt;&lt;CENTER&gt;&lt;P&gt;&lt;A HREF=\"#MortalRaces\"&gt;Mortal Races\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincRaces\"&gt;Reincarnate Races\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#MortalClasses\"&gt;Mortal Classes\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincClasses\"&gt;Reincarnate Classes &lt;/A&gt;\n");
		fprintf(fp, "&lt;br&gt;&lt;A HREF=\"#raceclassmenu\"&gt;\n");
		fprintf(fp, "Races and Classes of Clandestine&lt;/A&gt;&lt;/P&gt;&lt;/CENTER&gt;&lt;/FONT&gt;&lt;hr&gt;\n");
	    }
	}
/******END REINC RACES SECTION******/
/* We do a similar thing for classes, except the information in the
   table is different.  The headers and help info is the same. */
/******BEGIN MORTAL CLASS SECTION******/
  fprintf(fp, "&lt;hr&gt;&lt;h2&gt;&lt;center&gt;Mortal Classes&lt;/center&gt;&lt;/h2&gt;&lt;/A&gt;&lt;hr&gt;\n");
  for ( i = 0; i &lt; MAX_CLASS; i++ )
	{
        if (!class_table[i].reincarnate_class )
	    {
		/* The links and header are pretty much the same as for
		   races, names changed to protect the innocent */
		under_line(buf3,class_table[i].name);
		sprintf(buf2,"&lt;A NAME=\"%s\"&gt;&lt;br&gt;&lt;/A&gt;",
			buf3);
		fprintf(fp, buf2);
		fprintf(fp, "&lt;CENTER&gt;&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
		fprintf(fp, "&lt;tr ALIGN=\"LEFT\" VALIGN=\"CENTER\"&gt;&lt;td&gt;\n");
		sprintf(buf2,"&lt;center&gt;&lt;h2&gt;%s&lt;/h2&gt;&lt;/center&gt;",
			class_table[i].name);
		fprintf(fp, buf2);

		output = new_buf();
		for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp-&gt;next )
		{

     		one_argument( pHelp-&gt;keyword, buf3 );
		if ( (buf3[0] != '\0') &amp;&amp; !str_cmp( class_table[i].name,
			buf3 ) )
		  {		    
	 	    add_buf(output,pHelp-&gt;text);
		    strcpy(buf, buf_string(output));
		    raceclass_help_strip(buf2, buf, class_table[i].name);	    
		    free_buf(output);
	 	    fprintf(fp, "%s",buf2);
		    break;
		  }
		}

		fprintf(fp, "&lt;ul&gt;\n");
		/* Different info in class table from race table... 
		   this should be self explanatory. */
		/* Primary Statistic */
		fprintf(fp, "&lt;li&gt;Primary Statistic: \n");
		switch ( class_table[i].attr_prime )
		{
		  case STAT_STR: fprintf(fp, "Strength\n\r"); break;
		  case STAT_INT: fprintf(fp, "Intelligence\n\r"); break;
		  case STAT_WIS: fprintf(fp, "Wisdom\n\r"); break;
		  case STAT_DEX: fprintf(fp, "Dexterity\n\r"); break;
		  case STAT_CON: fprintf(fp, "Constitution\n\r"); break;
		}		    
		fprintf(fp, "&lt;/li&gt;\n");
		/* Here we could link to the basic/default lists on
		   skills and spells page, but better for user to
		   see the info here.  We'll link for spell groups
		   though.  The loop to create this list, of course,
		   is stolen from webspells.c.  Webspells.c must be
		   in use for these links to work. */
		/* Basic Skills */
		fprintf(fp, "&lt;li&gt;Basic Skills: \n");
		sprintf(buf3,"%s basics",class_table[i].name);
		for (gn = 0; gn &lt; MAX_GROUP; gn++)
		{
	    	  if (group_table[gn].name == NULL)
		    break;
	    	  if (!str_cmp(group_table[gn].name,buf3))
		  {
		    /* Print the skills in the group */	    
		    buf[0] = '\0';  
		    for ( sn = 0; sn &lt; MAX_IN_GROUP; sn++)
		    {
		        if (group_table[gn].spells[sn] == NULL)
		            break;
			  k = skill_lookup(group_table[gn].spells[sn]);
			  if (k != -1) 
			  /* a skill, no link */
			  {
				sprintf(buf2,"%s, ",group_table[gn].spells[sn]);
			  }
			  else
			  /* not a skill, link to spell group */
			  {
				under_line(buf3,group_table[gn].spells[sn]);
				sprintf(buf2,"&lt;A HREF=\"./spells.html#%s\"&gt;%s&lt;/A&gt;, ",
					buf3,
					group_table[gn].spells[sn]);
			  }
			  strcat(buf, buf2);
		    }
		    if ((buf[0] != '\0') &amp;&amp; (strlen(buf) &gt; 2))
			buf[(strlen(buf)-2)] = '\0'; /* remove last ", " */
		    else
			strcpy(buf, "None");
		    break;
		  }
		}
 	      fprintf(fp, "%s",buf);
		fprintf(fp, "&lt;/li&gt;\n");

		/* Default Skills */
		fprintf(fp, "&lt;li&gt;Default Skills and Spell Groups: \n");
		sprintf(buf3,"%s default",class_table[i].name);
		for (gn = 0; gn &lt; MAX_GROUP; gn++)
		{
	    	  if (group_table[gn].name == NULL)
		    break;
	    	  if (!str_cmp(group_table[gn].name,buf3))
		  {
		    /* Print the skills in the group */	    
		    buf[0] = '\0';  
		    for ( sn = 0; sn &lt; MAX_IN_GROUP; sn++)
		    {
		        if (group_table[gn].spells[sn] == NULL)
		            break;
			  k = skill_lookup(group_table[gn].spells[sn]);
			  if (k != -1) 
			  /* a skill, no link */
			  {
				sprintf(buf2,"%s, ",group_table[gn].spells[sn]);
			  }
			  else
			  /* not a skill, link to spell group */
			  {
				under_line(buf3,group_table[gn].spells[sn]);
				sprintf(buf2,"&lt;A HREF=\"./spells.html#%s\"&gt;%s&lt;/A&gt;, ",
					buf3,
					group_table[gn].spells[sn]);
			  }
			  strcat(buf, buf2);
		    }
		    if ((buf[0] != '\0') &amp;&amp; (strlen(buf) &gt; 2))
			buf[(strlen(buf)-2)] = '\0'; /* remove last ", " */
		    else
			strcpy(buf, "None");
		    break;
		  }
		}
 	      fprintf(fp, "%s",buf);
		fprintf(fp, "&lt;/li&gt;&lt;/ul&gt;\n");
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;/TABLE&gt;&lt;/CENTER&gt;\n");

		/* Don't forget yer links. */
		fprintf(fp, "&lt;FONT SIZE=0&gt;&lt;CENTER&gt;&lt;P&gt;&lt;A HREF=\"#MortalRaces\"&gt;Mortal Races\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincRaces\"&gt;Reincarnate Races\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#MortalClasses\"&gt;Mortal Classes\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincClasses\"&gt;Reincarnate Classes &lt;/A&gt;\n");
		fprintf(fp, "&lt;br&gt;&lt;A HREF=\"#raceclassmenu\"&gt;\n");
		fprintf(fp, "Races and Classes of Clandestine&lt;/A&gt;&lt;/P&gt;&lt;/CENTER&gt;&lt;/FONT&gt;&lt;hr&gt;\n");
	    }
	}
/******END MORTAL CLASS SECTION******/
/* And now the same thing again with one minor change */
/******BEGIN REINC CLASS SECTION******/
  fprintf(fp, "&lt;hr&gt;&lt;h2&gt;&lt;center&gt;Reincarnate Classes&lt;/center&gt;&lt;/h2&gt;&lt;/A&gt;&lt;hr&gt;\n");
  for ( i = 0; i &lt; MAX_CLASS; i++ )
	{
	  /* The minor change goes right here!*/
        if (class_table[i].reincarnate_class )
	    {
		/* The links and header are pretty much the same as for
		   races, names changed to protect the innocent */
		under_line(buf3,class_table[i].name);
		sprintf(buf2,"&lt;A NAME=\"%s\"&gt;&lt;br&gt;&lt;/A&gt;",
			buf3);
		fprintf(fp, buf2);
		fprintf(fp, "&lt;CENTER&gt;&lt;TABLE BORDER=0 CELLPADDING=2&gt;\n");
		fprintf(fp, "&lt;tr ALIGN=\"LEFT\" VALIGN=\"CENTER\"&gt;&lt;td&gt;\n");
		sprintf(buf2,"&lt;center&gt;&lt;h2&gt;%s&lt;/h2&gt;&lt;/center&gt;",
			class_table[i].name);
		fprintf(fp, buf2);

		output = new_buf();
		for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp-&gt;next )
		{

     		one_argument( pHelp-&gt;keyword, buf3 );
		if ( (buf3[0] != '\0') &amp;&amp; !str_cmp( class_table[i].name,
			buf3 ) )
		  {		    
	 	    add_buf(output,pHelp-&gt;text);
		    strcpy(buf, buf_string(output));
		    raceclass_help_strip(buf2, buf, class_table[i].name);	    
		    free_buf(output);
	 	    fprintf(fp, "%s",buf2);
		    break;
		  }
		}
	
		fprintf(fp, "&lt;ul&gt;\n");
		/* Different info in class table from race table... 
		   this should be self explanatory. */
		/* Primary Statistic */
		fprintf(fp, "&lt;li&gt;Primary Statistic: \n");
		switch ( class_table[i].attr_prime )
		{
		  case STAT_STR: fprintf(fp, "Strength\n\r"); break;
		  case STAT_INT: fprintf(fp, "Intelligence\n\r"); break;
		  case STAT_WIS: fprintf(fp, "Wisdom\n\r"); break;
		  case STAT_DEX: fprintf(fp, "Dexterity\n\r"); break;
		  case STAT_CON: fprintf(fp, "Constitution\n\r"); break;
		}		    
		fprintf(fp, "&lt;/li&gt;\n");
		/* Here we could link to the basic/default lists on
		   skills and spells page, but better for user to
		   see the info here.  We'll link for spell groups
		   though.  The loop to create this list, of course,
		   is stolen from webspells.c.  Webspells.c must be
		   in use for these links to work. */
		/* Basic Skills */
		fprintf(fp, "&lt;li&gt;Basic Skills: \n");
		sprintf(buf3,"%s basics",class_table[i].name);
		for (gn = 0; gn &lt; MAX_GROUP; gn++)
		{
	    	  if (group_table[gn].name == NULL)
		    break;
	    	  if (!str_cmp(group_table[gn].name,buf3))
		  {
		    /* Print the skills in the group */	    
		    buf[0] = '\0';  
		    for ( sn = 0; sn &lt; MAX_IN_GROUP; sn++)
		    {
		        if (group_table[gn].spells[sn] == NULL)
		            break;
			  k = skill_lookup(group_table[gn].spells[sn]);
			  if (k != -1) 
			  /* a skill, no link */
			  {
				sprintf(buf2,"%s, ",group_table[gn].spells[sn]);
			  }
			  else
			  /* not a skill, link to spell group */
			  {
				under_line(buf3,group_table[gn].spells[sn]);
				sprintf(buf2,"&lt;A HREF=\"./spells.html#%s\"&gt;%s&lt;/A&gt;, ",
					buf3,
					group_table[gn].spells[sn]);
			  }
			  strcat(buf, buf2);
		    }
		    if ((buf[0] != '\0') &amp;&amp; (strlen(buf) &gt; 2))
			buf[(strlen(buf)-2)] = '\0'; /* remove last ", " */
		    else
			strcpy(buf, "None");
		    break;
		  }
		}
 	      fprintf(fp, "%s",buf);
		fprintf(fp, "&lt;/li&gt;\n");

		/* Default Skills */
		fprintf(fp, "&lt;li&gt;Default Skills and Spell Groups: \n");
		sprintf(buf3,"%s default",class_table[i].name);
		for (gn = 0; gn &lt; MAX_GROUP; gn++)
		{
	    	  if (group_table[gn].name == NULL)
		    break;
	    	  if (!str_cmp(group_table[gn].name,buf3))
		  {
		    /* Print the skills in the group */	    
		    buf[0] = '\0';  
		    for ( sn = 0; sn &lt; MAX_IN_GROUP; sn++)
		    {
		        if (group_table[gn].spells[sn] == NULL)
		            break;
			  k = skill_lookup(group_table[gn].spells[sn]);
			  if (k != -1) 
			  /* a skill, no link */
			  {
				sprintf(buf2,"%s, ",group_table[gn].spells[sn]);
			  }
			  else
			  /* not a skill, link to spell group */
			  {
				under_line(buf3,group_table[gn].spells[sn]);
				sprintf(buf2,"&lt;A HREF=\"./spells.html#%s\"&gt;%s&lt;/A&gt;, ",
					buf3,
					group_table[gn].spells[sn]);
			  }
			  strcat(buf, buf2);
		    }
		    if ((buf[0] != '\0') &amp;&amp; (strlen(buf) &gt; 2))
			buf[(strlen(buf)-2)] = '\0'; /* remove last ", " */
		    else
			strcpy(buf, "None");
		    break;
		  }
		}
 	      fprintf(fp, "%s",buf);
		fprintf(fp, "&lt;/li&gt;&lt;/ul&gt;\n");
		fprintf(fp, "&lt;/td&gt;&lt;/tr&gt;&lt;/TABLE&gt;&lt;/CENTER&gt;\n");

		/* Don't forget yer links. */
		fprintf(fp, "&lt;FONT SIZE=0&gt;&lt;CENTER&gt;&lt;P&gt;&lt;A HREF=\"#MortalRaces\"&gt;Mortal Races\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincRaces\"&gt;Reincarnate Races\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#MortalClasses\"&gt;Mortal Classes\n");
		fprintf(fp, "&lt;/A&gt; | &lt;A HREF=\"#ReincClasses\"&gt;Reincarnate Classes &lt;/A&gt;\n");
		fprintf(fp, "&lt;br&gt;&lt;A HREF=\"#raceclassmenu\"&gt;\n");
		fprintf(fp, "Races and Classes of Clandestine&lt;/A&gt;&lt;/P&gt;&lt;/CENTER&gt;&lt;/FONT&gt;&lt;hr&gt;\n");
	    }
	}
/******END REINC CLASS SECTION******/

  fprintf(fp, "&lt;font face=\"Times New Roman\"&gt;&lt;center&gt;\n");
  sprintf(buf, "This file last updated at %s Eastern Time.\n", ((char *) ctime( &amp;current_time )));
  fprintf(fp, buf);
  fprintf(fp, "&lt;/center&gt;&lt;/font&gt;\n");

  /* One final link to the top page, change if you want */
  fprintf(fp, "&lt;br&gt;&lt;br&gt;\n");
  fprintf(fp, "&lt;CENTER&gt;&lt;P&gt;&lt;A HREF=\"./\" TARGET=\"_top\"&gt;&lt;Font Size=+1&gt;\n");
  fprintf(fp, "The Official Clandestine Homepage&lt;/A&gt; &lt;/P&gt;&lt;/CENTER&gt;&lt;/Font&gt;\n");

  fprintf(fp, "&lt;/body&gt;\n");
  fprintf(fp, "&lt;/html&gt;\n");
  fclose( fp ); 
  OPEN_RESERVE();
  } /*end if */ 
  return;
}/* end function */


</PRE></BODY></HTML>
