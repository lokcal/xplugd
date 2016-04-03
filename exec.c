/* Script exec handling
 *
 * Copyright (C) 2016  Joachim Nilsson <troglobit@gmail.com>
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

#include "xplugd.h"

static Display *display = NULL;

static void catch_child(int sig)
{
	(void)sig;
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
}

int exec_init(Display *dpy)
{
	display = dpy;
	signal(SIGCHLD, catch_child);

	return 0;
}

int exec(char *type, char *device, char *status, char *name)
{
	syslog(LOG_DEBUG, "Calling %s %s %s %s %s", cmd, type, device, status, name ? name : "");

	if (!fork()) {
		char *args[] = {
			cmd,
			type,
			device,
			status,
			name ? name : "",
			NULL
		};

		setsid();
		if (display)
			close(ConnectionNumber(display));

		execv(args[0], args);
		syslog(LOG_ERR, "Failed calling %s: %s", cmd, strerror(errno));
		exit(0);
	}

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
