/* xplugd - monitor/keyboard/mouse plug/unplug helper
 *
 * Copyright (C) 2012-2015  Stefan Bolte <portix@gmx.net>
 * Copyright (C) 2013-2015  Andrew Shadura <andrewsh@debian.org>
 * Copyright (C) 2016       Joachim Nilsson <troglobit@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define SYSLOG_NAMES
#include "xplugd.h"

int   loglevel = LOG_NOTICE;
char *cmd;


static int loglvl(char *level)
{
	for (int i = 0; prioritynames[i].c_name; i++) {
		if (!strcmp(prioritynames[i].c_name, level))
			return prioritynames[i].c_val;
	}

	return atoi(level);
}

static int error_handler(void)
{
	exit(1);
}

static int usage(int status)
{
	printf("Usage: %s [OPTIONS] script\n\n"
	       "Options:\n"
	       "  -h        Print this help text and exit\n"
	       "  -l LEVEL  Set log level: none, err, info, notice*, debug\n"
	       "  -n        Run in foreground, do not fork to background\n"
	       "  -s        Use syslog, even if running in foreground, default w/o -n\n"
	       "  -v        Show program version\n\n"
	       "Copyright (C) 2012-2015 Stefan Bolte\n"
	       "Copyright (C)      2016 Joachim Nilsson\n\n"
	       "Bug report address: https://github.com/troglobit/xplugd/issues\n\n", __progname);
	return status;
}

static int version(void)
{
	printf("v%s\n", VERSION);
	return 0;
}

int main(int argc, char *argv[])
{
	int c, log_opts = LOG_CONS | LOG_PID;
	XEvent ev;
	Display *dpy;
	int background = 1, logcons = 0;
	uid_t uid;

	while ((c = getopt(argc, argv, "hl:nsv")) != EOF) {
		switch (c) {
		case 'h':
			return usage(0);

		case 'l':
			loglevel = loglvl(optarg);
			break;

		case 'n':
			background = 0;
			logcons++;
			break;

		case 's':
			logcons--;
			break;

		case 'v':
			return version();

		default:
			return usage(1);
		}
	}

	if (optind >= argc)
		return usage(1);
	cmd = argv[optind];

	if (((uid = getuid()) == 0) || uid != geteuid()) {
		fprintf(stderr, "%s may not run as root\n", __progname);
		exit(1);
	}

	if ((dpy = XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "Cannot open display\n");
		exit(1);
	}

	if (background) {
		if (daemon(0, 0)) {
			fprintf(stderr, "Failed backgrounding %s: %s", __progname, strerror(errno));
			exit(1);
		}
	}
	if (logcons > 0)
		log_opts |= LOG_PERROR;

	openlog(NULL, log_opts, LOG_USER);
	setlogmask(LOG_UPTO(loglevel));

	exec_init(dpy);
	input_init(dpy);
	randr_init(dpy);
	XSync(dpy, False);
	XSetIOErrorHandler((XIOErrorHandler)error_handler);

	while (1) {
		XNextEvent(dpy, &ev);

		if (is_input_event(dpy, &ev))
			input_event(dpy, &ev);
		else
			randr_event(dpy, &ev);
	}

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
