/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 7.1.1   2017-03-29

The MIT License (MIT)
Copyright (c) 2009-2017 Atollic AB

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/*==========================================================================*
 * 																			*
 * 					EEE3017W: DIGITAL ELECTRONICS - 2017					*
 * 								PRACTICAL 3								*
 * 																			*
 *==========================================================================*
 *
 *	Group #: 1
 *
 *	Student #1: Kuziwakwashe Mashingaidze   MSHKUZ001
 *	Student #2:           Bongani Teshane   TSHBON010
 *
 ****************************************************************************
 */

//==========================================================================*
//	INCLUDES
//==========================================================================*

	/*Include your header files here*/

#include "stm32f0xx.h"
#include "lcd_stm32f0.h"
#include <stdint.h>

//==========================================================================*
//	MACROS
//==========================================================================*

	/*Define your macros here*/



//==========================================================================*
//	GLOBAL CONSTANTS
//==========================================================================*

	/*Define your global constants here*/



//==========================================================================*
//	GLOBAL VARIABLES
//==========================================================================*

	/*Define your global variables here*/
int centiseconds = 0; //Initialises a counter variable to be used to keep track of
                      //the centiseconds that have elapsed.

int switch_pressed = 3; //Initialises a variable to keep track of the display.
                        //0: Start
                        //1: Lap
                        //2: Stop
					    //3: Welcome screen
                        //Note: variable is initialised at 3 because we want to
                        //see the welcome message when the microcontroller
                        //powers up.

char first_digit;  //Declares a variable to store the most significant digit
                   //on the LCD display.

char second_digit; //Declares a variable to store the second most significant
                   //digit on the LCD display.

char third_digit;  //Declares a variable to store the third most significant
                   //digit on the LCD display.

char fourth_digit; //Declares a variable to store the fourth most significant
                   //digit on the LCD display.

char fifth_digit;  //Declares a variable to store the fifth most significant
                   //digit on the LCD display.

char sixth_digit;  //Declares a variable to store the least significant digit
                   //on the LCD display.

//==========================================================================*
//	FUNCTION PROTOTYPES
//==========================================================================*

	/*Declare your functions here*/



//==========================================================================*
//	FUNCTIONS
//==========================================================================*

	/*Write your function descriptions here*/


int init_ports()
{
	RCC -> AHBENR |= RCC_AHBENR_GPIOBEN; //Activates part of RCC_AHBENR that
	                                     //governs Port B

	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN; //Activates part of RCC_AHBENR that
	                                     //governs Port A

	GPIOB -> MODER = (GPIOB -> MODER)|0x5555; //Sets pins 0 to 7 in as outputs
	                                          //by placing 0x5555 in GPIOB_MODER

	GPIOA -> PUPDR= (GPIOA -> PUPDR)|0b01010101; //Pulls up the internal
	                                             //resistor, tying this to our
	                                             //switches in GPIOA_PUPDR

    GPIOA -> MODER= (GPIOA -> MODER)|0x28003C00; //Sets all switches to input
                                                 //mode in GPIOA_MODER and sets
                                                 //potentiometers to analogue mode,
                                                 //while leaving all other GPIOA
                                                 //pins unchanged.
}

int init_TIM14()
{
	RCC -> APB1ENR |= RCC_APB1ENR_TIM14EN; //Activates part of RCC_APB1ENR that
	                                       //governs TIM14

	TIM14->PSC = 1; //Sets our prescaler to 1, since PSC + 1 was calculated to be
	                //2.

	TIM14->ARR = 39999; //Sets our ARR value to 39999, since ARR+1 was calculated to
	                    //be 40000.

	TIM14->DIER |= TIM_DIER_UIE; //Enables UIE (Update Interrupt Enable) in TIM14_DIER
	TIM14->CR1 |= TIM_CR1_CEN; //Enables CEN in TIM14_CR1. Starts timer.

}

int init_NVIC()
{
	NVIC_EnableIRQ(TIM14_IRQn);  //Enables TIM14 interrupt in the Nested Vector Interrupt
	                             //Controller.
}


int checkpb()
{
	if (((GPIOA->IDR)&0b1)==0) {      //If SW0 has been pressed
		switch_pressed=0;             //Sets display to counting. Start state.
	}
	else if (((GPIOA->IDR)&0b10)==0) { //Else If SW1 has been pressed
		switch_pressed=1;              //Lap state.
	}
	else if (((GPIOA->IDR)&0b100)==0) { //Else If SW2 has been pressed
		switch_pressed=2;               //Stop state.
	}
	else if (((GPIOA->IDR)&0b1000)==0) { //Else If SW3 has been pressed
		switch_pressed=3; //Sets display to welcome screen again. Welcome state.
	}
}

int display()
{
	if (switch_pressed==3) {                  //If welcome screen state active
		lcd_command(CURSOR_HOME);             //Sends cursor to start of line 1
		lcd_putstring("Stop Watch         "); //Display "Stop Watch" on line 1
		lcd_command(LINE_TWO); 				  //Move cursor to line 2
		lcd_putstring("Press SW0...       "); //Display "Press SW0" on line 2

		//Note, additional padding in both putstring commands is done in order
		//to avoid clearing the LCD display.
	}
	else {                              //Else If any other state is active.
		lcd_command(CURSOR_HOME);
		lcd_putstring("Time        "); 	//Display "Time" on line 1
		lcd_command(LINE_TWO);          //Move cursor to line 2

		lcd_putchar(first_digit);       //Display tens of minutes character
		                                //on the LCD.

		lcd_putchar(second_digit);      //Display ones of minutes character
		                                //on the LCD.

		lcd_putstring(":");             //Display a colon separator.

		lcd_putchar(third_digit);       //Display tens of seconds character
		                                //on the LCD.

		lcd_putchar(fourth_digit);      //Display ones of seconds character
		                                //on the LCD.

		lcd_putstring(".");             //Display a decimal point separator.

		lcd_putchar(fifth_digit);       //Display tens of centiseconds character
		                                //on the LCD.

		lcd_putchar(sixth_digit);       //Display ones of centiseconds character
		                                //on the LCD.

		lcd_putstring("        ");      //Additional padding to avoid using clear
		                                //command.
	}

}

void TIM14_IRQHandler(void)
{
	TIM14->SR &= ~TIM_SR_UIF;  //Clears UIF (update interrupt flag) bit in TIM 14 SR
	                           //to acknowledge interrupt.

	checkpb();                 //Checks which pushbutton has been pressed.

	if (switch_pressed==0){                               //If Start state active.

		first_digit = (centiseconds/60000)+48;            //BCD conversion for tens of
		                                                  //minutes.

		second_digit = ((centiseconds%60000)/6000)+48;    //BCD conversion for ones of
		                                                  //minutes.

		third_digit = (((centiseconds%60000)%6000)/1000)+48; //BCD conversion for tens of
		                                                     //seconds.

		fourth_digit = ((((centiseconds%60000)%6000)%1000)/100)+48; //BCD conversion for ones
		                                                            //of seconds.

		fifth_digit = (((((centiseconds%60000)%6000)%1000)%100)/10)+48; //BCD conversion for
		                                                                //tens of centiseconds.

		sixth_digit = (((((centiseconds%60000)%6000)%1000)%100)%10)+48; //BCD conversion for
		                                                                //ones of centiseconds.

		//Format of each respective display, however, is mapped out in the function display()
		//Note: 48(base 10) added to each variable in order to convert the result of the BCD
		//conversion to ASCII.

		centiseconds = centiseconds + 1;   //Increments centiseconds variable every hundredth
		                                   //of a second.
	}
	else if (switch_pressed==1){           //If Lap state active.

		centiseconds = centiseconds + 1;   //Increments centiseconds variable every hundredth
		                                   //of a second.

		//No BCD conversion here, however, so LCD display will remain the same as the values of
		//first_digit, second_digit, etc are not being updated.
	}
	else if (switch_pressed==2){           //If Stop state active.
		//Do nothing.
		//No BCD conversion, and no incrementing of centiseconds variable.
	}
	else if (switch_pressed==3){           //If Reset state active.
		centiseconds=0;      			   //Resets timer to 00:00.00
		//No BCD conversion.
	}

}

//==========================================================================*

/****************************************************************************
/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{

	init_LCD(); 						//Initialise LCD
	init_ports();                       //Inititalise the GPIOA and GPIOB.

	init_NVIC();                        //Initialise the Nested Vector
	                                    //Interrupt Controller

	init_TIM14();                       //Initialise TIM14


  /* Infinite loop */
  while (1)
  {
	  display(); //Displays welcome message if switch_pressed variable is 3,
	  		     //otherwise displays time.
  }
}
