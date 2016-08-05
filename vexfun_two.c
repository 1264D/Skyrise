#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port2,            ,             tmotorVex393, openLoop, reversed, encoder, encoderPort, I2C_1, 1000)
#pragma config(Motor,  port3,            ,             tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port4,            ,             tmotorVex393, openLoop, encoder, encoderPort, I2C_2, 1000)
#pragma config(Motor,  port5,            ,             tmotorVex393, openLoop)
#pragma config(Motor,  port6,            ,             tmotorVex393, openLoop)
#pragma config(Motor,  port7,            ,             tmotorVex269, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          Pre-Autonomous Functions
//
// You may want to perform some actions before the competition starts. Do them in the
// following function.
//
/////////////////////////////////////////////////////////////////////////////////////////

void pre_auton()
{

	bStopTasksBetweenModes = true;


}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Autonomous Task
//
// This task is used to control your robot during the autonomous phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////
// port 2: right front wheel, port 3: right back wheel, port 4: left front, port 5: left back, port 6: lift top, port 7: the claw, port 8: claw
task autonomous()
{
	motor[port2] = 85; motor[port4] = 85;
	wait10Msec(250);
	motor[port2] = 0; motor[port4] = 0;
	motor[port7] = -50;
	wait10Msec(50);
	motor[port7] = 0;

}
/*
int pos;

motor[port2] = 127;
wait10Msec(100);
motor[port2] = 0;
if(SensorBoolean(touchsensor) == 0)
{
int pos = 1;
}
else
{
int pos = 0;
}
if(pos == 1)
{	//right
motor[port2] = 75;
motor[port4] = 75;
wait10Msec(100);
motor[port2] = 0;
wait10Msec(50);
motor[port2] = 75;
motor[port2] = 0; motor[port4] = 0;
}

else
{//left
motor[port2] = 75; motor[port4] = 75;
wait10Msec(100);
motor[port4] = 0;
wait10Msec(50);
motor[port4] = 75;
motor[port2] = -75; motor[port4] = -75;
motor[port2] = 0; motor[port4] = 0;
}

*/









/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control Task
//
// This task is used to control your robot during the user control phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task usercontrol()
{

	while (true)
	{
		//wheels
		motor[port2] = vexRT[Ch2];
		motor[port3] = vexRT[Ch2];
		motor[port4] = vexRT[Ch3];
		motor[port5] = vexRT[Ch3];
		//the clawww
		int clu[3];
		clu[2] = vexRT[Btn6U]*100;                                              //u wot
		clu[1] = vexRT[Btn6D]*-100;
		motor[port7] = clu[2] + clu[1];

		//lift motor
		int shrek[3];
		shrek[2] = vexRT[Btn5D]*-90;
		shrek[1] = vexRT[Btn5U]*90;
		motor[port6] = shrek[2] + shrek[1];
		//lift lifter
		//int kek[3];
		//kek[2] = vexRT[Btn8U]*100;
		//kek[1] = vexRT[Btn8D]*-100;
		//motor[port8] = kek[2] + kek[1];
		//motor[port9] = kek[2] + kek[1];




	}



}
