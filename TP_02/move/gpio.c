#include <stm32f407xx.h>
#include <gpio.h>
#include <main.h>

void gpio_config_input_pd(GPIO_TypeDef *port, unsigned int pin)
{

    // Pull-down : PUPDRy = 10
    port->PUPDR = (port->PUPDR & ~(3 << (pin * 2))) | (2 << (pin * 2));

    // Output speed highest : OSPEEDRy = 11
    port->OSPEEDR |= (3 << (pin * 2));

    // Input mode : MODERy = 00
    port->MODER &= ~(3 << (pin * 2));
}


void gpio_config_output_opendrain(GPIO_TypeDef *port, unsigned int pin)
{
    // Output type open-drain : OTy = 1
    port->OTYPER |= (1 << pin);

    // Output data low : ODRy = 0
    port->ODR &= ~(1 << pin);

    // Pull-up : PUPDRy = 01
    port->PUPDR = (port->PUPDR & ~(3 << (pin * 2))) | (1 << (pin * 2));

    // Output speed highest : OSPEEDRy = 11
    port->OSPEEDR |= (3 << (pin * 2));

    // Output mode : MODERy = 01
    port->MODER = (port->MODER & ~(3 << (pin * 2))) | (1 << (pin * 2));
}

void gpio_config_output_pushpull(GPIO_TypeDef *port, unsigned int pin)
{
    // Output type pushpull : OTy = 0
    port->OTYPER &= ~(1 << pin);

    // Output data low : ODRy = 0
    port->ODR &= ~(1 << pin);

    // Floating, no pull-up/down : PUPDRy = 00
    port->PUPDR &= ~(3 << (pin * 2));

    // Output speed highest : OSPEEDRy = 11
    port->OSPEEDR |= (3 << (pin * 2));

    // Output mode : MODERy = 01
    port->MODER = (port->MODER & ~(3 << (pin * 2))) | (1 << (pin * 2));
}

void gpio_config_output_af_pushpull(GPIO_TypeDef *port, unsigned int pin)
{
	// Output type pushpull : OTy = 0
	port->OTYPER &= ~(1 << pin);

	// Output data low : ODRy = 0
	port->ODR &= ~(1 << pin);

	// Floating, no pull-up/down : PUPDRy = 00
	port->PUPDR &= ~(3 << (pin * 2));

	// Output speed highest : OSPEEDRy = 11
	port->OSPEEDR |= (3 << (pin * 2));

	// Output mode : MODERy = 01
	port->MODER = (port->MODER & ~(3 << (pin * 2))) | (2 << (pin * 2));
}

void gpio_config_af(GPIO_TypeDef *port, unsigned int pin, uint8_t af_sel){
	if(pin < 8) //AFRL
		port->AFR[0] |= (af_sel << (4*pin));
	else if(pin < 16) //AFRH
		port->AFR[1] |= (af_sel << (4*(pin-8)));
}


void gpio_set(GPIO_TypeDef *port, unsigned int pin)
{
    port->BSRR = (1 << pin);
}

void gpio_clear(GPIO_TypeDef *port, unsigned int pin)
{
    port->BSRR = (1 << (pin + 16));
}

void gpio_toggle(GPIO_TypeDef *port, unsigned int pin)
{
    if (port->ODR & (1<<pin)) {
        gpio_clear(port, pin);
    } else {
    	gpio_set(port, pin);
    }
}

bool gpio_read(GPIO_TypeDef *port, unsigned int pin)
{
    return (port->IDR & (1<<pin));
}
