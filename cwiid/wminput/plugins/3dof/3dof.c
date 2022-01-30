/* Copyright (C) 2010 N. Patry <nicolas.patry@student.ecp.fr>
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

#include <math.h>
#include <time.h>
#include "quaternion.h"

#include "wmplugin.h"

#define PI	3.14159265358979323

static unsigned char info_init = 0;
static struct wmplugin_info info;
static struct wmplugin_data data;

static struct acc_cal acc_cal;
static struct motionplus_cal motionplus_cal;

static int plugin_id;

wmplugin_info_t wmplugin_info;
wmplugin_init_t wmplugin_init;
wmplugin_exec_t wmplugin_exec;
static void process_acc(struct cwiid_acc_mesg *mesg);
static void process_motionplus(struct cwiid_motionplus_mesg *mesg);

//User sensitivity scaling
static float Yaw_Scale = 1.0;
static float Roll_Scale = 1.0;
static float Pitch_Scale = 1.0;

struct quaternion orientation; //contains full orientation of wiimote orientation_motionplus * orientation_acc
struct quaternion orientation_acc;
struct quaternion orientation_motionplus; //contains the vector that support the axis of rotation, depends on psi and acc
struct quaternion g; // gravity

struct wmplugin_info *wmplugin_info() {
	if (!info_init) {
		info.button_count = 0;
        info.axis_count = 3;
        info.axis_info[0].name = "Roll";
        info.axis_info[0].type = WMPLUGIN_ABS | WMPLUGIN_REL;
        info.axis_info[0].max  = 3141;
        info.axis_info[0].min  = -3141;
        info.axis_info[0].fuzz = 0;
        info.axis_info[0].flat = 0;
        info.axis_info[1].name = "Yaw";
        info.axis_info[1].type = WMPLUGIN_ABS | WMPLUGIN_REL;
        info.axis_info[1].max  = 3141;
        info.axis_info[1].min  = -3141;
        info.axis_info[1].fuzz = 0;
        info.axis_info[1].flat = 0;
        info.axis_info[2].name = "Pitch";
        info.axis_info[2].type = WMPLUGIN_ABS | WMPLUGIN_REL;
        info.axis_info[2].max  = 1570;
        info.axis_info[2].min  = -1570;
        info.axis_info[2].fuzz = 0;
        info.axis_info[2].flat = 0;
        info.param_count = 3;
        info.param_info[0].name = "Roll_Scale";
        info.param_info[0].type = WMPLUGIN_PARAM_FLOAT;
        info.param_info[0].ptr = &Roll_Scale;
		info.param_info[1].name = "Yaw_Scale";
        info.param_info[1].type = WMPLUGIN_PARAM_FLOAT;
        info.param_info[1].ptr = &Yaw_Scale;
        info.param_info[2].name = "Pitch_Scale";
        info.param_info[2].type = WMPLUGIN_PARAM_FLOAT;
        info.param_info[2].ptr = &Pitch_Scale;

		info_init = 1;
	}
	return &info;
}

int wmplugin_init(int id, cwiid_wiimote_t *wiimote)
{
	plugin_id = id;

	motionplus_cal.zero[0] = 8048;
	motionplus_cal.zero[1] = 7945;
	motionplus_cal.zero[2] = 8584;
	data.buttons = 0;
	data.axes[0].valid = 1;
	data.axes[1].valid = 1;
	data.axes[2].valid = 1;

	g.r=0.;
	g.i=0.;
	g.j=0.;
	g.k=1.;

	orientation_motionplus=qidentity();
	orientation=qidentity();
	orientation_acc=qidentity();

	if (wmplugin_set_rpt_mode(id, CWIID_RPT_ACC | CWIID_RPT_MOTIONPLUS)) {
		return -1;
	}

	if(cwiid_enable(wiimote, CWIID_FLAG_MOTIONPLUS)) {
		wmplugin_err(id, "You need wiimotion plus to run this plugin");
		return -1;
	}


	if (cwiid_get_acc_cal(wiimote, CWIID_EXT_NONE, &acc_cal)) {
		wmplugin_err(id, "accelerometers calibration error");
		return -1;
	}

	return 0;
}

#define NEW_AMOUNT 0.1
#define OLD_AMOUNT (1.0-NEW_AMOUNT)

struct wmplugin_data *wmplugin_exec(int mesg_count, union cwiid_mesg mesg[], struct timespec* timestamp)
{
	(void)timestamp;

	double roll, yaw, pitch;
	int i;

	for (i=0; i < mesg_count; i++) {
		switch (mesg[i].type) {
		case CWIID_MESG_ACC:
			process_acc(&mesg[i].acc_mesg);
			break;
		case CWIID_MESG_MOTIONPLUS:
			process_motionplus(&mesg[i].motionplus_mesg);
			break;
		default:
			break;
		}
	}
	orientation = qmul(orientation_motionplus, orientation_acc);
	qtoangles(orientation, &roll, &yaw, &pitch);

//	wmplugin_err(plugin_id,"roll, yaw, pitch(%g,%g,%g) ", roll*180/PI, yaw*180/PI, pitch*180/PI);

	data.axes[0].value = roll * 1000 * Roll_Scale;
	data.axes[1].value = -yaw * 1000 * Yaw_Scale;
	data.axes[2].value = pitch * 1000 * Pitch_Scale;

//	wmplugin_err(plugin_id,"COMMANDS roll, yaw, pitch(%d,%d,%d) \n", data.axes[0].value, data.axes[1].value, data.axes[2].value);
	return &data;
}

double a_x = 0, a_y = 0, a_z = 0;

static void process_acc(struct cwiid_acc_mesg *mesg)
{


	a_x = (((double)mesg->acc[CWIID_X] - acc_cal.zero[CWIID_X]) /
	      (acc_cal.one[CWIID_X] - acc_cal.zero[CWIID_X]))
	      * NEW_AMOUNT + a_x*OLD_AMOUNT
	      ;
	a_y = (((double)mesg->acc[CWIID_Y] - acc_cal.zero[CWIID_Y]) /
	      (acc_cal.one[CWIID_Y] - acc_cal.zero[CWIID_Y]))
	      * NEW_AMOUNT + a_y*OLD_AMOUNT
	      ;
	a_z = (((double)mesg->acc[CWIID_Z] - acc_cal.zero[CWIID_Z]) /
	      (acc_cal.one[CWIID_Z] - acc_cal.zero[CWIID_Z]))
	      * NEW_AMOUNT + a_z*OLD_AMOUNT
	      ;

	struct quaternion acc;
	//Wiimote accelerometers use different axis
	acc.r=0;
	acc.i=-a_y;
	acc.j=a_x;
	acc.k=a_z;

	acc = qunit(acc);

	struct quaternion cross_product = qscal ( .5, qsub ( qmul ( acc, g ) , qmul( qconj(g) , qconj(acc) ) ) );
	if (qequals( acc , g ))
		cross_product = qidentity();
	else if(qequals( acc, qneg(g) ))
		cross_product = qneg(qidentity());
	double dot_product = qmul( acc, qconj(g) ).r;
	double angle = asin(qlen(cross_product));

	if (dot_product < 0)
		angle = ( (angle<0)?-1:1 )* (PI - angle);
	orientation_acc = qrotation (angle , qunit(cross_product) );
//	wmplugin_err(plugin_id, "angle %g dot_product %g", angle , dot_product);
//	wmplugin_err(plugin_id, "orientation_acc (%g,%g,%g,%g)", orientation_acc.r,orientation_acc.i,orientation_acc.j,orientation_acc.k);
}

#define MOTIONPLUS_THRESHOLD 0.5

static void process_motionplus(struct cwiid_motionplus_mesg *mesg)
{

	double rollVel, yawVel, pitchVel;
	rollVel = ( mesg->angle_rate[1] - motionplus_cal.zero[1])/20;
	// Todo not dummy calibration
	yawVel	= (mesg->angle_rate[2] - motionplus_cal.zero[2])/20; // in deg/s
	pitchVel = (mesg->angle_rate[0] - motionplus_cal.zero[0])/20;


//	wmplugin_err(plugin_id, "cal (%i,%i,%i)",mesg->angle_rate[0] ,mesg->angle_rate[1], mesg->angle_rate[2]);

	if(!mesg->low_speed[2])
		yawVel *= 5;
	if(!mesg->low_speed[0])
		pitchVel *=5;
	if(!mesg->low_speed[1])
		rollVel *=5;

	rollVel = (fabs(rollVel)>MOTIONPLUS_THRESHOLD)?rollVel:0;
	yawVel = (fabs(yawVel)>MOTIONPLUS_THRESHOLD)?yawVel:0;
	pitchVel = (fabs(pitchVel)>MOTIONPLUS_THRESHOLD)?pitchVel:0;

	//compute dq
	double roll, yaw, pitch;
	qtoangles(orientation, &roll, &yaw, &pitch);
	struct quaternion orientation_dt = qfromangles( roll + rollVel*.01*PI/180, yaw+ yawVel*.01*PI/180, pitch+ pitchVel*.01*PI/180);
	struct quaternion dorientation_motionplus = qsub( orientation_dt, orientation );

	//change basis of dq from local to absolute orientation.
	dorientation_motionplus = qmul( orientation_acc, qdiv( dorientation_motionplus, orientation_acc) );

	//integrate  dq
	orientation_motionplus = qadd( orientation_motionplus, dorientation_motionplus);

	//make orientation from motion plus only accounts for unknown dimension.
	orientation_motionplus = qnew_rotation_axis( qunit(orientation_motionplus), g);
}
