#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#define PIN_LED 6 // 스트랩의 6번 포트
#define NUM_LEDS 60 // 스트랩의 LED 개수

SoftwareSerial BTSerial(10, 11); // HC-06 모듈의 RX, TX 핀을 10번, 11번으로 설정
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);
int pattern = 0;
bool patternRunning = false; // 패턴 실행 여부를 저장하는 변수

// 초음파 센서 핀 정의
#define TRIGGER_PIN 2
#define ECHO_PIN 3

void setup() {
  BTSerial.begin(9600); // 블루투스 시리얼 통신 시작
  pixels.begin(); // pixels 객체를 초기화

  // 초음파 센서 핀 모드 설정
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  while (BTSerial.available()) {
    if (patternRunning) {
      char input = BTSerial.read();

      // case문 실행했어도 다른 case문 실행하는 코드
      patternRunning = false;
      changePattern(pattern);
    } else {
      pattern = BTSerial.parseInt();
      changePattern(pattern);
      patternRunning = true;
    }
  }

  // 초음파 센서로 거리 측정
  long duration, distance;
  digitalWrite(TRIGGER_PIN, LOW); // 초음파 신호 준비
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH); // 초음파 신호 발생
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH); // 초음파 반사 시간 측정
  distance = duration * 0.034 / 2; // 거리 계산 (음속 340m/s, 왕복 거리이므로 2로 나눔)

  // 일정 거리 이하일 때 LED 켜기
  if (distance < 40) { // 40cm 이하로 접근했을 때 LED를 켜도록 설정
    rainbowCycle(10); // 무지개 패턴 1
  } else {
    turnOffLEDs(); // led 꺼짐
  }
}

// 모든 LED를 끄기
void turnOffLEDs() {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, 0);
  }
  pixels.show();
}

// 모든 LED를 켜기
void turnOnLEDs() {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();
}

// 다양한 패턴 실행
void changePattern(int pattern) {
  switch (pattern) {
    case 1:
      theaterChaseRainbow(80); // 무지개 패턴 2
      turnOffLEDs();
      break;
    // 2, 3, 4 케이스문은 일부로 break 빼서 연속적으로 실행하도록 함
    case 2:
      for (int j = 0; j < 5; j++) {
        colorWipe(pixels.Color(255, 0, 0), 20); // 빨간색 패턴(RGB 색상 패턴)
        turnOffLEDs();
      }
      
    case 3:
      for (int j = 0; j < 5; j++) {
        colorWipe(pixels.Color(0, 0, 255), 20); // 파란색 패턴(RGB 색상 패턴)
        turnOffLEDs();
      }
      
    case 4:
      for (int j = 0; j < 5; j++) {
        colorWipe(pixels.Color(0, 255, 0), 20); // 초록색 패턴(RGB 색상 패턴)
        turnOffLEDs();
      }
      
  }
}



// 일정한 속도로 모든 색상을 순환하며 LED를 조정 (무지개 패턴 1)
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 2; j++) { // 5 사이클 동안 모든 색상을 순환
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// 일정한 속도로 256가지 색상을 순환하며 LED를 조정 (무지개 패턴 2)
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) { // 256가지 색상을 순환
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, Wheel((i + j) % 255)); // 3번째 픽셀마다 켜기
      }
      pixels.show();
      delay(wait);
      for (uint16_t i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, 0); // 3번째 픽셀마다 끄기
      }
    }
  }
}

//지정한 색상으로 LED를 하나씩 채워주는 패턴 (RGB 색상 패턴)
void colorWipe(uint32_t color, uint8_t wait) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(wait);
  }
}

// 지정한 색상으로 LED를 특정 패턴으로 채워주는 패턴
void theaterChase(uint32_t color, uint8_t wait) {
  for (int j = 0; j < 10; j++) {  // 10번 반복
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, color);  // 3번째 픽셀마다 지정한 색상 켜기
      }
      pixels.show();
      delay(wait);
      for (uint16_t i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, 0);  // 3번째 픽셀마다 끄기
      }
    }
  }
}

// 주어진 위치에 따라 빨강, 초록, 파랑을 조합하여 색상을 계산
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
