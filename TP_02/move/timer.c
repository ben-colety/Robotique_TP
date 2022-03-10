#include <stm32f4xx.h>
#include <gpio.h>
#include <main.h>

#define TIMER_CLOCK         84000000    // APB1 clock
#define PRESCALER_TIM7      8400        // timer frequency: 10kHz
#define COUNTER_MAX_TIM7    10000       // timer max counter -> 1Hz

#define PRESCALER_TIM4		8400		// timer frequency: 10kHz
#define COUNTER_MAX_TIM4	100			// timer max counter -> 100Hz

void timer4_start(void)
{
    // Enable TIM4 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // Configure TIM4
    TIM7->PSC = PRESCALER_TIM4 - 1;      // Note: final timer clock  = timer clock / (prescaler + 1)
    TIM7->ARR = COUNTER_MAX_TIM4 - 1;    // Note: timer reload takes 1 cycle, thus -1

    // Enable timer
    TIM4->CR1 |= TIM_CR1_CEN;
}

void tim4ch3_pwm_config(uint8_t mode, uint16_t duty){
	if(mode < 16)
		TIM4->CCMR2 |= (mode << 4) | (1 << 3) |(1 << 7); //OC3M to set output mode and OC3PE to enable preload and OC3CE to enable clear
	TIM4->CCR3  = duty;
	TIM4->CCER	|= (0b0001 << 8); 				//b11 = CC3NP (0 for output); b10 = res; b9 = CC3P (output 0 active high, 1 active low); b8 = CC3E (enable)
	TIM4->CR1	|= (1 << 7);					//enable ARR
}

void timer7_start(void)
{
    // Enable TIM7 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    // Enable TIM7 interrupt vector
    NVIC_EnableIRQ(TIM7_IRQn);

    // Configure TIM7
    TIM7->PSC = PRESCALER_TIM7 - 1;      // Note: final timer clock  = timer clock / (prescaler + 1)
    TIM7->ARR = COUNTER_MAX_TIM7 - 1;    // Note: timer reload takes 1 cycle, thus -1
    TIM7->DIER |= TIM_DIER_UIE;          // Enable update interrupt
    TIM7->CR1 |= TIM_CR1_CEN;            // Enable timer
}

/*
*   Commented because used for the motors
*/

// // Timer 7 Interrupt Service Routine
// void TIM7_IRQHandler(void)
// {
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

//     /* do something ... */
//     gpio_toggle(BODY_LED);

//     // Clear interrupt flag
//     TIM7->SR &= ~TIM_SR_UIF;
//     TIM7->SR;	// Read back in order to ensure the effective IF clearing
// }
