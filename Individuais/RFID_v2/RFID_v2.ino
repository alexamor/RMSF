#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10
#define GREEN_LED 8
#define REED_SWITCH 6

byte readCard[4];
char* myTags[100] = {};
int tagsCount = 0;
String tagID = "";
boolean successRead = false, correctTag = false;
int proximitySensor; //Nao temos mas poderá dar jeito
boolean doorOpened; //Para quando acrescentar ao ReedSwitch, por agora desprezar


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
  pinMode(REED_SWITCH, INPUT);
  
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(9600);

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
    digitalWrite(GREEN_LED, LOW);
    Serial.println("Your door is Closed");
  }
  else{
    digitalWrite(GREEN_LED, HIGH);
    Serial.println("Your door is Open");
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
              myTags[i] = "";
              Serial.println("Tag removed");
              printNormalModeMessage();
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
    successRead = false;
  
    //check if authorized tag
    //////Começar no 0 ou 1?
    for(int i = 0; i < 100; i++){
      if(tagID == myTags[i]){
        Serial.println("Access Granted");
        printNormalModeMessage();
        correctTag = true;
      }
    }
  
    if(correctTag == false){
      Serial.println("Access Denied");
    }
  }
  //Door Open here
}
