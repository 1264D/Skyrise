#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    ,               sensorAnalog)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port2,            ,             tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,            ,             tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,            ,             tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,            ,             tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,            ,             tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,            ,             tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           Motor_FW1,     tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port9,           Motor_FW2,     tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Setings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"  //Main competition background code...do not modify!
// Update inteval (in mS) for the flywheel control loop
#define FW_LOOP_SPEED              25

// Maximum power we want to send to the flywheel motors
#define FW_MAX_POWER              127

// encoder counts per revolution depending on motor
#define MOTOR_TPR_269           240.448
#define MOTOR_TPR_393R          261.333
#define MOTOR_TPR_393S          392
#define MOTOR_TPR_393T          627.2
#define MOTOR_TPR_QUAD          360.0

// Structure to gather all the flywheel ralated data
typedef struct _fw_controller {
    long            counter;                ///< loop counter used for debug

    // encoder tick per revolution
    float           ticks_per_rev;          ///< encoder ticks per revolution

    // Encoder
    long            e_current;              ///< current encoder count
    long            e_last;                 ///< current encoder count

    // velocity measurement
    float           v_current;              ///< current velocity in rpm
    long            v_time;                 ///< Time of last velocity calculation

    // TBH control algorithm variables
    long            target;                 ///< target velocity
    long            current;                ///< current velocity
    long            last;                   ///< last velocity
    float           error;                  ///< error between actual and target velocities
    float           last_error;             ///< error last time update called
    float           gain;                   ///< gain
    float           drive;                  ///< final drive out of TBH (0.0 to 1.0)
    float           drive_at_zero;          ///< drive at last zero crossing
    long            first_cross;            ///< flag indicating first zero crossing
    float           drive_approx;           ///< estimated open loop drive

    // final motor drive
    long            motor_drive;            ///< final motor control value
    } fw_controller;

// Make the controller global for easy debugging
static  fw_controller   flywheel;

/*-----------------------------------------------------------------------------*/
/** @brief      Set the flywheen motors                                        */
/** @param[in]  value motor control value                                      */
/*-----------------------------------------------------------------------------*/
void
FwMotorSet( int value )
{
    motor[ Motor_FW1 ] = value;
    motor[ Motor_FW2 ] = value;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Get the flywheen motor encoder count                           */
/*-----------------------------------------------------------------------------*/
long
FwMotorEncoderGet()
{
    return( nMotorEncoder[ Motor_FW1 ] );
}

/*-----------------------------------------------------------------------------*/
/** @brief      Set the controller position                                    */
/** @param[in]  fw pointer to flywheel controller structure                    */
/** @param[in]  desired velocity                                               */
/** @param[in]  predicted_drive estimated open loop motor drive                */
/*-----------------------------------------------------------------------------*/
void
FwVelocitySet( fw_controller *fw, int velocity, float predicted_drive )
{
    // set target velocity (motor rpm)
    fw->target        = velocity;

    // Set error so zero crossing is correctly detected
    fw->error         = fw->target - fw->current;
    fw->last_error    = fw->error;

    // Set predicted open loop drive value
    fw->drive_approx  = predicted_drive;
    // Set flag to detect first zero crossing
    fw->first_cross   = 1;
    // clear tbh variable
    fw->drive_at_zero = 0;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Calculate the current flywheel motor velocity                  */
/** @param[in]  fw pointer to flywheel controller structure                    */
/*-----------------------------------------------------------------------------*/
void
FwCalculateSpeed( fw_controller *fw )
{
    int     delta_ms;
    int     delta_enc;

    // Get current encoder value
    fw->e_current = FwMotorEncoderGet();

    // This is just used so we don't need to know how often we are called
    // how many mS since we were last here
    delta_ms   = nSysTime - fw->v_time;
    fw->v_time = nSysTime;

    // Change in encoder count
    delta_enc = (fw->e_current - fw->e_last);

    // save last position
    fw->e_last = fw->e_current;

    // Calculate velocity in rpm
    fw->v_current = (1000.0 / delta_ms) * delta_enc * 60.0 / fw->ticks_per_rev;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Update the velocity tbh controller variables                   */
/** @param[in]  fw pointer to flywheel controller structure                    */
/*-----------------------------------------------------------------------------*/
void
FwControlUpdateVelocityTbh( fw_controller *fw )
{
    // calculate error in velocity
    // target is desired velocity
    // current is measured velocity
    fw->error = fw->target - fw->current;

    // Use Kp as gain
    fw->drive =  fw->drive + (fw->error * fw->gain);

    // Clip - we are only going forwards
    if( fw->drive > 1 )
          fw->drive = 1;
    if( fw->drive < 0 )
          fw->drive = 0;

    // Check for zero crossing
    if( sgn(fw->error) != sgn(fw->last_error) ) {
        // First zero crossing after a new set velocity command
        if( fw->first_cross ) {
            // Set drive to the open loop approximation
            fw->drive = fw->drive_approx;
            fw->first_cross = 0;
        }
        else
            fw->drive = 0.5 * ( fw->drive + fw->drive_at_zero );

        // Save this drive value in the "tbh" variable
        fw->drive_at_zero = fw->drive;
    }

    // Save last error
    fw->last_error = fw->error;
}

/*-----------------------------------------------------------------------------*/
/** @brief     Task to control the velocity of the flywheel                    */
/*-----------------------------------------------------------------------------*/
task
FwControlTask()
{
    fw_controller *fw = &flywheel;

    // Set the gain
    fw->gain = 0.00025;

    // We are using Speed geared motors
    // Set the encoder ticks per revolution
    fw->ticks_per_rev = MOTOR_TPR_393R;

    while(1)
        {
        // debug counter
        fw->counter++;

        // Calculate velocity
        FwCalculateSpeed( fw );

        // Set current speed for the tbh calculation code
        fw->current = fw->v_current;

        // Do the velocity TBH calculations
        FwControlUpdateVelocityTbh( fw ) ;

        // Scale drive into the range the motors need
        fw->motor_drive  = (fw->drive * FW_MAX_POWER) + 0.5;

        // Final Limit of motor values - don't really need this
        if( fw->motor_drive >  127 ) fw->motor_drive =  127;
        if( fw->motor_drive < -127 ) fw->motor_drive = -127;

        // and finally set the motor control value
        FwMotorSet( fw->motor_drive );

        // Run at somewhere between 20 and 50mS
        wait1Msec( FW_LOOP_SPEED );
        }
}
void intake()
{
	motor[port6] = vexRT[Btn5D]*90 + vexRT[Btn6D] * -85;
	motor[port7] = vexRT[Btn5D]*90 + vexRT[Btn6D] * -85;
}
// 2 front left
// 3 back left
// 4 front right
// 5 back right
void wheels()
{
	motor[port2] = vexRT[Ch3] /*+ vexRT[Btn7U]*80*/ + vexRT[Btn6U]*80 + vexRT[Btn5U]*-80;
	motor[port3] = vexRT[Ch3] /*+ vexRT[Btn7D]*-80*/ + vexRT[Btn6U]*-80 +vexRT[Btn5U]*80;
	motor[port4] = vexRT[Ch2] /*+ vexRT[Btn7U]*80*/ + vexRT[Btn6U]*-80 + vexRT[Btn5U]*80;
	motor[port5] = vexRT[Ch2] /*+ vexRT[Btn7D]*-80*/  + vexRT[Btn6U]*80  + vexRT[Btn5U]*-80;
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
	motor[port2] = 0;
	motor[port3] = 0;
	motor[port4] = 0;
	motor[port5] = 0;
	motor[port6] = 0;
	motor[port7] = 0;
	motor[port8] = 0;
	motor[port9] = 0;

	//////////////////////////make this less stupid///////
	bLCDBacklight = true;
	while(true)
	{
		    char  str[32];

    bLCDBacklight = true;

    // Start the flywheel control task
    startTask( FwControlTask );

    // Main user control loop
    while(1)
        {
        // Different speeds set by buttons
        if( vexRT[ Btn8L ] == 1 )
            FwVelocitySet( &flywheel, 144, 0.55 );
        if( vexRT[ Btn8U ] == 1 )
            FwVelocitySet( &flywheel, 120, 0.38 );
        if( vexRT[ Btn8R ] == 1 )
            FwVelocitySet( &flywheel, 50, 0.2 );
        if( vexRT[ Btn8D ] == 1 )
            FwVelocitySet( &flywheel, 00, 0 );

        // Display useful things on the LCD
        sprintf( str, "%4d %4d  %5.2f", flywheel.target,  flywheel.current, nImmediateBatteryLevel/1000.0 );
        displayLCDString(0, 0, str );
        sprintf( str, "%4.2f %4.2f ", flywheel.drive, flywheel.drive_at_zero );
        displayLCDString(1, 0, str );
		//////////////////////////////////////////////////////
		intake();
		wheels();
		rollers();
	}
}
}


//wew code is over
//wew lad
//A E S T H I C L I S A F R A N K M O D E R N C O M P U T I N G 4 2 0
