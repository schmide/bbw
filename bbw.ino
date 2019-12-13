// bbw.cpp : bit bang wheele
//
/*
The defines calculate the base ratios and set some generic functions for porting.

SendBytes and RecieveBytes basically do an operation then a delay. 
The delays are in 2 arrays and are generally in order.

*/

#define INPUT_PIN 13
#define OUTPUT_PIN 13
#define TIME() millis()
#define MSB_FIRST true
#define BAUD 19200
#define PERIOD 1000000
#define PERIOD_BAUD (PERIOD / BAUD)
#define PERIOD_SLEW (0.5 * PERIOD_BAUD)
#define STARTDELAY (13 * PERIOD_BAUD)
#define READTIMEOUT (32 * PERIOD_BAUD)
#define DELAY(a) delay((unsigned long)(a));
#define SET_BIT(a) digitalWrite(OUTPUT_PIN, a)
#define GET_BIT() digitalRead(INPUT_PIN)
#define DUCKGOOSEEGG(duck, goose, egg) ( [=, &egg]() { if (duck - egg < goose) return 0; egg = duck; return -1; } () )

unsigned char sendBuffer[] = { 0x55, 0x9c };
unsigned char receiveBuffer[9];
float defaultSendDelays[] = { 1.0 * STARTDELAY, 1.0 * PERIOD_BAUD, 1.0 * PERIOD_BAUD, 1.0 * PERIOD_BAUD, 1.0 * PERIOD_BAUD };
float defaultReceiveDelays[] = { 1.0 * PERIOD_SLEW, 1.0 * READTIMEOUT, 1.0 * PERIOD_BAUD, 1.0 * PERIOD_BAUD, 1.0 * PERIOD_BAUD };

void SendBytes(unsigned char *send, int sizeSend, float *delays = defaultSendDelays)
{
  pinMode(OUTPUT_PIN, OUTPUT);
  SET_BIT(false);
  DELAY(delays[0]);
  SET_BIT(true);
  DELAY(delays[1]);
  for (int i = 0; i < sizeSend; i++) {
    SET_BIT(false);
    DELAY(delays[2]);
    unsigned char bit = MSB_FIRST ? 1 << 7 : 1;
    unsigned char end = MSB_FIRST ? 1 : 1 << 7;
    do {
      SET_BIT(send[i] & bit ? true : false);
      DELAY(delays[3]);
      MSB_FIRST ? bit <<= 1 : bit >>= 1;
    } while (!(bit & end));
    SET_BIT(true);
    DELAY(delays[4]);
  }
}

int ReceiveBytes(unsigned char *receive, int sizeReceive, float *delays = defaultReceiveDelays)
{
  pinMode(INPUT_PIN, INPUT);
  DELAY(delays[0]);
  int readCount = 0;
  int egg = TIME();
  for (int i = 0; i < sizeReceive; i++) {
    do {
      if (GET_BIT()) {
        if (DUCKGOOSEEGG(TIME(), delays[1], egg))
          return readCount;
        DELAY(delays[2]);
      } else {
        DELAY(delays[3]);
        unsigned char bit = MSB_FIRST ? 1 << 7 : 1;
        unsigned char end = MSB_FIRST ? 1 : 1 << 7;
        do {
          GET_BIT() ? receive[i] |= bit : receive[i] &= ~bit;
          DELAY(delays[4]);
          MSB_FIRST ? bit <<= 1 : bit >>= 1;
        } while (!(bit & end));
        readCount++;
        egg = TIME();
        break;
      }
    } while (true);
  }
  return readCount;
}

void setup() {
}

void loop() {
  SendBytes(sendBuffer, sizeof(sendBuffer) / sizeof(sendBuffer[0]));
  ReceiveBytes(receiveBuffer, sizeof(receiveBuffer) / sizeof(receiveBuffer[0]));
}
