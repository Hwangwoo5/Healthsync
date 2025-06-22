#include <AltSoftSerial.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>  // JSON 처리 라이브러리 추가
#include <MHZ19PWM.h>  // 'MHZ19PWM.h' 포함

// 🔹 KY-015 온습도 센서 설정
#define KY015_PIN A0
#define DHTTYPE DHT11  // KY-015은 DHT11 센서 내장
float TEMP_THRESHOLD = 25.0;  // 온도 임계값 (°C)
float HUMIDITY_THRESHOLD = 50.0;  // 습도 임계값 (%)

DHT dht(KY015_PIN, DHTTYPE);

// 🔹 팬 핀 설정
#define FAN2_A 5
#define FAN2_B 6

// 🔹 블루투스 설정 (HC-05 연결)
AltSoftSerial BTSerial(8, 9);  // HC-05 TX: 8, RX: 9

// 🔹 MH-Z19E 이산화탄소 센서 설정
#define MH_Z19E_TX_PIN 10
#define MH_Z19E_RX_PIN 11
#define MH_Z19E_PWM_PIN 2

MHZ19PWM mhz(MH_Z19E_PWM_PIN, MHZ_DELAYED_MODE);

// 🔹 MQ-7 일산화탄소 센서 설정
#define MQ7_PIN A1
#define CO_THRESHOLD 400  // 일산화탄소 임계값 (ppm)
                    
// 자동 모드 상태 변수
bool fanAutoMode = false;

// 데이터 전송 타이머 설정
unsigned long lastSendTime = 0;
unsigned long sendInterval = 10000;  // 10초마다 데이터 전송

// 📌 MH-Z19E 센서 패킷 데이터
unsigned char Send_data[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
unsigned char Receive_Buff[9];

// 기준 시간: 2025년 2월 19일 13시 55분
unsigned long baseTime = 1708352100;  // Unix Timestamp: 2025-02-19 13:55:00

void setup() {
  BTSerial.begin(9600);
  mhz.useLimit(5000);

  // 🔹 핀 모드 설정
  pinMode(FAN2_A, OUTPUT);
  pinMode(FAN2_B, OUTPUT);
  pinMode(MH_Z19E_PWM_PIN, INPUT);

  digitalWrite(FAN2_A, LOW);
  digitalWrite(FAN2_B, LOW);

  // 🔹 DHT 센서 시작
  dht.begin();

}

void loop() {
  // 📌 온도 및 습도 측정
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
  } else {

    // 📌 자동 모드에서 임계 온도 또는 습도 초과 시 팬 작동
    if (fanAutoMode && (temperature >= TEMP_THRESHOLD || humidity >= HUMIDITY_THRESHOLD)) {
      turnOnFan2();
    } else if (fanAutoMode) {
      turnOffFan2();
    } 
  }

  // 📌 블루투스 명령 수신 (JSON 지원 추가)
  if (BTSerial.available()) {
      String command = BTSerial.readStringUntil('\n');
      command.trim();

      // JSON 데이터인지 확인
      if (command.startsWith("{") && command.endsWith("}")) {
          StaticJsonDocument<128> doc;
          DeserializationError error = deserializeJson(doc, command);
          

          if (!error) {
              const char* cmd = doc["command"];

              if (String(cmd) == "THRESHOLD") {
                  TEMP_THRESHOLD = doc["temperature"];
                  HUMIDITY_THRESHOLD =doc["humidity"];
              }
              else if (String(cmd) == "AUTO") {
                fanAutoMode = doc["value"];
              }
          }
      } 
      else {  
          // 기존 문자열 명령 처리 (ON2, OFF2, AUTO)
          if (command.equalsIgnoreCase("ON2")) { turnOnFan2();}
          else if (command.equalsIgnoreCase("OFF")) { turnOffFan2();}
      }
  }


  // 📌 10초마다 블루투스로 센서 데이터 전송
  if (millis() - lastSendTime >= sendInterval) {
    lastSendTime = millis();
    sendSensorData();
  }

  delay(2000);
}

// 🚀 팬 제어 함수
void turnOnFan2() { digitalWrite(FAN2_A, HIGH); digitalWrite(FAN2_B, LOW); }
void turnOffFan2() { digitalWrite(FAN2_A, LOW); digitalWrite(FAN2_B, LOW);  }

// 📌 MH-Z19E 이산화탄소 측정
float getCO2PPM() {
  int co2 = mhz.getCO2();

  if (co2) {
    return co2;
  }
  return -1;
}

// 📌 MQ-7 일산화탄소 측정
int getCO() {
  int co = analogRead(MQ7_PIN);
  return co;
}

// 📡 블루투스로 센서 데이터 전송
void sendSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float co2 = getCO2PPM();
  int co = getCO();

  // 현재 타임스탬프 생성
  String timestamp = getTimestamp();

  // JSON 객체 생성
  StaticJsonDocument<512> doc;
  doc["type"] = "sensor_data";
  JsonObject data = doc.createNestedObject("data");
  data["temperature"] = temperature;
  data["humidity"] = humidity;
  data["co2"] = co2;  // CO2 값 추가
  data["co"] = co;    // CO 값 추가
  data["timestamp"] = timestamp;

  String output;
  serializeJson(doc, output);  // JSON 문자열로 변환

  BTSerial.println(output);  // 블루투스로 데이터 전송
}

// 📅 타임스탬프 생성 함수
String getTimestamp() {
  unsigned long currentMillis = millis() + baseTime; // 기준 시간 더하기

  // 시간을 초, 분, 시간으로 변환
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;

  // 24시간 형식으로 시간을 변환
  hours = hours % 24;
  minutes = minutes % 60;
  seconds = seconds % 60;

  // 타임스탬프 생성
  char timestamp[20];
  snprintf(timestamp, sizeof(timestamp), "Day %lu %02lu:%02lu:%02lu", days, hours, minutes, seconds);
  return String(timestamp);
}