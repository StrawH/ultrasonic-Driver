/*
 * ultrasonic.c
 *
 *  Created on: Feb 27, 2022
 *      Author: Omar
 */


#include "ultrasonic.h"
#include "icu.h"
#include "gpio.h"
#include "std_types.h"
#include <util/delay.h>
#include <avr/io.h>

/* number of edges ICU captured Falling or rising */
uint8 g_edgeCount = 0;

/* time between rising and falling edge */
uint16 g_timeHigh = 0;

/* initialize the ultrasonic sensor by initializing ICU unit and configure it
 * with Rising edge detection for first edge .
 */
void Ultrasonic_init(void)
{
	/* configure ICU with RISING edge and prescaler FCPU/8 */
	Icu_ConfigType Icu_Config = {F_CPU_8,RISING};

	Icu_init(&Icu_Config);

	/*call Ultrasonic_edgeProcessing every time interrupt occures (edge detected) */
	Icu_setCallBack(Ultrasonic_edgeProcessing);

	/* set trigger pin as output */
	GPIO_setupPinDirection(PORTB_ID, PIN5_ID, PIN_OUTPUT);

	/* set indicator led pin as out put to flash every time edge detected (ISR called call back function) */
	GPIO_setupPinDirection(PORTD_ID, PIN0_ID, PIN_OUTPUT);

	/* Enable Global Interrupt I-Bit */
	SREG |= (1 << 7);
}

/* set trigger wave for ultrasonic with sequence __----__ 1 0 1*/
void Ultrasonic_Trigger(void)
{
	/* High */
	GPIO_writePin(PORTB_ID, PIN5_ID, LOGIC_HIGH);

	/* delay 10us (ultrasonic datasheet) */
	_delay_us(10);

	/* Low */
	GPIO_writePin(PORTB_ID, PIN5_ID, LOGIC_LOW);
}

/* read distance form ultrasonic by calculating the time between the rising edge and falling edge */
uint16 Ultrasonic_readDistance(void)
{
	/* start measuring */
	Ultrasonic_Trigger();

	/* distance  = speed x time
	 * sound speed 340 m/s divided by 2
	 * prescaler = fcpu/8 >> 1us every count from timer
	 * distance = (340 * 100)/2 ) * 10^-6
	 */
	return g_timeHigh / 58;
}

/* called back function */
void Ultrasonic_edgeProcessing(void)
{
	PORTD ^= (1<<PD0);
	g_edgeCount++;
	if(g_edgeCount == 1)
	{
		/*
		 * Clear the timer counter register to start measurements from the
		 * first detected rising edge
		 */
		Icu_clearTimerValue();

		/* Detect falling edge */
		Icu_setEdgeDetectionType(FALLING);
	}
	else if(g_edgeCount == 2)
	{
		/* Store the High time value */
		g_timeHigh = Icu_getInputCaptureValue();

		/* Clear the timer counter register to start measurements again */
		Icu_clearTimerValue();

		/* Detect rising edge */
		Icu_setEdgeDetectionType(RISING);

		/* reset g_edgeCount to count edges again */
		g_edgeCount = 0;
	}
}

