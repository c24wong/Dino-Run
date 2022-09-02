#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

extern uint8_t startscreen[];
extern uint8_t winscreen[];
extern uint8_t losescreen[];
extern uint8_t standing1[];
extern uint8_t standing2[];
extern uint8_t jumping[];
extern uint8_t crouching[];
extern uint8_t cactus[];

Adafruit_PCD8544 lcd = Adafruit_PCD8544(3, 4, 5, 7, 6);

const char start_pattern_size = 40;
const double start_pattern[start_pattern_size] = {293.66, 293.66, 587.33, 440.00, 415.30, 392.00, 349.23, 293.66, 349.23, 392.00, 261.63, 261.63, 587.33, 440.00, 415.30, 392.00, 349.23, 293.66, 349.23, 392.00, 493.88, 493.88, 587.33, 440.00, 415.30, 392.00, 349.23, 293.66, 349.23, 392.00, 466.16, 466.16, 587.33, 440.00, 415.30, 392.00, 349.23, 293.66, 349.23, 392.00}; 

const char win_pattern_size = 63;
const double win_pattern[win_pattern_size] = {261.63, 277.18, 277.18, 415.30, 415.30, 415.30, 261.63, 261.63, 261.63, 261.63, 261.63, 261.63, 261.63, 277.18, 277.18, 261.63, 261.63, 277.18, 277.18, 415.30, 415.30, 415.30, 261.63, 261.63, 261.63, 261.63, 261.63, 261.63, 261.63, 277.18, 277.18, 277.18, 261.63, 277.18, 277.18, 415.30, 415.30, 415.30, 261.63, 261.63, 261.63, 261.63, 261.63, 261.63, 277.18, 277.18, 261.63, 261.63, 277.18, 277.18, 415.30, 415.30, 415.30, 277.18, 311.13, 311.13, 523.25, 523.25, 523.25, 415.30, 415.30, 415.30, 466.16};   

const char lose_pattern_size = 26;
const double lose_pattern[lose_pattern_size] = {261.63, 622.25, 349.23, 369.99, 349.23, 622.25, 261.63, 0, 466.16, 293.66, 261.63, 0, 261.63, 622.25, 349.23, 369.99, 349.23, 622.25, 369.99, 369.99, 349.23, 622.25, 369.99, 349.23, 622.25, 261.63};

const double jump_sound[1] = {392.00};
const double crouch_sound[1] = {622.25};

int counter = 0;

const int buzzer = 10;
const int up = 8;
const int down = 9;
//const int button = 4;

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
    
} task;

int delay_gcd;
const unsigned short tasksNum = 2;
task tasks[tasksNum];

bool game_start = 0;
bool lose_flag = 0;
bool win_flag = 0;
bool end_flag = 0;

int win_counter = 0;
int score = 0;
int endgame_time = 0;

int duration = 0;

int dinoXposition = 21;
int dinoYposition = 0;

int obstacleXposition = 84;
int obstacleYposition = 3;

void generate_obstacle(){  
  int obstacle = rand() % 2;
  if(obstacle == 1){ //cactus
    if( (obstacleXposition == 22) && (dinoYposition != 5) ){ //contact w/ player model
      lose_flag = 1;
    }
    lcd.drawBitmap(obstacleXposition, 20, cactus, 11, 27, 1);
    lcd.display(); 
    obstacleXposition--;
    if(obstacleXposition == 19){
      obstacleXposition = 84;
      score++;
      EEPROM.update(0, score);
        if(score >= 2){
          win_flag = 1;
        }
    }
  }
  else if(obstacle == 0){ //bird
    if( (obstacleXposition == 22) && (dinoYposition != 1) ){
      lose_flag = 1;
    }
    //lcd.drawBitmap(obstacleXposition, 20, bird, 11, 27, 1);
    lcd.display(); 
    obstacleXposition--;
    if(obstacleXposition == 19){
      obstacleXposition = 84;
      score++;
      EEPROM.update(0, score);
        if(score >= 2){
          win_flag = 1;
        }
    }
  }
  else
   //No spawn do nothing
  return;
}

enum SM1_States {SM1_INIT, ENTER, UP, DOWN};
int SM1_Tick(int state){
    //Read thing
    switch(state){ // State transitions
      case SM1_INIT:
            if(end_flag == 1)
              state = SM1_INIT;
            else if(game_start == 1){
              end_flag == 0;
              state = ENTER;
            }
            else 
              state = SM1_INIT;
        break;
      case ENTER:
         if(end_flag == 1)
              state = SM1_INIT;
         else if(digitalRead(up) != 1){ 
            state = UP;    
         }
         else if(digitalRead(down) != 1){ 
            state = DOWN;     
         }
         else 
            state = ENTER;   
        break;
      case UP:
           if(end_flag == 1)
              state = SM1_INIT;
           else if(digitalRead(up) != 1){ 
            state = UP;    
           }
           else 
            state = ENTER;    
        break;
     case DOWN:
           if(end_flag == 1)
              state = SM1_INIT;
           else if(digitalRead(down) != 1){ 
            state = DOWN;     
          }  
          else 
            state = ENTER; 
        break;
    //case BUTTON:
         //if(digitalRead(button) != 1){ 
            //state = BUTTON;   
         //}
         //else 
            //state = ENTER;   
         //State Transition
        //break;
    }
    switch(state){ // State Action
      case SM1_INIT:
         //State Action
        break;
      case UP:
            dinoYposition = 5;
            lcd.clearDisplay();
            lcd.drawBitmap(0, 0, jumping, 84, 48, 1);
            lcd.display();
            if(digitalRead(up) != 1){ 
              tone(buzzer, jump_sound[0]);  
            }
            duration++;
        break;
     case DOWN:
            dinoYposition = 1;
            lcd.clearDisplay();
            lcd.drawBitmap(0, 0, crouching, 84, 48, 1);
            lcd.display();
            if(digitalRead(down) != 1){ 
              tone(buzzer, crouch_sound[0]);     
            }
            duration++;  
        break;
      case ENTER:
           noTone(buzzer);
           dinoYposition = 3;
           if(game_start == 1){
            if(duration % 2 == 0){
                lcd.clearDisplay();
                lcd.drawBitmap(0, 0, standing1, 84, 48, 1);
                lcd.display();
                duration++;
            }
            else if(duration % 2 == 1){
                lcd.clearDisplay();
                lcd.drawBitmap(0, 0, standing2, 84, 48, 1);
                lcd.display();
                duration++;
            }
           }
           else
              
        break;
    //case BUTTON:
           //b_buf = light_pattern[3];
           //writeBuffer(b_buf);   
         //State Action
        //break;
    }
    return state;
}

enum SM2_States {SM2_INIT, START, PLAY_STATE, WIN_SCREEN, LOSE_SCREEN};
int SM2_Tick(int state){
    switch(state){ // State transitions
      case SM2_INIT:
         //State Transition
            state = START;
        break;
      case START:
        if(digitalRead(up) !=1){
          game_start = 1;
        }
        else if(game_start == 1)
          state = PLAY_STATE;
        else
          state = START;
        break;
      case PLAY_STATE:
          if( (win_flag == 1) || (score == 1) ){
          //if( (win_flag == 1) || (duration >= 35) ) {
            end_flag = 1; 
            state = WIN_SCREEN;
          }
          else if(lose_flag == 1){
            end_flag = 1;
            state = LOSE_SCREEN;
          }
          else
            state = PLAY_STATE;
        break;
      case WIN_SCREEN:
        if(endgame_time == 126){
          win_flag = 0;
          game_start = 0;
          end_flag = 0;
          endgame_time = 0;
          obstacleXposition = 84;
          state = START;
        }
        else
          state = WIN_SCREEN;
        break;
      case LOSE_SCREEN:
        if(endgame_time == 50){
          lose_flag = 0;
          game_start = 0;
          end_flag = 0;
          endgame_time = 0;
          obstacleXposition = 84;
          state = START;
        }
        else
          state = LOSE_SCREEN;
        break;
    }
    switch(state){ // State Action
      case SM2_INIT:
         //State Action
        break;
      case START:
        duration = 0;
        lcd.clearDisplay();
        lcd.drawBitmap(0, 0, startscreen, 84, 48, 1);
        lcd.display();
        tone(buzzer, start_pattern[counter]);
           counter++;
           if(counter == 40){
              counter = 0;
            }
        break;
      case PLAY_STATE:
            if(game_start){
              generate_obstacle();
            }
            //else
              
         //State Action
        break;
      case WIN_SCREEN:
            win_flag = 0;

            lcd.clearDisplay();
            lcd.drawBitmap(0, 0, winscreen, 84, 48, 1);
            lcd.display();

            lcd.setCursor(0, 0);
            lcd.print("High: ");
            lcd.print(EEPROM.read(0));
            lcd.display();
            
            tone(buzzer, win_pattern[counter]);
            counter++;
            endgame_time++;
            if(counter == 63){
              counter = 0;
            }
            else if(endgame_time == 126){
              game_start = 0;
            }
        break;
       case LOSE_SCREEN:
            lose_flag = 0;
            lcd.clearDisplay();
            lcd.drawBitmap(0, 0, losescreen, 84, 48, 1);
            lcd.display();
            lcd.setCursor(0, 0);
            lcd.print("High: ");
            lcd.print(EEPROM.read(0));
            lcd.display();
            tone(buzzer, lose_pattern[counter]);
            counter++;
            endgame_time++;
            if(counter == 25){
              counter = 0;
            }
            else if(endgame_time == 50){
              game_start = 0;
            }
        break;
     }  
     return state;
}

void setup() {
    //some set up (Default Arduino Function)

    // LEDs
    EEPROM.write(0, score); 
    lcd.begin();
    lcd.setContrast(70);
    lcd.clearDisplay();
    
    Serial.begin(9600);
    
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  //pinMode(button, INPUT_PULLUP);
  
  unsigned char i = 0;
  tasks[i].state = SM1_INIT;
  tasks[i].period = 100;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM1_Tick;
  i++;
  tasks[i].state = SM2_INIT;
  tasks[i].period = 20;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM2_Tick;
  i++;

  delay_gcd = 500; // GCD
  
} 

void loop() {
  // put your main code here, to run repeatedly:
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
     if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
        tasks[i].state = tasks[i].TickFct(tasks[i].state);
        tasks[i].elapsedTime = millis(); // Last time this task was ran
        
        //Serial.println(digitalRead(up));
        //Serial.println(digitalRead(down));
        //Serial.println(lose_flag);
        Serial.println(win_counter);
        Serial.println(duration);
        Serial.println(endgame_time);
        Serial.println(tasks[i].state);
     }
   }
}
