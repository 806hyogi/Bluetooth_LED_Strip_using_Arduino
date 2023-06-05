#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#define PIN_LED 6 // 스트랩의 6번 포트
#define NUM_LEDS 60 // 스트랩의 LED 개수

SoftwareSerial BTSerial(10, 11); // HC-06 모듈의 RX, TX 핀을 10번, 11번으로 설정
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);
int delayval = 100; // 지연 시간
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

    case 2:
      for (int j = 0; j < 5; j++) {
        colorWipe(pixels.Color(255, 0, 0), 20); // 빨간색 패턴(RGB 색상 패턴)
        turnOffLEDs();
      }
      break;
      
    case 3:
      for (int j = 0; j < 5; j++) {
        colorWipe(pixels.Color(0, 0, 255), 20); // 파란색 패턴(RGB 색상 패턴)
        turnOffLEDs();
      }
      break;
      
    case 4:
      for (int j = 0; j < 5; j++) {
        colorWipe(pixels.Color(0, 255, 0), 20); // 초록색 패턴(RGB 색상 패턴)
        turnOffLEDs();
      }
      break;
    
    case 5:
      alternateColor(); // 빨강과 초록이 섞여있는 패턴 
      break;

    case 6:
    for (int j = 0; j < 5; j++) { // 빨,초,파 3개의 LED만 켜지는 패턴
      setPixelColors();
    }
      break;

    case 7:
    for (int j = 0; j < 5; j++) { // 여러가지 LED가 한번에 켜지는 패턴
      setPixelColors2();
    }
      break;
    
    case 8:
    for (int j = 0; j < 5; j++) { // 자연스럽게 꺼지고 켜지는 Fadein 패턴
      fadeRainbow(250);
    }
    break;
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

void alternateColor() {
  for (int count = 0; count < 5; count++) {
    for (int i = 0; i < NUM_LEDS; i += 2) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));  // 빨간색 LED
    }

    for (int i = 1; i < NUM_LEDS; i += 2) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));  // 초록색 LED
    }
  
  pixels.show();
  delay(500);

  pixels.clear();  // 모든 LED를 끔
  pixels.show();
  delay(500);
  }
}

void setPixelColors() {
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));    // 첫 번째 픽셀: 빨강
  pixels.setPixelColor(1, pixels.Color(0, 255, 0));    // 두 번째 픽셀: 초록
  pixels.setPixelColor(2, pixels.Color(0, 0, 255));    // 세 번째 픽셀: 파랑
  // 나머지 픽셀도 필요한 만큼 설정

  pixels.show();  // 설정한 색으로 픽셀 표시

  delay(1000);  // 1초 동안 유지

  // 모든 픽셀 끄기
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }

  pixels.show();  // 픽셀 끄기 적용

  delay(1000);  // 1초 동안 유지
}

void setPixelColors2() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i % 3 == 0) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));    // 빨강
    } else if (i % 3 == 1) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));    // 초록
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 255));    // 파랑
    }
  }

  pixels.show();    // 설정한 색으로 픽셀 표시

  delay(1000);      // 1초 동안 유지

  // 모든 픽셀 끄기
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }

  pixels.show();    // 픽셀 끄기 적용

  delay(1000);      // 1초 동안 유지
}


int fadeAmount = 100;  // 각 단계에서 페이드인/페이드아웃되는 양

void fadeRainbow(int wait) {
  // 무지개 색상 초기화
  for (int i = 0; i < pixels.numPixels(); i++) {
    uint32_t color = Wheel((i * 256 / pixels.numPixels()) & 255);
    pixels.setPixelColor(i, color);
  }

  pixels.show();  // 초기 무지개 색상 표시
  delay(wait);    // 지연 시간

  for (int k = 0; k < 2; k++) {  // 전체 픽셀 페이드인/페이드아웃을 2번 반복
    // 페이드아웃
    for (int brightness = 255; brightness >= 0; brightness -= fadeAmount) {
      for (int i = 0; i < pixels.numPixels(); i++) {
        // 현재 픽셀의 색상 값을 가져와 밝기를 조절
        uint32_t color = pixels.getPixelColor(i);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        r = (r * brightness) / 255;
        g = (g * brightness) / 255;
        b = (b * brightness) / 255;
        pixels.setPixelColor(i, pixels.Color(r, g, b));
      }

      pixels.show();  // 픽셀 색상 업데이트
      delay(wait);    // 지연 시간
    }

    // 페이드인
    for (int brightness = 0; brightness <= 255; brightness += fadeAmount) {
      for (int i = 0; i < pixels.numPixels(); i++) {
        // 현재 픽셀의 색상 값을 가져와 밝기를 조절
        uint32_t color = pixels.getPixelColor(i);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        r = (r * brightness) / 255;
        g = (g * brightness) / 255;
        b = (b * brightness) / 255;
        pixels.setPixelColor(i, pixels.Color(r, g, b));
      }

      pixels.show();  // 픽셀 색상 업데이트
      delay(wait);    // 지연 시간
    }
  }

  // 모든 픽셀 끄기
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }

  pixels.show();  // 픽셀 끄기 적용
  delay(wait);    // 지연 시간
}