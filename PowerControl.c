/*
 ATTiny13a Power Controller
 SmokingResistor.com
 Date: April 26, 2015

 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).  Don't laugh at my code, I'm a hardware guy.
 
 Software:
 Turn on GPS device initially for 5 minutes to fully acquire satellites

 Start Loop
 Turn off the GPS device for 5 minutes
 Turn on GPS device for 1 minutes to acquire current position
 Loop
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>

short watchdog_counter;
int gps_on;

ISR(WDT_vect) {
	// Toggle Port B pin 4 output state
	//PORTB ^= 1<<PB4;
	watchdog_counter++; //8 second increments
}

int main(void) {
	// Set up Port B pin 4 mode to output
	DDRB = 1<<DDB4;

	// temporarily prescale timer to 8 seconds
	WDTCR |= (1<<WDP3) | (1<<WDP0);

	// Enable watchdog timer interrupts
	WDTCR |= (1<<WDTIE);

	sei(); // Enable global interrupts
	
	// Use the Power Down sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	PORTB = 1<<PB4; //turn on gps
	if (watchdog_counter > 37) {  //initially turn on for 5 minutes to get solid acquisition
		PORTB = 0<<PB4;
		gps_on = 0;
		watchdog_counter = 0;
	}
	else sleep_mode();
	
	for (;;) {
		sleep_mode();   // go to sleep and wait for interrupt...
		if (gps_on & (watchdog_counter > 7)) {  //turn off after 60 seconds, each watchdog count increments 8 seconds
			PORTB = 0<<PB4;
			gps_on = 0;
			watchdog_counter = 0;
		}
		if (~gps_on & (watchdog_counter > 37)) {  //turn on after 300 seconds, each watchdog count increments 8 seconds
			PORTB = 1<<PB4;
			gps_on = 1;
			watchdog_counter = 0;
		}
	}
  }