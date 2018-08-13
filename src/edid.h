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

enum interface {
	UNDEFINED,
	DVI,
	HDMI_A,
	HDMI_B,
	MDDI,
	DISPLAY_PORT
};

enum color_type {
	UNDEFINED_COLOR,
	MONOCHROME,
	RGB,
	OTHER_COLOR
};

enum stereo_type {
	NO_STEREO,
	FIELD_RIGHT,
	FIELD_LEFT,
	TWO_WAY_RIGHT_ON_EVEN,
	TWO_WAY_LEFT_ON_EVEN,
	FOUR_WAY_INTERLEAVED,
	SIDE_BY_SIDE
};

struct timing {
	int width;
	int height;
	int frequency;
};

struct detailed_timing {
	int pixel_clock;
	int h_addr;
	int h_blank;
	int h_sync;
	int h_front_porch;
	int v_addr;
	int v_blank;
	int v_sync;
	int v_front_porch;
	int width_mm;
	int height_mm;
	int right_border;
	int top_border;
	int interlaced;
	enum stereo_type stereo;

	int digital_sync;
	union {
		struct {
			int bipolar;
			int serrations;
			int sync_on_green;
		} analog;

		struct {
			int composite;
			int serrations;
			int negative_vsync;
			int negative_hsync;
		} digital;
	};
};

struct monitor_info {
	int checksum;
	char manufacturer_code[4];
	int product_code;
	unsigned int serial_number;

	int production_week;	/* -1 if not specified */
	int production_year;	/* -1 if not specified */
	int model_year;		/* -1 if not specified */

	int major_version;
	int minor_version;

	int is_digital;

	union {
		struct {
			int bits_per_primary;
			enum interface interface;
			int rgb444;
			int ycrcb444;
			int ycrcb422;
		} digital;

		struct {
			double video_signal_level;
			double sync_signal_level;
			double total_signal_level;

			int blank_to_black;

			int separate_hv_sync;
			int composite_sync_on_h;
			int composite_sync_on_green;
			int serration_on_vsync;
			enum color_type color_type;
		} analog;
	};

	int width_mm;		/* -1 if not specified */
	int height_mm;		/* -1 if not specified */
	double aspect_ratio;	/* -1.0 if not specififed */

	double gamma;		/* -1.0 if not specified */

	int standby;
	int suspend;
	int active_off;

	int srgb_is_standard;
	int preferred_timing_includes_native;
	int continuous_frequency;

	double red_x;
	double red_y;
	double green_x;
	double green_y;
	double blue_x;
	double blue_y;
	double white_x;
	double white_y;

	struct timing established[24];	/* Terminated by 0x0x0 */
	struct timing standard[8];

	/* If monitor has a preferred mode, it is the first one (whether it has,
	 * is determined by the preferred_timing_includes bit.
	 */
	int n_detailed_timings;
	struct detailed_timing detailed_timings[4];

	/* Optional product description */
	char dsc_serial_number[14];
	char dsc_product_name[14];
	char dsc_string[14];	/* Unspecified ASCII data */
};
int print_edid_heading(const char *prefix, const char *data, const char *postfix, int level);
int print_edid_str(const char *prefix, const char *data, const char *postfix, int level);
int print_edid_bool(const char *prefix, int data, const char *postfix, int level);
int print_edid_integer(const char *prefix, int data, const char *postfix, int level);
int print_edid_double(const char *prefix, double data, const char *postfix, int level);

struct monitor_info *edid_decode(const unsigned char *data);

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
