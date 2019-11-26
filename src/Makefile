#I use distcc to make. Way faster.  Change distcc to gcc to fix.
#No more distcc 010409
CC      = gcc 
PROF    = -O -ggdb
NOCRYPT = -DNOCRYPT
C_FLAGS =  $(M_FLAGS) -DIMC -DIMCROM -Wall -DOLD_RAND $(PROF) $(NOCRYPT)
L_FLAGS =  $(PROF)
M_FLAGS =  -ansi -Dunix
.PHONY: clean build all


SRC = *.c
INC = *.h


all::	 build 
	 make -s mindor

rcs:: 	diff 
	checkin

#This was just an idea I had for keeping all user files up to date.
#Just comment this out if ya don't want to use it.
#Or just never type "make rcs"
#I developed this game on FreeBSD using RCS/CVS.  Remove/uncomment if you don't 
#wish to use this.
checkin: 
	ci -l $(SRC) $(INC)
	cp ./RCS/*.c,v /home/mcbride/public_html/Mindor/CodeVersion
	cp ./RCS/*.h,v /home/mcbride/public_html/Mindor/CodeVersion


diff:
	rcsdiff -c  $(SRC) $(INC) > mindor.diff 

build:
	@if [ -f .build ]; then \
	    expr `cat .build` + 1 > .build; \
	else \
	    echo '1' > .build; \
	fi
	@if [ -f .version ]; then \
	    expr `cat .version` + 1 > .version; \
	else \
	    echo '10001' > .version; \
	fi
	@if [ `cat .build` -ge 51 ]; then \
	    echo '1' > .build; \
	fi
	@echo '#ifndef BUILD_H' > build.h
	@echo '#define BUILD_H' >> build.h
	@echo "#define LINKCOUNT   `cat .build`" >> build.h
	@echo '#endif' >> build.h
	@echo '#ifndef VERSION_H' > version.h
	@echo '#define VERSION_H' >> version.h
	@echo "#define VERSION   `cat .version`" >> version.h
	@echo '#endif' >> version.h
	@echo "Build number `cat .build`, version `cat .version` set."


#IMC2 - Comment out to disable IMC2 support
IMC = 1

C_FILES = act_comm.c act_enter.c act_info.c act_move.c act_obj.c act_wiz.c \
          alias.c ban.c clans.c comm.c const.c curse.c db.c db2.c effects.c \
	  fight.c finger.c flags.c forget.c handler.c healer.c interp.c note.c \
	  lookup.c magic.c magic2.c music.c recycle.c repent.c save.c scan.c \
	  sign.c skills.c special.c tables.c update.c wizlist.c \
          mob_cmds.c mob_prog.c olc.c olc_act.c olc_save.c bit.c mem.c \
          string.c olc_mpcode.c spellup.c  check.c whoweb.c webspells.c \
          homes.c ftp.c rebirth.c petname.c hometown.c stat.c \
          timer.c shops.c act_move2.c social-edit.c relationship.c quest.c \
   	  room_map.c dice.c bladecast.c


O_FILES = $(subst %.c,o/%.o,$(C_FILES))

H_FILES = $(wildcard *.h)

mindor: $(O_FILES)
	rm -f mindor
	$(CC) $(L_FLAGS) -o mindor $(O_FILES)
	echo "Done compiling mindor...";

o/%.o: %.c
	echo "  Now compiling... $@";
	$(CC) -c $(C_FLAGS) $< -o $@

clean:
	rm -f o/*.o mindor $(MINDOR_FILES)
