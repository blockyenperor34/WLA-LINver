#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

//----------------------------[Config]------------------------------------

#define WIFISSID "B10cky" //wifi SSID
#define WIFIPASSWD "123456789" //wifi Passowrd
#define API_KEY "AIzaSyDSYJCBhEkr6lyluw1QH9-LG29-IReoT8Y "
#define DATABASE_URL "https://wlas-51abf-default-rtdb.asia-southeast1.firebasedatabase.app/" 
#define USER_EMAIL "blocky34@blocky34.tech"
#define USER_PASSWORD "blockytech"
#define WriteRate 5000 //write to firebase every 5 seconds(two minutes)
#define BtnPin 16 //button that connects to our device
#define RelayPin 17 //relay that controls the Led output
#define trigPin 22 // (RX) Pin to send trigger pulse
#define echoPin 23 // (TX) Pin to receive echo pulse

//------------------------------------------------------------------------

long duration;
int distance;
int pressed = 1;
bool aleart = false;
long lastUpload = -120000;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void measure();
void connectWIFI();
void initFirebase();
void sendFirebase();

void setup() 
{
    pinMode(BtnPin,INPUT_PULLUP); //按鈕訊號輸入
    pinMode(RelayPin,OUTPUT); //繼電器
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    Serial.begin(9600);
    connectWIFI();
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    initFirebase();
    
}

void loop() 
{

  pressed = digitalRead(BtnPin); //讀取按鈕現在的狀態
  //Serial.print(pressed);
  if(pressed == 0)
  {
    Serial.println("按鈕被觸發了");
    //distance = random(0, 100); //debug
    //digitalWrite(17, HIGH);   //啟動繼電器
    aleart = true; 
    //Serial.printf("triggered");
  }
  else
  {
    //aleart = false;  
    //digitalWrite(17, LOW); //關閉繼電器
    //measure();
    //distance = random(0, 100); //debug
    //Serial.printf("triggered");
  }

  if(aleart)
  {
    digitalWrite(17, HIGH);   //啟動繼電器
  }
  else
  {
    digitalWrite(17, LOW); //關閉繼電器
  }


  //every 2 minutes send data to firebase
  if(millis() -lastUpload >= WriteRate)
  {
    Serial.println("sending data to firebase");
    sendFirebase();
    lastUpload = millis();
    aleart = false;
  }
}


void measure()
{
  // Clear the trigPin by setting it LOW:
  digitalWrite(trigPin, LOW);
  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance:
  distance = duration * 0.034 / 2;

}


void connectWIFI()
{
  int time = 0;
  WiFi.begin(WIFISSID, WIFIPASSWD);
  Serial.print("Connecting to Wi-Fi");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    time += 500;
    Serial.print(".");

    if(time >= 60000)
    {
      Serial.println("WIFI Time out");
      return;
    }
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void initFirebase()
{

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);
  fbdo.setResponseSize(2048);
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
  config.timeout.serverResponse = 10 * 1000;
}

void sendFirebase()
{

  Serial.printf("Set bool... %s\n", Firebase.RTDB.setBool(&fbdo, F("/isAleart"), aleart) ? "ok" : fbdo.errorReason().c_str());
  //Serial.printf("Set int... %s\n", Firebase.RTDB.setInt(&fbdo, F("/WaterLevel"), distance) ? "ok" : fbdo.errorReason().c_str());

  
}