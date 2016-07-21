#include "include.h"
#include "height_ctrl.h"
#include "ultrasonic.h"
#include "mymath.h"
#include "filter.h"
#include "ctrl.h"
#include "rc.h"
#include "data_transfer.h"

#define ULTRA_SPEED 		 300    // mm/s
#define ULTRA_MAX_HEIGHT 1200   // mm
#define ULTRA_INT        300    // 积分幅度
#define EXP_Z_SPEED  ( 4.0f *my_deathzoom( (thr-500), 50 ) )

//_st_height_pid_v wz_speed_pid_v;
//_st_height_pid wz_speed_pid;

_st_height_pid_v  ultra_ctrl;
_st_height_pid  ultra_pid;

float height_ctrl_out;
float wz_acc;

float exp_height_speed,exp_height=1000;
float ultra_speed;
float ultra_dis_lpf;
float ultra_ctrl_out;
float current_height;
float height_old;
float ultra_pid_display_p=0,
	ultra_pid_display_i=0,
	ultra_pid_display_d=0,
	ultra_pid_display_pid=0;
//float Height[3];

u8 height_ctrl_flag_star=0;

float wz_speed,wz_speed_old;
float wz_acc_mms2;

void Ultra_PID_Init()  //PID初始化
{
	  ultra_pid.kp = 1.0f*pid_setup.groups.hc_height.kp;
	  ultra_pid.kd = 1.0f*pid_setup.groups.hc_height.kd;
  	ultra_pid.ki = 1.0f*pid_setup.groups.hc_height.ki;
}

/*void IncrementalPID(_st_height_pid *ultra_pid,float target,float measure)  //增量式
{ 
  ultra_pid->error=target-measure;
  ultra_pid->output=ultra_pid->kp*(ultra_pid->error-ultra_pid->preerror)+ultra_pid->ki*ultra_pid->error+ultra_pid->kd*(ultra_pid->error-2*ultra_pid->preerror+ultra_pid->prepreerror);
  ultra_pid->preerror=ultra_pid->error;
  ultra_pid->prepreerror=ultra_pid->preerror;
}*/

void PositionPID(_st_height_pid *ultra_pid_p,float target,float measure) //位置式
{
	ultra_pid_p->error=target-measure;
	ultra_pid_p->intergral+=ultra_pid_p->error;
	ultra_pid_p->intergral=LIMIT(ultra_pid_p->intergral,-3000,3000);
	//ultra_pid_p->lastoutput=ultra_pid_p->output;

	ultra_pid_p->output=ultra_pid_p->kp*ultra_pid_p->error+ultra_pid_p->ki*ultra_pid_p->intergral+ultra_pid_p->kd*(-ultra_delta);
	
	//if((ultra_pid_p->output-ultra_pid_p->lastoutput) >50)
	//ultra_pid_p->output = ultra_pid_p->lastoutput+50;
	//else if((ultra_pid_p->lastoutput-ultra_pid_p->output) >50)
  //ultra_pid_p->output=ultra_pid_p->lastoutput-50;
	
  
	ultra_pid_display_p=ultra_pid_p->kp*ultra_pid_p->error;
	ultra_pid_display_i=ultra_pid_p->ki*ultra_pid_p->intergral;
	ultra_pid_display_d=ultra_pid_p->kd*(-ultra_delta);
	ultra_pid_display_pid=ultra_pid_p->output;
	
	ultra_pid_p->preerror=ultra_pid_p->error;
}

void My_Height_Ctrl(float T,float thr)  
{
	if(height_ctrl_flag_star)
	{
	 //Height[2]=Height[1];
	 //Height[1]=Height[0];
	 //Height[0]=ultra_distance;//不启用平滑
	  // height_old=ultra_distance_old;
	   current_height=ultra_distance;
	 
	 if(ultra_delta>200)
	    current_height=ultra_distance-ultra_delta+200;
	 else if(ultra_delta<-200)
	    current_height=ultra_distance-ultra_delta-200;
	 
	 if(exp_height>ULTRA_MAX_HEIGHT)
		 exp_height=ULTRA_MAX_HEIGHT;
   
	  //IncrementalPID(&ultra_pid,exp_height,current_height);
	   PositionPID(&ultra_pid,exp_height,current_height);
	  //ultra_pid.output=LIMIT(ultra_pid.output,-100,100); //取消限幅
		 ultra_ctrl_out=thr+ultra_pid.output;
		 height_ctrl_out=ultra_ctrl_out;
	   CH[7]=ultra_pid_display_pid;
     CH[4]=ultra_pid_display_p;
	   CH[5]=ultra_pid_display_i;
	   CH[6]=ultra_pid_display_d;
	   
  }  
}



