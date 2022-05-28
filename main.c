/*
 * main.c
 *
 *  Created on: Feb 26, 2022
 *      Author: Omar
 */
#include "lcd.h" 		/* to use LCD */
#include <util/delay.h> /* To use delays */
#include "ultrasonic.h" /* to use ultrasonic sensor*/
#include "icu.h" 		/* to use input capture unit */

/* main function */
int main(void)
{
	/* intialize LCD 2x16 */
	LCD_init();

	/* intialize ultrasonic sensor */
	Ultrasonic_init();

	/* declare variable to get distance from ultrasonic sensor */
	uint16 distance = 0;

	/* super loop */
	for(;;)
	{

		/* read distance */
		distance = Ultrasonic_readDistance();

		/* display the distance on the LCD */
		LCD_moveCursor(0,0);
		LCD_displayString("Distance = ");
		LCD_moveCursor(0,10);
		LCD_intgerToString(distance);
		LCD_displayString(" cm  ");
	}

	/* end of super loop */
	return 0;
}

