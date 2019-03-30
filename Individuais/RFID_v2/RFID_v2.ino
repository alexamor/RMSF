#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

byte readCard[4];
char* myTags[100] = {};
int tagsCount = 0;
String tagID = "";
boolean sucessRead = false, correctTag = false;
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
  // put your main code here, to run repeatedly:

  //Put if after to verify if door is closed (ReedSwitch)
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
    Serial.Println("MasterMode: Add or remove tag");  
  }

}
