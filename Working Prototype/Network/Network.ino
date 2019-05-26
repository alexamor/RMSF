#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClient.h> 
/*#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>*/

#define RST_PIN 9
#define SS_PIN 10
#define GREEN_LED 4
#define YELLOW_LED 2
#define RED_LED 3
#define REED_SWITCH 6
#define INPUT_SWITCH 7

byte readCard[4];
char* myTags[100] = {};
int tagsCount = 0;
String tagID = "";
boolean successRead = false, correctTag = false;
int grantAccess = 0; //0 - cant enter, 1 - can enter, 2 - entering

// WiFi Constants
const char* ssid = "eduroam";
const char* password = "ash192RUI";

unsigned char identity[] = "ist425485@tecnico.ulisboa.pt";



MFRC522 mfrc522(SS_PIN, RST_PIN);

uint8_t getID(){
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  
  tagID = "";
  
  for ( uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  
  return 1;
}

void printNormalModeMessage(){
  delay(1000);
  Serial.println("Scan tag");
}


void setup() {
  // put your setup code here, to run once:
  // pin initialization
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(REED_SWITCH, INPUT);
  pinMode(INPUT_SWITCH, INPUT);
  
  digitalWrite(INPUT_SWITCH, HIGH); // enable pull-ups
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

   // Connect to Wifi
  Serial.begin(115200);
  Serial.println("Connecting to ");
  Serial.println(ssid);

/*  WiFi.mode(WIFI_STA);
  wifi_station_set_username(identity, sizeof(identity));

    WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  */
  
  
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Scanning Master");
  
  while(!successRead){
    successRead = getID();
    if(successRead){
      myTags[tagsCount] = strdup(tagID.c_str());
      Serial.println("Master set");
      tagsCount++;
    }
  }
  successRead = false;

  printNormalModeMessage();
}

void loop() {

  // check if door is closed
  if(digitalRead(REED_SWITCH) == HIGH){
    //digitalWrite(GREEN_LED, LOW);
    //Serial.println("Your door is Closed");
    if(grantAccess == 2){
      grantAccess = 0;
    }

    digitalWrite(YELLOW_LED, LOW);
  }
  else{
    //digitalWrite(GREEN_LED, HIGH);
    //Serial.println("Your door is Open");
    if(grantAccess == 1) //Authorized person entered
    {
      grantAccess = 2;
      Serial.println("Welcome, authorized person");
    }
    else if(grantAccess == 0) //unauthorized opening
    {
      Serial.print("-BREACH-");      
      // é preciso por os digital reads antes do led 
      if(digitalRead(INPUT_SWITCH)== LOW){
        Serial.print("HALL");
      }
      else{
        Serial.print("SERVER ROOM");
      }
      digitalWrite(YELLOW_LED, HIGH); 


      //TODO put function to post alarm 
    }

    digitalWrite(GREEN_LED, LOW);
    
    return;
  }

  
  //Put if after to verify if door is closed (ReedSwitch)
  {
    if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
      return;
    }
    
    if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
      return;
    }
    
    tagID = "";
    
    // The MIFARE PICCs that we use have 4 byte UID
    for ( uint8_t i = 0; i < 4; i++) {  //
      readCard[i] = mfrc522.uid.uidByte[i];
      tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
    }
    
    tagID.toUpperCase();
    mfrc522.PICC_HaltA(); // Stop reading
    
    correctTag = false;
  
    if(tagID == myTags[0]){
      Serial.println("MasterMode: Add or remove tag");
      while(!successRead){
        successRead = getID();
        if(successRead){
          
          for(int i = 0; i < 100; i++){
            if(tagID == myTags[i]){
              if(tagID == myTags[0]){
                Serial.println("Wrong tag (pass another besides master tag)");
              }
              else{
                myTags[i] = "";
                Serial.println("Tag removed");
                printNormalModeMessage();
              }
              return;
            }
          }
  
          myTags[tagsCount] = strdup(tagID.c_str());
          Serial.println("Tag Added");
          printNormalModeMessage();
          tagsCount++;
          return;
        }
      }
    }
    else{
      //check if authorized tag
      //////Começar no 0 ou 1?
      for(int i = 1; i < 100; i++){
        
        
        if(tagID == myTags[i]){
          Serial.println("Access Granted");
          printNormalModeMessage();
          grantAccess = 1;
          digitalWrite(GREEN_LED, HIGH);
          correctTag = true;
        }
      }
    
      if(correctTag == false && tagID != myTags[0]){
        Serial.println("Access Denied");
        digitalWrite(RED_LED, HIGH);
        delay(200);
        digitalWrite(RED_LED, LOW);
      }
    }
    successRead = false;
  }
  
  //Door Open here
}
/*
void postAlarm(){
  HTTPClient http;  //new object of class httpclient
  String local, postData;

  if(digitalRead(INPUT_SWITCH) == LOW){
    Serial.println("Breach on hall");

    local = "hall";
  }
  else{
    Serial.println("Breach on servers room");

    local = "server";
  }

  postData = "local=" + local;
  
  http.begin("https://http://web.ist.utl.pt/ist425485/post_alarm.php");              //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  int httpCode = http.POST(postData);
  String payload = http.getString();

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
 
  http.end();  //Close connection
  
}

void postRecord(String id){

  HTTPClient http;  //new object of class httpclient
  String local, postData;

  if(digitalRead(INPUT_SWITCH) == LOW){
    Serial.println("Breach on hall");

    local = "hall";
  }
  else{
    Serial.println("Breach on servers room");

    local = "server";
  }

  postData = "id=" + id + "&local=" + local;
  
  http.begin("https://http://web.ist.utl.pt/ist425485/post_record.php");              //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  int httpCode = http.POST(postData);
  String payload = http.getString();

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
 
  http.end();  //Close connection
  
  
}

bool getPermissions(String id){
  HTTPClient http;
  String getData, link;

  getData = "?id=" + id;
  link = "https://http://web.ist.utl.pt/ist425485/get_permissions.php" + getData;

  http.begin(link);
  
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload
 
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  
  http.end();  //Close connection
  
  if(digitalRead(INPUT_SWITCH) == LOW && (payload == "1" || payload == "2")){
    return true;
  }
  else if(digitalRead(INPUT_SWITCH) == HIGH && (payload == "2")){
    return true;
  }
  else{
    return false;
  }
 
  
}*/
