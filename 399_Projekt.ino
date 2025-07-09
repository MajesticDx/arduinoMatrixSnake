#include <Esprit.h>
#include <FreematicsOLED.h>
#include <SPI.h>

#define CS  D10
#define UserButton_state  PC13

static HardwareTimer mytimer = HardwareTimer(TIM2);

unsigned char snake[] = {0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};

unsigned char maze1[] = {0x80, 0xE0, 0xE0, 0x01, 0x01, 0x8F,0x8F, 0x8F};
unsigned char maze2[] = {0x8F ,0x8F ,0x08 ,0x08 ,0xF9 ,0xF9 ,0x01 ,0x8F};
unsigned char maze3[] = {0x00 ,0x00 ,0xFC ,0x00 ,0xCF ,0x00 ,0x7E ,0x7E};
unsigned char maze4[] = {0x7E ,0x7E ,0x00, 0xFC, 0xF8, 0xE1, 0x03, 0x7F};
unsigned char maze5[] = {0x4F ,0x00 ,0xF7 ,0x87, 0x3C, 0xE1, 0xCF, 0xCF};

unsigned char skull[] = {0x19, 0x3A, 0x6F, 0xFE, 0xFE, 0x6F, 0x3A, 0x19};
unsigned char victory[] = {0x78, 0xFC, 0xFE, 0x7F, 0x7F, 0xFE, 0xFC, 0x78};

volatile int stateX = 0;
volatile int SCNDstateX = 1;

volatile bool toggle = false;
volatile bool lose = false;
volatile bool win = false;

int game_state = 0;
bool line = false;



void setup()
{
  pinMode(CS, OUTPUT);
  pinMode(UserButton_state, INPUT_PULLUP);

  digitalWrite(CS, HIGH);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE0);
  delay(10);
  Init_MAX7219(); 

  attachInterrupt(digitalPinToInterrupt(UserButton_state), isr_EXT_Wurf, FALLING);
  mytimer.setOverflow(250);
  mytimer.setPrescaleFactor(32000);
  mytimer.attachInterrupt(isr_Timer2);

  Serial.begin(115200);
  Serial.println("Ende Setup");
}

void isr_EXT_Wurf(void) {
  mytimer.resume();
  toggle = !toggle;
}

void isr_Timer2 (void) {
  if (!toggle) {
    snake[SCNDstateX] = snake[stateX];
    snake[stateX] = 0;
    
    stateX++;
    if (stateX > 7) {
      stateX = 0;
    }
    SCNDstateX = stateX + 1;
    if (SCNDstateX > 7) {
      SCNDstateX = 0;
    }
  } else {
    snake[stateX] = snake[stateX] << 1;
    if (snake[stateX] == 0x00) {
      snake[stateX] = 0x01;
    }
  }
}

void loop()
{
  if (snake[0] == 0) {
    line = true;
  }
  if (line == true && snake[0] > 0) {
    game_state++;
    line = false;
  }

  if (win) {
    for_loop(victory);
  } else {
    if (!lose) {
      for_loop(snake);

      switch(game_state) {
        case 0:
          for_loop(maze1);
          overlapcheck(maze1);
          break;
        case 1:
          for_loop(maze2);
          overlapcheck(maze2);
          break;
        case 2:
          for_loop(maze3);
          overlapcheck(maze3);
          break;
        case 3:
          for_loop(maze4);
          overlapcheck(maze4);
          break;
        case 4:
          for_loop(maze5);
          overlapcheck(maze5);
          break;
        case 5:
          win = true;
          break;
      }
    } else {
      for_loop(skull);
    }
  }
}