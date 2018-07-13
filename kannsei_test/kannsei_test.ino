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

  byte heart[30] = {B00000000, B00000000, B00000000, B00000000, B00011100, B01110000, B00111110, B11111000, B01111111, B11111100,
               B01111111, B11111100, B01111111, B11111100, B00111111, B11111000, B00011111, B11110000, B00001111, B11100000,
               B00000111, B11000000, B00000011, B10000000, B00000001, B00000000, B00000000, B00000000, B00000000, B00000000};
               
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
  pinMode(21, INPUT);

  pinMode(ploadPin2, OUTPUT);
  pinMode(clockEnablePin2, OUTPUT);
  pinMode(clockPin2, OUTPUT);
  pinMode(dataPin2, INPUT);

  digitalWrite(clockPin2, LOW);
  digitalWrite(ploadPin2, HIGH);
    
  parse_led();
}

void loop() {
    led_Control();
}

//배열을 바이트 형식으로 파싱
void parse_led(){
  for(int i = 1, k = 0; i < 30; i += 2, k++){
    for(int j = k*15, z = 128; j < k*15+15; j++, z /= 2){
      
      if(z == 0){
        z = 128;
      }
      
      if(j%15 > 7){
        led_arr[i] += (byte)toUnity[j] == 48 ? 0 : z;
      }else{
        led_arr[i-1] += (byte)toUnity[j] == 48 ? 0 : z;
      }
    }
  }
}

//led로 출력
void led_Control(){
  for( int i = 128, j = 1; i >= 1; i /= 2, j += 2){
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, led_arr[j - 1]);
    shiftOut(dataPin, clockPin, LSBFIRST, led_arr[j]); 
  
    shiftOut(dataPin, clockPin, LSBFIRST, i);
    shiftOut(dataPin, clockPin, LSBFIRST, 0); 
    digitalWrite(latchPin, HIGH);
  }

  for( int i = 128, j = 16; i >= 2; i /= 2, j += 2){
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, led_arr[j - 1]);
    shiftOut(dataPin, clockPin, LSBFIRST, led_arr[j]); 
  
    shiftOut(dataPin, clockPin, LSBFIRST, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, i); 
    digitalWrite(latchPin, HIGH);
  }
}

//센서 값 출력
void print_censor(){
  digitalWrite(clockEnablePin2, HIGH);
  digitalWrite(ploadPin2, LOW);
  delayMicroseconds(PULSE_WIDTH_USEC);
  digitalWrite(ploadPin2, HIGH);
  digitalWrite(clockEnablePin2, LOW);
  
  for(int i = 1; i <= DATA_WIDTH + 1; i++)
    {
        Serial.print(censor_arr[i-1]);
        Serial.print("  ");
        if(i%15 == 0){
          Serial.println();
        }
    }
    Serial.println();
    Serial.println();
    Serial.println();
}

//센서 값 가져옴
void get_censor(){
  for(int i = 0, count = 7; i < DATA_WIDTH; i++)
    {
        censor_arr[i + count] = digitalRead(dataPin2);

        if(!censor_arr[i + count]){
          censor_arr[i + count] = 1;
        }else{
          censor_arr[i + count] = 0;
        }

        if(count == -7){
          count = 7;
        }else{
          count = count - 2;
        }

        digitalWrite(clockPin2, HIGH);
        delayMicroseconds(PULSE_WIDTH_USEC);
        digitalWrite(clockPin2, LOW);
    }

    censor_arr[DATA_WIDTH] = !digitalRead(21);
}

