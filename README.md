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

## The novelty element of the project
The novelty element of the project consists in the implementation of a Single Player game mode in which the player faces a robot controlled by algorithms that imitates the movements of a human player. In this mode, the robot is configured with an expanded paddle to 4 squares, making it more capable of intercepting the ball and providing a more balanced difficulty level.

Thus, while most traditional Pong games include two paddles controlled by human players, this project introduces a robot with a larger paddle in single player mode, which is an interesting and innovative adaptation of the classic game to address the difficulties encountered in the game against a scheduled.

## Use of laboratory functionalities in the project

### 1. SPI (Serial Peripheral Interface)
SPI is a serial communication protocol used to transmit data between the microcontroller and various peripherals. In this project, SPI is used to communicate with the LED matrix (using the MD_MAX72XX library), which is used to display the game state.

Rationale: SPI was chosen for LED matrix communication due to its speed and efficiency. Unlike other serial communication protocols, SPI allows fast and high-speed data transmission between the microcontroller and the LED matrix, essential to render the game in real time, with fast visual updates and without significant delays. This is important to ensure a smooth gaming experience.

### 2. Interrupts
In the project, an interrupt is used to detect joystick button presses (via the joystick1SW pin). It is configured to trigger an interrupt routine when the button is pressed (FALLING).

Rationale: Using interrupts is essential to respond efficiently and quickly to external events such as button presses. Instead of constantly checking the status of the button in a main loop (which would consume resources and introduce delays), interrupts allow the microcontroller to react immediately when the button is pressed without interrupting the rest of the program logic.

Thus, the interrupt is used to start or stop the game depending on the state of the menu, ensuring fast and efficient user interaction.

### 3. Debouncing
Debouncing refers to the technique of filtering out spurious signals (usually electrical fluctuations) that can occur when a button is pressed or released. Without debouncing, a single click could be recorded multiple times due to oscillations in the electrical signal, which could lead to unexpected game behaviors.

Justification: In the case of this project, debouncing is implemented to prevent multiple joystick button presses, which could cause multiple and uncontrolled game menu entries. This is done by checking the time between actions, ensuring that only an actual button press is recorded (either by checking for interruption with a minimal time lag, or by using an algorithm that ignores rapid signal fluctuations).

In the project, there is an implementation of software debouncing, where a time check is added between button state changes, ensuring that changes are made only when they are significant. This ensures a more stable interaction and prevents errors caused by false "presses" of the button.

## Explain how, why and where you made optimizations

### 1. Optimizing the update of the display with the LED matrix
How: Using the displayAnimate() function from the MD_Parola library to update the display only when needed. It helps maintain a constant framerate and prevents overloading with unnecessary updates. Why: If we were to update the display every time in each cycle of the main loop, it would use more CPU resources and slow down the game performance. Thus, we chose to update the display only when necessary. Where: In the loop() function, using myDisplay.displayAnimate() and myDisplay.displayReset() to ensure that the display update is only done when the animation is complete and a new update is needed.

```
if (millis() > future) {
  if (myDisplay.displayAnimate()) {
    myDisplay.displayReset();
  }
}
```

### 2. Game speed optimization based on difficulty
How: Ball speed has been adjusted for difficulty, using a speed multiplier that decreases the time between ball movements as difficulty increases. Why: By adjusting ball speed based on difficulty, we've created a more challenging game experience for users who choose a harder level, without compromising performance on easier levels. Where: In the loop() function, the update time of the ball is adjusted according to the difficulty.

```
future = millis() + 100 - difficulty * 30; // Adjust speed based on difficulty
```

### 3. Reducing memory consumption
How: We used optimized global variables and data structures to save memory, such as using an array to store game state and palettes, and reduced the number of temporary variables that could have consumed extra memory. Why: Since Arduino microcontrollers have limited memory, memory optimization is essential to prevent exceeding available resources and ensure the game runs correctly. Where: In data structures such as t_Position and t_Ball, and in variables that store game state and scores.
```
typedef struct Position {
  int x;
  int y;
} t_Position;

t_Position paddle1, paddle2;

typedef struct Ball {
  int x;
  int y;
  int dx;
  int dy;
} t_Ball;

t_Ball ball;
```

## Pictures of the physical setup
![WhatsApp Image 2024-12-14 at 16 26 11](https://github.com/user-attachments/assets/e50ddeee-1fcc-44d1-be19-0695ab76166c)

## Block Scheme
![Screenshot 2024-12-14 162404](https://github.com/user-attachments/assets/93762b02-d2bd-49df-b366-54a48f58502c)

## Wiring Diagram (Wokwi)
![Screenshot 2025-01-14 111653](https://github.com/user-attachments/assets/67a3354d-3a5f-4825-8cd2-1103ee725de2)


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
[![Watch the video] (https://www.youtube.com/watch?v=Imv30JR2QkI&ab_channel=AlexandruApostu)



