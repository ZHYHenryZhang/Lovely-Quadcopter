#ifndef _MYCTRL_H
#define	_MYCTRL_H

#include "stm32f4xx.h"
#include "include.h"
#include "pwm_out.h"
#include "rc.h"
#include "imu.h"
#include "mpu6050.h"
#include "data_transfer.h"

/*
void take_off(void);
void land(void);
*/

typedef struct{
	float kp;
	float ki;
	float kd;
}_height_direct_ctrl;

//void Take_off();
void Landing(void);
//void Height_Direct_Ctrl();
void myctrl_PID_Init(void);
void Safety_Belt(void);

extern vs16 roll_take_off,pitch_take_off,thr_take_off,yaw_take_off;
static int thr_landing=1400;
static int thr_height_direct_ctrl=1680;
static int thr_soft_landing=1620;

extern u8 take_off_flag,landing_flag,height_ctrl_flag,soft_landing_flag,landing_done_flag;
static _height_direct_ctrl thr_height_direct_ctrl_pid;


#endif

