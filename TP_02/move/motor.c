#include <stdlib.h>
#include <stdint.h>
#include <stm32f4xx.h>
#include <gpio.h>
#include <timer.h>
#include <motor.h>

//pour tester
#define LED1     	GPIOD, 5
#define LED3     	GPIOD, 6
#define LED5     	GPIOD, 10
#define LED7     	GPIOD, 11



#define TIMER_CLOCK         84000000
#define TIMER_FREQ          100000 // [Hz]
#define MOTOR_SPEED_LIMIT   13 // [cm/s]
#define NSTEP_ONE_TURN      1000 // number of step for 1 turn of the motor
#define NSTEP_ONE_EL_TURN   4  //number of steps to do 1 electrical turn
#define NB_OF_PHASES        4  //number of phases of the motors
#define WHEEL_PERIMETER     13 // [cm]
#define ROBOT_PERIMETER		17.3// [cm] =ROBOT_WHEEL_GAP*PI
#define ROBOT_WHEEL_GAP		5.5 // [cm]
#define ROBOT_RADIUS		(ROBOT_WHEEL_GAP/2)

#define PI	3.14159

#define STANDARD_SPEED		7 // [cm/s]

//timers to use for the motors
#define MOTOR_RIGHT_TIMER       TIM6
#define MOTOR_RIGHT_TIMER_EN    RCC_APB1ENR_TIM6EN
#define MOTOR_RIGHT_IRQHandler  TIM6_DAC_IRQHandler
#define MOTOR_RIGHT_IRQ         TIM6_DAC_IRQn

#define MOTOR_LEFT_TIMER        TIM7
#define MOTOR_LEFT_TIMER_EN     RCC_APB1ENR_TIM7EN
#define MOTOR_LEFT_IRQ          TIM7_IRQn
#define MOTOR_LEFT_IRQHandler   TIM7_IRQHandler

/*
*
*   TO COMPLETE (Completed)
*   Complete the right GPIO port and pin to be able to control the motors
*/
#define MOTOR_RIGHT_A	GPIOE, 13
#define MOTOR_RIGHT_B	GPIOE, 12
#define MOTOR_RIGHT_C	GPIOE, 14
#define MOTOR_RIGHT_D	GPIOE, 15

#define MOTOR_LEFT_A	GPIOE, 9
#define MOTOR_LEFT_B	GPIOE, 8
#define MOTOR_LEFT_C	GPIOE, 11
#define MOTOR_LEFT_D	GPIOE, 10

//Position states
#define RIGHT_PLACE 	0
#define ON_ITS_WAY		1

//Speed states
#define GOING_FORWARD	1
#define GOING_BACKWARD	2

//define if the motor should stop after a nb or steps
#define POSITION_CONTROLLED	0
#define SPEED_CONTROLLED	1
/*
*
*   TO COMPLETE (completed)
*   step_halt is an array containing 4 elements describing the state when the motors are off.
*   step_table is an array of 4 lines of 4 elements. Each line describes a step.
*/
static const uint8_t step_halt[NB_OF_PHASES] = {0, 0, 0, 0};
static const uint8_t step_table[NSTEP_ONE_EL_TURN][NB_OF_PHASES] = {
    {1,0,1,0},
    {0,1,1,0},
    {0,1,0,1},
    {1,0,0,1}
};
//variables going between 0 and NSTEP_ONE_EL_TURN controlling the electrical steps
static int16_t el_step_turn_r = 0;
static int16_t el_step_turn_l = 0;

/*
*
*   Hint :
*   You can declare here static variables which can be used to store the steps counter of the motors
*   for example. They will be available only for the code of this file.
*/

static uint16_t step_counter_r = 0;	//nb steps done by the right motor since the beginning of the movement
static uint16_t step_counter_l = 0;	//nb steps done by the left motor since the beginning of the movement
static uint16_t goal_nsteps_position_r = 0; //nb steps needed to reach the position wanted for the right motor
static uint16_t goal_nsteps_position_l = 0;	//nb steps needed to reach the position wanted for the left motor
static uint8_t motor_r_position_state = 0; 	//0 (RIGHT_PLACE) if the motor is at the right position, 1 (ON_ITS_WAY) if the position isn't reached yet
static uint8_t motor_l_position_state = 0; 	//0 (RIGHT_PLACE) if the motor is at the right position, 1 (ON_ITS_WAY) if the position isn't reached yet
static uint8_t motor_r_speed_state = 0; 	//0 (NOT_MOVING) if motor stopped, 1 (GOING_FORWARD) if motor must move forward, 2 (GOING_BACKWARD) if move backward
static uint8_t motor_l_speed_state = 0; 	//0 (NOT_MOVING) if motor stopped, 1 (GOING_FORWARD) if motor must move forward, 2 (GOING_BACKWARD) if move backward
static uint8_t motor_state_r = 0;	//0 = POSITION_CONTROLLED, 1 = SPEED_CONTROLLED
static uint8_t motor_state_l = 0;	//0 = POSITION_CONTROLLED, 1 = SPEED_CONTROLLED

/*
*
*   TO COMPLETE
*
*   Performs the init of the timers and of the gpios used to control the motors
*/
void motor_init(void)
{
	// Enable clocks for motor timers
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	RCC->APB1ENR |= MOTOR_RIGHT_TIMER_EN | MOTOR_LEFT_TIMER_EN;

	// right motor
	gpio_config_output_pushpull(MOTOR_RIGHT_A);
	gpio_config_output_pushpull(MOTOR_RIGHT_B);
	gpio_config_output_pushpull(MOTOR_RIGHT_C);
	gpio_config_output_pushpull(MOTOR_RIGHT_D);
	// left motor
	gpio_config_output_pushpull(MOTOR_LEFT_A);
	gpio_config_output_pushpull(MOTOR_LEFT_B);
	gpio_config_output_pushpull(MOTOR_LEFT_C);
	gpio_config_output_pushpull(MOTOR_LEFT_D);

	// Enable interrupt vector for motor timers
	NVIC_EnableIRQ(MOTOR_RIGHT_IRQ);
	NVIC_EnableIRQ(MOTOR_LEFT_IRQ);


	// Configure right motor timer
	MOTOR_RIGHT_TIMER->PSC = (TIMER_CLOCK/TIMER_FREQ) - 1;      // Note: final timer clock  = timer clock / (prescaler + 1)
	MOTOR_RIGHT_TIMER->ARR = 0;   					// 0 so interrupt is not thrown
	MOTOR_RIGHT_TIMER->DIER |= TIM_DIER_UIE;        // Enable update interrupt
	MOTOR_RIGHT_TIMER->CR1 |= TIM_CR1_CEN;          // Enable timer

	// Configure left motor timer
	MOTOR_LEFT_TIMER->PSC = (TIMER_CLOCK/TIMER_FREQ) - 1;      // Note: final timer clock  = timer clock / (prescaler + 1)
	MOTOR_LEFT_TIMER->ARR = 0;   				 	// 0 so interrupt is not thrown
	MOTOR_LEFT_TIMER->DIER |= TIM_DIER_UIE;         // Enable update interrupt
	MOTOR_LEFT_TIMER->CR1 |= TIM_CR1_CEN;           // Enable timer
}

static void right_motor_update(const uint8_t *out)
{
	out[0] ? gpio_set(MOTOR_RIGHT_A) : gpio_clear(MOTOR_RIGHT_A);
	out[1] ? gpio_set(MOTOR_RIGHT_B) : gpio_clear(MOTOR_RIGHT_B);
	out[2] ? gpio_set(MOTOR_RIGHT_C) : gpio_clear(MOTOR_RIGHT_C);
	out[3] ? gpio_set(MOTOR_RIGHT_D) : gpio_clear(MOTOR_RIGHT_D);
}

static void left_motor_update(const uint8_t *out)
{
	out[0] ? gpio_set(MOTOR_LEFT_A) : gpio_clear(MOTOR_LEFT_A); //fancy if else statements
	out[1] ? gpio_set(MOTOR_LEFT_B) : gpio_clear(MOTOR_LEFT_B);
	out[2] ? gpio_set(MOTOR_LEFT_C) : gpio_clear(MOTOR_LEFT_C);
	out[3] ? gpio_set(MOTOR_LEFT_D) : gpio_clear(MOTOR_LEFT_D);

}

void motor_stop(void)
{
	right_motor_update(step_halt);
	left_motor_update(step_halt);
	MOTOR_RIGHT_TIMER->ARR = 0;
	MOTOR_LEFT_TIMER->ARR = 0;
}


void motor_set_position_right(float position_r, float speed_r)
//position_r is the absolute value (in cm) to reach and speed_r defines the speed and the direction
//maximum value allowed :max position_r = 851, max speed_r = MOTOR_SPEED_LIMIT
{
	//reinitialization
	step_counter_r = 0;
	position_r = abs(position_r);

	//motor's position state flags and goal positions settings
	if(position_r==0)
	{
		motor_r_position_state = RIGHT_PLACE;
		goal_nsteps_position_r = 0;
	}
	else
	{
		goal_nsteps_position_r = position_r * NSTEP_ONE_TURN/WHEEL_PERIMETER; //conversion cm to steps
		motor_r_position_state = ON_ITS_WAY;
	}

	motor_set_speed_right(speed_r);
	motor_state_r = POSITION_CONTROLLED;
}

void motor_set_position_left(float position_l, float speed_l)
//position_l is the absolute value (in cm) to reach and speed_l defines the speed and the direction
//maximum value allowed :max position_l = 851, max speed_l = MOTOR_SPEED_LIMIT
{
	//reinitialization
	step_counter_l = 0;
	position_l = abs(position_l);

	//motor's position state flags and goal positions settings
	if(position_l==0)
	{
		motor_l_position_state = RIGHT_PLACE;
		goal_nsteps_position_l = 0;
	}
	else
	{
		goal_nsteps_position_l = position_l * NSTEP_ONE_TURN/WHEEL_PERIMETER; //conversion cm to steps
		motor_l_position_state = ON_ITS_WAY;
	}

	motor_set_speed_left(speed_l);
	motor_state_l = POSITION_CONTROLLED;
}

void motor_set_position(float position_r, float position_l, float speed_r, float speed_l)
//position_r and position_l is the absolute value (in cm) to reach, speed_r and speed_l define the speed and the direction
//maximum value allowed :max position = 851, max speed = MOTOR_SPEED_LIMIT
{
	motor_set_position_right(position_r, speed_r);
	motor_set_position_left(position_l, speed_l);
}

/*
*
*   TO COMPLETE
*
*   Sets the speed of the motors.
*   The parameters are in cm/s for the speed.
*   To set the speed, you need to change the ARR value of the timers.
*   Remember : the timers generate an interrupt when they reach the value of ARR.
*   Don't forget to convert properly the units in order to have the correct ARR value
*   depending on the TIMER_FREQ and the speed chosen.
*/
void motor_set_speed_right(float speed_r)
// speed_r (in cm/s) define the speed and the direction
{
	if(speed_r == 0)
	{	motor_r_speed_state = NOT_MOVING; //motor is stopped
		motor_r_position_state = RIGHT_PLACE;
		right_motor_update(step_halt);
		MOTOR_RIGHT_TIMER->ARR = 0;
	}
	else
	{
		if(speed_r > MOTOR_SPEED_LIMIT)
		{
			MOTOR_RIGHT_TIMER->ARR = TIMER_FREQ*WHEEL_PERIMETER/(MOTOR_SPEED_LIMIT*NSTEP_ONE_TURN); //with speed transformation from cm/s to step/s
			motor_r_speed_state = GOING_FORWARD; //motor moves forward
		}
		else if(speed_r < -MOTOR_SPEED_LIMIT)
		{
			MOTOR_RIGHT_TIMER->ARR = TIMER_FREQ*WHEEL_PERIMETER/(MOTOR_SPEED_LIMIT*NSTEP_ONE_TURN); //with speed transformation from cm/s to step/s
			motor_r_speed_state = GOING_BACKWARD; //motor moves backward
		}
		else
		{
			MOTOR_RIGHT_TIMER->ARR = TIMER_FREQ*WHEEL_PERIMETER/(abs(speed_r)*NSTEP_ONE_TURN); //with speed transformation from cm/s to step/s
			if(speed_r>0){motor_r_speed_state = GOING_FORWARD;} //motor moves forward
			else{motor_r_speed_state = GOING_BACKWARD;} //motor moves backward
		}
		motor_state_r = SPEED_CONTROLLED;
	}
}
void motor_set_speed_left(float speed_l)
{
	if(speed_l == 0)
	{
		motor_l_speed_state = NOT_MOVING; //motor is stopped
		motor_l_position_state = RIGHT_PLACE;
		left_motor_update(step_halt);
		MOTOR_LEFT_TIMER->ARR = 0;
	}
	else
	{
		if(speed_l > MOTOR_SPEED_LIMIT)
		{
			MOTOR_LEFT_TIMER->ARR = TIMER_FREQ*WHEEL_PERIMETER/(MOTOR_SPEED_LIMIT*NSTEP_ONE_TURN); //with speed transformation from cm/s to step/s
			motor_l_speed_state = GOING_FORWARD; //motor moves forward
		}
		else if(speed_l < -MOTOR_SPEED_LIMIT)
		{
			MOTOR_LEFT_TIMER->ARR = TIMER_FREQ*WHEEL_PERIMETER/(MOTOR_SPEED_LIMIT*NSTEP_ONE_TURN); //with speed transformation from cm/s to step/s
			motor_l_speed_state = GOING_BACKWARD; //motor moves backward
		}
		else
		{
			MOTOR_LEFT_TIMER->ARR = TIMER_FREQ*WHEEL_PERIMETER/(abs(speed_l)*NSTEP_ONE_TURN); //with speed transformation from cm/s to step/s
			if(speed_l>0){motor_l_speed_state = GOING_FORWARD;} //motor moves forward
			else{motor_l_speed_state = GOING_BACKWARD;} //motor moves backward
		}
		motor_state_l = SPEED_CONTROLLED;
	}
}

void motor_set_speed(float speed_r, float speed_l)
{
	motor_set_speed_right(speed_r);
	motor_set_speed_left(speed_l);
}

uint8_t motor_right_move(void)
{
	if(motor_r_speed_state != NOT_MOVING){return MOVING;}
	else{return NOT_MOVING;}
}

uint8_t motor_left_move(void)
{
	if(motor_l_speed_state != NOT_MOVING){return MOVING;}
	else{return NOT_MOVING;}
}

uint8_t motors_move(void)
{
	if(motor_left_move() || motor_right_move()){return MOVING;}
	else{return NOT_MOVING;}
}
/*
*
*   TO COMPLETE
*
*   Interrupt of the timer of the right motor.
*   Performs a step of the motor and stops it if it reaches the position given in motor_set_position().
*/
void MOTOR_RIGHT_IRQHandler(void)
{
	/*
    *
    *   BEWARE !!
    *   Based on STM32F40x and STM32F41x Errata sheet - 2.1.13 Delay after an RCC peripheral clock enabling
    *
    *   As there can be a delay between the instruction of clearing of the IF (Interrupt Flag) of corresponding register (named here CR) and
    *   the effective peripheral IF clearing bit there is a risk to enter again in the interrupt if the clearing is done at the end of ISR.
    *
    *   As tested, only the workaround 3 is working well, then read back of CR must be done before leaving the ISR
    *
    */

	/* do something ... */
	if(step_counter_r < goal_nsteps_position_r || motor_state_r == SPEED_CONTROLLED)
	{
		if(motor_r_speed_state == GOING_BACKWARD) //motor going backward
		{
			right_motor_update(step_table[el_step_turn_r]);
			el_step_turn_r++;
			if(el_step_turn_r == NSTEP_ONE_EL_TURN){el_step_turn_r=0;}
		}
		else if(motor_r_speed_state == GOING_FORWARD) //motor going forward
		{
			right_motor_update(step_table[el_step_turn_r]);
			el_step_turn_r--;
			if(el_step_turn_r == -1){el_step_turn_r=NSTEP_ONE_EL_TURN-1;}
		}
		step_counter_r++;

	}
	else
	{
		motor_r_position_state = RIGHT_PLACE;
		motor_r_speed_state = NOT_MOVING;
		right_motor_update(step_halt); //avoid letting electricity in the motor phases
		MOTOR_RIGHT_TIMER->ARR = 0; //stop the timer
	}

	// Clear interrupt flag
	MOTOR_RIGHT_TIMER->SR &= ~TIM_SR_UIF;
	MOTOR_RIGHT_TIMER->SR;	// Read back in order to ensure the effective IF clearing
}

/*
*
*   TO COMPLETE
*
*   Interrupt of the timer of the left motor.
*   Performs a step of the motor and stops it if it reaches the position given in motor_set_position().
*/
void MOTOR_LEFT_IRQHandler(void)
{
	/*
    *
    *   BEWARE !!
    *   Based on STM32F40x and STM32F41x Errata sheet - 2.1.13 Delay after an RCC peripheral clock enabling
    *
    *   As there can be a delay between the instruction of clearing of the IF (Interrupt Flag) of corresponding register (named here CR) and
    *   the effective peripheral IF clearing bit there is a risk to enter again in the interrupt if the clearing is done at the end of ISR.
    *
    *   As tested, only the workaround 3 is working well, then read back of CR must be done before leaving the ISR
    *
    */

	/* do something ... */


	if(step_counter_l < goal_nsteps_position_l || motor_state_l == SPEED_CONTROLLED)
	{
		if(motor_l_speed_state == GOING_FORWARD) //motor going forward
		{
			left_motor_update(step_table[el_step_turn_l]);
			el_step_turn_l++;
			if(el_step_turn_l == NSTEP_ONE_EL_TURN){el_step_turn_l=0;}

		}
		else if(motor_l_speed_state == GOING_BACKWARD) //motor going backward
		{
			left_motor_update(step_table[el_step_turn_l]);
			el_step_turn_l--;
			if(el_step_turn_l == -1){el_step_turn_l=NSTEP_ONE_EL_TURN-1;}
		}
		step_counter_l++;
	}
	else
	{
		motor_l_position_state = RIGHT_PLACE;
		motor_l_speed_state = NOT_MOVING;
		left_motor_update(step_halt); //avoid letting electricity in the motor phases
		MOTOR_LEFT_TIMER->ARR = 0; //stop the timer
	}

	// Clear interrupt flag
    MOTOR_LEFT_TIMER->SR &= ~TIM_SR_UIF;
    MOTOR_LEFT_TIMER->SR;	// Read back in order to ensure the effective IF clearing
}

//INSTRUCTIONS DE COMMANDE DU ROBOT ***********************************************************************************************************
void robot_rotation_right(float speed, float angle)
{
	motor_set_position(ROBOT_PERIMETER*angle/360, ROBOT_PERIMETER*angle/360, -speed, speed);
}

void robot_rotation_left(float speed, float angle)
{
	motor_set_position(ROBOT_PERIMETER*angle/360, ROBOT_PERIMETER*angle/360, speed, -speed);
}

void robot_rotation_180(void)
{
	motor_set_position(ROBOT_PERIMETER/2, ROBOT_PERIMETER/2, -STANDARD_SPEED, STANDARD_SPEED);
}

void robot_turn_right(float speed, float final_angle, float radius)
//radius is the average radius of the 2 wheels, min radius possible = ROBOT_WHEEL_GAP/2
{//ne marche pas encore
	if(radius<ROBOT_RADIUS)
		radius = ROBOT_RADIUS;
	float r_big = (radius+ROBOT_RADIUS);
	float r_small = (radius-ROBOT_RADIUS);
	float s_big = speed*r_big/radius;
	float s_small = speed*r_small/radius;

	motor_set_position(r_small*2*PI*final_angle/360, r_big*2*PI*final_angle/360, s_small, s_big);
//	motor_set_speed(s_small, s_big);
}

/*void robot_turn_left(float radius)
{

}*/

void robot_straight_speed(float speed)
{
	motor_set_speed(speed, speed);
}

void robot_straight_position(float position)
//go straight the amount of [cm] define in position, max position = 851, min position = -851
{
	if(position>0){motor_set_position(position, position, STANDARD_SPEED, STANDARD_SPEED);}
	else{motor_set_position(position, position, -STANDARD_SPEED, -STANDARD_SPEED);}
}

/*void robot_stop(void)
{
	motor_stop();
}*/








