/*
 * Copyright 2007 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* Author: Soren Sandmann <sandmann@redhat.com> */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "edid.h"

static int is_edid_header(const unsigned char *edid)
{
	if (memcmp(edid, "\x00\xff\xff\xff\xff\xff\xff\x00", 8) == 0)
		return 1;

	return 0;
}

static void decode_lf_string(const unsigned char *s, int n_chars, char *result)
{
	for (int i = 0; i < n_chars; ++i) {
		if (s[i] == 0x0a) {
			*result++ = '\0';
			break;
		}

		if (s[i] == 0x00)
			/* Convert embedded 0's to spaces */
			*result++ = ' ';
		else
			*result++ = s[i];
	}
}

static void decode_display_descriptor(const unsigned char *desc, struct monitor_info *info)
{
	switch (desc[0x03]) {
	case 0xFC:
		decode_lf_string(desc + 5, 13, info->dsc_product_name);
		break;

	case 0xFF:
		decode_lf_string(desc + 5, 13, info->dsc_serial_number);
		break;

	case 0xFE:
		decode_lf_string(desc + 5, 13, info->dsc_string);
		break;

	case 0xFD:
		/* Range Limits */
		break;

	case 0xFB:
		/* Color Point */
		break;

	case 0xFA:
		/* Timing Identifications */
		break;

	case 0xF9:
		/* Color Management */
		break;

	case 0xF8:
		/* Timing Codes */
		break;

	case 0xF7:
		/* Established Timings */
		break;

	case 0x10:
		break;
	}
}

static void decode_descriptors(const unsigned char *edid, struct monitor_info *info)
{
	for (int i = 0; i < 4; ++i) {
		int index = 0x36 + i * 18;

		if (edid[index + 0] == 0x00 && edid[index + 1] == 0x00)
			decode_display_descriptor(edid + index, info);
	}
}

static void decode_checksum(const unsigned char *edid, struct monitor_info *info)
{
	unsigned char check = 0;

	for (int i = 0; i < 128; ++i)
		check += edid[i];

	info->checksum = check;
}

struct monitor_info *edid_decode(const unsigned char *edid, size_t len)
{
	struct monitor_info *info;

	if (!edid) {
		errno = EINVAL;
		return NULL;
	}
	if (len != 128) {
		errno = ENODATA;
		return NULL;
	}

	info = calloc(1, sizeof(struct monitor_info));
	if (!info)
		return NULL;

	decode_checksum(edid, info);

	if (!is_edid_header(edid)) {
		errno = ENOENT;
		return NULL;
	}

	decode_descriptors(edid, info);

	return info;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
