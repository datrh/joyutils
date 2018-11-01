/*
 * Serial joystick driver attach program.
 *  (c) 1998 David Thompson <dcthomp@mail.utexas.edu>
 *  (c) 1999 Vojtech Pavlik <vojtech@suse.cz>
 *
 * Sponsored by SuSE  
 */

/*
 * This program is used for attaching a serial line to a joystick
 * device, via a line discipline. 
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@suse.cz>, or by paper mail:
 * Vojtech Pavlik, Ucitelska 1576, Prague 8, 182 00 Czech Republic
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

struct joy_types {
	char name[16];
	int speed;
	int flags;
	int ldisc;
	int (*init)(int fd);
};

void setline(int fd, int speed, int flags)
{
	struct termios tio;

	tcgetattr(fd, &tio);

	tio.c_cflag = flags | HUPCL | CLOCAL | CREAD;
	tio.c_iflag = IGNBRK | IGNPAR;
	tio.c_oflag = 0;
	tio.c_lflag = 0;

	tio.c_cc[VMIN ] = 1;
	tio.c_cc[VTIME] = 0;
	tio.c_cc[VEOL ] = '\r';
	tio.c_cc[VERASE] = 0;
	tio.c_cc[VKILL] = 0;

	cfsetispeed(&tio, speed);
	cfsetospeed(&tio, speed);

	tcsetattr(fd, TCSANOW, &tio);
}

int readchar(int fd, unsigned char *c, int timeout)
{
	struct timeval tv;
	fd_set set;

	tv.tv_sec = 0;
	tv.tv_usec = timeout * 1000;

	FD_ZERO(&set);
	FD_SET(fd, &set);

	if (!select(fd+1, &set, NULL, NULL, &tv)) return -1;

	read(fd, c, 1);

	return 0;
}

int warrior_init(int fd)
{
	unsigned char c;

	if (readchar(fd, &c, 2000) || c != 'J') return -1;
	while (c != ')' - 0x20) if (readchar(fd, &c, 1000)) return -1;
	if (write(fd, "*S", 2) != 2) return -1;
	if (readchar(fd, &c, 1000) || c != '*' ||
            readchar(fd, &c, 1000) || c != 'S') return -1;
	setline(fd, B4800, CS8);

	return 0;
}

int sball4000_init(int fd)
{
	unsigned char c;

	if (readchar(fd, &c, 4000) || c != 0x11 ||
            readchar(fd, &c, 1000) || c != 0x0d ){
		fprintf(stderr, "This doesn't look like a Spaceball 4000 FLX.\n");
		return -1;
	    }

	sleep(2);			/* Wait a few seconds for the Spaceball to initialize. */

	if (write(fd,"YS\rM\r",5)!=5)	/* Set high-resolution & enable axis events */
		return -1;

	return 0;
}

struct joy_types joy_types[] = {

{ "--sball4",   B9600, CS8,                    12, sball4000_init },
{ "--spaceorb", B9600, CS8,                    15, NULL },
{ "--warrior",  B1200, CS7,                    13, warrior_init },
{ "--magellan", B9600, CS8 | CSTOPB | CRTSCTS, 14, NULL },
{ "", 0, 0 }

};

int main(int argc, char **argv)
{
	int ldisc;
	int type;
	int fd;

	if (argc < 2 || argc > 3 || !strcmp("--help", argv[1])) {
		puts("");
		puts("Usage: jsttach <mode> <device>");
		puts("");
		puts("Modes:");
		puts("  --sball4           SpaceBall 4000 FLX");
		puts("  --spaceorb         SpaceOrb 360 / SpaceBall Avenger");
		puts("  --warrior          WingMan Warrior");
		puts("  --magellan         Magellan / SpaceMouse");
		puts("");
		return 1;
	}
	
	for (type = 0; joy_types[type].speed; type++) {
		if (!strncasecmp(argv[1], joy_types[type].name, 16))
			break;
	}

	if (!joy_types[type].speed) {
		fprintf(stderr, "jsattach: invalid mode\n");
		return 1;
	}

	if ((fd = open(argv[2], O_RDWR|O_NOCTTY|O_NONBLOCK)) < 0) {
		perror("jsattach");
		return 1;
	}

	setline(fd, joy_types[type].speed, joy_types[type].flags);

	if (joy_types[type].init && joy_types[type].init(fd)) {
		fprintf(stderr, "jsattach: device initialization failed\n");
		return 1;
	}

	ldisc = joy_types[type].ldisc;
	if(ioctl(fd, TIOCSETD, &ldisc)) {
		fprintf(stderr, "jsattach: can't set line discipline\n"); 
		return 1;
	}

	while (1) sleep(60);

	ldisc = 0;
	ioctl(fd, TIOCSETD, &ldisc);
	close(fd);

	return 0;
}
