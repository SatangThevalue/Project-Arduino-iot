#include <SimpleTimer.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial
SimpleTimer Timer;
#define sw_ResetWifi D1
#define relay1 D6
#define relay2 D7
#define relay3 D5
#define relay4 D8
char blynk_token[] = "HoL7PpzQnGiCG3eIZF5q1Tkd0SlH806x";
char blynk_host[] = "en2.cmtc.ac.th";
int blynk_port = 8080;
int set_Humi;
BLYNK_CONNECTED() {
  Blynk.syncAll();
}
BLYNK_WRITE(V0) {
  int val_relay1 = param.asInt();
  digitalWrite(relay1, !val_relay1);
  Serial.printf("relay1 :: %d\n",val_relay1);
}
BLYNK_WRITE(V1) {
  int val_relay2 = param.asInt();
  digitalWrite(relay2, !val_relay2);
  Serial.printf("relay2 :: %d\n",val_relay2);
}
BLYNK_WRITE(V2) {
  int val_relay3 = param.asInt();
  digitalWrite(relay3, !val_relay3);
  Serial.printf("relay3 :: %d\n",val_relay3);
}
BLYNK_WRITE(V3) {
  int val_relay4 = param.asInt();
  digitalWrite(relay4, !val_relay4);
  Serial.printf("relay4 :: %d\n",val_relay4);
}
BLYNK_WRITE(V6) {
  set_Humi = param.asInt();
}
void setup() {
  Serial.begin(9600);
  WiFiManager wm;
  WiFiManagerParameter custom_blynk_token("Blynk Token", "blynk token", blynk_token, 34);
  wm.addParameter(&custom_blynk_token);

  Serial.println("Reset WiFiManager Config?");
  for (int i = 5; i > 0; i--) {
    Serial.print(String(i) + " ");
    delay(1000);
  }
  pinMode(sw_ResetWifi, INPUT_PULLUP);
  if (digitalRead(sw_ResetWifi) == LOW) {
    wm.resetSettings();
    Serial.println("WiFiManager :: Reset WiFiManager OK!");
    Serial.println("WiFiManager :: ESP8266 IOT-Big-Data65 restart!!!!");
    delay(5000);
    ESP.restart();
  }

  bool res;
  res = wm.autoConnect("IOT-Big-Data65", "00000000");

  if (!res) {
    Serial.println("WiFiManager :: Failed to connect");
    delay(5000);
    Serial.println("WiFiManager :: ESP8266 IOT-Big-Data65 restart!!!!");
    ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    Serial.println("WiFiManager :: connected...yeey :)");
  }
  Blynk.config(blynk_token, blynk_host, blynk_port);
  pinMode(A0, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(sw_ResetWifi, INPUT_PULLUP);
  Timer.setInterval(3000L, read_soil);

  if (Blynk.connected() == 0) {
    Serial.printf("Blynk Server connected :: Connected %d\n");
    Serial.print("Blynk Server Host :: ");
    Serial.print(blynk_host);
    Serial.print("\n");
    Serial.printf("Blynk Server Host :: %d\n", blynk_port);
  } else if (Blynk.connected() == 0) {
    Serial.printf("Blynk Server connected :: Failed to connect!! %d\n");
  }
}

void loop() {
  Blynk.run();
  Timer.run();
}
void read_soil(void) {
  int soil_value = analogRead(A0);
  //อ่านค่าจาก A0
  int soil_percent = map(soil_value, 1023, 0, 0, 100);
  //แปลงค่า
  Serial.printf("Raw Value - %d percent - %d \n", soil_value, soil_percent);
  //แสดงออกทางจอภาพ
  delay(500);
  Blynk.virtualWrite(V4, soil_value); //ส่งค่าไปยังเซิฟเวอร์
  Blynk.virtualWrite(V5, soil_percent); //ส่งค่าไปยังเซิฟเวอร์
  Blynk.virtualWrite(V6, set_Humi); //ส่งค่าไปยังเซิฟเวอร์
  if (soil_percent < 40) {
    //ถ้าความชื้อนน้อยกว่า 40 เปอร์เซ็น ให้เปิดปั้ม
    Blynk.virtualWrite(V0, 0);
    digitalWrite(relay1, HIGH);
    Serial.println("Relay1 :: ON By soil_percent < 40");
  } else if (soil_percent >= 80) {
    //ถ้าความชื้อนมากกว่า 80 เปอร์เซ็น ให้ปิดปั้ม
    Blynk.virtualWrite(V0, 1);
    digitalWrite(relay1, LOW);
    Serial.println("Relay1 :: OFF By soil_percent = 80");
  } else if (soil_percent < set_Humi) {
    //ถ้าความชื้อนน้อยกว่าที่ตั้งค่าไว้ ให้เปิดปั้ม
    Blynk.virtualWrite(V0, 0);
    digitalWrite(relay1, HIGH);
    Serial.println("Relay1 :: ON By soil_percent < set_Humi");
  } else if (soil_percent > set_Humi) {
    //ถ้าความชื้อนมากกว่าที่ตั้งค่าไว้ ให้ปิดปั้ม
    Blynk.virtualWrite(V0, 1);
    digitalWrite(relay1, LOW);
    Serial.println("Relay1 :: OFF By soil_percent > set_Humi");
  }
}
