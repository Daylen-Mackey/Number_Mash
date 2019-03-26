/*Author: Daylen Mackey, Payam Tavakoli, Zehua Li

In the game “Number Mash,” numbers will fly from the right side of the LCD display towards the user’s numbered 
spacecraft (positioned in the leftmost column). When the enemy numbers reach the leftmost column, 
the player’s spacecraft must be in the same row with the same displayed number for a point to be 
awarded (a life is subtracted otherwise). The up/down buttons control the spacecraft’s row,
and the increase/decrease buttons will change the user’s number. Speed increases every 3 
of enemies eliminated until the player has lost all 3 lives. If all 3 lives are lost, 
a “Game Over” screen will display the final score, and an option to restart. 
*/



#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h> //required for interrupt usage
#include "defines.h"
#include "hd44780.h"
#include "lcd.h"
#include <time.h>

//include all the necessary libraries, headers, etc...


//defining pins and ports for switches 
//improves readability 
#define SWITCH_PORT			PORTD
#define SWITCH_PIN			PIND
#define SWITCH_DDR			DDRD
#define SWITCH_SELECT		(1<<PORTD0)
#define SWITCH_UP			(1<<PORTD2)
#define SWITCH_DOWN			(1<<PORTD3)
#define SWITCH_INCREASE		(1<<PORTD5)
#define SWITCH_DECREASE		(1<<PORTD6)

//defining some miscaleanous values to improve readibility 
#define top					0
#define bottom				1
#define min					0

// global variables
volatile int interrupt_count = 0;
volatile int enemy_x;
volatile int enemy_y;
volatile int difficulty = 1;
volatile int enemy_val;
volatile int player_val = 0;
volatile int player_position = top;
volatile int lives = 3;
volatile int temp_y;
volatile int score = 0;



//initializing functions 
void main_menu(void);
void game_start(void);
void generate_enemy(void);
void collision(void);

void game_over(void);
int gen_rand(int);




ISR(TIMER1_COMPA_vect){ //interrupt service routine. Responsible for moving enemy across screen 

	char enemy_string[2]; //initialize string 
	
		if (1 == enemy_x ) // if the enemy is about to be moved to the leftmost position
		{
			lcd_gotoxy(1, enemy_y);
			lcd_puts(" "); //clear the enemy from the screen
			collision(); //call the collision function to check if player values match enemy values 
		}
		else { //if we just need to move the enemy to the left (no collision)
			int temp_x = enemy_x;
			
		
			int temp_val = enemy_val;
			
			enemy_x -=1; // subtract 1 from the enemy's x position 
			
			lcd_gotoxy(temp_x,enemy_y); //move to previous enemy location
			lcd_puts(" "); //"clear" the enemy 
			
			lcd_gotoxy(enemy_x,enemy_y); //move to new enemy location

			itoa(temp_val, enemy_string,10); //convert value to string 
			
			lcd_puts(enemy_string); //print enemy in new location 
			
		}
		
		if (10 == enemy_x ) 
		{
			lcd_gotoxy(3,0);
			lcd_puts("     "); //clear the "nice" / "oops" comments 
			
			if (!(score %3)) //if the score is a multiple of 3
			{
			lcd_gotoxy(3,bottom);
			lcd_puts("       "); //clear the "Faster" comment displayed 
			}
		}
	
}




int gen_rand(int max){
	//srand seed has already been initialized 

	int return_value;
	
	

	

	
	return_value = rand()%(max); //fetch random number, take the modulo with the max input
	return  return_value; //return the value 
	
	
	
}

void game_over(void){
	int restart = false; // 
	char final_score[3];
	itoa(score,final_score,10);
	cli(); //turning off interrupts 
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("Game Over");
	lcd_gotoxy(0,1);
	lcd_puts("Score:");
	lcd_gotoxy(7,1);
	lcd_puts(final_score);
	while (!restart)
	{
	
		
		if(!(SWITCH_PIN & SWITCH_SELECT)) {//green button is pushed
			_delay_ms(50);//wait 50ms
			if(!(SWITCH_PIN & SWITCH_SELECT)) {
				restart = true;
				
				
			}
		}
		
		
	} while (!(SWITCH_PIN & (SWITCH_SELECT))){
		// do nothing. Waits for player to release switch
	}
	

	
}

void collision(void){ //collision when the enemy reaches the leftmost position on the screen.
	if ((enemy_val == player_val) && (enemy_y == player_position)) //if the player and enemy's values and position match
	{	
		lcd_gotoxy(3,top);
		lcd_puts("NICE!"); //display congratulatory string
		score+=1; //increase score by 1
		if (!(score % 3)) // if the score is a multiple of 3
		{
			lcd_gotoxy(3,bottom);
			lcd_puts("Faster!"); //inform the player the game is speeding up.
			
			OCR1A = OCR1A - 1000; //increase the speed.
		}	
	}
	
	else{ //if the collision was unsuccessful for the player
		lcd_gotoxy(3,0);
		lcd_puts("Oops!"); //display failure string
		lives -= 1; //decrease number of lives by 1
	}
	generate_enemy(); //generate another enemy
	
}


void generate_enemy(void){
	char enemy_string[1]; //initialize string to be printed 
	enemy_x = 15; //start enemy in the rightmost position 
	
	enemy_val = gen_rand(difficulty+1); //randomly pick the enemy value 
	itoa(enemy_val, enemy_string,10); //convert enemy value to string
	enemy_y = gen_rand(2); // randomly generate enemy y position 
	lcd_gotoxy(enemy_x,enemy_y); //go to enemy position 
	lcd_puts(enemy_string); //print enemy character
	
}



void main_menu(){
	difficulty = 1;
	player_val = 0;
	player_position = top;
	lives = 3;
	score = 0;
	int game_start = 0;
	//int difficulty = 1;
	char diff_str[1];
	OCR1A = 27000;
	
	lcd_clrscr(); //
	lcd_gotoxy( 0, 0); // move cursor to leftmost position of row 2
	lcd_puts("Diff Level: "); //display the string
	lcd_gotoxy(0,1);

	lcd_puts("Hit Green 2 Strt"); 
	lcd_gotoxy(15,0);
	lcd_puts("1");

	
	
	while (!(game_start))
	{
		
		
		if(!(SWITCH_PIN & SWITCH_INCREASE)&& (difficulty != 9 )) {//increase button pressed
			_delay_ms(50);//wait 50ms for debouncing effects 
			if(!(SWITCH_PIN & SWITCH_INCREASE)) {// if switch is still being pressed
				
				difficulty += 1; //increase difficulty by 1
				itoa(difficulty,diff_str,10);//convert duty cycle percentage to string
				
				lcd_gotoxy(15,0);
				
				lcd_puts(diff_str); //print the difficulty
				while (!(SWITCH_PIN & SWITCH_INCREASE)) //wait for player to release switch
				{
				}
			}
		}
		
		if(!(SWITCH_PIN & SWITCH_DECREASE) && (difficulty != 1 )) {//decrease button pressed
			_delay_ms(50);//wait 50ms
			if(!(SWITCH_PIN & SWITCH_DECREASE) ) {
				
				difficulty -= 1; //increase difficulty by 1
				itoa(difficulty,diff_str,10);// convert to string
				lcd_gotoxy(15,0);
				lcd_puts(diff_str); //print difficulty 
				
				while(!(SWITCH_PIN & SWITCH_DECREASE)){ //wait for player to release switch 
					
				}
			}
		}
		
		
		if(!(SWITCH_PIN & SWITCH_SELECT)) {//if select button pressed
			_delay_ms(50);//wait 50ms
			if(!(SWITCH_PIN & SWITCH_SELECT)) {//if button is still being pressed
				lcd_clrscr();
				lcd_gotoxy(0,0);
				

				lcd_puts("Difficulty:") ;
				lcd_gotoxy(0,1);
				itoa(difficulty,diff_str,10);
				lcd_puts(diff_str); //display diffculty selected 
				game_start = 1; //exit the loop, start the game.
				
				
			}
		}
		
		
	}

	

}


void game_start(void){
	char player_val_str[1];
	

	
	//string conversion function
	
	itoa(player_val,player_val_str,10);//
	lcd_clrscr();

	lcd_gotoxy(0,0);
	
	lcd_puts(player_val_str); 
	while (lives > 0) //while the player is still alive 
	{
		
		if(!(SWITCH_PIN & SWITCH_UP)){ //moving the player to the top state
		
			_delay_ms(50); //wait for debouncing
			if(!(SWITCH_PIN & SWITCH_UP)){
				if (top == player_position  ) //if the is already in the top row 
				{ //do nothing
				}
			}
			else{ // if the player is in the bottom row
				player_position = top; //move to top row
				lcd_gotoxy(0,bottom);
				lcd_puts(" "); //clear bottom character 
				lcd_gotoxy(0,top);
				lcd_puts(player_val_str); //print new character
				
			
			}
		
		}
		
	
	
		if(!(SWITCH_PIN & SWITCH_DOWN)){ //if down switch is being pressed
		
			_delay_ms(50); //wait for debouncing 
			if(!(SWITCH_PIN & SWITCH_DOWN)){ //if button is still being pressed
				if (bottom == player_position  ) //if player is still in the bottom row 
				{ //do nothing
				}
			}
			else{// if the player is in the top row
				player_position = bottom; //move player to bottom row
				lcd_gotoxy(0,top);
				lcd_puts(" "); //clear top character 
				lcd_gotoxy(0,bottom);
				lcd_puts(player_val_str); //print new character 
			
			}
		
		}
		
		
		
		
		if(!(SWITCH_PIN & SWITCH_INCREASE)) {//increase button pressed
			_delay_ms(50);//wait 50ms for debouncing 
			if(!(SWITCH_PIN & SWITCH_INCREASE)) { //if button is still being pressed
				if (player_val < difficulty) //if player value is less than the max defined value
				{
				
					player_val += 1; //increase difficulty by 1
					itoa(player_val,player_val_str,10);//convert duty cycle percentage to string
				
					lcd_gotoxy(0,player_position);

					lcd_puts(player_val_str);//print new character value to current position 
					
					
				}
				else{
					player_val = 0; //wrap to zero
					itoa(player_val,player_val_str,10);//convert duty cycle percentage to string
				
					lcd_gotoxy(0,player_position); 

					lcd_puts(player_val_str);//print new character value to current position 
					
				}
				
				
				
			}while(!(SWITCH_PIN & SWITCH_INCREASE)){} //wait for player to release button
		}
		
		if(!(SWITCH_PIN & SWITCH_DECREASE)) {//increase button pressed
			_delay_ms(50);//wait 50ms for debouncing 
			if(!(SWITCH_PIN & SWITCH_DECREASE)) { //if button is still being pressed
				if (player_val > min) //if the player value is greater than zero
				{
				
				
					player_val -= 1; //increase difficulty by 1
					itoa(player_val,player_val_str,10);//convert duty cycle percentage to string
				
					lcd_gotoxy(0,player_position);

					lcd_puts(player_val_str); //print new player value to current position 
					
				}
				else{
					player_val  = difficulty; //wrap to maximum value
					itoa(player_val,player_val_str,10);//convert duty cycle percentage to string
					lcd_gotoxy(0,player_position);
					lcd_puts(player_val_str); //print new player value to current position 
					
				}
				
				
			}while(!(SWITCH_PIN & SWITCH_DECREASE)){} //wait for player to release button
		}
		
		

	}
	game_over(); //once the player loses 3 lives, break out of the while lope and call "game_over"
}
	
	




int main(void)
{

	srand(time(NULL)); //generate seed of random values 
	
	
	
	//Initializing our Timer interrupt
	//MODE 4
	//WGM13-10 --> 0 1 0 0
	//TOP Value Stored at ICR1
	//COM1A1/COM1B1 COM1A0/COM1B0 --> 0 0 Normal Port Operation
	TCCR1B |= (1 << WGM12) | (1 << CS11);
	
	OCR1A = 27000; //how many counts before an interrupt is triggered
	//
	TIMSK1 |= (1<< OCIE1A); //enable timer interrupt for outcompare 1A
	//
	
	
	
	//Mode 14 on Timer 1 will be used to trigger ISR and move numbers towards player
	MCUCR &= ~(1<<PUD); //enabling pull up resistors
	SWITCH_PORT = (SWITCH_DECREASE | SWITCH_INCREASE | SWITCH_SELECT | SWITCH_UP | SWITCH_DOWN );
	
	SWITCH_DDR &= ~(SWITCH_DECREASE | SWITCH_INCREASE | SWITCH_SELECT | SWITCH_UP | SWITCH_DOWN);//set our switches to input
	


	lcd_init(LCD_DISP_ON) ; //initialize LCD display
	lcd_gotoxy(0,0);
	


	// Initialize Screen Display () //
	

	


	while (1) //infinite while loop
	{
	main_menu(); // call main menu
	_delay_ms(2000); //delay 2 seconds
	lcd_clrscr(); //clear screen 
	lcd_gotoxy(0,0);
	lcd_puts("Hello Mr. Ta,") ;
	lcd_gotoxy(0,1);
	lcd_puts("Good Luck");
	_delay_ms(2000); //print message before game starts 
	lcd_clrscr();

	generate_enemy(); //always need to generate the first enemy 
	sei(); //turn on interrupts (turn on enemy movement)
	game_start(); //start the game

		

	}
}
