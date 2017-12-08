/* Author(s): Yehia Naja & Mohammad Khodor
 *            Copyright (c) 2018
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Contributors:
 *                   mbed Microcontroller Library
 *                   Copyright (c) 2006-2012 ARM Limited
 *
 *          This  project  was  made  possible  by  the  generous  
 *          donation    from   The   ARM     University   Program 
 *          of   STMicroelectronics   NUCLEO-F401RE   Development                  
 *          Boards    to    the    University    of      Balamand
 *         
 *      This   Project   was  done  at   the  University  of   Balamand
 *      under the supervision of Dr. Rafic Ayoubi and Mr. Ghattas Akkad
 *      as   part    of   the   CPEN309    course    in    Fall    2017
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */


#include "mbed.h"
#include "TextLCD.h"
#include "string.h"

// Hyper terminal serial
Serial pc(SERIAL_TX, SERIAL_RX);
// The character LCD bus
TextLCD lcd(D8, D9, D4, D5, D6, D7);
// Pushbutton to restart the game
DigitalIn button(PC_13);
// LED of the color white
DigitalOut led_white(D11);
// LED of the color red
DigitalOut led_red(D12);
// LED of the color yellow
DigitalOut led_yellow(D13);
// Potentiometer to vary the speed
AnalogIn pot(A0);

// Number of attempts for the game
const int scores_array_size = 10;

// This will make any character an uppercase
char makeUpperCase(char c)
{
    // if the character is lower
    if(c >= 'a' && c <= 'z')
        // offseting it according to ascii
        return c - ('a' - 'A');
    else
        // return the character
        return c;
}
// return the avarage of a function that the length is global
float average(float * a)
{
    // carries the sum of the function
    float sum = 0;
    // calculate the sum
    for(int i = 0; i < scores_array_size; i++) {
        sum += a[i];
    }
    // return the average
    return sum/scores_array_size;
}

// display the message on the lcd screen
void congrats()
{

    // first line
    lcd.locate(0,0);
    // the message to be displayed
    char msg[] = "Congratulations! Press the blue PB to restart...";
    // message length
    int len = strlen(msg);

    // variable to hold the current positioning on both the screen and the string
    int counter = 0, i = 0;
    while(1) {
        // locate at one element after the current position
        lcd.locate((counter + 1) % 16, 0);
        // insert a white space
        lcd.putc( ' ');
        // move back to current location
        lcd.locate(counter++ % 16, 0);
        // display the current character
        lcd.putc(msg[i++ % len]);
        // wait for the character to be visible on LCD
        wait(0.15);
        // if the user presses the blue button, restart the game
        if(!button) {
            // clear the scren
            lcd.cls();
            break;
        }
    }
}



int main()
{
    // characters that are going to hold the entered characters
    char user_character, current_character;

    wait(1);

    // initialize timer
    Timer t;
    // prompt the user to start the game
    pc.printf("\nLet's play a game! (Press Y to continue): \r");
    // wait for the user to be ready
    while(user_character != 'Y')
        user_character = makeUpperCase(pc.getc()); // Read hyperterminal
    // reset the character to 0
    user_character = 0;
    // explain the game to the user
    pc.printf("\nA letter will be displayed on the screen for 5 times.\r");
    pc.printf("\nWrite it in the terminal.\r");
    // wait for the user to be ready
    while(1) {
        pc.printf("\nReady? (Press Y to continue): \r");
        // wait for the user to be ready
        while(user_character != 'Y')
            user_character = makeUpperCase(pc.getc()); // Read hyperterminal
        // count down to 1
        pc.printf("\n3\r");
        wait(1);
        pc.printf("\n2\r");
        wait(1);
        pc.printf("\n1\r\n");
        wait(1);
        // initate the scores array to 0
        float scores[scores_array_size];
        for(int i = 0; i < scores_array_size; i++)
            scores[i] = 0;
        // reprint characters to be guessed for scores_array_size times
        for(int i = 0 ; i < scores_array_size; i++) {
            // randomly choose a number between 0 and 35
            current_character = (char) rand() % 36;
            // if the number is less than 26, offset it by 'A' in ascii
            // to be a valid ascii character and capital letter
            if( current_character < 26) {
                current_character += 'A';
            }
            // otherwise, it is a number; do the same for numbers
            else {
                current_character -= 26;
                current_character += '0';
            }

            // reset screen
            lcd.cls();
            // the user waits some time before getting the character to be guessed
            // this time is determined by the potentiometer
            wait(pot * 5);
            // display the character
            lcd.printf("Enter letter: %c\n\r", current_character );
            // reset the timer
            t.reset();
            // start the timer
            t.start();
            // reset the entered value
            user_character = 0;
            // wait for the entered value to be entered
            while(!user_character)
                user_character = makeUpperCase(pc.getc()); // Read hyperterminal

            // print it on the pc console
            pc.printf("%c\n\r", user_character);
            t.stop();
            // go to the 2nd line
            lcd.locate(0,1);
            // if he entered the correct value
            if(user_character == current_character) {
                // clear the screen
                lcd.cls();
                // print the time
                lcd.printf("%-14.2fs!", (float)t.read() );
                // then record the time
                scores[i] = t.read();
            }
            // if entered incorrectly
            else {
                // clear the lcd
                lcd.cls();
                // reset the cursor to the second line
                lcd.locate(0,1);
                // print a message
                lcd.printf("Nope :(" );
                // add a penality to the time of response
                scores[i] = 10 + t.read();

            }


        } // end for
        // At the end of the rounds
        // reset the cursor to the second line
        lcd.locate(0,1);
        // print the average time
        float avg = average(scores);
        // reset the leds
        led_yellow = 0;
        led_red = 0;
        led_white = 0;
        // if the average time is less than 2 seconds
        if(avg < 2) {
            // turn on the yellow led
            led_yellow = 1 ;
        }
        // if not but the average time is less than 4 seconds
        else if ( avg < 4) {
            // turn on the red led
            led_red = 1;
        }
        // if not
        else {
            // turn the white (orange) led
            led_white = 1;
        }
        // wait 3 seconds for the result to be displayed
        wait(3);
        // clear the lcd
        lcd.cls();
        // set the cursor to the second line
        lcd.locate(0,1);
        // print the average time
        lcd.printf("END. Avg: %6.2f", avg);
        // also print to the pc hyper terminal
        pc.printf("\nEND. Avg: %6.2f\r", avg );
        // display a message for the end of the game
        // also ask the user to press a button to continue
        congrats();

    }
}
