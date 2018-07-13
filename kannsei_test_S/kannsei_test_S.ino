#include <Wire.h>
#define SLAVE 4

int led_arr[30];
int b_success = 0;
int b_fail = 0;
int b_led = 0;

int latchPin = 7;
int clockPin = 5;
int dataPin = 6;

void setup() {
  // Wire 라이브러리 초기화
  // 슬레이브로 참여하기 위해서는 주소를 지정해야 한다.
  Wire.begin(SLAVE);
  // 마스터의 데이터 전송 요구가 있을 때 처리할 함수 등록
  Wire.onReceive(r_select);

    pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);

    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    digitalWrite(latchPin, HIGH);
}

void loop () {
  
  if(b_led){
    led_Control();
  }
  
  if(b_fail){
    fail();
    b_fail = 0;
  }
  
  if(b_success){
    success();
    b_led = 0;
    b_success = 0;
  }
}


void fail(){
  for(int i = 0; i < 2; i++)
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, 255);
    shiftOut(dataPin, clockPin, LSBFIRST, 254);

    shiftOut(dataPin, clockPin, LSBFIRST, 255);
    shiftOut(dataPin, clockPin, LSBFIRST, 254);
    digitalWrite(latchPin, HIGH);
    
    delay(300);
    
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);

    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    digitalWrite(latchPin, HIGH);

    delay(300);
  }
}

void success(){
  for(int i = 0; i < 2; i++){
    for ( int i = 128, j = 0; i >= 1; i /= 2, j += 2) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, 255);
    shiftOut(dataPin, clockPin, LSBFIRST, 254);

    shiftOut(dataPin, clockPin, LSBFIRST, i);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    digitalWrite(latchPin, HIGH);
    
    delay(150);
  }
    for ( int i = 128, j = 16; i >= 2; i /= 2, j += 2) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, LSBFIRST, 255);
      shiftOut(dataPin, clockPin, LSBFIRST, 254);
  
      shiftOut(dataPin, clockPin, LSBFIRST, 0);
      shiftOut(dataPin, clockPin, LSBFIRST, i);
      digitalWrite(latchPin, HIGH);
      
      delay(150);
    }
  }
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, 255);
    shiftOut(dataPin, clockPin, LSBFIRST, 254);

    shiftOut(dataPin, clockPin, LSBFIRST, 255);
    shiftOut(dataPin, clockPin, LSBFIRST, 254);
    digitalWrite(latchPin, HIGH);

    delay(1000);
  
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);

    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    digitalWrite(latchPin, HIGH);

/*
          for(int i = 0; i < 30; i++){
            led_arr[i] = 0;
          }
*/

}

void led_Control() {
  for ( int i = 128, j = 0; i >= 1; i /= 2, j += 2) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, led_arr[j]);
    shiftOut(dataPin, clockPin, LSBFIRST, led_arr[j+1]);

    shiftOut(dataPin, clockPin, LSBFIRST, i);
    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    digitalWrite(latchPin, HIGH);
  }

  for ( int i = 128, j = 16; i >= 2; i /= 2, j += 2) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, led_arr[j]);
    shiftOut(dataPin, clockPin, LSBFIRST, led_arr[j+1]);

    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, i);
    digitalWrite(latchPin, HIGH);
  }
}

void r_select(){
  char select = Wire.read();
  int lg = Wire.available();

  switch (select) {
      case 'L':
        for(int i = 0; i < 30; i++){
          led_arr[i] = Wire.read();
        }
        b_led = 1;
        break;

      case 'F':
        b_fail = 1;
        break;

      case 'S':
        b_success = 1;
        break;

      default:
        Serial.print("false");
  }
}


