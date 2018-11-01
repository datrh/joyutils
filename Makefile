#
# Makefile for Linux joystick driver v1.2
#
# (c) 1998-1999 Vojtech Pavlik
#
# Sponsored by SuSE
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# Should you need to contact me, the author, you can do so either by
# e-mail - mail your message to <vojtech@suse.cz>, or by paper mail:
# Vojtech Pavlik, Ucitelska 1576, Prague 8, 182 00 Czech Republic
#
#
# Edit the options below to suit your needs
#

CC		= gcc

CPPFLAGS	= -I.#						# for joystick.h

CFLAGS		= -O2 -Wall -pipe -m486 -fomit-frame-pointer#	# i386
# CFLAGS	= -O2 -Wall -pipe -m68020 -ffixed-a2#		# m68k
# CFLAGS	= -O2 -Wall -pipe -fomit-frame-pointer -ffixed-8# Alpha
# CFLAGS	= -O2 -Wall -pipe#				# Generic

MODFLAGS	= -D__KERNEL__ -DMODULE#			# Normal modules
#MODFLAGS	= -D__KERNEL__ -DMODULE -DMODVERSIONS\
#		  -include /usr/src/linux/include/linux/modversions.h # Versioned modules

DRIVERS		= joy-analog.o joy-sidewinder.o joy-logitech.o\
		  joy-assassin.o joy-gravis.o joy-lightning.o\
		  joy-thrustmaster.o joy-creative.o joy-console.o\
		  joy-db9.o joy-turbografx.o joy-spaceorb.o\
		  joy-spaceball.o joy-magellan.o joy-warrior.o\
		  joy-pci.o#					# PC drivers
# DRIVERS	= joy-amiga.o#					# Amiga driver

MODDIR		= /lib/modules/preferred/misc#			# RedHat location
# MODDIR	= /lib/modules/current/misc#			# For some others?

PROGRAMS	= jstest jscal jsattach 

#
# Nothing should need to be changed below this line
#

JOYPATH		:= $(shell pwd)
JOYDIR		:= $(shell basename "$(JOYPATH)")
JOYVER		:= $(shell expr "$(JOYDIR)" : "joystick-\(.*\)")
JOYOPRE		:= $(shell if [ -f .prerelease ]; then cat .prerelease; else echo 0; fi)
JOYPRE		:= $(shell expr $(JOYOPRE) + 1)

compile: joystick.o $(DRIVERS) $(PROGRAMS)

install: compile
	mkdir -p $(MODDIR)
	rm -f $(MODDIR)/joy*.o
	cp joy*.o $(MODDIR)
	/sbin/depmod -a
	install -m 644 joystick.h /usr/include/linux
	install -m 755 jscal /usr/local/bin
	install -m 755 jstest /usr/local/bin
	install -m 755 jsattach /usr/local/bin
	install -m 644 jscal.1 /usr/local/man/man1
	install -m 644 jstest.1 /usr/local/man/man1
	install -m 644 jsattach.1 /usr/local/man/man1

joystick.o: joystick.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -DEXPORT_SYMTAB -c joystick.c -o joystick.o

joy-analog.o: joy-analog.c joy-analog.h joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-analog.c -o joy-analog.o

joy-sidewinder.o: joy-sidewinder.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-sidewinder.c -o joy-sidewinder.o

joy-logitech.o: joy-logitech.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-logitech.c -o joy-logitech.o

joy-gravis.o: joy-gravis.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-gravis.c -o joy-gravis.o

joy-assassin.o: joy-assassin.c joy-analog.h joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-assassin.c -o joy-assassin.o

joy-thrustmaster.o: joy-thrustmaster.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-thrustmaster.c -o joy-thrustmaster.o

joy-creative.o: joy-creative.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-creative.c -o joy-creative.o

joy-lightning.o: joy-lightning.c joy-analog.h joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-lightning.c -o joy-lightning.o

joy-pci.o: joy-pci.c joy-analog.h joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-pci.c -o joy-pci.o

joy-amiga.o: joy-amiga.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-amiga.c -o joy-amiga.o

joy-console.o: joy-console.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-console.c -o joy-console.o

joy-db9.o: joy-db9.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-db9.c -o joy-db9.o

joy-turbografx.o: joy-turbografx.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-turbografx.c -o joy-turbografx.o

joy-spaceorb.o: joy-spaceorb.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-spaceorb.c -o joy-spaceorb.o

joy-spaceball.o: joy-spaceball.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-spaceball.c -o joy-spaceball.o

joy-magellan.o: joy-magellan.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-magellan.c -o joy-magellan.o

joy-warrior.o: joy-warrior.c joystick.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joy-warrior.c -o joy-warrior.o

joydump: joydump.o
	
joydump.o: joydump.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c joydump.c -o joydump.o

legacy.o: legacy.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MODFLAGS) -c legacy.c -o legacy.o

jscal: jscal.o
	$(CC) $(cflags) $(cppflags) -lm $^ -o $@

jstest: jstest.o
	$(CC) $(cflags) $(cppflags) $^ -o $@

devs:
	-$(RM) /dev/js0 /dev/js1 /dev/js2 /dev/js3
	mknod /dev/js0 c 15 0
	mknod /dev/js1 c 15 1
	mknod /dev/js2 c 15 2
	mknod /dev/js3 c 15 3

clean:
	-$(RM) *.o *.swp $(PROGRAMS) *.orig *.rej map

dist: clean
	( \
	  rm -f *.diff ;\
	  cd .. ;\
	  mkdir -p arch ;\
	  rm -f arch/$(JOYDIR).tar.gz ;\
	  tar czf arch/$(JOYDIR).tar.gz $(JOYDIR) ;\
	)

bumprel: clean
	echo $(JOYPRE) > .prerelease

pre: compile bumprel dist
	( \
	  cd .. ;\
	  mv arch/$(JOYDIR).tar.gz arch/devel/joystick-pre-$(JOYVER)-$(JOYPRE).tar.gz ;\
	  scp arch/devel/joystick-pre-$(JOYVER)-$(JOYPRE).tar.gz atrey:joystick/devel ;\
	  scp arch/devel/joystick-pre-$(JOYVER)-$(JOYPRE).tar.gz suse-ftp:joystick/devel ;\
	)

relclean: clean
	$(RM) .prerelease

release: compile relclean dist
	( \
	  echo 0 > .prerelease ;\
	  cd .. ;\
	  scp arch/$(JOYDIR).tar.gz atrey:joystick ;\
	  scp arch/$(JOYDIR).tar.gz suse-ftp:joystick ;\
	  ssh atrey rm -f "joystick/devel/*" &\
	  ssh suse-ftp rm -f "joystick/devel/*" &\
	)

jstest.o: jstest.c joystick.h
jscal.o: jscal.c joystick.h
jsattach.o: jsattach.c
