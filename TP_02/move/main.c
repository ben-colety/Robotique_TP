#include <stm32f4xx.h>
#include <system_clock_config.h>
#include <gpio.h>
#include <main.h>
#include <timer.h>
#include <motor.h>
#include <selector.h>

#define PI                  3.1415926536f
//TO ADJUST IF NECESSARY. NOT ALL THE E-PUCK2 HAVE EXACTLY THE SAME WHEEL DISTANCE
#define WHEEL_DISTANCE      5.35f    //cm
#define PERIMETER_EPUCK     (PI * WHEEL_DISTANCE)


// Init function required by __libc_init_array
void _init(void) {}

// Simple delay function
void delay(unsigned int n)
{
    while (n--) {
        __asm__ volatile ("nop");
    }
}


int main(void)
{
    SystemClock_Config();

    // Enable GPIOD and GPIOE peripheral clock
    RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOCEN;

    //configure selector
    gpio_config_input_pd(SEL_0);
    gpio_config_input_pd(SEL_1);
    gpio_config_input_pd(SEL_2);
    gpio_config_input_pd(SEL_3);
    unsigned int selector_val = 0;
    selector_val = (gpio_read(SEL_0) | (gpio_read(SEL_1) << 1) | (gpio_read(SEL_1) << 2) | (gpio_read(SEL_1) << 3));

    //configure and start timer 4
    timer4_start();
    tim4ch3_pwm_config(PWM_MODE_1, selector_val);

    //configure Front LED signal + set AFR for front led to rely on Ch3 Tim4
    gpio_config_output_af_pushpull(FRONT_LED);
    gpio_config_af(FRONT_LED,2);

    while (1) {
    	selector_val = gpio_read(SEL_0);
    	selector_val += (gpio_read(SEL_1) << 1);
    	selector_val += (gpio_read(SEL_2) << 2);
    	selector_val += (gpio_read(SEL_3) << 3);
    	selector_val *= 0x1111;
    	tim4ch3_pwm_set_duty(selector_val);


    }
}

