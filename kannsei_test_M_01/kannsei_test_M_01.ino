#include <Wire.h>

#define NUMBER_OF_SHIFT_CHIPS   28
#define DATA_WIDTH   NUMBER_OF_SHIFT_CHIPS * 8
#define PULSE_WIDTH_USEC   5
#define POLL_DELAY_MSEC   1

#define BYTES_VAL_T unsigned int

int ploadPin2        = 12;
int clockEnablePin2  = 11;
int dataPin2         = 10;
int clockPin2        = 13;

BYTES_VAL_T pinValues;
BYTES_VAL_T oldPinValues;

char toUnity[225] = "0000000000000000000000000000000001110001110000011111011111000111111111111100111111111111100111111111111100011111111111000001111111110000000111111100000000011111000000000001110000000000000100000000000000000000000000000000000000";
String str = "";
byte leng;
byte censor_arr[DATA_WIDTH + 1];
byte led_arr[30];


void setup() {
  // put your setup code here, to run once:
  Wire.begin(); // Wire 라이브러리 초기화
  Serial.begin(9600); // 직렬 통신 초기화

  //센서
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);

  
  pinMode(ploadPin2, OUTPUT);
  pinMode(clockEnablePin2, OUTPUT);
  pinMode(clockPin2, OUTPUT);
  pinMode(dataPin2, INPUT);

  digitalWrite(clockPin2, LOW);
  digitalWrite(ploadPin2, HIGH);
}

void loop() {

  if (Serial.available())
  {
    leng = Serial.readBytes(toUnity, 1);  //toUnity 인덱스 0 을 셀렉터로 사용

    switch (toUnity[0]) {
      case 'N':
        Serial.print("PFEJAF31T8UJ95");
        break;

      case 'L':
        parse_led();
        Wire.beginTransmission(4);
        Wire.write('L');
        for(int i = 0; i < 30; i++){
          Wire.write(led_arr[i]);
        }
        Wire.endTransmission(true);
        break;

      case 'C':
        get_censor();
        break;

      case 'F':
        Wire.beginTransmission(4);
        Wire.write('F');
        Wire.endTransmission(true);
        break;

      case 'S':
        Wire.beginTransmission(4);
        Wire.write('S');
        Wire.endTransmission(true);
        break;

      default:
        Serial.print("false");
    }
  }
}

//유니티에서 받은 배열을 바이트 단위로 파싱
void parse_led() {
  leng = Serial.readBytes(toUnity, 225);
  
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

//센서 값 출력
void print_censor() {
  for (int i = 1; i <= DATA_WIDTH + 1; i++)
  {
    Serial.print(censor_arr[i - 1] == 1 ? "★" : "☆");
    Serial.print("  ");
    if (i % 15 == 0) {
      Serial.println();
    }
  }
  Serial.println();
  Serial.println();
  Serial.println();
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
  
  censor_arr[DATA_WIDTH] = !digitalRead(A0);
  censor_arr[DATA_WIDTH-1] = !digitalRead(9);
  censor_arr[DATA_WIDTH-2] = !digitalRead(8);
  censor_arr[DATA_WIDTH-3] = !digitalRead(7);
  censor_arr[DATA_WIDTH-4] = !digitalRead(6);
  censor_arr[DATA_WIDTH-5] = !digitalRead(5);
  censor_arr[DATA_WIDTH-6] = !digitalRead(4);
  censor_arr[DATA_WIDTH-7] = !digitalRead(3);
  censor_arr[DATA_WIDTH-8] = !digitalRead(2);
  
  int save = censor_arr[127]; censor_arr[127] = censor_arr[128]; censor_arr[128] = save;

  parse_censor();
}

// 센서 값들을 스트링으로 파싱
void parse_censor() {
  str = "";

  for (int i = 0; i < 225; i++) {
    str += censor_arr[i];
  }

  Serial.print(str);
  Serial.println();
}
