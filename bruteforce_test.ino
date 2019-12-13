/** DEFINES **/
#define BAUDRATE 19200
#define LIN_BREAK 20
#define LIN_SYNC 0x55
#define LIN_SLAVE_READ_BUTTONS 0x9C

#define TX2_PIN 16

/** GLOBALLS **/
byte readStuff[32] = {0};

/** FUNCTIONS **/
bool wait_for_serial_data(int timeout){
  bool data_ready = false;
  unsigned long previous_time = millis();
  unsigned long current_time = previous_time;
  unsigned long max_timeout = timeout; // max timeout of 10ms
  while((current_time - previous_time < max_timeout) && data_ready == false){
    if (Serial2.available() > 0){
      data_ready = true;
    }
    current_time = millis();
  }
  return data_ready;
}

int read_serial_data(unsigned long timeout){ // return number of bytes read
  int index = 0;
  unsigned long previous_time = millis();
  unsigned long current_time = previous_time;
  
  do { // do loop while timeout not exceeded, AND index smaller than 32
    while(Serial2.available() > 0 && index < 32){ // if data available AND index smaller than 32
      readStuff[index] = Serial2.read(); // read data
      index++; // increase buffer index
      previous_time = millis(); // update previous time to reset the timeout
    }
    current_time = millis();
  } while(current_time - previous_time < timeout && index < 32);

  return index;
}

void clear_serial_data(void){
  // if data still available in the buffer, clear it by reading it
  while (Serial2.available() > 0){
    byte temp = Serial2.read();
  }
}

// SETUP
void setup(){
  Serial.begin(BAUDRATE);
  Serial.println("Bruteforcing LIN:");
}

// LOOP
void loop(){
  Serial.println("Sending break...");
  Serial2.end();
  pinMode(TX2_PIN, OUTPUT);
  digitalWrite(TX2_PIN, LOW);
  delayMicroseconds((1000000 * LIN_BREAK) / BAUDRATE);

  Serial.println("Setting up serial2...");
  Serial2.begin(BAUDRATE);
  
  wait_for_serial_data(10);
  clear_serial_data();

  Serial.println("Sending SYNC...");
  Serial2.write(LIN_SYNC);
  Serial.println("Sending read buttons address...");
  Serial2.write(LIN_SLAVE_READ_BUTTONS);
  
  // optional delay : Serial works with interrupt, and arduino has an internal buffer of 64 bytes
  // this delay waits a bit until all/most of the data has been read by the interrupt
  // From my tests it works without delay
  delay(20); 

  bool data_ready = wait_for_serial_data(10);

  if (data_ready){
    int index = 0;
    Serial.println("Reading data from Serial2 buffer...");
    index = read_serial_data(10); // read data, timeout after 10ms of no data
    
  
    Serial.print("    data = ");
    for (int i = 0; i < index; i++){ // write as many bytes to serial monitor as has been received
      Serial.print("0x");
      Serial.print(readStuff[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  clear_serial_data();

  delay(1000);
}
