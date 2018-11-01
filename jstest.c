/*
 * jstest.c  Version 1.2
 *
 * Copyright (c) 1996-1999 Vojtech Pavlik
 *
 * Sponsored by SuSE
 */

/*
 * This program can be used to test all the features of the Linux
 * joystick API, including non-blocking and select() access, as
 * well as version 0.x compatibility mode. It is also intended to
 * serve as an example implementation for those who wish to learn
 * how to write their own joystick using applications.
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

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <linux/joystick.h>

#define NAME_LENGTH 128

int main (int argc, char **argv)
{
	int fd;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";

	if (argc < 2 || argc > 3 || !strcmp("--help", argv[1])) {
		puts("");
		puts("Usage: jstest [<mode>] <device>");
		puts("");
		puts("Modes:");
		puts("  --normal           One-line mode showing immediate status");
		puts("  --old              Same as --normal, using 0.x interface");
		puts("  --event            Prints events as they come in");
		puts("  --nonblock         Same as --event, in nonblocking mode");
		puts("  --select           Same as --event, using select() call");
		puts("");
		exit(1);
	}
	if ((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
		perror("jstest");
		exit(1);
	}

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

	printf("Joystick (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
		name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);
	printf("Testing ... (interrupt to exit)\n");

/*
 * Old (0.x) interface.
 */

	if ((argc == 2 && version < 0x010000) || !strcmp("--old", argv[1])) {

		struct JS_DATA_TYPE js;

		while (1) {

			if (read(fd, &js, JS_RETURN) != JS_RETURN) {
				perror("\njstest: error reading");
				exit(1);
			}

			printf("Axes: X:%3d Y:%3d Buttons: A:%s B:%s\r",
				js.x, js.y, (js.buttons & 1) ? "on " : "off", (js.buttons & 2) ? "on " : "off");

			fflush(stdout);

			usleep(10000);
		}
	}

/*
 * Event interface, single line readout.
 */

	if (argc == 2 || !strcmp("--normal", argv[1])) {

		int *axis;
		int *button;
		int i;
		struct js_event js;

		axis = calloc(axes, sizeof(int));
		button = calloc(buttons, sizeof(char));

		while (1) {
			if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
				perror("\njstest: error reading");
				exit (1);
			}

			switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				break;
			case JS_EVENT_AXIS:
				axis[js.number] = js.value;
				break;
			}

			printf("\r");

			if (axes) {
				printf("Axes: ");
				for (i = 0; i < axes; i++)
					printf("%2d:%6d ", i, axis[i]);
			}

			if (buttons) {
				printf("Buttons: ");
				for (i = 0; i < buttons; i++)
					printf("%2d:%s ", i, button[i] ? "on " : "off");
			}

			fflush(stdout);
		}
	}


/*
 * Event interface, events being printed.
 */

	if (!strcmp("--event", argv[1])) {

		struct js_event js;

		while (1) {
			if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
				perror("\njstest: error reading");
				exit (1);
			}

			printf("Event: type %d, time %d, number %d, value %d\n",
				js.type, js.time, js.number, js.value);

		}
	}

/*
 * Reading in nonblocking mode.
 */

	if (!strcmp("--nonblock", argv[1])) {

		struct js_event js;

		fcntl(fd, F_SETFL, O_NONBLOCK);

		while (1) {

			while (read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event))  {
				printf("Event: type %d, time %d, number %d, value %d\n",
					js.type, js.time, js.number, js.value);
			}

			if (errno != EAGAIN) {
				perror("\njstest: error reading");
				exit (1);
			}

			usleep(10000);
		}
	}

/*
 * Using select() on joystick fd.
 */

	if (!strcmp("--select", argv[1])) {

		struct js_event js;
		struct timeval tv;
		fd_set set;

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		while (1) {

			FD_ZERO(&set);
			FD_SET(fd, &set);

			if (select(fd+1, &set, NULL, NULL, &tv)) {

				if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
					perror("\njstest: error reading");
					exit (1);
				}

				printf("Event: type %d, time %d, number %d, value %d\n",
					js.type, js.time, js.number, js.value);

			}

		}
	}

	printf("jstest: unknown mode: %s\n", argv[1]);
	return -1;
}
