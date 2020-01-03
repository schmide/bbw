// bbw.cpp : bit bang wheele
//
/*
  The defines calculate the base ratios and set some generic functions for porting.
  SendBytes and RecieveBytes basically do an operation then a delay.
  The delays are in 2 arrays and are generally in order.
*/

#define TIME() micros()
#define INPUT_PIN 13
#define OUTPUT_PIN 18
#define MSB_FIRST true
// below tested on 16mhz mega
#define PERIOD_DELAY_MICRO 12 // 90 = 9600 36 = 19200 10 = 38400 3 = 48000 0 = 53.5k
#define PERIOD_SLEW (0.5 * PERIOD_DELAY_MICRO)
#define STARTDELAY (13 * PERIOD_DELAY_MICRO)
#define READTIMEOUT (32 * PERIOD_DELAY_MICRO)
#define DELAY(a) { unsigned long egg = TIME() + (unsigned long)(a); while (TIME() <= egg); }
#define DELAY_HAMMER(a, b) { unsigned char h=a; do { (b); } while(--h); } 
#define SET_BIT(a) digitalWrite(OUTPUT_PIN, a)
#define GET_BIT() digitalRead(INPUT_PIN)

unsigned char sendBuffer[] = { 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55 };
unsigned char receiveBuffer[9];
unsigned char defaultSendDelays[] = { \
(unsigned char)(1.0 * PERIOD_DELAY_MICRO), \
(unsigned char)(1.0 * PERIOD_DELAY_MICRO), \
(unsigned char)(1.0 * PERIOD_DELAY_MICRO) };

void SendBytes(unsigned char *send, int sizeSend, unsigned char *delays = defaultSendDelays)
{
  unsigned char *end = &send[sizeSend];
  do {
    DELAY_HAMMER(delays[0], SET_BIT(false));
    unsigned char bit = MSB_FIRST ? 1 << 7 : 1;
    do {
      bool bitBool = *send & bit ? true : false;
      DELAY_HAMMER(delays[1], SET_BIT(bitBool));
      MSB_FIRST ? bit >>= 1 : bit <<= 1;
    } while (bit);
    DELAY_HAMMER(delays[2], SET_BIT(true));
  } while (++send < end);
}
/*
// not near done.
int ReceiveBytes(unsigned char *receive, int sizeReceive, float *delays = defaultReceiveDelays)
{
  DELAY(delays[0]);
  int readCount = 0;
  int egg = TIME();
  unsigned char *end = &receive[sizeReceive];
  do {
    while (GET_BIT()) {
      if (DUCKGOOSEEGG(TIME(), delays[1], egg))
        return readCount;
      DELAY(delays[2]);
    }
    DELAY(delays[3]);
    unsigned char bit = MSB_FIRST ? 1 << 7 : 1;
    do {
      if (GET_BIT()) {
        *receive |= bit;
        DELAY(delays[4]);
      } else {
        *receive &= ~bit;
        DELAY(delays[5]);
      }
      MSB_FIRST ? bit >>= 1 : bit <<= 1;
    } while (bit);
    readCount++;
    egg = TIME();
    DELAY(delays[6]);
  } while (++receive < end);
  return readCount;
}
*/

void setup() {
  Serial.begin(9600);
  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  SET_BIT(true);
}

bool state = true;

void loop() {
  if (true) {
    SendBytes(sendBuffer, sizeof(sendBuffer) / sizeof(sendBuffer[0]));
//    DELAY(16);
    delay(1000);
//    Serial.print("Delay = ");
//    Serial.println(((unsigned int)defaultSendDelays[3]));
    //  ReceiveBytes(receiveBuffer, sizeof(receiveBuffer) / sizeof(receiveBuffer[0]));
  } else {
    SET_BIT(state);
    state = !state;
    //  DELAY(1);
  }
}
