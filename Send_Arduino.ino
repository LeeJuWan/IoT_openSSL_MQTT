#include <FS.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "LEA_core.h"
#include "LEA_core.c"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
// LEA_core.c LEA_core.h 는 KISA에서 아두이노 소스코드.zip 제공해줌. 찾아보는 성의 발휘 요망

#define SSID        "ssid"
#define PASSWORD    "password"

int counting_Read_File=0; // 파일 저장 순서를 의미하는 변수
int serverDie_Check = 0; // 브로커 서버 통신 여부
char LeaKey[50]; //Lea 암호키

//MQTT broker ip Cent OS
const char* mqtt_server = "broker-server-ip";

// 인증서 업로드에 사용될 소켓 통신용 단말기 포트번호
int port = 9999; 

WiFiServer server(port);
WiFiClient Socket_client;
WiFiClientSecure espClient;

// 서버 인증서 16진수 대입
const uint8_t mqttCertFingerprint[] = {0x11,0x22,0x33,0x3D,0x1A,0x37,0xB4,0xF0,0x39,0x28,0x25,0x52,0xC8,0x77,0xF4,0x0F,0xE1,0x46,0x22,0x28};

//Publish시 전송할 itemlist
long lastMsg = 0; 
char msg[50];
int value = 0;


//scribe topic으로부터 메시지 수신 시 발동
void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    LeaKey[i]= (char)payload[i]; //암호키 저장
  }
  // (옵션1) callback 받자마자 단말 간 통신 진행
  WiFiClient Send_client;
  BYTE pdwRoundKey[384] = { 0x0, };
  BYTE pbData[16] = { "serverdie"};
  char sendData[16] = { 0,};
   while(true){
     if(Send_client.connect("192.168.xxx.xxx(단말기ip주소)",8888)){
          break; //succsess!
      }
    }
    LEA_Key(LeaKey, pdwRoundKey); //서버로부터 수신받은 대칭키로 라운드키 생성
    LEA_Enc(pdwRoundKey,pbData); //생성된 라운드키로 암호화 진행
    // 암호화 진행
    for(int j=0; j<1;j++){
      for(int i=0;i<16;i++)
        sendData[i]=(char)pbData[i];
     }
     while(1){
       // 다른 단말기로 암호화된 데이터 전송
       Send_client.print(sendData);
       delay(4000); 
     }
}

//set MQTT port number to 8883 as per //standard
PubSubClient client(mqtt_server, 8883, callback, espClient);
/*
  X509List serverCertList(getCertificate());
  PrivateKey serverPrivKey(getPrivateKey());
  espClient.setClientRSACert(&serverCertList, &serverPrivKey);
*/

//와이파이 셋업
void wifi_setup(){
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD); //Connect to wifi

  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED) {   
    delay(500);
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
 
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
  server.begin(); //tcp start
  Serial.println("Server Open");
  Serial.print("Start wifi server and connect to IP:");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(port);
}

//재연결 지원
void reconnect(){ 
 
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if(client.connect("NodeMCU1")){ //esp8266 단말 식별자 이름 임의부여 가능
      Serial.println("connected");
      ////////////////////////////////////////////////////////////////
      // Pub(outTopic) & Sub(inTopic)
      // 해당 토픽으로 hellow world 송신
      client.publish("outTopic", "hello world");
      
      // 암호키를 받기위한 토픽 수신
      client.subscribe("inTopic");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      char buf[256];
      espClient.getLastSSLError(buf, 256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() 
{
  Serial.setDebugOutput(true);
  SPIFFS.begin();
  
   ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  
  ArduinoOTA.begin();
  Serial.begin(115200);
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // 소켓 Open & WiFi 통신 Open
  wifi_setup(); 
  //서버 인증서검증
  espClient.setFingerprint(mqttCertFingerprint);
  
  if(!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Load certificate file
  File cert = SPIFFS.open("/clientesp.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert) {
    Serial.println("Failed to open cert file");
  }
  else
    Serial.println("Success to open cert file");
  delay(1000);
  
  if (espClient.loadCertificate(cert))
    Serial.println("cert loaded");
  else
    Serial.println("cert not loaded");
  cert.close();
  
  // Load private key file
  File private_key = SPIFFS.open("/espkey.der", "r");
  if (!private_key) {
    Serial.println("Failed to open private cert file");
  }
  else
    Serial.println("Success to open private cert file");
  delay(1000);
  
  if(espClient.loadPrivateKey(private_key))
    Serial.println("private key loaded");
  else
    Serial.println("private key not loaded");
  private_key.close();
  
  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r");
  if (!ca)
    Serial.println("Failed to open ca ");
  else
    Serial.println("Success to open ca");
  delay(1000);

  if(espClient.loadCACert(ca))
    Serial.println("ca loaded");
  else
    Serial.println("ca failed");
}

void loop()
{
  ArduinoOTA.handle();
  File file;
  Socket_client = server.available();
  
  if(Socket_client){ //인증서 업로드를 위한 소켓전송 시 if 진행
     if(Socket_client.connected()){ //tcp 접속 시 while로 진행
        Serial.println("Client Connected");
        if(counting_Read_File == 0){
          file = SPIFFS.open("/clientesp.der","w");
          ++counting_Read_File; //다음 파일 위치로 이동
        }else if(counting_Read_File == 1){
          file = SPIFFS.open("/espkey.der","w");
          ++counting_Read_File; //다음 파일 위치로 이동
        }else if(counting_Read_File == 2){
          file = SPIFFS.open("/finger.txt","w");
          counting_Read_File=0; //처음 위치로 이동
        }
      }

     while(Socket_client.connected()){ //tcp 접속 시
        while(Socket_client.available()>0){
          // read data from the Cent OS
           file.print((char)Socket_client.read());
         }
      }
     file.close();
     
     Socket_client.stop();
     Serial.println("Client disconnected");

  }
  
  delay(2000); 
  if(counting_Read_File == 0){ //처음위치로 갔다는 건 파일전송이 다 끝났기에 다시 MQTT SSL통신
     //처음위치로 가지 않으면 MQTT통신을 수행하지 않고 'counting_Read_File'값이 0이 되길 기다림
     //위의 else if()문에서 counting_Read_File=0 셋업 수행
     
    if (!client.connected()) {
      reconnect();
      ++serverDie_Check; //서버 연결 fail 카운팅
      
      delay(3000); 
      if(serverDie_Check==10){
        //10회 fail 시 서버 다운으로 간주
        // (옵션2) serverDown시 단말 간 통신 진행
        WiFiClient Send_client;
        BYTE pdwRoundKey[384] = { 0x0, };
        BYTE pbData[16] = { "serverdie"};
        char sendData[16] = { 0,};
        while(true){
          if(Send_client.connect("192.168.xxx.xxx(단말기ip주소)",8888)){
            break; //succsess!
          }
        }
        LEA_Key(LeaKey, pdwRoundKey); //서버로부터 수신받은 대칭키로 라운드키 생성
        LEA_Enc(pdwRoundKey,pbData); //생성된 라운드키로 암호화 진행
        // 암호화 진행
        for(int j=0; j<1;j++){
          for(int i=0;i<16;i++)
            sendData[i]=(char)pbData[i];
        }
        while(1){
        // 다른 단말기로 암호화된 데이터 전송
          Send_client.print(sendData);
          delay(4000); 
        }
      }
    }
    client.loop();
  
    long now = millis();
    if (now - lastMsg > 2000) {
      lastMsg = now;
      ++value;
      snprintf (msg, 75, "{\"message\": \"hello world #%ld\"}", value);
      client.publish("outTopic", msg);
    }
  }
}
