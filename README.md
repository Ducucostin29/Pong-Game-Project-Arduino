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
- 1x Matrix Shield
- 2x RGB LED (1 for each player)
- 2x Joystick
- 1x Buzzer
- 1x LCD
- 2x Potentiometers(1 for LCD contrast, 1 for buzzer volume)
- 1x Breadboard
- Resistors
- Cables
- Arduino Uno Board
- 104 ceramic capacitor
- 1x Button

## How to use the menu
When the board is plugging in, a message appears on the LCD and a animation appears on the matrix. The user must press the button of the joystick to access the menu. At the end of the game, an interesting animation will appear again along with some ending sounds.  
The menu contains the following submenus:  
- Start game
- Settings
  - The names of the players: one of the users can enter their name(but  only 6 characters)
  - LCD brightness: one of the users can set the brightness of the LCD
  - Matrix brightness: one of the users can set the brightness of the matrix
  - Difficulty: one of the users can choose between easy, medium and hard (the ball and the paddles speed)
  - Sounds: one of the users can choose between sounds on or off
  - Score system: display real-time scores and Game Over when a limit is reached one of the users can choose between score is on or off
  - Power-ups: one of the users can choose between power-ups are on or off (Expanding the palette; Acceleration/deceleration of the ball)
  - Game mod: single player(against computer) or 1v1
- How to play - information about the game
- About - information about the author
  

## Pictures of the physical setup

## Wiring Diagram (TinkerCAD)

## Video with physical assembly functionality

## Wiring Diagram (TinkerCAD)

## Conclusions

## Bibliography


