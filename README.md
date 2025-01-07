# Pong-Game
This repository project implements a version of the classic Pong game. This project is for the 3rd year Introduction to Robotics course at the Faculty of Mathematics and Informatics at the University of Bucharest.

## Backstory
This project was born out of a desire to recreate the classic Pong game using an Arduino and a few basic components. Inspired by the simplicity of retro gaming, I wanted to demonstrate how creatively a microcontroller can be used.  
Every line of code and component has been thought to bring a nostalgic experience to life, combining passion for hardware and software in an accessible and fun way.

## How to play
The game is very simple and intuitive to play: the game is usually played by 2 people, but in this project I will implement that the game can be played by one person as well. The game consists in passing a ball from one player to another without losing the ball. To pass the ball players use a paddle.  
The game contains walls at the top and bottom for the ball to hit them to reach the other player. There are no walls on the left and right sides, there are paddles that move from above in the game controlled by the joystick.  
The game will be played on the 8x8 LED matrix in which 2 paddles will appear that will measure 2 hoops in height and one hoop in width and a ball of one square length and width.  

Objective: 
  - Use the paddles to return the ball to the opponent.
  - Move the paddle with the joystick to catch the ball when it returns to you
  - Be careful that the ball does not pass the paddle because the game will end

If the difficulty of the game is set to easy, which means that the ball moves quite slowly and the paddles faster so that the players can catch the ball. The higher the difficulty, the faster the ball will move and the slower the paddles

## Components
- 2x 8x8 LED matrix
- 1x Led matrix module 4x max7219
- 2X Led (Red for player 1 , Blue for player 2)
- 2x Joystick
- 1x Buzzer
- 1x LCD
- 2x Potentiometers(1 for LCD contrast, 1 for buzzer volume)
- 1x Breadboard
- Resistors
- Cables
- Arduino Uno Board

## How to use the menu
When the board is plugging in, a message appears on the LCD and a animation appears on the matrix. The user must press the button of the joystick to access the menu. At the end of the game, an interesting animation will appear again along with some ending sounds.  
The menu contains the following submenus:  
- Start game
- Settings
  - Matrix brightness: one of the users can set the brightness of the matrix
  - Difficulty: one of the users can choose between easy, medium and hard (the ball and the paddles speed)
  - Sounds: one of the users can choose between sounds on or off
  - Score system: display real-time scores and Game Over when a limit is reached one of the users can choose between score is on or off
  - Game mod: single player(against computer) or 1v1
- How to play - information about the game
- About - information about the author
  

## Pictures of the physical setup
![WhatsApp Image 2024-12-14 at 16 26 11](https://github.com/user-attachments/assets/e50ddeee-1fcc-44d1-be19-0695ab76166c)

## Block Scheme
![Screenshot 2024-12-14 162404](https://github.com/user-attachments/assets/93762b02-d2bd-49df-b366-54a48f58502c)

## Wiring Diagram (Wokwi)
![Screenshot 2024-12-14 160838](https://github.com/user-attachments/assets/e38aaebc-a09d-40c6-9644-1460301231a8)  

1. LCD 16x2:
  - K connected to GND
  - A connected to 220 Ohm connected to Vcc(5V)
  - D7,D6,D5,D4 connected to D2,D3,D4,D5 (Arduino)
  - VSS connected to GND
  - VDD connected to Vcc(5V)
  - VO connected to SIG (Potentiometer 1)
  - RS,E connected to D7,D6 (Arduino)
  - RW connected to GND  
2. Potentiometer 1:
  - Vcc connected to Vcc(5V)
  - GND connected to GND
  - SIG connected to VO (LCD 16x2)  
3. Joystick 1:
  - GND connected to GND
  - 5V connected to Vcc(5V)
  - VRx connected to A3 (Arduino)
  - VRy connected to A2 (Arduino)
  - SW connected to D9 (Arduino)  
4. Joystick 2:
  - GND connected to GND
  - 5V connected to Vcc(5V)
  - VRx connected to A5 (Arduino)
  - VRy connected to A4 (Arduino)
  - SW connected to D8 (Arduino)
5. Buzzer:
  - Vcc connected to 220 Ohm connected to Vcc(5V)
  - GND connected to D10 (Arduino)
6. Potentiometer 2:
  - Vcc connected to Vcc(5V)
  - GND connected to GND
  - SIG connected to A1 (Arduino)
7. Red Led:
  - Vcc connected to A0 (Arduino)
  - GND connected to 220 Ohm connected to GND
8. Blue Led:
  - Vcc connected to D1 (Arduino)
  - GND connected to 220 Ohm connected to GND  
9. Led matrix module 4x max7219:
  - Vcc connected to Vcc(5V)
  - GND connected to GND
  - DIN,CS,CLK connected to D11,D12,D13

## BOM (Bill Of Materials)
| Nr. |      Compound      | Amount |                               Link and Datasheet                                    |
|-----|--------------------|--------|-------------------------------------------------------------------------------------|
|  1  |      LCD 16x2      |   1    |             https://docs.arduino.cc/learn/electronics/lcd-displays/                 |
|  2  |    Potentiometer   |   2    |           https://docs.arduino.cc/learn/electronics/potentiometer-basics/           |
|  3  |      Joystick      |   2    |                 https://components101.com/modules/joystick-module                   |
|  4  |       Buzzer       |   1    | https://arduinogetstarted.com/tutorials/arduino-potentiometer-triggers-piezo-buzzer |
|  5  |      Red Led       |   1    |                                    N/A                                              |
|  6  |      Blue Led      |   1    |                                    N/A                                              |
|  7  | Module 4x max7219  |   1    |    https://how2electronics.com/8x32-max7219-dot-matrix-led-display-with-arduino/    |
|  7  |    BreadBord       |   1    |                                    N/A                                              |  

## Details of the pins used
| Nr. |      Compound      |    Arduino pins  |            Reason for use                                        |
|-----|--------------------|------------------|------------------------------------------------------------------|
|  1  |      LCD 16x2      |     D2 - D7      | LCD data and control (4-bit mode)                                |
|  2  |    Potentiometer   |        D9        |        Read analog values                                        |
|  3  |      Joystick      | A2 - A6 && D8,D9 |  The pallets will move with them                                 |
|  4  |       Buzzer       |        D10       |        PWM signal for sound                                      |
|  5  |      Red Led       |         A0       | It will light up for the player (team) who has to catch the ball |
|  6  |      Blue Led      |         D1       | It will light up for the player (team) who has to catch the ball |
|  7  | Module 4x max7219  |     D11 - D13    |    The place where the game will be played                       |

## Estimated power consumption
 - Arduino UNO: ~50mA.
 - LCD: ~20mA (backlight).
 - Buzzer: ~15-30 mA (operation).
 - Touch sensors: ~10 mA each.
 - LED arrays: ~60-100 mA depending on the number of active LEDs.
 - Potentiometers: Negligible consumption.

## Video with physical assembly functionality

## Conclusions

## Bibliography



