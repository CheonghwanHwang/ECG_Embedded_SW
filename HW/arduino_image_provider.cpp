/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  ==============================================================================*/

#include "image_provider.h"

/*
   The sample requires the following third-party libraries to be installed and
   configured:

   Arducam
   -------
   1. Download https://github.com/ArduCAM/Arduino and copy its `ArduCAM`
      subdirectory into `Arduino/libraries`. Commit #e216049 has been tested
      with this code.
   2. Edit `Arduino/libraries/ArduCAM/memorysaver.h` and ensure that
      "#define OV2640_MINI_2MP_PLUS" is not commented out. Ensure all other
      defines in the same section are commented out.

   JPEGDecoder
   -----------
   1. Install "JPEGDecoder" 1.8.0 from the Arduino library manager.
   2. Edit "Arduino/Libraries/JPEGDecoder/src/User_Config.h" and comment out
      "#define LOAD_SD_LIBRARY" and "#define LOAD_SDFAT_LIBRARY".
*/

#if defined(ARDUINO) && !defined(ARDUINO_ARDUINO_NANO33BLE)
#define ARDUINO_EXCLUDE_CODE
#endif  // defined(ARDUINO) && !defined(ARDUINO_ARDUINO_NANO33BLE)

#ifndef ARDUINO_EXCLUDE_CODE

#include <Arduino.h>

#define sr     130
#define bpm    60
#define sr_time(x) (double) 60/sr/x*1000000
#define sr_time()  (double) 60/sr/bpm*1000000

TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, int8_t* image_data) {

  digitalWrite(LEDR, LOW);

  int ecg_buf[sr] = {};
  
  int beat = (double) analogRead(A1) / 1000;

  int peak = 0;
  bool peak_check = false;

  for( int i=0 ; i<sr ; i++){
      int ecg = analogRead(A0);
      int ecg_v = (double) (ecg-1550) / 1650 * 96;
      ecg_buf[i] = ecg_v;
      
      TF_LITE_REPORT_ERROR(error_reporter, "%d ", ecg_v);
  
      int beat = (double)analogRead(A1)/5;
          
      if( (!peak_check) && (beat>500) ){
          peak_check = true;
          peak = i;
       }
     delayMicroseconds( sr_time() );
  }

  if( (48 < peak) && (peak < (sr-48) ) ){
    for(int x = 0; x<96; x++){      
        int index = 96 * ecg_buf[x + peak - 48] + x;
        image_data[index] = static_cast<int8_t>(1);

        TF_LITE_REPORT_ERROR(error_reporter, "%d ", ecg_buf[x + peak - 48]);
//        Serial.println(ecg_buf[x + (peak - 48)]);
        delayMicroseconds(10);
    }  
  }   
  
/*
  for(int x = 0; x<96; x++){      
      int index = 96 * ecg_arr[x + peak - 48] + x;
      image_data[index] = static_cast<int8_t>(1);

      Serial.println(ecg_arr[x + peak - 48]);
      delay(1);
  } 
*/  
  digitalWrite(LEDR, HIGH);
  return kTfLiteOk;
}

#endif  // ARDUINO_EXCLUDE_CODE
