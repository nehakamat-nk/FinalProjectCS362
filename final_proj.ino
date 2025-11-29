
#include <LiquidCrystal.h>
#include <Wire.h>

//Test led for players
bool roundStarted=0;

//Pins for Mode changing button and start button to start the game
const int modeButtonPin = 2;
const int startButtonPin = 3;
const int player1Button = A0;
const int player2Button = A1;
const int player3Button = A2;
const int player4Button = A3;

const int testLed=13;

//Tracking states
int prevModeButtonState =1;
int prevStartButtonState=1;
int prevPlayer1State =1;
int prevPlayer2State=1;
int prevPlayer3State =1;
int prevPlayer4State=1;

int startButtonState=1;
int modeButtonState=1;
int player1State=1;
int player2State=1;
int player3State=1;
int player4State=1;

//For button debounce
unsigned long startButtonTime=0;
unsigned long modeButtonTime=0;
unsigned long player1Time=0;
unsigned long player2Time=0;
unsigned long player3Time=0;
unsigned long player4Time=0;

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
unsigned long delayTime=5000; //delay between rounds, will eventually be random
unsigned long currTime=0;
unsigned long roundStartTime=0; //Track the start of each round

const int clockAdr = 4;
const int playerLedAdr = 0x08;
const int buzzerAdr = 0x09;

const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 6, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

enum GameState{  //Track game states of selecting mode, playing game, and displaying end of game stats
  MODE_SELECTION,
  PLAYING,
  END_OF_GAME
};

GameState currentState;

//Return Mode
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

//Return true if playerScores reach max
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


void updatePlayingScreen() //Show player scores within PLAYING state after each round
{
  lcd.setCursor(0,0);
  char topLine[20];
  char bottomLine[20];
  if (teamMode)
  {
    team1Score = player1Score + player2Score;
    team2Score = player3Score+player4Score;
  snprintf(topLine, sizeof(topLine), "    Team 1: %d ", player1Score);
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

//DISPLAY END STATS on SCREEN
void endOfGame() 
{

  char topLine[20];
  char bottomLine[20];
  if (teamMode)
  {
    winningTeam=1;
    if (team2Score > team1Score) { winningTeam=2;};
     snprintf(topLine, sizeof(topLine), "TEAM %d WINS!", winningTeam); 
  }
  else
  {
    if (player1Score ==10)
    {
      winningPlayer=1;
    }
    else if (player2Score ==10)
    {
      winningPlayer=2;
    }
    else if (player3Score ==10)
    {
      winningPlayer=3;
    }
    else if (player4Score ==10)
    {
      winningPlayer=4;
    }
      snprintf(topLine, sizeof(topLine), "PLAYER %d WINS!", winningPlayer); 
  }
  lcd.setCursor(0,0);
  lcd.print(topLine);
  lcd.setCursor(0,1);
  snprintf(bottomLine, sizeof(bottomLine), "Best Time: %lu", fastestTime);
  lcd.print(bottomLine);
}


//MODE SELECTION,
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
          player1Score=0;
          player2Score=0;
          player3Score=0;
          player4Score=0;

          team1Score=0;
          team2Score=0;
          winningPlayer=0;
          winningTeam=0;
          currTime=millis();
          updatePlayingScreen();
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

void clockWrite(byte x)
{
  Wire.beginTransmission(clockAdr); 
  Wire.write(x);              
  Wire.endTransmission();
}

void playerLedWrite(byte x)
{
  Wire.beginTransmission(playerLedAdr); 
  Wire.write(x);              
  Wire.endTransmission();
}

void buzzerWrite(byte x)
{
  Wire.beginTransmission(buzzerAdr); 
  Wire.write(x);              
  Wire.endTransmission();
}

void readPlayerButtons()
{
  int reading1 = digitalRead(player1Button); //PLAYER 1
  int reading2 = digitalRead(player2Button); //PLAYER 2
  int reading3 = digitalRead(player3Button); //PLAYER 2
  int reading4 = digitalRead(player4Button); //PLAYER 2


  if (reading1 != prevPlayer1State)
  {
    player1Time =millis();
  }

  if (reading2 != prevPlayer2State)
  {
    player2Time =millis();
  }

  if (reading3 != prevPlayer3State)
  {
    player3Time =millis();
  }

  if (reading4 != prevPlayer4State)
  {
    player4Time =millis();
  }


  if ((millis() - player1Time) >= 50 ) //Handle player 1 Press
  {
    if (reading1 != player1State)
    {
      player1State=reading1;
      if (player1State ==0)
      {
        if (roundStarted) //IF in round, add to score, update screen, track if fastest, restart clock
        {
          roundStarted=false; //End Round
          player1Score++; 

          lcd.clear();
          updatePlayingScreen();

          clockWrite(0);
          buzzerWrite(1); //TODO - buzzer code that buzzes when 1 is read
          playerLedWrite(1); //TODO - led code that shows player buttons

          previousRoundTime = millis()-roundStartTime;
          if (previousRoundTime <= fastestTime) {fastestTime=previousRoundTime;};
          currTime=millis();
          delayTime = random(2000,6000);
          return;
        }
        else
        {
          currTime=millis();
        }
      }
    }
  }

  if ((millis() - player2Time) >= 50 ) //handle player 2 press
  {
    if (reading2 != player2State)
    {
      player2State=reading2;
      if (player2State ==0)
      {
        if (roundStarted) //IF in round, add to score, update screen, track if fastest, restart clock
        {
          roundStarted=false; //End Round
          player2Score++; 

          lcd.clear();
          updatePlayingScreen();

          clockWrite(0);
          buzzerWrite(1); //TODO - buzzer code that buzzes when 1 is read
          playerLedWrite(2); //TODO - led code that shows player buttons

          previousRoundTime = millis()-roundStartTime;
          if (previousRoundTime <= fastestTime) {fastestTime=previousRoundTime;};
          currTime=millis();
          delayTime = random(2000,6000);
          return;
        }
        else
        { //If not in round reset countdown
          currTime=millis();
        }
      }
    }
  }


  if ((millis() - player3Time) >= 50 ) //handle player 3 press
  {
    if (reading3 != player3State)
    {
      player3State=reading3;
      if (player3State ==0)
      {
        if (roundStarted) //IF in round, add to score, update screen, track if fastest, restart clock
        {
          roundStarted=false; //End Round
          player3Score++; 

          lcd.clear();
          updatePlayingScreen();

          clockWrite(0); 
          buzzerWrite(1); //TODO - buzzer code that buzzes when 1 is read
          playerLedWrite(3); //TODO - led code that shows player buttons

          previousRoundTime = millis()-roundStartTime;
          if (previousRoundTime <= fastestTime) {fastestTime=previousRoundTime;};
          currTime=millis();
          delayTime = random(2000,6000);
          return;
        }
        else
        {
          currTime=millis();
        }
      }
    }
  }

  if ((millis() - player4Time) >= 50 ) //handle player 4 press
  {
    if (reading4 != player4State)
    {
      player4State=reading4;
      if (player4State ==0)
      {
        if (roundStarted) //IF in round, add to score, update screen, track if fastest, restart clock
        {
          roundStarted=false; //End Round
          player4Score++; 

          lcd.clear();
          updatePlayingScreen();

          clockWrite(0);
          buzzerWrite(1); //TODO - buzzer code that buzzes when 1 is read
          playerLedWrite(4); //TODO - led code that shows player buttons

          previousRoundTime = millis()-roundStartTime;
          if (previousRoundTime <= fastestTime) {fastestTime=previousRoundTime;};
          currTime=millis();
          delayTime = random(2000,6000);
          return;
        }
        else
        {
          currTime=millis();
        }
      }
    }
  }

    //Update Previous States
    prevPlayer1State = reading1;
    prevPlayer2State = reading2;
    prevPlayer3State = reading3;
    prevPlayer4State = reading4;
}
//SEND SIGNAL TO START CLOCK, RECEIVE SIGNAL FROM BUTTONS, SEND FINAL SIGNAL TO TURN OFF CLOCK AND ON BUTTON
void play_game()
{
    // SEND SIGNAL TO DAWID"S BOARD TO TURN ON BOARD LED AND START CLOCK
    if (!roundStarted && (millis() - currTime >= delayTime)) { //turn on led after random delayTime period

      roundStarted = 1;
      clockWrite(1); //Tell clock to start
      playerLedWrite(0); //TODO - led code to turn off player Leds

      roundStartTime=millis(); //keep track of start of round
    }

    readPlayerButtons();

    //TEST LED , will be on when round is active
    if (roundStarted)
    {
      digitalWrite(testLed, HIGH);
    }
    else
    {
      digitalWrite(testLed, LOW);
    }

    // game end condition
    if (isEnd()) {
      lcd.clear();
      endOfGame();
      currentState = END_OF_GAME;
    }
}

void end_of_game() //Show final stats, Press either button to continue back to mode select
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
          lcd.clear();
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
  pinMode(player1Button, INPUT_PULLUP);
  pinMode(player2Button, INPUT_PULLUP);
  pinMode(player3Button, INPUT_PULLUP);
  pinMode(player4Button, INPUT_PULLUP);
  pinMode(testLed, OUTPUT);

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
