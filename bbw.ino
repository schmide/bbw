// bbw.cpp : bit bang wheele
//
/*
  Delays are back in fashion.

  While determining the delays between 9600 and 115200, a couple things happened.
  First the ratio for 115200 was exactly 1.0. Second the ratio for 9600 was 0.72
  So I started building a linear equation to translate mhz baud and period such 
  that it would map from 9600-115200. As I was doing it the mhz and baud would
  each become unnecessary. Well it turns out 115200 * 100 / 16mhz = 0.72 and
  the max it will run at it 16 * 9600 = 153600. I can only assume the writing to 
  the ports is keeping things synchronized. 

  Actual PERIOD_DELAY is 2 nanoseconds or 1:500000
*/

#define TIME() micros()
#define INPUT_PIN 13
#define OUTPUT_PIN 7
#define DEFAULT_LOGIC true
#define MSB_FIRST false
// below derived on 16mhz mega
#define PERIOD_BAUD 38400.0
#define PERIOD_MHZ 16000000.0
#define PERIOD 1000000.0
#define PERIOD_LOW 9600.0
#define PERIOD_HIGH 115200.0
#define PERIOD_CLOCK_SCALE_RATIO (100.0 * PERIOD_HIGH / PERIOD_MHZ)
#define PERIOD_RATIO (1.0 + (PERIOD_CLOCK_SCALE_RATIO * (1.0 - (PERIOD_BAUD - PERIOD_LOW) / (PERIOD_HIGH - PERIOD_LOW))))
#define PERIOD_DELAY (PERIOD_RATIO * PERIOD / PERIOD_BAUD)
#define PERIOD_SHIFT (0.0 * PERIOD_DELAY)
#define PERIOD_SLEW (0.5 * PERIOD_DELAY)
#define STARTDELAY (13 * PERIOD_DELAY)
#define READTIMEOUT (32 * PERIOD_DELAY)
#define DELAY(a) { unsigned long egg = ((unsigned long)(a)); while (--egg) { __asm__("nop\n\t"); } }
#define SET_BIT(a) digitalWrite(OUTPUT_PIN, a)
#define GET_BIT() digitalRead(INPUT_PIN)

unsigned char sendBuffer[] = { 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55, 0xaa, 0x9c, 0x55 };
unsigned char receiveBuffer[9];
unsigned long defaultSendDelays[] = { \
                                      (unsigned long)(1.0 * PERIOD_DELAY + PERIOD_SHIFT), \
                                      (unsigned long)(1.0 * PERIOD_DELAY), \
                                      (unsigned long)(2.0 * PERIOD_DELAY - PERIOD_SHIFT), \
                                    };

void SendBytes(unsigned char *send, int sizeSend, unsigned long *delays = defaultSendDelays)
{
  noInterrupts();
  unsigned char *end = &send[sizeSend];
  SET_BIT(DEFAULT_LOGIC); // sync to port
  do {
    SET_BIT(!DEFAULT_LOGIC);
    DELAY(delays[0]);
    unsigned char bit = MSB_FIRST ? 1 << 7 : 1;
    do {
      bool bitBool = *send & bit ? DEFAULT_LOGIC : !DEFAULT_LOGIC;
      SET_BIT(bitBool);
      MSB_FIRST ? bit >>= 1 : bit <<= 1;
      DELAY(delays[1]);
    } while (bit);
    SET_BIT(DEFAULT_LOGIC);
    DELAY(delays[2]);
  } while (++send < end);
  interrupts();
}

void setup() {
  Serial.begin(9600);
  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  SET_BIT(DEFAULT_LOGIC);
  //  Serial1.begin(38400);
}

bool state = true;

void loop() {
  unsigned long startTime = TIME();
  if (true) {
    SendBytes(sendBuffer, sizeof(sendBuffer) / sizeof(sendBuffer[0]));
    //    DELAY(16);
    //    delay(10);
//        Serial.print("Delay = ");
//        Serial.println(((unsigned int)defaultSendDelays[0]));
    //  ReceiveBytes(receiveBuffer, sizeof(receiveBuffer) / sizeof(receiveBuffer[0]));
  } else {
    //    Serial1.write(sendBuffer, sizeof(sendBuffer) / sizeof(sendBuffer[0]));
    //    delay(10);
  }
  //  DELAY_UNTIL(startTime+1000);
  delay(100);
}
