
#include <LiquidCrystal.h>
#include <Wire.h>

//Test led for players
int testLed=4;
bool ledState=0;

//Pins for Mode changing button and start button to start the game
const int modeButtonPin = 2;
const int startButtonPin = 3;
//Tracking states
int prevModeButtonState =1;
int prevStartButtonState=1;
int startButtonState=1;
int modeButtonState=1;

//For button debounce
unsigned long startButtonTime=0;
unsigned long modeButtonTime=0;

const int player1Button = 8;
unsigned long player1Time=0;
int player1ButtonState=0;
int prevPlayer1ButtonState =0;

const int player2Button=7;
unsigned long player2Time=0;
int player2ButtonState=0;
int prevPlayer2ButtonState=0;

bool teamMode = false; //true if in 2v2 mode

int player1Score=0;
int player2Score=0;
int player3Score=0;
int player4Score=0;

unsigned long previousRoundTime=0; //track time for specific round
unsigned long fastestTime=30000; // tracks fastest time for the game
unsigned long delayTime=1000; //delay between rounds, will eventually be random
unsigned long currTime=0;

const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 6, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

enum GameState{  //Track game states of selecting mode, playing game, and displaying end of game stats
  MODE_SELECTION,
  PLAYING,
  END_OF_GAME
};

GameState currentState;

String getMode()
{
  if (teamMode)
  {
    return "2v2";
  }
  else
  {
    return "4v4";
  }
}

void updatePlayingScreen() //Show player scores within PLAYING state
{
  lcd.setCursor(0,0);
  char topLine[20];
  if (player1Score >=11)
  {
    snprintf(topLine, sizeof(topLine), "P1:%d   P2:%d ", player1Score, player2Score);
  }
  snprintf(topLine, sizeof(topLine), "P1:%d   P2:%d ", player1Score, player2Score);
  lcd.print(topLine);
  lcd.setCursor(0,1);

  char bottomLine[20];
  snprintf(bottomLine, sizeof(bottomLine), "P3:%d   P4:%d", player3Score, player4Score);
  lcd.print(bottomLine);
}

void endOfGame() //Show Final after-game screen
{

  //Print player Scores and most recent time
  lcd.setCursor(0,0);
  char topLine[20];
  snprintf(topLine, sizeof(topLine), "PLAYER 1 WINS!");
  lcd.print(topLine);
  lcd.setCursor(0,1);
  char bottomLine[20];
  snprintf(bottomLine, sizeof(bottomLine), "Best Time: %lu", fastestTime);
  lcd.print(bottomLine);
}

void setup() {
  // set up the LCD's number of columns and rows:
  Wire.begin();
  lcd.begin(16, 2);

  pinMode(player1Button, INPUT_PULLUP);
  pinMode(player2Button, INPUT_PULLUP);
  pinMode(modeButtonPin, INPUT_PULLUP);
  pinMode(startButtonPin, INPUT_PULLUP);

  pinMode(testLed, OUTPUT);
  Serial.begin(9600);

  currentState = MODE_SELECTION; //Start in MODE SELECTION state
}

void mode_select(){
  lcd.setCursor(0,0);
    lcd.print("WELCOME");
    lcd.setCursor(0,1);
    char bottomLine[20];
    snprintf(bottomLine, sizeof(bottomLine), "Mode: %s", getMode().c_str());
    lcd.print(bottomLine);

    //Read button Pin for changing mode
    int reading = digitalRead(modeButtonPin);
    if (reading != prevModeButtonState)
    {
      modeButtonTime =millis();
    }

    //Change mode if applicable
    if ((millis() - modeButtonTime) >= 50 )
    {
      if (reading != modeButtonState)
      {
        modeButtonState=reading;
        if (modeButtonState ==0)
        {
          lcd.clear();
          previousRoundTime=0;
          fastestTime=30000;
          currTime =0;
          player1Score=0;
          player2Score=0;
          player3Score=0;
          player4Score=0;
          currentState = PLAYING;
        }
      }
    }

    //read start button pin for starting game
    int reading2 = digitalRead(startButtonPin);
    if (reading2 != prevStartButtonState)
    {
      startButtonTime =millis();
    }

    //start game accordingly
    if ((millis() - startButtonTime) >= 50 )
    {
      if (reading2 != startButtonState)
      {
        startButtonState=reading2;
        if (startButtonState ==0)
        {
          teamMode = !teamMode;
          lcd.clear();
        }
      }
    }

    prevModeButtonState = reading;
    prevStartButtonState = reading2;
}

void play_game()
{
    updatePlayingScreen();

    unsigned long roundStartTime=0;
    // SEND SIGNAL TO DAWID"S BOARD TO TURN ON BOARD LED AND START CLOCK
    if (!ledState && (millis() - currTime >= delayTime)) {
      ledState = 1;
      digitalWrite(testLed, HIGH);
      roundStartTime=millis();
    }

    
    //**READ FROM NEHA/PAULINA'S BOARD
    //UPDATE SCORES
    //SEND SIGNAL TO DAWID's BOARD TO TURN OFF LED
    
    // --- End condition ---
    if (player1Score >= 10 || player2Score >= 10 || player3Score >=10 || player4Score >= 10) {
      lcd.clear();
      endOfGame();
      currentState = END_OF_GAME;
    }
}

void end_of_game()
{
  int reading = digitalRead(modeButtonPin);
    if (reading != prevModeButtonState)
    {
      modeButtonTime =millis();
    }


    if ((millis() - modeButtonTime) >= 50 )
    {
      if (reading != modeButtonState)
      {
        modeButtonState=reading;
        if (modeButtonState ==0)
        {
          currentState = MODE_SELECTION;
        }
      }
    }


    int reading2 = digitalRead(startButtonPin);
    if (reading2 != prevStartButtonState)
    {
      startButtonTime =millis();
    }

    if ((millis() - startButtonTime) >= 50 )
    {
      if (reading2 != startButtonState)
      {
        startButtonState=reading2;
        if (startButtonState ==0)
        {
          lcd.clear();
          currentState = MODE_SELECTION;
        }
      }
    }

    prevModeButtonState = reading;
    prevStartButtonState = reading2;
}


void loop() {
if (currentState == MODE_SELECTION)
  {
    mode_select();
  }
else if (currentState == PLAYING) {
    play_game();
  }
else if (currentState == END_OF_GAME)
  {
    end_of_game();
  }
}
