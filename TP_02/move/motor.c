#include <stdlib.h>
#include <stdint.h>
#include <stm32f4xx.h>
#include <gpio.h>
#include <timer.h>
#include <motor.h>

#define TIMER_CLOCK         84000000
#define TIMER_FREQ          100000 // [Hz]
#define MOTOR_SPEED_LIMIT   13 // [cm/s]
#define NSTEP_ONE_TURN      1000 // number of step for 1 turn of the motor
#define NSTEP_ONE_EL_TURN   4  //number of steps to do 1 electrical turn
#define NB_OF_PHASES        4  //number of phases of the motors
#define WHEEL_PERIMETER     13 // [cm]

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

#define MOTOR_LEFT_A	GPIOE, 09
#define MOTOR_LEFT_B	GPIOE, 08
#define MOTOR_LEFT_C	GPIOE, 11
#define MOTOR_LEFT_D	GPIOE, 10


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

/*
*
*   Hint :
*   You can declare here static variables which can be used to store the steps counter of the motors
*   for example. They will be available only for the code of this file.
*/

static int16_t step_counter = 0;
static uint8_t motor_right_state = 0;
static uint8_t motor_left_state = 0;

/*
*
*   TO COMPLETE
*
*   Performs the init of the timers and of the gpios used to control the motors
*/
void motor_init(void)
{
	// Enable clocks for motor timers
	RCC->APB1ENR |= MOTOR_RIGHT_TIMER_EN | MOTOR_LEFT_TIMER_EN;

	// Enable interrupt vector for motor timers
	NVIC_EnableIRQ(MOTOR_RIGHT_IRQ);
	NVIC_EnableIRQ(MOTOR_LEFT_IRQ);


	// Configure right motor timer
	MOTOR_RIGHT_TIMER->PSC = (TIMER_CLOCK/TIMER_FREQ) - 1;      // Note: final timer clock  = timer clock / (prescaler + 1)
	MOTOR_RIGHT_TIMER->ARR = 0;   				 // 0 so interrupt is not thrown
	MOTOR_RIGHT_TIMER->DIER |= TIM_DIER_UIE;          // Enable update interrupt
	MOTOR_RIGHT_TIMER->CR1 |= TIM_CR1_CEN;            // Enable timer

	// Configure left motor timer
	MOTOR_LEFT_TIMER->PSC = (TIMER_CLOCK/TIMER_FREQ) - 1;      // Note: final timer clock  = timer clock / (prescaler + 1)
	MOTOR_LEFT_TIMER->ARR = 0;   				 //
	MOTOR_LEFT_TIMER->DIER |= TIM_DIER_UIE;          // Enable update interrupt
	MOTOR_LEFT_TIMER->CR1 |= TIM_CR1_CEN;            // Enable timer


}

/*
*
*   TO COMPLETE (completed)
*
*   Updates the state of the gpios of the right motor given an array of 4 elements
*   describing the state. For example step_table[0] which gives the first step.
*/
static void right_motor_update(const uint8_t *out)
{
	out[0] ? set_gpio(MOTOR_RIGHT_A) : clear_gpio(MOTOR_RIGHT_A);
	out[1] ? set_gpio(MOTOR_RIGHT_B) : clear_gpio(MOTOR_RIGHT_B);
	out[2] ? set_gpio(MOTOR_RIGHT_C) : clear_gpio(MOTOR_RIGHT_C);
	out[3] ? set_gpio(MOTOR_RIGHT_D) : clear_gpio(MOTOR_RIGHT_D);
}

/*
*
*   TO COMPLETE (completed)
*
*   Updates the state of the gpios of the left motor given an array of 4 elements
*   describing the state. For exeample step_table[0] which gives the first step.
*/
static void left_motor_update(const uint8_t *out)
{
	out[0] ? set_gpio(MOTOR_LEFT_A) : clear_gpio(MOTOR_LEFT_A); //fancy if else statements
	out[1] ? set_gpio(MOTOR_LEFT_B) : clear_gpio(MOTOR_LEFT_B);
	out[2] ? set_gpio(MOTOR_LEFT_C) : clear_gpio(MOTOR_LEFT_C);
	out[3] ? set_gpio(MOTOR_LEFT_D) : clear_gpio(MOTOR_LEFT_D);
}

/*
*
*   TO COMPLETE (completed)
*
*   Stops the motors (all the gpio must be clear to 0) and set 0 to the ARR register of the timers to prevent
*   the interrupts of the timers (because it never reaches 0 after an increment)
*/
void motor_stop(void)
{
	right_motor_update(step_halt);
	left_motor_update(step_halt);
	MOTOR_RIGHT_TIMER->ARR = 0;
	MOTOR_LEFT_TIMER->ARR = 0;
}

/*
*
*   TO COMPLETE
*
*   Sets the position to reach for each motor.
*   The parameters are in cm for the positions and in cm/s for the speeds.
*/
void motor_set_position(float position_r, float position_l, float speed_r, float speed_l)
{

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
void motor_set_speed(float speed_r, float speed_l)
{
	if(speed_r < MOTOR_SPEED_LIMIT)
		MOTOR_RIGHT_TIMER->ARR = ;
	if(speed_l < MOTOR_SPEED_LIMIT)
		MOTOR_LEFT_TIMER ->ARR = ;
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

	// Clear interrupt flag
    MOTOR_LEFT_TIMER->SR &= ~TIM_SR_UIF;
    MOTOR_LEFT_TIMER->SR;	// Read back in order to ensure the effective IF clearing
}

