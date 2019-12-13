   unsigned long previous_time = millis();
   unsigned long current_time = millis();
   unsigned long max_timeout = 100; // 100ms
   int index = 0;
   do {
      if (Serial2.available() > 0) {
         readStuff[index] = Serial2.read(); // read data
         index++; // increase buffer index
         previous_time = millis(); // update previous time not to timeout
      } else {
         if (millis() - previous_time > max_timeout) {
            break; break;
         }
      }
   } while (index < bufferSize);
   // index = number of chars read
