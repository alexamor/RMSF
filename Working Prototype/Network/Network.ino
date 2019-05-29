#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10
#define GREEN_LED 4
#define YELLOW_LED 2
#define RED_LED 3
#define REED_SWITCH 6
#define INPUT_SWITCH 7

#define DEBUG 1

byte readCard[4];
String masterCard = "";
int tagsCount = 0;
String tagID = "";
boolean successRead = false, correctTag = false;
int grantAccess = 0; //0 - cant enter, 1 - can enter, 2 - entering
boolean registedAlarm = false;

// WiFi Constants
String ssid = "Sifia";
String password = "ftdn7813";
String host = "web.tecnico.ulisboa.pt";
const int PORT = 80; 
const int CH_PD = 5;






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
  //Serial.println(tagID);
  mfrc522.PICC_HaltA(); // Stop reading
  
  return 1;
}

void printNormalModeMessage(){
  delay(1000);
  //Serial.println("Scan tag");
}


void setup() {
  
  //enable connection to Module ESP8266 
  pinMode(CH_PD, OUTPUT);

  digitalWrite(CH_PD, LOW);
  delay(1000);
  digitalWrite(CH_PD, HIGH);
  delay(2500);
  
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

  Serial.begin(115200);

  
  
  SPI.begin();
  mfrc522.PCD_Init();
  //Serial.println("Scanning Master");

  if(DEBUG == 1)
    digitalWrite(GREEN_LED, HIGH);
  while(!successRead){
    successRead = getID();
    if(successRead){
      masterCard = tagID;
    }
  }
  successRead = false;

  printNormalModeMessage();
  if(DEBUG == 1)
    digitalWrite(GREEN_LED, LOW);

  // connect to Access Point
  //Serial.flush();
  /*Serial.print("AT+RST\r\n");
  delay(500);*/
  connect_to_AP();


  
}

void loop() {

  // check if door is closed
  if(digitalRead(REED_SWITCH) == HIGH){
    //digitalWrite(GREEN_LED, LOW);
    //Serial.println("Your door is Closed");
    if(grantAccess == 2){
      grantAccess = 0;
    }
    registedAlarm = false;
    digitalWrite(YELLOW_LED, LOW);
  }
  else{
    //digitalWrite(GREEN_LED, HIGH);
    //Serial.println("Your door is Open");
    if(grantAccess == 1) //Authorized person entered
    {
      grantAccess = 2;
      //Serial.println("Welcome, authorized person");
    }
    else if(grantAccess == 0) //unauthorized opening
    {
      registedAlarm = true,
      digitalWrite(YELLOW_LED, HIGH); 
      
      // POST ALARMS TO SERVER
      if(digitalRead(INPUT_SWITCH)== LOW){
        begin_TCP_connection(); 
        POST("local=hall", "/ist425485/post_alarm.php");
        delay(2500);
        end_TCP_connection();
      }
      else{
        begin_TCP_connection(); 
        POST("local=server", "/ist425485/post_alarm.php");
        delay(2500);
        end_TCP_connection();
      }

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
  
    if(tagID == masterCard){
      if(DEBUG == 1)
        digitalWrite(GREEN_LED, HIGH);
      //Serial.println("MasterMode: Add or remove tag");
      while(!successRead){
        successRead = getID();
        if(successRead){

          begin_TCP_connection();
          POST("id="+tagID, "/ist425485/new_user.php");
          Serial.setTimeout(10000);
          if(Serial.find("added")){
            end_TCP_connection();
          }
          else{
            end_TCP_connection();
          }
          
          /*for(int i = 0; i < 100; i++){
            if(tagID == myTags[i]){
              if(tagID == myTags[0]){
                //Serial.println("Wrong tag (pass another besides master tag)");
              }
              else{
                myTags[i] = "";
                //Serial.println("Tag removed");
                printNormalModeMessage();
              }
              return;
            }
          }
  
          myTags[tagsCount] = strdup(tagID.c_str());
          //Serial.println("Tag Added");
          printNormalModeMessage();
          tagsCount++;*/
          return;
          digitalWrite(GREEN_LED, LOW);
        }
      }
    }
    else{
      //check if authorized tag

      
      String url_ = "/ist425485/get_permissions.php?id="+tagID;
      bool granted = false;
      begin_TCP_connection();

      if(digitalRead(INPUT_SWITCH)==LOW){
        granted = GET(url_, 1);
      }
      else{
        granted = GET(url_,2);
      }
      end_TCP_connection();
      
      if(granted){
        //Serial.println("Access Granted");
        printNormalModeMessage();
        grantAccess = 1;
        digitalWrite(GREEN_LED, HIGH);
        correctTag = true;
        String send_string ="";
        if(digitalRead(INPUT_SWITCH)==LOW){
          send_string = "id="+tagID+"&local=hall";
        }
        else{
          send_string = "id="+tagID+"&local=server";
        }

        begin_TCP_connection();
        POST(send_string, "/ist425485/post_record.php");
        delay(2500);
        end_TCP_connection();
          
      }
      
    
      if(correctTag == false && tagID != masterCard){
        //Serial.println("Access Denied");
        digitalWrite(RED_LED, HIGH);
        delay(200);
        digitalWrite(RED_LED, LOW);
      }
    }
    successRead = false;
  }
  
  //Door Open here
}

void connect_to_AP(){
  String connect_ = String("AT+CWJAP_CUR=") + '\"' + ssid + "\",\"" + password + '\"';
  send_wifi_command(connect_);
  //Serial.setTimeout(10000);
  if(Serial.find("OK")){
    delay(5000);
  }else{
    delay(5000);
    connect_to_AP();
  }
}

void begin_TCP_connection(){
  String tcp_connect = String("AT+CIPSTART=") + String("\"TCP\"") + ",\"" + host + "\"," + PORT; 
  send_wifi_command(tcp_connect);
  if(Serial.find("OK")){
    delay(5000);
  }
  else{
    delay(5000);
    begin_TCP_connection();
  }
  
}

void end_TCP_connection(){
  String end_connect = "AT+CIPCLOSE";
  send_wifi_command(end_connect);
  delay(2500);
}

bool GET(String url, int permissions){
  String full = "GET" + url + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  String get_[3];
  get_[0] = "GET" + url + " HTTP/1.1";
  get_[1] = "Host: " + host;
  get_[2] = "";

  String find_permissions = "<p>" + String(permissions) + "</p>";
  int nr = find_permissions.length();
  char send_array[nr];
  find_permissions.toCharArray(send_array, nr);
  String bytes_to_send = "AT+CIPSEND="+String(full.length());
  send_wifi_command(bytes_to_send);
  for(int i = 0; i < 3; i++){
    send_wifi_command(get_[i]);
  }

  //delay(500);
  Serial.setTimeout(10000);
  while(Serial.available()){
    Serial.println(Serial.readString());
  }
  /*if(Serial.find(send_array)){
    return true;
  }
  else{
    return false;
  }*/
}

void POST(String data, String url){
  Serial.flush();
  String full = "POST "+url+" HTTP/1.1\r\nHost: "+host+"\r\nUser-Agent: ESP8266\r\nContent-Length: "+(String)data.length()+"\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n"+data+"\r\n";
  String post[6];
  post[0] = "POST "+url+" HTTP/1.1";
  post[1] = "Host: "+host;
  post[2] = "User-Agent: ESP8266";
  post[3] = "Content-Length: "+(String)data.length();
  post[4] = "Content-Type: application/x-www-form-urlencoded\r\n";
  post[5] = data + "\r\n";
  String bytes_to_send = "AT+CIPSEND="+(String)full.length();
  send_wifi_command(bytes_to_send);
  for(int i = 0; i < 6; i++){
    send_wifi_command(post[i]);
  }
  //send_without_delay(post[5]);
}

void send_wifi_command(String command){
  Serial.print(command);
  Serial.print("\r\n"); //every wifi command must end with \r\n
  delay(500);
}

void send_without_delay(String command){
  Serial.print(command);
  Serial.print("\r\n"); //every wifi command must end with \r\n
  delay(500);
}
