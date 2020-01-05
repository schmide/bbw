// bbw.cpp : bit bang wheele
//
/*
  Went back to using the clock. Works up to 57600 on a mega. Though 38400 is more reliable.

  The algorithm is basically a series of delays polling the clock while shifting out bits.
  Using the clock keeps the frame in sync. However this is an integer operation and thus 
  ratios are never exact. Long messages may need breaks.

  At 57600 you have 17.36 ticks per bit which causes a bit of jitter. Lowering the 
  PERIOD_ADJUST may help a bit but errors were there. 38400 with 26 ticks a bit was
  very consistant.
*/

#define TIME() micros()
#define INPUT_PIN 13
#define OUTPUT_PIN 7
#define MSB_FIRST false
// below tested on 16mhz mega
#define PERIOD_BAUD 38400.0
#define PERIOD_ADJUST 1.0
#define PERIOD_SHIFT 3.0 // shifts the data to the left 38400 likes 3-4 57600 5+
#define PERIOD_DELAY_MICRO (1000000.0 * PERIOD_ADJUST / PERIOD_BAUD)
#define PERIOD_SLEW (0.5 * PERIOD_DELAY_MICRO)
#define STARTDELAY (13 * PERIOD_DELAY_MICRO)
#define READTIMEOUT (32 * PERIOD_DELAY_MICRO)
#define DELAY(a) { unsigned long egg = TIME() + ((unsigned long)(a)); while (TIME() <= egg); }
#define DELAY_UNTIL(a) { unsigned long egg = (unsigned long)(a); while (TIME() <= egg); }
#define DELAY_HAMMER(a, b) { unsigned char h=a; do { (b); } while(--h); } 
#define SET_BIT(a) digitalWrite(OUTPUT_PIN, a)
#define GET_BIT() digitalRead(INPUT_PIN)

unsigned char sendBuffer[] = { 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55 };
unsigned char receiveBuffer[9];
unsigned int defaultSendDelays[] = { \
(unsigned int)(1.0 * PERIOD_DELAY_MICRO - PERIOD_SHIFT), \ 
(unsigned int)(1.0 * PERIOD_DELAY_MICRO), \
(unsigned int)(10.0 * PERIOD_DELAY_MICRO) };

void SendBytes(unsigned char *send, int sizeSend, unsigned int *delays = defaultSendDelays)
{
  unsigned char *end = &send[sizeSend];
  SET_BIT(true); // sync to port
  unsigned long startTime = TIME();
  do {
    SET_BIT(false);
    unsigned long nextTime = startTime+delays[0];
    DELAY_UNTIL(nextTime);    
    unsigned char bit = MSB_FIRST ? 1 << 7 : 1;
    do {
      bool bitBool = *send & bit ? true : false;
      SET_BIT(bitBool);
      nextTime += delays[1];
      MSB_FIRST ? bit >>= 1 : bit <<= 1;
      DELAY_UNTIL(nextTime);    
    } while (bit);
    SET_BIT(true);
    startTime+=delays[2];
    DELAY_UNTIL(startTime);    
  } while (++send < end);
}

void setup() {
  Serial.begin(9600);
  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  SET_BIT(true);
  Serial1.begin(38400);
}

bool state = true;

void loop() {
  if (true) {
    SendBytes(sendBuffer, sizeof(sendBuffer) / sizeof(sendBuffer[0]));
//    DELAY(16);
    delay(100);
//    Serial.print("Delay = ");
//    Serial.println(((unsigned int)defaultSendDelays[3]));
    //  ReceiveBytes(receiveBuffer, sizeof(receiveBuffer) / sizeof(receiveBuffer[0]));
  } else {
    Serial1.write(sendBuffer, sizeof(sendBuffer) / sizeof(sendBuffer[0]));
    delay(100);
  }
}
