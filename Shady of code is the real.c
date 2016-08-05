#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    ,               sensorAnalog)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port2,            ,             tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,            ,             tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,            ,             tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,            ,             tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,            ,             tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,            ,             tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,            ,             tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port9,            ,             tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Setings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!
int incrementAmount = 5;
int clk[3]; //clock
int lawnchair[2];
int lawnswap;
int ideal;
void intake()
{
	motor[port6] = vexRT[Btn5D]*90 + vexRT[Btn6D] * -90;
	motor[port7] = vexRT[Btn5D]*90 + vexRT[Btn6D] * -90;
}
// 2 front left
// 3 back left
// 4 front right
// 5 back right
void feedback()
{
	// overshot
	if (vexRT[Btn8UXmtr2] == 1)
	{
		ideal = ideal + incrementAmount;
	}
	if (vexRT[Btn8DXmtr2] == 1)
	{
		ideal = ideal + incrementAmount;
	}
	if (vexRT[Btn8LXmtr2] == 1)
	{
		incrementAmount ++;
	}
	if (vexRT[Btn8RXmtr2] == 1)
	{
		incrementAmount --;
	}
}
void wheels()
{
	motor[port2] = vexRT[Ch3] /*+ vexRT[Btn7U]*80*/ + vexRT[Btn6U]*80 + vexRT[Btn5U]*-80;
	motor[port3] = vexRT[Ch3] /*+ vexRT[Btn7D]*-80*/ + vexRT[Btn6U]*-80 +vexRT[Btn5U]*80;
	motor[port4] = vexRT[Ch2] /*+ vexRT[Btn7U]*80*/ + vexRT[Btn6U]*-80 + vexRT[Btn5U]*80;
	motor[port5] = vexRT[Ch2] /*+ vexRT[Btn7D]*-80*/  + vexRT[Btn6U]*80  + vexRT[Btn5U]*-80;
}

void launcher()
{
	if(vexRT[Btn7U] == 1)
	{
		lawnswap = 	1;
	}
	if(vexRT[Btn7D] == 1)
	{
		lawnswap = 	0;
	}
	//l 50, d 100, u 127, r 0 <--- What does this mean? I think they're button bindings but what is 1 50?

	if(vexRT[Btn8L] == 1 && lawnswap == 0)
	{
		ideal = 37;
		if(lawnchair[0] < 30){
			lawnchair[0] = 30;
		}
	}

	if(vexRT[Btn8D] == 1 && lawnswap == 0)
	{
		ideal = 32;
		if(lawnchair[0] < 30){
			lawnchair[0] = 30;
		}
	}

	if(vexRT[Btn8U] == 1 && lawnswap == 0)
	{
		ideal = 39;
		if(lawnchair[0] < 30){
			lawnchair[0] = 30;
		}
	}
	//Stop
	if(vexRT[Btn8R] == 1 && lawnswap == 0)
	{
		ideal = 0;
		lawnchair[0] = 0;
	}
	//Half Decrease
	if(vexRT[Btn8L] == 1 && lawnswap == 1)
	{
		lawnchair[1] = lawnchair[1] - 5;
		waitUntil(vexRT[Btn8L] == 0);
	}
	//Full Decrease
	if(vexRT[Btn8D] == 1 && lawnswap == 1)
	{
		lawnchair[1] = lawnchair[1] - 10;
		waitUntil(vexRT[Btn8D] == 0);
	}
	//Bump Up
	if(vexRT[Btn8U] == 1 && lawnswap == 1)
	{
		lawnchair[1] = lawnchair[1] + 10;
		waitUntil(vexRT[Btn8U] == 0);
	}
	//Stop
	if(vexRT[Btn8R] == 1 && lawnswap == 1)
	{
		lawnchair[1] = 0;
		waitUntil(vexRT[Btn8R] == 0);
	}
	//motor[port8] = lawnchair[lawnswap];
	//motor[port9] = lawnchair[lawnswap];
	motor[port9] = lawnchair[lawnswap];
	motor[port8] = lawnchair[lawnswap];
}

void rollers()
{
	if(vexRT[Btn7R] == 1)
	{
		motor[port1] = -127;
	}
	if(vexRT[Btn7L] == 1)
	{
		motor[port1] = 0;
	}
}




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
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;
	// All activities that occur before the competition starts
	// Example: clearing encoders, setting servo positions, ...
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Autonomous Task
//
// This task is used to control your robot during the autonomous phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task autonomous()
{
	int ideala;
	int lawnchaira[2];
	int clka[3];
	int past1;
	int past2;
	int enca;
	int next;
	ideala = 63;
	lawnchaira[0] = 40;
	next = 1;
	string line1;
	while(true)
	{
		line1 = enca;
		displayLCDCenteredString(0, line1);
		if(next == 1 && enca <= 55)
		{
			lawnchaira[0] = 60;
			next = 0;
		}
		if(nPgmTime - clka[0] >= 100)
		{
			clka[0] = nPgmTime;
			past2 = past1;
			past1 = nMotorEncoder[port8];
			enca = nMotorEncoder[port8] - past2;
		}
		if(nPgmTime - clka[1] >= 300)
		{
			clka[1] = nPgmTime;
			if(enca < ideala)
			{
				lawnchaira[0] = lawnchaira[0] + 2;
			}
		}
		if(nPgmTime - clka[2] >= 550)
		{
			clka[2] = nPgmTime;
			if(enca > ideala)
			{
				lawnchaira[0] --;
			}
		}
		motor[port8] = lawnchaira[0];
		motor[port9] = lawnchaira[0];

		if(enca >= 55 && enca <= 59)
		{
			motor[port6] = -90;
			motor[port7] = -90;
			next = 1;
		}
		else
		{
			motor[port6] = 0;
			motor[port7] = 0;
		}
	}


}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control Task
//
// This task is used to control your robot during the user control phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////
// 1 rollers
// 2 front left
// 3 back left
// 4 front right
// 5 back right
// 6 7 intake
// 8 9 flywheels


task usercontrol()
{
	lawnswap = 0;
	lawnchair[0] = 0;
	lawnchair[1] = 0;
	motor[port2] = 0;
	motor[port3] = 0;
	motor[port4] = 0;
	motor[port5] = 0;
	motor[port6] = 0;
	motor[port7] = 0;
	motor[port8] = 0;
	motor[port9] = 0;

	//////////////////////////make this less stupid///////
	int egg;
	int lel;
	int past1;
	int past2;
	int enc; //encoder
	string Battery;
	string battery;
	string sneakydeaky;
	float cisstupid;
	Battery = "Bat: ";
	bLCDBacklight = true;
	clk[0] = 0;
	enc = nMotorEncoder[port8];
	int batdivk[2]; //batterydividedby
	string line1; //line1 of lcd
	string line2; //line2 of lcd
	lawnswap = 0;
	while(true)
	{
		battery = Battery + batdivk[0];
		batdivk[0] = nImmediateBatteryLevel;
		batdivk[1] = SensorValue[in1];
		if(lawnswap == 1)
		{
			line1 = "MANUAL MODE"; //or Battery or batdivk[0];
			lel = 1;
			line2 = enc;
		}
		else
		{
			if(lel == 1)
			{
				line1 = "PID";
				lel = 0;
			}
			if(vexRT[Btn8U] == 1)
			{
				line1 = "HIGH";
			}
			if(vexRT[Btn8L] == 1)
			{
				line1 = "MEDIUM";
			}
			if(vexRT[Btn8D] == 1)
			{
				line1 = "LOW";
			}
			if(vexRT[Btn8R] == 1)
			{
				line1 = "OFF";
			}

		}
		line2 = enc;  //enc
		if(nPgmTime - clk[0] >= 70)
		{
			clk[0] = nPgmTime;
			past2 = past1;
			past1 = nMotorEncoder[port8];

			enc = nMotorEncoder[port8] - past2;
		}
		if(nPgmTime - clk[1] >= 200)
		{
			clk[1] = nPgmTime;
			if(enc < ideal)
			{
				lawnchair[0] ++;
			}
		}
		if(nPgmTime - clk[2] >= 440)
		{
			clk[2] = nPgmTime;
			if(enc > ideal)
			{
				lawnchair[0] = lawnchair[0] - 2;
			}
		}
		//why
		egg = nLCDButtons;
		if (egg == 7) {
			line1 = "Nick is dumb.";
		}
		else if (egg == 4) {
			cisstupid = batdivk[0]/1000.;
			line1 = cisstupid;
		}
		else if(egg == 2) {
			if(bVEXNETActive == true){
				line1 = "Vexnet is on.";
			}
			else {
				line1 =  "Vexnet is off.";
			}
		}
		else if(egg == 1) {
			line1 = lawnchair[0];
		}
		else {
		}
		displayLCDCenteredString(0, line1);
		displayLCDCenteredString(1, line2);
		//////////////////////////////////////////////////////
		intake();
		wheels();
		launcher();
		feedback();
		rollers();
	}
}


//wew code is over
//wew lad
