/*	Author: kmaka003
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//0.954 hz is lowest frequency possible with this function,
//based on settings in PWN_on()
//Passing in 0 as the frequency will stop the speaker from generating sound

void set_PWM(double frequency){
	static double current_frequency; // keeps track of the currently set frequency
	//Will only update the registers when the frequency changes, otherwise allows
	//music to play uninterrupted
	
	if(frequency != current_frequency)
	{
		if(!frequency) {TCCR1B &= 0x08;}//stops timer/counter
		else {TCCR1B |= 0x03;} //resumes/continues timer/counter
			
		//prevents OCR3A from overflowing using prescaler 64
		//0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) {OCR3A = 0xFFFF;}
		
		//prevents OCR3A from underflowing using prescaler 64
		//31250 is largest frequency that will not result in underflow
		if (frequency > 31250) {OCR3A = 0x0000;}
			
		else { OCR3A = (short) (8000000/(128*frequency))-1; }
			
			TCNT3 = 0; //resets counter
			current_frequency = frequency; // updates the current frequency
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
		//COM3A0: Toggle PB3 on compare match between counter and OCR3A 
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
		// wGM32: When counter (TCNT3) matches OCR3A, reset counter
		//CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_oFF (){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}


typedef enum States {init, OnOff, WaitOnOff, Up, Down, waitUp, waitDown} States;
int SM (int state);
int SM_ON (int state);
unsigned char On = 0x00;
unsigned char i = 0;
double NOTES[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PINA = 0xFF;
	DDRB = 0xFF; PINB = 0x00; 
    /* Insert your solution below */
	PWM_on();
	States state = init;
	States state2 = init;
	
    while (1) {
	state = SM(state);
	if(On){
		state2 = SM_ON(state2);
	}
	else{
		set_PWM(0);
	}
    }
    return 1;
}


int SM(int state){
	unsigned char A0 = ~PINA & 0x01;
	
	switch (state){
		case init:
			state = A0? OnOff: init;
			break;
		case OnOff:
			state = WaitOnOff;
			break;
		case WaitOnOff:
			state = A0? WaitOnOff: init;
			break;
	}
	switch (state){
		case init:
			PORTB = 0x00;
			break;
		case OnOff:
			On = ~On;
			break;
		case WaitOnOff:
			break;
	}
	return state;	
}
int SM_ON(int state){
	unsigned char A1 = (~PINA >> 1) & 0x01;
	unsigned char A2 = (~PINA >> 2) & 0x01;
	switch(state){

		case init:
			state = A1? Up: init;
			state = A2? Down: state; 
			break;
		case Up:
			state = waitUp;
			break;
		case Down:
			state = waitDown;
			break;
		case waitUp:
			state = A1? waitUp: init;
			break;
		case waitDown:
			state = A2? waitDown: init;
			break;
	}
	switch(state){
		case init:
			break;
		case waitUp:
			break;
		case waitDown:
			break;
		case Up:
			if(i < 7){
				i++;
			}
			//set_PWM(NOTES[i]);
			break;
		case Down:
			if(i > 0){
				i--;
			}
			//set_PWM(NOTES[i]);
			break;
	}
	set_PWM(NOTES[i]);
return state;
}




 
