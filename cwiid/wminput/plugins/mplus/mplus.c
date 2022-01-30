/* Copyright (C) 2007 L. Donnie Smith <donnie.smith@gatech.edu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdlib.h>
#include <math.h>

#include "wmplugin.h"

#define PI	3.14159265358979323

static unsigned char info_init = 0;
static struct wmplugin_info info;
static struct wmplugin_data data;

static struct acc_cal acc_cal;

static int plugin_id;

wmplugin_info_t wmplugin_info;
wmplugin_init_t wmplugin_init;
wmplugin_exec_t wmplugin_exec;
static void process_acc(struct cwiid_motionplus_mesg *mesg);

static float Roll_Scale = 1.0;
static float Pitch_Scale = 1.0;
static float X_Scale = 1.0;
static float Y_Scale = 1.0;

struct wmplugin_info *wmplugin_info() {
	if (!info_init) {
		info.button_count = 0;
		info.axis_count = 4;
		info.axis_info[0].name = "Roll";
		info.axis_info[0].type = WMPLUGIN_ABS;
		info.axis_info[0].max  =  3141;
		info.axis_info[0].min  = -3141;
		info.axis_info[0].fuzz = 0;
		info.axis_info[0].flat = 0;
		info.axis_info[1].name = "Pitch";
		info.axis_info[1].type = WMPLUGIN_ABS;
		info.axis_info[1].max  =  1570;
		info.axis_info[1].min  = -1570;
		info.axis_info[1].fuzz = 0;
		info.axis_info[1].flat = 0;
		info.axis_info[2].name = "X";
		info.axis_info[2].type = WMPLUGIN_ABS | WMPLUGIN_REL;
		info.axis_info[2].max  =  16;
		info.axis_info[2].min  = -16;
		info.axis_info[2].fuzz = 0;
		info.axis_info[2].flat = 0;
		info.axis_info[3].name = "Y";
		info.axis_info[3].type = WMPLUGIN_ABS | WMPLUGIN_REL;
		info.axis_info[3].max  =  16;
		info.axis_info[3].min  = -16;
		info.axis_info[3].fuzz = 0;
		info.axis_info[3].flat = 0;
		info.param_count = 4;
		info.param_info[0].name = "Roll_Scale";
		info.param_info[0].type = WMPLUGIN_PARAM_FLOAT;
		info.param_info[0].ptr = &Roll_Scale;
		info.param_info[1].name = "Pitch_Scale";
		info.param_info[1].type = WMPLUGIN_PARAM_FLOAT;
		info.param_info[1].ptr = &Pitch_Scale;
		info.param_info[2].name = "X_Scale";
		info.param_info[2].type = WMPLUGIN_PARAM_FLOAT;
		info.param_info[2].ptr = &X_Scale;
		info.param_info[3].name = "Y_Scale";
		info.param_info[3].type = WMPLUGIN_PARAM_FLOAT;
		info.param_info[3].ptr = &Y_Scale;
		info_init = 1;
	}
	return &info;
}

int wmplugin_init(int id, cwiid_wiimote_t *wiimote)
{
	plugin_id = id;

	data.buttons = 0;
	data.axes[0].valid = 1;
	data.axes[1].valid = 1;

	if (cwiid_enable(wiimote, CWIID_FLAG_MOTIONPLUS))
		return -1;

	cwiid_enable(wiimote, CWIID_FLAG_MESG_IFC);

	if (wmplugin_set_rpt_mode(id, CWIID_RPT_EXT | CWIID_RPT_STATUS))
		return -1;

	return 0;
}

struct wmplugin_data *wmplugin_exec(int mesg_count, union cwiid_mesg mesg[], struct timespec *timestamp)
{
	int i;
	struct wmplugin_data *ret = NULL;

	for (i=0; i < mesg_count; i++) {
		switch (mesg[i].type) {
		case CWIID_MESG_MOTIONPLUS:
			process_acc(&mesg[i].motionplus_mesg);
			ret = &data;
			break;
		default:
			break;
		}
	}

	return ret;
}

#define NEW_AMOUNT 0.1
#define OLD_AMOUNT (1.0-NEW_AMOUNT)
double a_x = 0, a_y = 0, a_z = 0;

static void process_acc(struct cwiid_motionplus_mesg *mesg)
{

	int horiz = mesg->angle_rate[0] - 8090;
	int vert = mesg->angle_rate[2] - 8230;

	vert *= -1;

	data.axes[0].value = vert * Roll_Scale;
	data.axes[1].value = horiz * Pitch_Scale;

	if (abs(vert) > 100) {
		data.axes[2].valid = 1;
		data.axes[2].value = vert / (abs(vert) < 150 ? 100 : 200);
	} else
		data.axes[2].valid = 0;
	
	if (abs(horiz) > 100) {
		data.axes[3].valid = 1;
		data.axes[3].value = horiz / (abs(horiz) < 150 ? 100 : 200);
	} else
		data.axes[3].valid = 0;

}

