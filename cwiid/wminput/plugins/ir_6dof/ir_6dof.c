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

#include <math.h>
#include <time.h>

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
static void process_ir(struct cwiid_ir_mesg *mesg);

//All distances are in pixel size
static const float CWIID_LEDBAR_SIZE = 1000; //Todo dummy value
static const float CWIID_FOCAL_LENGTH = 1280;
static struct cwiid_ir_src ccd_center;
static float Yaw_Scale = 1.0;
static float Roll_Scale = 1.0;
static float Pitch_Scale = 1.0;
static float X_Scale = 1.0;
static float Y_Scale = 1.0;
static float Z_Scale = 1.0;
double PITCH_STARTGYRO = 0.0;
double ROLL_STARTGYRO = 0.0;

static double roll = 0 , yaw = 0, pitch = 0; //These are the absolute roll yaw and pitch not the commands.
static double x = 0, y = 0, z = 0;
double rollVel=0, yawVel=0, pitchVel=0;
int do_process = 1, not_moving =1;
static struct cwiid_ir_src led1;
static struct cwiid_ir_src led2;

double sigmoid (double x){
	return 1/(1+exp(-x));
}

struct wmplugin_info *wmplugin_info() {
	if (!info_init) {
		info.button_count = 0;
        info.axis_count = 6;
        info.axis_info[0].name = "X";
        info.axis_info[0].type = WMPLUGIN_ABS | WMPLUGIN_REL;
        info.axis_info[0].max  = 16;
        info.axis_info[0].min  = -16;
        info.axis_info[0].fuzz = 0;
        info.axis_info[0].flat = 0;
        info.axis_info[1].name = "Y";
        info.axis_info[1].type = WMPLUGIN_ABS | WMPLUGIN_REL;
        info.axis_info[1].max  = 16;
        info.axis_info[1].min  = -16;
        info.axis_info[1].fuzz = 0;
        info.axis_info[1].flat = 0;
        info.axis_info[2].name = "Z";
        info.axis_info[2].type = WMPLUGIN_ABS | WMPLUGIN_REL;
        info.axis_info[2].max  = 50;
        info.axis_info[2].min  = 16;
        info.axis_info[2].fuzz = -16;
        info.axis_info[2].flat = 0;
        info.axis_info[3].name = "Roll";
        info.axis_info[3].type = WMPLUGIN_ABS | WMPLUGIN_REL;
        info.axis_info[3].max  = 16;
        info.axis_info[3].min  = -16;
        info.axis_info[3].fuzz = 0;
        info.axis_info[3].flat = 0;
        info.axis_info[4].name = "Yaw";
        info.axis_info[4].type = WMPLUGIN_ABS | WMPLUGIN_REL;
        info.axis_info[4].max  = 16;
        info.axis_info[4].min  = -16;
        info.axis_info[4].fuzz = 0;
        info.axis_info[4].flat = 0;
        info.axis_info[5].name = "Pitch";
        info.axis_info[5].type = WMPLUGIN_ABS | WMPLUGIN_REL;
        info.axis_info[5].max  = 16;
        info.axis_info[5].min  = -16;
        info.axis_info[5].fuzz = 0;
        info.axis_info[5].flat = 0;
        info.param_count = 6;
        info.param_info[0].name = "X_Scale";
        info.param_info[0].type = WMPLUGIN_PARAM_FLOAT;
        info.param_info[0].ptr = &X_Scale;
        info.param_info[1].name = "Y_Scale";
        info.param_info[1].type = WMPLUGIN_PARAM_FLOAT;
        info.param_info[1].ptr = &Y_Scale;
		info.param_info[2].name = "Z_Scale";
        info.param_info[2].type = WMPLUGIN_PARAM_FLOAT;
        info.param_info[2].ptr = &Z_Scale;
        info.param_info[3].name = "Roll_Scale";
        info.param_info[3].type = WMPLUGIN_PARAM_FLOAT;
        info.param_info[3].ptr = &Roll_Scale;
		info.param_info[4].name = "Yaw_Scale";
        info.param_info[4].type = WMPLUGIN_PARAM_FLOAT;
        info.param_info[4].ptr = &Yaw_Scale;
        info.param_info[5].name = "Pitch_Scale";
        info.param_info[5].type = WMPLUGIN_PARAM_FLOAT;
        info.param_info[5].ptr = &Pitch_Scale;

		info_init = 1;
	}
	return &info;
}

int wmplugin_init(int id, cwiid_wiimote_t *wiimote)
{
	plugin_id = id;

	ccd_center.valid=1;
	ccd_center.size=1;
	ccd_center.pos[0]=512;
	ccd_center.pos[1]=384;
	motionplus_cal.zero[0] = 8050;
	motionplus_cal.zero[1] = 8000;
	motionplus_cal.zero[2] = 8580;
	data.buttons = 0;
	data.axes[0].valid = 1;
	data.axes[1].valid = 1;
	data.axes[2].valid = 1;
	data.axes[3].valid = 1;
	data.axes[4].valid = 1;
	data.axes[4].value = 0;
	data.axes[5].valid = 1;

	if (wmplugin_set_rpt_mode(id, CWIID_RPT_ACC | CWIID_RPT_MOTIONPLUS | CWIID_RPT_IR | CWIID_RPT_BTN)) {
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

struct wmplugin_data *wmplugin_exec(int mesg_count, union cwiid_mesg mesg[], struct timespec* timestamp)
{
   (void) timestamp;
	double old_x=x;
	double old_y=y;
	double old_z=z;
	double old_roll = roll;
	double old_yaw = yaw;
	double old_pitch = pitch;
	int i;
	struct wmplugin_data *ret = NULL;

	for (i=0; i < mesg_count; i++) {
		switch (mesg[i].type) {
		case CWIID_MESG_ACC:
			process_acc(&mesg[i].acc_mesg);
			ret = &data;
			break;
		case CWIID_MESG_MOTIONPLUS:
			process_motionplus(&mesg[i].motionplus_mesg);
			ret = &data;
		break;
		case CWIID_MESG_IR:
			process_ir(&mesg[i].ir_mesg);
			ret = &data;
			break;
		case CWIID_MESG_BTN:
			if( do_process && mesg[i].btn_mesg.buttons == CWIID_BTN_A)
				do_process=0;
			else
				do_process=1;
		default:
			break;
		}
	}

	if(roll>PI)
		roll = PI;
	else if(roll<-PI)
		roll = -PI;
	if(yaw>PI)
		yaw = PI;
	else if(yaw<-PI)
		yaw = -PI;
	if(pitch>PI/2)
		pitch = PI/2;
	else if(pitch<-PI/2)
		pitch = -PI/2;


	wmplugin_err(plugin_id,"XYZ(%g,%g,%g) roll, yaw, pitch(%g,%g,%g) \n", x, y, z, roll*180/PI, yaw*180/PI, pitch*180/PI);
	wmplugin_err(plugin_id,"XYZ(%g,%g,%g) roll, yaw, pitch(%g,%g,%g) \n", old_x, old_y, old_z, old_roll*180/PI, old_yaw*180/PI, old_pitch*180/PI);

	if (do_process){
		for (i=0 ;i<6;i++)
				data.axes[i].valid=1;

		data.axes[0].value = (old_x-x) /10/ X_Scale;
		data.axes[1].value = (y-old_y) /10/ Y_Scale;
		data.axes[2].value = (old_z-z) /10/ Z_Scale;
//		data.axes[3].value = (roll- old_roll) *180/PI /Roll_Scale;
		data.axes[3].value = rollVel /10 /Roll_Scale;
		data.axes[4].value = yawVel /10 /Yaw_Scale;
		data.axes[5].value = pitchVel /10 /Pitch_Scale;
//		data.axes[5].value = (pitch- old_pitch) *180/PI /Pitch_Scale;

		wmplugin_err(plugin_id,"COMMANDS XYZ(%d,%d,%d) roll, yaw, pitch(%d,%d,%d) \n", data.axes[0].value, data.axes[1].value, data.axes[2].value, data.axes[3].value, data.axes[4].value, data.axes[5].value);

	}else
		for (i=0 ;i<6;i++)
			data.axes[i].valid=0;
	return ret;
}

#define NEW_AMOUNT 0.1
#define OLD_AMOUNT (1.0-NEW_AMOUNT)
double a_x = 0, a_y = 0, a_z = 0;

static void process_acc(struct cwiid_acc_mesg *mesg)
{
	double a;

//	if( rollVel != 0 || pitchVel!=0 || yawVel!=0)
//		return;

	a_x = (((double)mesg->acc[CWIID_X] - acc_cal.zero[CWIID_X]) /
	      (acc_cal.one[CWIID_X] - acc_cal.zero[CWIID_X]))*NEW_AMOUNT +
	      a_x*OLD_AMOUNT;
	a_y = (((double)mesg->acc[CWIID_Y] - acc_cal.zero[CWIID_Y]) /
	      (acc_cal.one[CWIID_Y] - acc_cal.zero[CWIID_Y]))*NEW_AMOUNT +
	      a_y*OLD_AMOUNT;
	a_z = (((double)mesg->acc[CWIID_Z] - acc_cal.zero[CWIID_Z]) /
	      (acc_cal.one[CWIID_Z] - acc_cal.zero[CWIID_Z]))*NEW_AMOUNT +
	      a_z*OLD_AMOUNT;

	a = sqrt(pow(a_x,2)+pow(a_y,2)+pow(a_z,2));
	roll = atan(a_x/a_z);
	if (a_z <= 0.0) {
		roll += PI * ((a_x > 0.0) ? 1 : -1);
	}

	pitch = atan(a_y/a_z*cos(roll));

	roll *= 1-sigmoid(rollVel - ROLL_STARTGYRO);
	pitch *= 1-sigmoid(pitchVel - PITCH_STARTGYRO);


}

#define NEW_AMOUNT 0.1
#define OLD_AMOUNT (1.0-NEW_AMOUNT)

static void process_motionplus(struct cwiid_motionplus_mesg *mesg)
{

	rollVel = (- mesg->angle_rate[1] + motionplus_cal.zero[1])/20;
	// Todo not dummy calibration
	yawVel	= (-mesg->angle_rate[2] + motionplus_cal.zero[2])/20; // in deg/s
	pitchVel = (mesg->angle_rate[0] - motionplus_cal.zero[0])/20;

	if(!mesg->low_speed[2])
		yawVel *= 5;
	if(!mesg->low_speed[0])
		pitchVel *=5;
	if(!mesg->low_speed[1])
		rollVel *=5;

	rollVel = (fabs(rollVel)>0.5)?rollVel:0;
	yawVel = (fabs(yawVel)>0.5)?yawVel:0;
	pitchVel = (fabs(pitchVel)>0.5)?pitchVel:0;

//	if( fabs(rollVel-old_rollVel)<20 &&  fabs(yawVel-old_yawVel)<20 &&  fabs(pitchVel-old_pitchVel)<20 ){
//
//		motionplus_cal.zero[0] = mesg->angle_rate[0]*NEW_AMOUNT +motionplus_cal.zero[0]*OLD_AMOUNT;
//		motionplus_cal.zero[1] = mesg->angle_rate[1]*NEW_AMOUNT +motionplus_cal.zero[1]*OLD_AMOUNT;
//		motionplus_cal.zero[2] = mesg->angle_rate[2]*NEW_AMOUNT +motionplus_cal.zero[2]*OLD_AMOUNT;
//		wmplugin_err(plugin_id," Changed cal (%i,%i,%i)", motionplus_cal.zero[0], motionplus_cal.zero[1], motionplus_cal.zero[2]);
//	}

//	SAMPLE RATE NEEDS TO BE STABLE.. we chose 10ms
	wmplugin_err(plugin_id, " UPDATED PITCH %g", sigmoid(pitchVel - PITCH_STARTGYRO)*.01*PI/180);
	roll += sigmoid(rollVel - ROLL_STARTGYRO)*.01*PI/180;
	yaw	  += yawVel*.01*PI/180;
	pitch += sigmoid(pitchVel - PITCH_STARTGYRO)*.01*PI/180;
}

static void process_ir(struct cwiid_ir_mesg *mesg)
{
	int src_index1 = -1;
	int src_index2 = -1;
	int i = 0;
	double distance,ir_size,new_ir_size;

	//detect 2 brightest points (LED bar)
	for (i=0; i < CWIID_IR_SRC_COUNT; i++) {
		if (mesg->src[i].valid) {
			if ((src_index1 == -1) ||
			  (mesg->src[i].size > mesg->src[src_index1].size)) {
				src_index1 = i;
			}else if ((src_index2 == -1) ||
			  (mesg->src[i].size > mesg->src[src_index2].size)) {
				src_index2 = i;
			}
		}
	}
	if ( src_index1 == -1 || src_index2 == -1 || !data.axes[4].valid){
		wmplugin_err(plugin_id,"Not enough LEDs");
		return;
	}

	//computing distance of sources on virtual screen
	ir_size = 	sqrt(pow(mesg->src[src_index1].pos[0] - mesg->src[src_index2].pos[0],2)
				+  		pow(mesg->src[src_index1].pos[1] - mesg->src[src_index2].pos[1],2));

	if (ir_size==0)
			return;
	// D = f (L' / l + 1)
	distance = CWIID_FOCAL_LENGTH * CWIID_LEDBAR_SIZE / ir_size;

	//TODO Transform Infrared dots on {real screen} to {virtual
	// roll=0, yaw=0, pitch=0 screen}.
	static struct cwiid_ir_src virtual_led1;
	static struct cwiid_ir_src virtual_led2;
	//Remove Roll first
	virtual_led1.size=mesg->src[src_index1].size;
	virtual_led1.pos[0] = mesg->src[src_index1].pos[0]*cos(-roll) +  mesg->src[src_index1].pos[1]*sin(roll);
	virtual_led1.pos[1] = mesg->src[src_index1].pos[0]*sin(-roll) +  mesg->src[src_index1].pos[1]*cos(-roll);
	virtual_led2.size=mesg->src[src_index2].size;
	virtual_led2.pos[0] = mesg->src[src_index2].pos[0]*cos(-roll) +  mesg->src[src_index2].pos[1]*sin(roll);
	virtual_led2.pos[1] = mesg->src[src_index2].pos[0]*sin(-roll) +  mesg->src[src_index2].pos[1]*cos(-roll);

	//Check led image is flat (not y component left), if not correct error in roll
	if (virtual_led1.pos[0]-virtual_led2.pos[0] == 0)
		wmplugin_err(plugin_id," Warning invalid roll correction (have 90 deg, roll %g)",
			roll*180/PI);
	else if(fabs(atan((virtual_led1.pos[1]-virtual_led2.pos[1])/(virtual_led1.pos[0]-virtual_led2.pos[0]))*180/PI) > 5)
		//TODO correction if false
		wmplugin_err(plugin_id," Warning invalid roll correction (have %g deg, roll %g) ",
			fabs(atan((virtual_led1.pos[1]-virtual_led2.pos[1])/(virtual_led1.pos[0]-virtual_led2.pos[0]))*180/PI),roll*180/PI,
			virtual_led1.pos[0],virtual_led1.pos[1],virtual_led2.pos[0],virtual_led2.pos[1]);

	//Now remove yaw and pitch at the same time, it's easy they are independant now.
	virtual_led1.pos[0] = CWIID_FOCAL_LENGTH*tan(atan(virtual_led1.pos[0]/CWIID_FOCAL_LENGTH)-yaw);
	virtual_led1.pos[1] = CWIID_FOCAL_LENGTH*tan(atan(virtual_led1.pos[1]/CWIID_FOCAL_LENGTH)-pitch);
	virtual_led2.pos[0] = CWIID_FOCAL_LENGTH*tan(atan(virtual_led2.pos[0]/CWIID_FOCAL_LENGTH)-yaw);
	virtual_led2.pos[1] = CWIID_FOCAL_LENGTH*tan(atan(virtual_led2.pos[1]/CWIID_FOCAL_LENGTH)-pitch);

	new_ir_size = sqrt(pow(virtual_led1.pos[0] - virtual_led2.pos[0],2)
						+  		pow(virtual_led1.pos[1] - virtual_led2.pos[1],2));

	if(fabs(new_ir_size-ir_size)>50){ //check we didnt' screw anything with our corrections
		wmplugin_err(plugin_id," Warning invalid correction (difference in size %g) ir_size:%g , new:%g \n old:(%i,%i) (%i,%i) \nnew:(%i,%i) (%i,%i)"
				,fabs(new_ir_size-ir_size),ir_size,new_ir_size,
				led1.pos[0],led1.pos[1],led2.pos[0],led2.pos[1],
				virtual_led1.pos[0],virtual_led1.pos[1],virtual_led2.pos[0],virtual_led2.pos[1]);
		return;
	}

	// Now we have corrected orientation, let's compute our position relative to led1
	x = CWIID_LEDBAR_SIZE/ir_size*(virtual_led1.pos[0]-ccd_center.pos[0]);
	y = CWIID_LEDBAR_SIZE/ir_size*(virtual_led1.pos[1]-ccd_center.pos[1]);
	z = sqrt( pow(distance,2) - pow(x,2) - pow(y,2));
}
