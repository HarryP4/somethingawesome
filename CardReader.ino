/*
 * --------------------------------------------------------------------------------------------------------------------
 * Modified example sketch/program showing how to read new NUID from a PICC to serial - Harry Poulos (z5257055)
 * --------------------------------------------------------------------------------------------------------------------
 * This is based off a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * program that reads data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * 
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Stepper.h>
const int stepsPerRevolution = 2048;
// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 2, 4, 3, 5);


#define SS_PIN 10
#define RST_PIN 9
#define BUZZER_PIN 7
#define LOCK_PIN 6
//#define MOTOR_PIN 3
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte accessCards[8] = {0x69, 0xC1, 0x1B, 0xC9, 0x04, 0xF6, 0xB5, 0x2B};

void setup() { 
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  //pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LOCK_PIN, INPUT);

  myStepper.setSpeed(16);
}
 
void loop() {
  

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if ((rfid.uid.uidByte[0] == accessCards[0] && 
    rfid.uid.uidByte[1] == accessCards[1] && 
    rfid.uid.uidByte[2] == accessCards[2] && 
    rfid.uid.uidByte[3] == accessCards[3] ) ||
    (rfid.uid.uidByte[0] == accessCards[4] && 
    rfid.uid.uidByte[1] == accessCards[5] && 
    rfid.uid.uidByte[2] == accessCards[6] && 
    rfid.uid.uidByte[3] == accessCards[7] )) 
    {
        Serial.println(F("Access Granted."));
    
        /* Store NUID into accessCards array - Don't want to do this as it overwrites the stored cards allowed.
        for (byte i = 0; i < 4; i++) {
          accessCards[i] = rfid.uid.uidByte[i];
        }
       */
        Serial.println(F("The NUID tag is:"));
        Serial.print(F("In hex: "));
        printHex(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
        Serial.print(F("In dec: "));
        printDec(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
    

      
        // STEPPER MOTOR:
        myStepper.step(stepsPerRevolution*15);
        delay(5000);

        

        while(digitalRead(LOCK_PIN) == LOW) {
          delay(1);
        }
  
        myStepper.step(-stepsPerRevolution*15);
        delay(5000);
      
  }
  else 
  {
    //buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println(F("Access denied."));
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
