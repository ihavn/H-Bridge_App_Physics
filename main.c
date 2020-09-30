/*
* H-Bridge_App_Physics.c
*
* Created: 28/09/2020 15:37:14
* Author : IHA
*/

#include <stddef.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <ab_encoder/ab_encoder.h>
#include <display/display_7seg.h>
#include <vnh7100as/vnh7100as.h>

typedef enum {H_BRIDGE, BRAKE} appState_t;

appState_t appState = BRAKE;
int8_t percent = 0;

void setAppState(appState_t newState)
{
	if (appState != newState)
	{
		appState = newState;
		percent=0;

		if (appState == H_BRIDGE)
		{
			PORTA &= ~_BV(PA7);
			PORTA |= _BV(PA0);
		}
		else
		{
			PORTA &= ~_BV(PA0);
			PORTA |= _BV(PA7);
		}
	}
}

void ui_init(void)
{
	// LEDs
	DDRA |= _BV(DDA0) | _BV(DDA7);
	PORTA |= _BV(PA0) | _BV(PA7);
	// Switches - pull-up
	PORTD |= _BV(PD3);
	PORTC |= _BV(PC0);
}

int main(void)
{
	display_7seg_init(NULL);
	display_7seg_powerUp();
	ab_encoder_init();
	vnh7100as_init();
	ui_init();
	setAppState(H_BRIDGE);

	sei();
	float fp = 0.0;

	while (1)
	{
		percent += ab_encoder_currentPosition(true);

		_delay_ms(50);

		if (appState == H_BRIDGE)
		{
			if (percent < -100)
			{
				percent = -100;
			}
			else if (100 < percent)
			{
				percent = 100;
			}
			
			vnh7100as_setSpeed(percent);
		}
		else // Brake
		{
					if (percent < 0)
			{
				percent = 0;
			}
			else if (100 < percent)
			{
				percent = 100;
			}

			vnh7100as_setBrake(percent);
		}

		fp = (float)percent;
		display_7seg_display(fp,0);

		if (!(PIND & _BV(PIND3))) {
			setAppState(H_BRIDGE);
		}

		if (!(PINC & _BV(PINC0))) {
			setAppState(BRAKE);
		}
	}
}

