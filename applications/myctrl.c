#include "myctrl.h"
#include "include.h"
#include "data_transfer.h"
#include "rc.h"
#include "ctrl.h"
#include "ultrasonic.h"
#include "pwm_in.h"
#include "rc.h"

#define TAKE_OFF_HEIGHT 700
/*void take_off()
{
	if(ultra_distance>200)
	{
		CH[2]=-400;	
		fly_ready=1;
	}
		
//	delay_ms(2000);
	if(ultra_distance>TAKE_OFF_HEIGHT)
	{
		height_mode=0;//结束起飞模式
	}
}
void land()
{
	if(ultra_distance>600)
	{
		if(CH[2]>-200)
		{
			CH[2]=CH[2]-10;
		}
	}
	else if(ultra_distance>400)//400~600
	{
		if(CH[2]>-200)
		{
			CH[2]=CH[2]-1;
		}
	}
	else
	{
			CH[2]=-400;
			fly_ready=0;
			height_mode=0;//结束降落模式
	}	
}

*/
int pre_div=0,soft_pre_div=0,height_pre_div=0;
u8 take_off_flag=0;
u8 landing_flag=0;
u8 height_ctrl_flag=0;
u8 soft_landing_flag=0;
u8 landing_done_flag=0;
vs16 roll_take_off=1570,pitch_take_off=1500,thr_take_off=1550,yaw_take_off=1500;
s16 EXP_Height=1000,Delta_distance;
u8 height_ctrl_count=0;
u8 thr_landing_count=0;
u16 landing_done_count=0;
u16 safety_belt_count=0;
u8 ultra_buffer_counter=0;
u16 ultra_buffer[4]={0,0,0,0};
float soft_landing_k=0.025;
float reference_speed=0;
float REFERENCE_SPEED_K=8;
float SOFT_LANDING_MAX_SPEED=-200.0f;
s16 motor_test[4]={1000,1000,1000,1000};

//HZ：自己的参数初始化
void myctrl_PID_Init(void)
{
	thr_height_direct_ctrl_pid.kp=0.025;
	thr_height_direct_ctrl_pid.ki=0;
	thr_height_direct_ctrl_pid.kd=0;
}

//起飞，油门到高点直到离开地面一定高度（后进入height_direct_ctrl）
void Take_off(void)
{
	
	if(take_off_flag)
	{
		if(ultra_distance<600)//起飞至600mm
		{
			Feed_Rc_Dog(1);
			Rc_Pwm_In[0]=roll_take_off;
			Rc_Pwm_In[1]=pitch_take_off;
			Rc_Pwm_In[2]=thr_take_off;
			Rc_Pwm_In[3]=yaw_take_off;
		}
		else
		{
			take_off_flag=0;
			height_ctrl_flag=1;
		}
	}
}


void Landing(void)
{
	if(landing_flag)
	{
		take_off_flag=0;
		height_ctrl_flag=0;
		if(ultra_distance>400)
		{
			thr_landing_count++;
			thr_landing_count=thr_landing_count%2;
			thr_landing=1670-ultra_distance*4/7;
		}
		else
		{
			thr_landing=1601;
			pre_div++;
			pre_div=pre_div%4;
			if(pre_div==0)
			{
				landing_flag=0;
				fly_ready=0;
				height_ctrl_flag_star=0;//不启用mf定高
			}
		}
		Feed_Rc_Dog(1);
		Rc_Pwm_In[2]=thr_landing;
	}
}

//自己写的高度控制
void Height_Direct_Ctrl(void)
{
	thr_height_direct_ctrl=CH[2]+1500;
	if(height_ctrl_flag==1)
	{
		
	}
	
}

void Safety_Belt(void)
{
		//限高保护程序
		if(ultra_distance>1450)//1450<ULTRA_MAX_HEIGHT
		{
			landing_flag=1;
			take_off_flag=0;
			height_ctrl_flag=0;
			safety_belt_count++;
			safety_belt_count=safety_belt_count%20;
			if(safety_belt_count==0){
				fly_ready=0;
				height_ctrl_flag_star=0;//不启用mf定高
			}
		}
		
		//倾斜保护程序，比赛前删除或者角度置为70；
		if(ABS(Pitch)>50||ABS(Roll)>50)//
		{
			fly_ready=0;
			height_ctrl_flag_star=0;//不启用mf定高
		}
		
		//超声失效保护程序(未启用)
		ultra_buffer_counter++;
		ultra_buffer_counter%=4;
		ultra_buffer[ultra_buffer_counter]=ultra_distance;
		if(height_ctrl_flag_star||take_off_flag||height_ctrl_flag)
			if(ultra_buffer[0]==ultra_buffer[1]&&ultra_buffer[1]==ultra_buffer[2]&&ultra_buffer[2]==ultra_buffer[3])
				{
					//fly_ready=0;
				}
}



