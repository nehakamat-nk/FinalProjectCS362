
#include <LiquidCrystal.h>
#include <Wire.h>

//Test led for players
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

bool teamMode = false; //true if in 2v2 mode

int player1Score=0;
int player2Score=0;
int player3Score=0;
int player4Score=0;
int winningPlayer=0;

int team1Score=0;
int team2Score=0;
int winningTeam=0;

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

bool isEnd()
{
  if (teamMode)
  {
    if (team1Score>9 || team2Score >9)
    {
      return true;
    }
  }
  else
  {
    if (player1Score >9 || player2Score>9 || player3Score >9 || player4Score>9)
    {
      return true;
    }
  }
  return false;
}
void updatePlayingScreen() //Show player scores within PLAYING state
{
  lcd.setCursor(0,0);
  char topLine[20];
  char bottomLine[20];
  if (teamMode)
  {
  snprintf(topLine, sizeof(topLine), "    Team 1: %d ", team1Score);
  snprintf(bottomLine, sizeof(bottomLine), "    Team 2: %d ", team2Score);
  }
  else
  {
  snprintf(topLine, sizeof(topLine), "P1:%d   P2:%d ", player1Score, player2Score);
  snprintf(bottomLine, sizeof(bottomLine), "P3:%d   P4:%d", player3Score, player4Score);
  }

  lcd.print(topLine);
  lcd.setCursor(0,1);


  lcd.print(bottomLine);
}

void endOfGame() //Show Final after-game screen
{

  char topLine[20];
  char bottomLine[20];
  if (teamMode)
  {
     snprintf(topLine, sizeof(topLine), "TEAM %d WINS!", winningTeam); 
  }
  else
  {
      snprintf(topLine, sizeof(topLine), "PLAYER %d WINS!", winningPlayer); 
  }
  lcd.setCursor(0,0);
  lcd.print(topLine);
  lcd.setCursor(0,1);
  snprintf(bottomLine, sizeof(bottomLine), "Best Time: %lu", fastestTime);
  lcd.print(bottomLine);
}


void mode_select(){
  lcd.setCursor(0,0);
    lcd.print("WELCOME");
    lcd.setCursor(0,1);
    char bottomLine[20];
    snprintf(bottomLine, sizeof(bottomLine), "Mode: %s", getMode().c_str()); //show mode selected
    lcd.print(bottomLine);

    
    int reading = digitalRead(modeButtonPin);
    if (reading != prevModeButtonState)
    {
      modeButtonTime =millis();
    }

    //start game button
    if ((millis() - modeButtonTime) >= 50 )
    {
      if (reading != modeButtonState)
      {
        modeButtonState=reading;
        if (modeButtonState ==0)
        {
          lcd.clear(); //reset variables for start of game
          previousRoundTime=0;
          fastestTime=30000;
          currTime =0;
          player1Score=0;
          player2Score=0;
          player3Score=0;
          player4Score=0;

          team1Score=0;
          team2Score=0;
          winningPlayer=0;
          winningTeam=0;
          currentState = PLAYING;
        }
      }
    }

    //read start button pin for changing mode
    int reading2 = digitalRead(startButtonPin);
    if (reading2 != prevStartButtonState)
    {
      startButtonTime =millis();
    }

    //change mode
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
    if (!ledState && (millis() - currTime >= delayTime)) { //turn on led after random delayTime period
      ledState = 1;

      roundStartTime=millis();
    }

    
    //**READ FROM NEHA/PAULINA'S BOARD
    //UPDATE SCORES\
    //SEND SIGNAL TO DAWID's BOARD TO TURN OFF LED

    // --- End condition ---
    if (isEnd()) {
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

void setup() {
  // set up the LCD's number of columns and rows:
  Wire.begin();
  lcd.begin(16, 2);

  pinMode(modeButtonPin, INPUT_PULLUP);
  pinMode(startButtonPin, INPUT_PULLUP);

  Serial.begin(9600);

  currentState = MODE_SELECTION; //Start in MODE SELECTION state
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
