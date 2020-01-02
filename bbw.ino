// bbw.cpp : bit bang wheele
//
/*
  The defines calculate the base ratios and set some generic functions for porting.

  SendBytes and RecieveBytes basically do an operation then a delay.
  The delays are in 2 arrays and are generally in order.

*/

#define TIME() micros()
#define DUCKGOOSEEGG(duck, goose, egg) ( [=, &egg]() { if (duck - egg < goose) return 0; egg = duck; return -1; } () )
#define DUCKGOOSE(duck, goose) ( [=]() { static unsigned long egg = 0; if (duck - egg < goose) return 0; egg = duck; return -1; } () )

void DelayMicro(unsigned long micro)
{
  unsigned long egg = TIME() + micro;
  while (TIME() < egg);
}

#define INPUT_PIN 13
#define OUTPUT_PIN 18
#define MSB_FIRST true
// below tested on 16mhz mega
#define PERIOD_DELAY_MICRO 10 // 90 = 9600 36 = 19200 10 = 38400 3 = 48000 0 = 53.5k
#define PERIOD_SLEW (0.5 * PERIOD_DELAY_MICRO)
#define STARTDELAY (13 * PERIOD_DELAY_MICRO)
#define READTIMEOUT (32 * PERIOD_DELAY_MICRO)
#define DELAY(a) DelayMicro((unsigned long)(a))
//#define DELAY(a) while(DUCKGOOSE(TIME(), (unsigned long)(a)));
#define SET_BIT(a) digitalWrite(OUTPUT_PIN, a)
#define GET_BIT() digitalRead(INPUT_PIN)

unsigned char sendBuffer[] = { 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55 };
unsigned char receiveBuffer[9];
float defaultSendDelays[] = { 1.0 * PERIOD_DELAY_MICRO, 1.0 * PERIOD_DELAY_MICRO, 2.0 * PERIOD_DELAY_MICRO };
float defaultReceiveDelays[] = { 1.0 * PERIOD_SLEW, 1.0 * READTIMEOUT, 1.0 * PERIOD_DELAY_MICRO, 1.0 * PERIOD_DELAY_MICRO, 1.0 * PERIOD_DELAY_MICRO, 1.0 * PERIOD_DELAY_MICRO, 1 };

void SendBytes(unsigned char *send, int sizeSend, float *delays = defaultSendDelays)
{
  unsigned char *end = &send[sizeSend];
  do {
    SET_BIT(false);
    DELAY(delays[0]);
    unsigned char bit = MSB_FIRST ? 1 << 7 : 1;
    do {
      SET_BIT(*send & bit ? true : false);
      DELAY(delays[1]);
      MSB_FIRST ? bit >>= 1 : bit <<= 1;
    } while (bit);
    SET_BIT(true);
    DELAY(delays[2]);
  } while (++send < end);
}

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
    delay(1);
//    Serial.print("Delay = ");
//    Serial.println(((unsigned int)defaultSendDelays[3]));
    //  ReceiveBytes(receiveBuffer, sizeof(receiveBuffer) / sizeof(receiveBuffer[0]));
  } else {
    SET_BIT(state);
    state = !state;
    //  DELAY(1);
  }
}
