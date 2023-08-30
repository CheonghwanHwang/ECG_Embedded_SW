#include <SoftwareSerial.h>
#include "U8glib.h"
#include "display.h"

/** Pin Define **/
#define ETX_Pin 4
#define ERX_Pin 3

/** Instance Define **/
U8GLIB_ST7920_128X64_4X u8g(10);
SoftwareSerial ECG_UART(ERX_Pin, ETX_Pin); // RX, TX

/** Variable **/
#define mode_mesure 0
#define mode_lcd_N  1
#define mode_lcd_V  2

int lcd_mode = mode_mesure;

int M = 0, N = 0, V = 0;

/** Function **/
void draw(void) {
  switch(lcd_mode){
    case mode_mesure:
      if(M == 0)      u8g.drawBitmapP (32, 0, 8, 64, M0); 
      else if(M == 1) u8g.drawBitmapP (32, 0, 8, 64, M1); 
      else if(M == 2) u8g.drawBitmapP (32, 0, 8, 64, M2); 
      else if(M == 3) u8g.drawBitmapP (32, 0, 8, 64, M3); 
      else if(M == 4) u8g.drawBitmapP (32, 0, 8, 64, M4); 
      else if(M == 5) u8g.drawBitmapP (32, 0, 8, 64, M5); 
      else if(M == 6) u8g.drawBitmapP (32, 0, 8, 64, M6); 
      else if(M == 7) u8g.drawBitmapP (32, 0, 8, 64, M7); 
      else if(M == 8) u8g.drawBitmapP (32, 0, 8, 64, M8); 
      else if(M == 9) u8g.drawBitmapP (32, 0, 8, 64, M9); 

      if(M==9) M=0;
      else M = M + 1;
  
      break;
      
    case mode_lcd_N:
      N = random(3);
      if(N == 0)      u8g.drawBitmapP (0, 0, 16, 64, N0); 
      else if(N == 1) u8g.drawBitmapP (0, 0, 16, 64, N1); 
      else if(N == 2) u8g.drawBitmapP (0, 0, 16, 64, N2); 
      break;
      
    case mode_lcd_V:
      V = random(3);
      if(V == 0)      u8g.drawBitmapP (0, 0, 16, 64, V0); 
      else if(V == 1) u8g.drawBitmapP (0, 0, 16, 64, V1); 
      else if(V == 2) u8g.drawBitmapP (0, 0, 16, 64, V2); 
      break;
      
    default: 
      lcd_mode = mode_mesure;
      break; 
  }
}
 
void setup(void) {
  Serial.begin(115200);
  ECG_UART.begin(115200);
  
  randomSeed(analogRead(1));

  for(int i=0;i<50;i++){
    u8g.firstPage();  
    do {
      draw();
    } while( u8g.nextPage() );
    delay(100);
  }
}
 
void loop(void) {  
  // Edge Node Debuging
  if(Serial.available()){
    ECG_UART.write(Serial.read());
  }
  
  // Read Command
  if(ECG_UART.available()){
    char cmd = ECG_UART.read();
    Serial.write(cmd);

    M = 0; 
    
    if(cmd == 'M')      lcd_mode = mode_mesure;
    else if(cmd == 'N') lcd_mode = mode_lcd_N;
    else if(cmd == 'V') lcd_mode = mode_lcd_V;
  }
  
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
}
