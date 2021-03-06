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


    //test pour debug
    gpio_config_output_opendrain(LED1);
    gpio_config_output_opendrain(LED3);
    gpio_config_output_opendrain(LED5);
    gpio_config_output_opendrain(LED7);

    //config motors
    motor_init();

    //motor_set_position(5,5,1, -1);

    robot_test_small_radius_right(5,180,1);  //
    //robot_test_small_radius_right(10,180,1.5);	//marche pas, la roue droite s'arrete trop tot
    //robot_test_small_radius_right(5,180,1.5); //marche presque avec ces valeurs (petit probl?me, l'angle est moins de 180?)


    //robot_turn_right(-5,180,5); //marche
    //motor_set_position(5,5,-13,-13); //marche normalement

    //robot_rotation_180(); //marche normalement
    //motor_set_position_right(-5,-5); //marche normalement
    //motor_set_position_left(5,5); //marche normalement je pense
    //motor_set_speed(5,5); //marche normalement
    //motor_set_speed_left(5); //marche normalement
    //motor_set_speed_right(5); //marche normalement

    while (1) {
    	selector_val = gpio_read(SEL_0);
    	selector_val += (gpio_read(SEL_1) << 1);
    	selector_val += (gpio_read(SEL_2) << 2);
    	selector_val += (gpio_read(SEL_3) << 3);
    	selector_val *= 0x1111;
    	tim4ch3_pwm_set_duty(selector_val);


    }
}

