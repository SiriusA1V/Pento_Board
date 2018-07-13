#include <MsTimer2.h>
#include "Timer.h"

Timer t_led;
Timer t_censor;
Timer t_print;

int latchPin = 7;
int clockPin = 5;
int dataPin = 6;

#define NUMBER_OF_SHIFT_CHIPS   28
#define DATA_WIDTH   NUMBER_OF_SHIFT_CHIPS * 8
#define PULSE_WIDTH_USEC   5
#define POLL_DELAY_MSEC   1

#define BYTES_VAL_T unsigned int

int ploadPin2        = 11;
int clockEnablePin2  = 13;
int dataPin2         = 12;
int clockPin2        = 10;

BYTES_VAL_T pinValues;
BYTES_VAL_T oldPinValues;

char toUnity[225] = "0000000000000000000000000000000001110001110000011111011111000111111111111100111111111111100111111111111100011111111111000001111111110000000111111100000000011111000000000001110000000000000100000000000000000000000000000000000000";
String str = "";
byte leng;
byte censor_arr[DATA_WIDTH + 1];
byte led_arr[30];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  // Begin the serial monitor at 9600bps


  Serial1.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  Serial1.print("$$$");  // Print three times individually
  delay(100);  // Short delay, wait for the Mate to send back CMD
  Serial1.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  Serial1.print("---");
  delay(100);  // Short delay, wait for the Mate to send back CMD
  Serial1.begin(9600);  // Start bluetooth serial at 9600


  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(14, INPUT);

  pinMode(ploadPin2, OUTPUT);
  pinMode(clockEnablePin2, OUTPUT);
  pinMode(clockPin2, OUTPUT);
  pinMode(dataPin2, INPUT);

  digitalWrite(clockPin2, LOW);
  digitalWrite(ploadPin2, HIGH);

  t_led.every(0, led_Control);
  t_censor.every(0, get_censor);
  t_print.every(100, parse_censor);

  parse_led();
}

void loop() {

  if (Serial1.available())
  {
    leng = Serial1.readBytes(toUnity, 1);  //toUnity 인덱스 0 을 셀렉터로 사용

    switch (toUnity[0]) {
      case 'N':
        Serial1.print("PFEJAF31T8UJ95");
        break;

      case 'L':
        parse_led();
        break;

      case 'C':
        t_censor.update();
        break;

      case 'F':
        faild();
        break;

      case 'S':
        success();
        break;

      default:
        Serial1.print("false");
    }
  }

  get_censor();
  parse_censor();
}

void faild() {

}

void success() {

}

//배열을 바이트 형식으로 파싱
void parse_led() {
  for (int i = 1, k = 0; i < 30; i += 2, k++) {
    led_arr[i] = 0;
    led_arr[i - 1] = 0;
    for (int j = k * 15, z = 128; j < k * 15 + 15; j++, z /= 2) {

      if (z == 0) {
        z = 128;
      }

      if (j % 15 > 7) {
        led_arr[i] += (byte)toUnity[j] == 48 ? 0 : z;
      } else {
        led_arr[i - 1] += (byte)toUnity[j] == 48 ? 0 : z;
      }
    }
  }
}

//led로 출력
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

//센서 값 출력
void print_censor() {
  for (int i = 1; i <= DATA_WIDTH + 1; i++)
  {
    Serial1.print(censor_arr[i - 1]);
    Serial1.print("  ");
    if (i % 15 == 0) {
      Serial1.println();
    }
  }
  Serial1.println();
  Serial1.println();
  Serial1.println();
}

//센서 값 가져옴
void get_censor() {
  digitalWrite(clockEnablePin2, HIGH);
  digitalWrite(ploadPin2, LOW);
  delayMicroseconds(PULSE_WIDTH_USEC);
  digitalWrite(ploadPin2, HIGH);
  digitalWrite(clockEnablePin2, LOW);

  for (int i = 0, count = 7; i < DATA_WIDTH; i++)
  {
    censor_arr[i + count] = digitalRead(dataPin2);

    if (!censor_arr[i + count]) {
      censor_arr[i + count] = 1;
    } else {
      censor_arr[i + count] = 0;
    }

    if (count == -7) {
      count = 7;

    } else {
      count = count - 2;
    }

    digitalWrite(clockPin2, HIGH);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(clockPin2, LOW);
  }
  censor_arr[DATA_WIDTH] = !digitalRead(14);
}

// 센서 값들을 스트링으로 파싱
void parse_censor() {
  str = "";

  for (int i = 0; i < 225; i++) {
    str += censor_arr[i];
  }

  Serial1.print(str);
  Serial1.println();
}

