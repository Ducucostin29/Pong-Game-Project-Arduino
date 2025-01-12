#include <LiquidCrystal.h>
#include <Arduino.h>
#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>
#include <LedControl.h>

// Hardware definitions
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define DIN 11
#define CLK 13
#define CS_PIN 9
#define joystick1VRx A5
#define joystick1VRy A4
#define joystick2VRx A3
#define joystick2VRy A2
#define joystick1SW 8
#define joystick2SW 12
#define buzz 10
#define in_p A1
#define redled A0
#define blueled 1

// LCD Pins for communication with the display
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Create an LCD object for controlling the display

// LED Matrix setup for displaying graphical content (score, game state, etc.)
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX myMatrix = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Menu and settings variables for user interaction and game configuration
const String menuInput[4] = {"Start game", "Settings", "How to play", "About"};
const String settings[8] = {"Matrix bright.", "Difficulty", "Sounds", "Score", "Game mod"};
int matrixBrightness = 7;  // Initial brightness level for the LED matrix
int difficulty = 1;        // Game difficulty (0 = easy, 1 = medium, 2 = hard)
bool sounds = true;        // Sound setting (enabled by default)
bool scoreEnabled = true;  // Score display setting (enabled by default)
bool gameModeSinglePlayer = true; // Game mode (single player by default)
const char *name = "Apostu Alexandru-Costin"; // Developer name
const char *github = "github.com/Ducucostin29"; // Developer's GitHub page

// Player scores initialization
int player1Score = 0, player2Score = 0;

// Button press detection for menu navigation
volatile bool buttonPressed = false; // Used in ISR to detect button press

// Menu navigation and settings control variables
int menuIndex = 0; // Current menu selection index
bool inSubMenu = false; // Flag for tracking if in a submenu
int subMenuIndex = 0; // Current submenu selection index

// Game parameters: speed and paddle distance
int speedMultiplier = 1; // Ball speed multiplier
int paddleDistance = 3;  // Initial distance between paddles

// Timing variables for controlling game flow and animations
unsigned long prevMillis = 0, prevDisplayMillis = 0;
unsigned long future = millis();
char number = '3'; // Countdown for game start from 3
unsigned long timeLastUpdateStartAnimation = millis();
unsigned long timeLastSecondaryStartAnimation = millis();
unsigned long currentTime = millis();
unsigned long lastTimeHowToPlay = millis();
int indexMessageHowToPlay = 0; // Index for message display in the "How to Play" section

// Game states: started and playing flags
bool started = false; // Game started flag
bool playing = false; // Game playing flag

// LCD update flag for managing display updates
bool changeLCD = false; // Flag indicating when the LCD needs to be updated

// Menu display and sound settings
String menuDisplay = ""; // Current display screen (menu, game, etc.)
bool soundEnabled = true; // Sound enabled by default

// Sub-sub-menu navigation index
int subsubMenuIndex = 0;

// Maximum string length for menu and settings text
int maxLen;

String text1, text2; // Used for holding dynamic text content

// Joystick reading variables
int joystickY = analogRead(joystick1VRy); // Read Y-axis value for joystick 1
int joystickX = analogRead(joystick1VRx); // Read X-axis value for joystick 1
int joystick2Y = analogRead(joystick2VRy); // Read Y-axis value for joystick 2
int joystick2X = analogRead(joystick2VRx); // Read X-axis value for joystick 2

// Game frame and row configuration
const byte rows = 32; // Number of rows on the display (for game drawing)
int frame; // Frame count for the game loop
byte game[rows]; // Array for game state representation on the display

// Paddle position structure
typedef struct Position
{
  int x; // X-axis position of the paddle
  int y; // Y-axis position of the paddle
} t_Position;

t_Position paddle1, paddle2; // Paddle positions for player 1 and 2

// Ball structure for handling ball movement
typedef struct Ball
{
  int x; // Ball X position
  int y; // Ball Y position
  int dx; // Ball's X direction (speed and movement)
  int dy; // Ball's Y direction (speed and movement)
} t_Ball;

t_Ball ball; // Ball object

// Joystick structure for handling joystick inputs
typedef struct Joystick
{
  bool left;  // Left direction on joystick
  bool right; // Right direction on joystick
  bool up;    // Up direction on joystick
  bool down;  // Down direction on joystick
  bool button; // Button press state
} t_Joystick;

t_Joystick joystick1, joystick2; // Joystick objects for player 1 and 2


// Function prototypes
void initHardware();
void displayMenu();
void handleSettings();
void playGame();
void displayHowToPlay();
void displayAbout();
void displaySettings();
void resetToMenu();
void playTone(int frequency, int duration);
void startAnimation();
void welcomeAnimation();
void navigateMenu();
void play();
void initializeGame();
void displayDifficulty();
void adjustDifficulty();
void playPaddle1Sound();
void playPaddle2Sound();
void playGameOverMelody();
void proceseazaJoystick(int x, int y, t_Joystick *joystick);
bool compJoisticks(t_Joystick *a, t_Joystick *b);
void display();
void displayScoresInGame();
void displayScores();
void updateScore(int player);
void updateAndDisplayScores(int player);
void joystickButtonISR();

// Setup function is called once when the program starts
void setup()
{
  initHardware(); // Initialize hardware components (LCD, Joystick, etc.)
  welcomeAnimation(); // Display a welcome animation (usually startup visuals)
  displayMenu(); // Display the main menu on the screen
  randomSeed(analogRead(0)); // Initialize random number generator using analog pin 0 (for more randomness)
  menuDisplay = "menu"; // Set the initial display screen to the main menu
  attachInterrupt(digitalPinToInterrupt(joystick1SW), joystickButtonISR, FALLING); // Set up interrupt for joystick button press (ISR for FALLING edge)
}

// Main loop where the game logic happens repeatedly
void loop()
{
  // Read joystick positions for both players (X and Y axis)
  joystickY = analogRead(joystick1VRy); // Read Y-axis for joystick 1
  joystickX = analogRead(joystick1VRx); // Read X-axis for joystick 1
  joystick2Y = analogRead(joystick2VRy); // Read Y-axis for joystick 2
  joystick2X = analogRead(joystick2VRx); // Read X-axis for joystick 2

  // Check if a button press has been detected (flag set by interrupt)
  if (buttonPressed)
  {
    buttonPressed = false; // Reset the flag to avoid multiple triggers
    if (menuDisplay == "menu")
    {
      menuDisplay = "play"; // If in main menu, change to the 'play' screen to start the game
    }
    else
    {
      resetToMenu(); // If not in menu, return to main menu
    }
  }

  // Check if it's time to update game actions based on timing
  if (millis() > future)
  {
    if (myDisplay.displayAnimate()) // Animate the LED matrix display (e.g., scrolling text)
    {
      myDisplay.displayReset(); // Reset display when animation is finished
    }

    // Initialize the game if it's not started yet
    if (playing && !started)
    {
      initializeGame(); // Initialize game settings (paddles, ball, scores)
      started = true; // Set the flag that the game has started
    }

    // Run the game logic after initialization
    if (started)
    {
      play(); // Execute the play logic (e.g., move paddles, ball, update game state)
      future = millis() + 100 - difficulty * 30; // Adjust speed based on difficulty (faster for harder levels)
    }

    // If the game is not playing, update the display (e.g., showing menu)
    if (!playing)
      display(); // Update the display based on the current screen (menu, instructions, etc.)

    // Navigate the menu using the joystick inputs
    navigateMenu();
  }
}


/**
 * Initializes the hardware components of the system, such as the LCD, LED Matrix,
 * joysticks, and serial communication. Sets up input pins and configurations.
 */
void initHardware()
{
  // Initialize the LCD display with 16 columns and 2 rows
  lcd.begin(16, 2);
  lcd.print("Welcome!"); // Display a welcome message on the LCD
  
  // Configure pins for the buzzer and an additional input pin
  pinMode(buzz, INPUT);
  pinMode(in_p, INPUT);
  
  // Initialize the LED Matrix display
  myDisplay.begin();
  myDisplay.setIntensity(matrixBrightness); // Set initial brightness
  myDisplay.displayClear(); // Clear the display
  
  // Configure another LED matrix control
  myMatrix.begin();
  myMatrix.control(MD_MAX72XX::INTENSITY, 0); // Set intensity to the lowest
  myMatrix.clear(); // Clear the matrix
  
  // Set up the joystick pins as inputs
  pinMode(joystick1VRx, INPUT);
  pinMode(joystick1VRy, INPUT);
  pinMode(joystick1SW, INPUT_PULLUP); // Enable pull-up for the joystick switch
  pinMode(joystick2VRx, INPUT);
  pinMode(joystick2VRy, INPUT);
  pinMode(joystick2SW, INPUT_PULLUP); // Enable pull-up for the second joystick switch
  
  // Initialize serial communication for debugging or logging
  Serial.begin(28800);
}


/**
 * Plays a tone on the buzzer with the specified frequency and duration.
 * The sound is emitted only if the sound is enabled.
 * 
 * @param frequency The frequency of the tone to be played (in Hz).
 * @param duration The duration for which the tone should be played (in milliseconds).
 */
void playTone(int frequency, int duration)
{
  if (soundEnabled) // Check if sound is enabled
  {
    tone(buzz, frequency, duration); // Emit a tone on the buzzer with the given frequency and duration
  }
}

/**
 * Plays the sound effect when Paddle 1 hits the ball.
 * This function plays a 1000 Hz tone for 100 milliseconds.
 */
void playPaddle1Sound()
{
  playTone(1000, 100); // Play a 1000 Hz tone for 100ms when Paddle 1 hits the ball
}

/**
 * Plays the sound effect when Paddle 2 hits the ball.
 * This function plays a 1500 Hz tone for 100 milliseconds.
 */
void playPaddle2Sound()
{
  playTone(1500, 100); // Play a 1500 Hz tone for 100ms when Paddle 2 hits the ball
}

/**
 * Plays a melody for the game over sound.
 * The melody consists of four notes with frequencies 400 Hz, 500 Hz, 600 Hz, and 700 Hz,
 * each lasting 300 milliseconds.
 */
void playGameOverMelody()
{
  int melody[] = {400, 500, 600, 700};   // Define the frequencies for the notes
  int duration[] = {300, 300, 300, 300}; // Define the duration for each note in the melody
  for (int i = 0; i < 4; i++) // Loop through each note in the melody
  {
    playTone(melody[i], duration[i]); // Play each note with the specified frequency and duration
    delay(duration[i]); // Wait for the duration of the note before playing the next one
  }
}

/**
 * Interrupt Service Routine (ISR) for the joystick button press.
 * This function sets the flag to true when the button is pressed, indicating that
 * the button has been activated.
 */
void joystickButtonISR()
{
  buttonPressed = true; // Set the flag when the joystick button is pressed
}

/**
 * Resets the game state and returns to the main menu.
 * This function resets the scores, flags, and display settings to their default values.
 */
void resetToMenu()
{
  player1Score = 0; // Reset player 1's score
  player2Score = 0; // Reset player 2's score
  started = false;  // Mark the game as not started
  playing = false;  // Mark the game as not playing
  menuDisplay = "menu"; // Set the display to show the menu
}

/**
 * Displays a welcome animation on the LED matrix.
 * The animation scrolls the word "Welcome" from left to right on the display,
 * followed by a clear screen.
 */
void welcomeAnimation()
{
  myDisplay.begin(); // Initialize the LED matrix
  myDisplay.displayClear(); // Clear the display
  myDisplay.displayScroll("Welcome", PA_CENTER, PA_SCROLL_LEFT, 200); // Scroll "Welcome" across the screen
  myDisplay.displayClear(); // Clear the display after the animation
}

/*void startAnimation()
{
  while (currentTime - timeLastUpdateStartAnimation < 4000) { // Run for 4 seconds (3, 2, 1, GO!)
    currentTime = millis();

    if (currentTime - timeLastSecondaryStartAnimation >= 1000) { // Update every second
      if (number >= '1') {
        myDisplay.displayClear();
        myDisplay.setTextAlignment(PA_CENTER);
        myDisplay.print(number);
        number--;
      } else if (number == '0') {
        myDisplay.displayClear();
        myDisplay.setTextAlignment(PA_CENTER);
        myDisplay.print("GO!");
        number--; // Set to a value lower than '0' to stop updating
      }
      timeLastSecondaryStartAnimation = currentTime; // Reset the secondary timer
    }
  }

  myDisplay.displayClear(); // Clear display at the end of the countdown
}*/

/**
 * Displays a countdown animation on the LED matrix before starting the game.
 * The countdown shows "3", "2", "1", and "GO!" with 1-second intervals.
 */
void startAnimation()
{
  myDisplay.displayClear(); // Clear the display before starting
  myDisplay.setTextAlignment(PA_CENTER); // Center-align the text
  myDisplay.print("3"); // Display "3"
  delay(1000); // Wait for 1 second
  myDisplay.print("2"); // Display "2"
  delay(1000); // Wait for 1 second
  myDisplay.print("1"); // Display "1"
  delay(1000); // Wait for 1 second
  myDisplay.print("GO!"); // Display "GO!"
  delay(1000); // Wait for 1 second before clearing the display
  myDisplay.displayClear(); // Clear the display after the animation
}

/**
 * Displays the main menu on the LCD.
 * The menu header "<MENU>" is shown, and the current menu item is displayed based on the index.
 */
void displayMenu()
{
  lcd.clear(); // Clear the LCD
  lcd.print("<MENU>"); // Display the menu title
  lcd.setCursor(0, 1); // Set cursor to the second row
  lcd.print(menuInput[menuIndex]); // Display the current menu item based on the index
}

/**
 * Displays the settings submenu on the LCD.
 * The current setting is displayed, and additional details (like brightness, difficulty, etc.)
 * are shown based on the submenu index.
 */
void displaySettings()
{
  lcd.clear(); // Clear the LCD
  lcd.print(settings[subMenuIndex]); // Display the current setting menu item
  lcd.setCursor(0, 1); // Set cursor to the second row
  if (subMenuIndex == 0)
  { // Matrix Brightness
    lcd.print("Brightness: ");
    lcd.print(matrixBrightness); // Display the current matrix brightness
  }
  else if (subMenuIndex == 1)
  { // Difficulty
    lcd.print("Difficulty: ");
    displayDifficulty(); // Display the current difficulty level
  }
  else if (subMenuIndex == 2)
  { // Sounds
    lcd.print("Sounds: ");
    if (sounds)
      lcd.print("ON"); // Display "ON" if sounds are enabled
    else
      lcd.print("OFF"); // Display "OFF" if sounds are disabled
  }
  else if (subMenuIndex == 3)
  { // Score
    lcd.clear(); // Clear the screen for the score menu
    lcd.print("Score: ");
    if (scoreEnabled)
    {
      lcd.print("ON"); // Display "ON" if score display is enabled
    }
    else
    {
      lcd.print("OFF"); // Display "OFF" if score display is disabled
    }
  }
  else if (subMenuIndex == 4)
  { // Power-ups
    lcd.print("Game mod: ");
    if (gameModeSinglePlayer)
      lcd.print("Single Player"); // Display "Single Player" if single player mode is selected
    else
      lcd.print("Multiplayer"); // Display "Multiplayer" if multiplayer mode is selected
  }
}

/**
 * Handles the actions within the settings menu based on joystick input.
 * It manages the interactions for changing brightness, difficulty, sounds, score display, and game mode.
 */
void handleSettings()
{
  if (subMenuIndex == 0)
  { // Matrix Brightness
    if (subsubMenuIndex == 0 && joystick1.button)
    {
      subsubMenuIndex = 1; // Move to the next setting
      return;
    }
    if (subsubMenuIndex == 1)
    {
      if (joystick1.down && matrixBrightness > 1)
      { // Decrease brightness if joystick is moved down and brightness is greater than 1
        matrixBrightness--;
        myDisplay.setIntensity(matrixBrightness); // Update the matrix brightness
      }
      else if (joystick1.up && matrixBrightness < 15)
      { // Increase brightness if joystick is moved up and brightness is less than 15
        matrixBrightness++;
        myDisplay.setIntensity(matrixBrightness); // Update the matrix brightness
      }
    }
  }
  else if (subMenuIndex == 1)
  { // Difficulty
    if (subsubMenuIndex == 0 && joystick1.button)
    {
      subsubMenuIndex = 1; // Move to the next setting
      return;
    }
    if (subsubMenuIndex == 1)
    {
      adjustDifficulty(); // Allow the user to adjust the difficulty
    }
  }
  else if (subMenuIndex == 2)
  { // Sounds
    if (subsubMenuIndex == 0 && joystick1.button)
    {
      subsubMenuIndex = 1; // Move to the next setting
      return;
    }
    if (subsubMenuIndex == 1)
    {
      if (joystick1.down)
      { // Toggle sound setting when joystick is moved down
        sounds = !sounds; // Toggle the sound ON/OFF
      }
    }
  }
  else if (subMenuIndex == 3)
  { // Score
    if (subsubMenuIndex == 0 && joystick1.button)
    {
      subsubMenuIndex = 1; // Move to the next setting
      return;
    }
    if (subsubMenuIndex == 1)
    {
      if (joystick1.down || joystick1.up)
      { // Toggle score display when joystick is moved up or down
        scoreEnabled = !scoreEnabled; // Toggle the score display ON/OFF
      }
    }
  }
  else if (subMenuIndex == 4)
  { // Game Mode (Single player / Multiplayer)
    if (subsubMenuIndex == 0 && joystick1.button)
    {
      subsubMenuIndex = 1; // Move to the next setting
      return;
    }
    if (subsubMenuIndex == 1)
    {
      if (joystick1.down)
        gameModeSinglePlayer = false; // Set to multiplayer mode if joystick is moved down
      else if (joystick1.up)
        gameModeSinglePlayer = true; // Set to single player mode if joystick is moved up
    }
  }
}

/**
 * Displays the current difficulty level on the LCD.
 * The difficulty can be "Easy", "Medium", or "Hard" based on the difficulty variable.
 */
void displayDifficulty()
{
  if (difficulty == 0)
    lcd.print("Easy"); // Display "Easy" if difficulty is 0
  else if (difficulty == 1)
    lcd.print("Medium"); // Display "Medium" if difficulty is 1
  else if (difficulty == 2)
    lcd.print("Hard"); // Display "Hard" if difficulty is 2
}

/**
 * Adjusts the difficulty level based on joystick input.
 * The joystick's up and down movements increase or decrease the difficulty level.
 * The difficulty is limited between 0 (Easy) and 2 (Hard).
 */
void adjustDifficulty()
{
  if (joystick1.down && difficulty > 0)
  {               // If the joystick is moved down and difficulty is greater than 0
    difficulty--; // Decrease the difficulty
  }
  else if (joystick1.up && difficulty < 2)
  {               // If the joystick is moved up and difficulty is less than 2
    difficulty++; // Increase the difficulty
  }
}

/**
 * Updates the score of the specified player.
 * The player's score is incremented by 1.
 *
 * @param player The player whose score should be updated (1 or 2).
 */
void updateScore(int player)
{
  if (player == 1)
  {
    player1Score++; // Increase player 1's score
  }
  else if (player == 2)
  {
    player2Score++; // Increase player 2's score
  }
}

/**
 * Updates the score for the specified player and immediately displays the updated scores.
 *
 * @param player The player whose score should be updated (1 or 2).
 */
void updateAndDisplayScores(int player)
{
  // Update the score and then display the updated scores
  updateScore(player);
  displayScores();
}

/**
 * Displays the current scores for both players on the LCD.
 * If score display is enabled, the scores of player 1 and player 2 are shown.
 * If score display is disabled, a "Start game" message is shown instead.
 */
void displayScores()
{
  if (!changeLCD && started) // Ensure the game has started and LCD is not changing
  {
    lcd.clear(); // Clear the LCD screen
    if (scoreEnabled)
    {
      lcd.setCursor(0, 0); // Set cursor to the first row
      lcd.print("P1: ");
      lcd.print(player1Score); // Display player 1's score

      lcd.setCursor(0, 1); // Set cursor to the second row
      lcd.print("P2: ");
      lcd.print(player2Score); // Display player 2's score
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Start game"); // Display "Start game" if score display is disabled
    }
  }
}

/**
 * Displays a tutorial on how to play the game on the LCD.
 * The tutorial messages are displayed one by one with a delay between them.
 * The tutorial includes instructions for moving paddles, hitting the ball, and scoring.
 */
void displayHowToPlay()
{
  const unsigned long timpiMesaje[] = {3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000}; // Time for each message
  const char *mesaje[] = {
      "HOW TO PLAY",
      "1. Move paddles",
      "with joystick",
      "2. Hit the ball",
      "to opponent",
      "3. Don't let",
      "it pass paddle",
      "4. First to 3",
      "wins the game!",
      "^ Back"};

  lcd.clear(); // Clear the LCD before showing the tutorial
  lcd.print(mesaje[indexMessageHowToPlay]); // Display the first tutorial message

  while (indexMessageHowToPlay < 10)
  { // Loop through the tutorial messages
    currentTime = millis();

    if (currentTime - lastTimeHowToPlay >= timpiMesaje[indexMessageHowToPlay])
    {
      indexMessageHowToPlay++; // Move to the next message
      lastTimeHowToPlay = currentTime;

      if (indexMessageHowToPlay < 10)
      { // If there are more messages to display
        lcd.clear(); // Clear the LCD before showing the next message
        lcd.print(mesaje[indexMessageHowToPlay]); // Display the next message
      }
    }
  }

  // Finally, display "< Back"
  lcd.clear();
  lcd.print(mesaje[10]);
}

/**
 * Scrolls two lines of text on the LCD.
 * The text is displayed one part at a time with a delay between each scroll.
 * The scrolling is done by displaying 16 characters at a time from each line.
 *
 * @param line1 The first line of text to scroll.
 * @param line2 The second line of text to scroll.
 * @param delayMs The delay in milliseconds between each scroll step (default is 300 ms).
 */
void scrollTextOnLCD(const char *line1, const char *line2, int delayMs = 300)
{
  text1 = line1;
  text2 = line2;

  // Add spaces for smooth scrolling
  text1 += "    ";
  text2 += "    ";

  maxLen = max(text1.length(), text2.length()); // Find the maximum length of the two lines

  for (int i = 0; i < maxLen; i++)
  {
    lcd.clear(); // Clear the LCD before each scroll step

    lcd.setCursor(0, 0);
    lcd.print(text1.substring(i, i + 16)); // Display 16 characters from the first line
    lcd.setCursor(0, 1);
    lcd.print(text2.substring(i, i + 16)); // Display 16 characters from the second line

    delay(delayMs); // Wait for the specified delay before scrolling further
  }
}

/**
 * Displays information about the game or developer on the LCD.
 * This includes scrolling the developer's name and GitHub handle, followed by a "Back" message.
 */
void displayAbout()
{
  lcd.clear(); // Clear the LCD
  scrollTextOnLCD(name, github, 250); // Scroll the name and GitHub handle
  lcd.clear(); // Clear the LCD after scrolling
  lcd.print("^ Back"); // Display the "Back" option
}

/**
 * Initializes the game by setting the starting positions of the paddles and ball.
 * The function also resets the game state and adjusts the paddles based on the difficulty.
 */
void initializeGame()
{
  for (int i = 0; i < rows; i++)
    game[i] = 0; // Clear the game array for the board state
  
  frame = 0; // Reset frame count
  
  // Set initial positions for paddles
  paddle1.x = 4;
  paddle1.y = 3;
  paddle2.x = 4;
  paddle2.y = rows - 1 - 3;

  // Adjust paddle positions based on difficulty level
  if (difficulty == 1)
  {                 // Medium difficulty
    paddle1.y += 2; // Move paddle 1 closer
    paddle2.y -= 2; // Move paddle 2 closer
  }
  else if (difficulty == 2)
  {                 // Hard difficulty
    paddle1.y += 4; // Move paddle 1 even closer
    paddle2.y -= 4; // Move paddle 2 even closer
  }

  // Set random initial position for the ball
  ball.x = random(1, 7); 
  if (random(0, 2) == 0)
  {
    ball.y = paddle1.y + 1; // Ball starts near paddle1
    ball.dy = 1; // Ball direction is down
  }
  else
  {
    ball.y = paddle2.y - 1; // Ball starts near paddle2
    ball.dy = -1; // Ball direction is up
  }
  
  // Set random horizontal direction for the ball
  int r = random(0, 3);
  if (r == 0)
    ball.dx = 1;
  if (r == 1)
    ball.dx = -1;
  if (r == 2)
    ball.dx = 0;
}

/**
 * Renders the current game state on the LED matrix.
 * The game array represents the state of the game, including paddles and ball.
 */
void randareJoc()
{
  for (int i = 0; i < rows; i++)
  {
    myMatrix.setColumn(floor(i / 8), i % 8, game[i]); // Update matrix columns with game state
  }
}

/**
 * Runs the main game logic, updates the ball's position, checks for collisions,
 * and updates the game state (such as score and paddle positions).
 */
void play()
{
  // Clear the game state for the current frame
  for (int i = 0; i < rows; i++)
  {
    game[i] = 0; // Reset all rows
  }

  // Draw paddles and ball in the game state
  game[paddle1.y] = game[paddle1.y] | (1 << paddle1.x) | (1 << (paddle1.x + 1) | (1 << (paddle1.x + 2))); // Paddle 1
  game[paddle2.y] = game[paddle2.y] | (1 << paddle2.x) | (1 << (paddle2.x + 1) | (1 << (paddle2.x + 2))); // Paddle 2
  game[ball.y] = game[ball.y] | (1 << ball.x); // Ball

  // Update ball's position
  ball.x = ball.dx + ball.x;
  ball.y = ball.dy + ball.y;

  // Collision detection with paddle1
  if ((ball.y == paddle1.y && (ball.x == paddle1.x || ball.x == paddle1.x + 1 || ball.x == paddle1.x + 2)))
  {
    ball.dy = -ball.dy; // Reverse vertical direction
    if (ball.x == paddle1.x)
      ball.dx = -1; // Reflect ball horizontally
    if (ball.x == paddle1.x + 1)
      ball.dx = 0; // No horizontal movement
    if (ball.x == paddle1.x + 2)
      ball.dx = 1; // Reflect ball horizontally in the other direction
    playPaddle1Sound(); // Play sound when paddle 1 is hit
  }

  // Collision detection with paddle2
  if (ball.y == paddle2.y && (ball.x == paddle2.x || ball.x == paddle2.x + 1 || ball.x == paddle2.x + 2))
  {
    ball.dy = -ball.dy; // Reverse vertical direction
    if (ball.x == paddle2.x)
      ball.dx = -1; // Reflect ball horizontally
    if (ball.x == paddle2.x + 1)
      ball.dx = 0; // No horizontal movement
    if (ball.x == paddle2.x + 2)
      ball.dx = 1; // Reflect ball horizontally in the other direction
    playPaddle2Sound(); // Play sound when paddle 2 is hit
  }

  // Ball collision with the left or right wall
  if (ball.x == 0)
    ball.dx = 1; // Ball hits left wall
  if (ball.x == 7)
    ball.dx = -1; // Ball hits right wall

  // Check if a player has scored
  if (ball.y == 0)
    updateAndDisplayScores(2); // Player 2 scores
  if (ball.y == rows - 1)
    updateAndDisplayScores(1); // Player 1 scores

  // Check for game over conditions
  if (ball.y < paddle1.y - 1 || ball.y > paddle2.y + 1)
  {
    if (player1Score >= 2 || player2Score >= 2)
    {
      lcd.clear();
      lcd.print("Game Over");
      myDisplay.displayClear();
      // Announce winner
      if (player1Score > player2Score)
        myDisplay.displayScroll("Game Over <3 P1 wins", PA_LEFT, PA_SCROLL_LEFT, 200);
      else
        myDisplay.displayScroll("Game Over <3 P2 wins", PA_RIGHT, PA_SCROLL_LEFT, 200);
      myDisplay.displayClear();

      playGameOverMelody(); // Play game over sound
      resetToMenu(); // Return to the main menu
    }
    else
    {
      if (ball.y < paddle1.y - 1)
      {
        myDisplay.displayClear();
        if (gameModeSinglePlayer)
          myDisplay.print("Game Over <3 ROBOT wins"); // Robot wins in single-player mode
        else
          myDisplay.print("Game Over <3 P2 wins"); // Player 2 wins
        started = false;
        player2Score++;
        playGameOverMelody(); // Play game over sound
      }
      if (ball.y > paddle2.y + 1)
      {
        myDisplay.displayClear();
        myDisplay.print("Game Over <3 P1 wins"); // Player 1 wins
        started = false;
        player1Score++;
        playGameOverMelody(); // Play game over sound
      }
    }
  }
  else
  {
    // AI for single-player mode
    if (gameModeSinglePlayer)
    {
      int r = random(0, 3); // Random movement for AI paddle
      paddle2.x += r - 1; // AI adjusts paddle position randomly
      if (paddle2.x < 0)
        paddle2.x = 0; // Ensure paddle doesn't go out of bounds
      if (paddle2.x > 5)
        paddle2.x = 5; // Ensure paddle doesn't go out of bounds
    }
  }

  randareJoc(); // Render the game state to the LED matrix
  // Serial.println("randare joc"); // Uncomment for debugging purposes
}

/**
 * Starts the game, displaying a "Game Start" message and an animation.
 * Then, the game enters the playing state.
 */
void playGame()
{
  lcd.clear(); // Clear the screen
  lcd.print("Game Start"); // Display "Game Start"
  startAnimation(); // Display countdown animation
  playing = true; // Set the game to the playing state
  started = false; // Set the game as not started yet (waiting for input)
}

/**
 * Plays a sound at the given frequency and duration using the buzzer.
 * @param frequency The frequency of the sound in Hertz.
 * @param duration The duration of the sound in milliseconds.
 */
void playSound(int frequency, int duration)
{
  tone(buzz, frequency, duration); // Play the tone
  delay(duration); // Wait for the sound duration
  noTone(buzz); // Stop the sound after the delay
}

/**
 * Processes the joystick input and updates the joystick state.
 * The joystick is mapped to four directions (left, right, up, down).
 * @param x The X-axis value of the joystick.
 * @param y The Y-axis value of the joystick.
 * @param joystick The joystick object to be updated.
 */
void proceseazaJoystick(int x, int y, t_Joystick *joystick)
{
  joystick->left = x < 200; // Joystick left if X value is below 200
  joystick->right = x > 800; // Joystick right if X value is above 800
  joystick->up = y < 200; // Joystick up if Y value is below 200
  joystick->down = y > 800; // Joystick down if Y value is above 800
}

/**
 * Compares the state of two joysticks.
 * @param a The first joystick to compare.
 * @param b The second joystick to compare.
 * @return True if both joysticks have the same state, false otherwise.
 */
bool compJoisticks(t_Joystick *a, t_Joystick *b)
{
  // Compare joystick directions and button state
  if (a->left == b->left && a->right == b->right && a->down == b->down && a->up == b->up && a->button == b->button)
    return true;
  return false;
}

/**
 * Displays the appropriate screen or game state based on the current menu selection.
 */
void display()
{
  unsigned long actualMilis = millis(); // Get the current time in milliseconds
  if (actualMilis - prevDisplayMillis < 100)
    return; // Prevent rapid joystick presses from triggering multiple actions
  prevDisplayMillis = actualMilis; // Update previous display time

  // Display different screens based on menu selection
  if (menuDisplay == "play")
    playGame(); // Start the game
  if (menuDisplay == "howToPlay")
    displayHowToPlay(); // Show how to play instructions
  if (menuDisplay == "about")
    displayAbout(); // Show about information
  if (menuDisplay == "settings")
    displaySettings(); // Show settings screen
  if (menuDisplay == "menu")
    displayMenu(); // Show main menu
}

/**
 * Navigates the menu and updates the joystick state for the current player(s).
 * This handles menu navigation, game controls, and transitions between screens.
 */
void navigateMenu()
{
  t_Joystick joystickNou1, joystickNou2;
  proceseazaJoystick(joystickY, joystickX, &joystickNou1); // Process joystick input for player 1
  proceseazaJoystick(joystick2Y, joystick2X, &joystickNou2); // Process joystick input for player 2

  joystickNou1.button = digitalRead(joystick1SW) == LOW; // Check if player 1's joystick button is pressed
  joystickNou2.button = digitalRead(joystick2SW) == LOW; // Check if player 2's joystick button is pressed

  unsigned long actualMilis = millis(); // Get current time in milliseconds
  if (started)
  {
    // Prevent rapid joystick presses during the game
    if (compJoisticks(&joystickNou1, &joystick1) && compJoisticks(&joystickNou2, &joystick2))
    {
      if (actualMilis - prevMillis < 50)
        return; // Prevent rapid joystick presses
    }
    else
    {
      if (actualMilis - prevMillis < 10)
        return; // Prevent rapid joystick presses
    }
  }
  else
  {
    // Prevent rapid joystick presses when not in a game
    if (!(compJoisticks(&joystickNou1, &joystick1) && compJoisticks(&joystickNou2, &joystick2)))
      prevMillis = actualMilis;
    else
      return;

    if (actualMilis - prevMillis > 50)
      return; // Prevent rapid joystick presses
  }

  joystick1 = joystickNou1; // Update joystick state for player 1
  joystick2 = joystickNou2; // Update joystick state for player 2

  if (started)
  {
    // Handle paddle movement during the game
    if (joystick1.up)
      paddle1.x = max(0, paddle1.x - 1); // Move paddle1 up
    else if (joystick1.down)
      paddle1.x = min(5, paddle1.x + 1); // Move paddle1 down

    if (joystick2.up)
      paddle2.x = max(0, paddle2.x - 1); // Move paddle2 up
    else if (joystick2.down)
      paddle2.x = min(5, paddle2.x + 1); // Move paddle2 down
  }
  else
  {
    // Handle menu navigation
    if (!inSubMenu)
    {
      // Navigate in the main menu
      if (joystick1.left)
      {                                      // Go up in the menu
        menuIndex = (menuIndex - 1 + 4) % 4; // Circular rotation in the menu
      }
      else if (joystick1.right)
      {                                  // Go down in the menu
        menuIndex = (menuIndex + 1) % 4; // Circular rotation in the menu
      }
      else if (joystick1.button)
      { // Button pressed to enter submenus or start the game
        if (menuIndex == 0)
          menuDisplay = "play"; // Start game
        else if (menuIndex == 2)
          menuDisplay = "howToPlay"; // How to play screen
        else if (menuIndex == 3)
        {
          menuDisplay = "about"; // About screen
        }
        else
        { // Settings
          inSubMenu = true;
          subMenuIndex = 0; // Start with the first sub-menu
          subsubMenuIndex = 0;
          menuDisplay = "settings";
        }
      }
      if (joystick1.up)
      {
        if (menuIndex == 2 || menuIndex == 3)
        {
          menuDisplay = "menu"; // Go back to the main menu
        }
      }
    }
    else
    {
      // Handle navigation in submenus
      if (subsubMenuIndex == 0)
      {
        if (joystick1.up)
        { // Exit the submenu
          inSubMenu = false;
          menuDisplay = "menu";
          return;
        }
        else if (joystick1.left)
        {                                            // Go up in the submenu
          subMenuIndex = (subMenuIndex - 1 + 6) % 6; // Circular rotation within the settings options
          return;
        }
        else if (joystick1.right)
        {                                        // Go down in the submenu
          subMenuIndex = (subMenuIndex + 1) % 6; // Circular rotation within the settings options
          return;
        }
      }

      // Handle settings adjustment
      if (subsubMenuIndex > 0 && joystick1.button)
      {
        subsubMenuIndex = 0;
        menuDisplay = "settings";
        return;
      }

      handleSettings(); // Adjust settings based on joystick input
    }
  }
}
