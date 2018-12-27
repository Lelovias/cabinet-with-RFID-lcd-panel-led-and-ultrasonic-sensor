#include<LiquidCrystal.h>                                                       //include LCD Library
#include <AddicoreRFID.h>                                                       //include RFID Library
#include <SPI.h>                                                                //include Serial Peripheral Interface Library
#include <Servo.h>                                                              //include Servo Library

#define  uchar unsigned char
#define uint  unsigned int
#define RED 3                                                                   //red LED on pin 3
#define GREEN 2                                                                 //green led on pin 2
#define trigPin A5                                                              //trigpin on pin A5
#define echoPin A4                                                              //echopin on pin A4

uchar fifobytes;
uchar fifoValue;

LiquidCrystal lcd(A2,A0,7,6,A3 ,4);                                             //LCD panel pins
Servo servo;


AddicoreRFID myRFID;                                                            // create AddicoreRFID object to control the RFID module

int duration, distance;

const int chipSelectPin = 10;                                                   //define chipSelectPin to pin 10
const int NRSTPD = 5;                                                           //define NRSTPD to pin 5

#define MAX_LEN 16                                                              //Maximum length of the array

void setup() {
   Serial.begin(9600);                                                          // RFID reader SOUT pin connected to Serial RX pin at 9600bps

  // start the SPI library:
  SPI.begin();                                                                  //start SPI
  pinMode(RED, OUTPUT);                                                         //set Red pin as an output
  pinMode(chipSelectPin,OUTPUT);                                                // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin
    digitalWrite(chipSelectPin, LOW);                                           // Activate the RFID reader
  pinMode(NRSTPD,OUTPUT);                                                       // Set digital pin 10 , Not Reset and Power-down
    digitalWrite(NRSTPD, HIGH);                                                 //give pin NRSTPD power

    pinMode(trigPin, OUTPUT);                                                   //set Trigpin as Output
    pinMode(echoPin, INPUT);                                                    //set echopin as Input

    lcd.begin(16,2);                                                            //start LCD panel
    servo.attach(8);                                                            //define pin of Servo
    servo.write(0);                                                             //set servo to 0°
    delay(500);                                                                 //delay 0.5s

    lcd.setCursor(0,0);                                                         //set cursor to (0,0)
    lcd.write("Please put your");                                               //Print Please put your on LCD
    lcd.setCursor(3,1);                                                         //set Cursor to (3,1)
    lcd.write("card here!");                                                    //Print card here! on LCD

  myRFID.AddicoreRFID_Init();
}

void loop()
{
    distance = 0;

    uchar i, tmp, checksum1;
  uchar status;
        uchar str[MAX_LEN];
        uchar RC_size;
        uchar blockAddr;                                                        //Selection operation block address 0 to 63
        String mynum = "";

        str[1] = 0x4400;
  //Find tags, return tag type
  status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str);
  if (status == MI_OK)
  {
          Serial.println("\n \n \n");
          Serial.println("RFID ta³g detected");


  }

  //Anti-collision, return tag serial number 4 bytes
  status = myRFID.AddicoreRFID_Anticoll(str);
  if (status == MI_OK)
  {
          lcd.clear();
          checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
          Serial.print("The tag's number is:\t");
          Serial.print(str[0]);
            Serial.print(" , ");
          Serial.print(str[1]);
            Serial.print(" , ");
          Serial.print(str[2]);
            Serial.print(" , ");
          Serial.println(str[3]);


            if(str[0] == 226 && str[1] == 76 && str[2] == 106 && str[3] == 27)  //Test if Tag Number = the card

            {
                Serial.print("ACCESS GRANTED");                                 //Console Print ACCESS GRANTED

                lcd.setCursor(1,0);                                             //set Cursor to (1,0)
                lcd.print("DRIVE THROUGH");                                     //set Cursor to (1,0)
                lcd.setCursor(0,1);                                             //set Cursor to (0,1)
                lcd.print("--------------->");                                  //Print arrow

                delay(150);                                                     //delay 0.15s

                digitalWrite(GREEN, HIGH);                                      //Turn Green light on
                servo.write(110);                                               //set Servo to 110°




                  while ((distance >= 80 || distance <= 0)) {                   //while distance >= 80


                  digitalWrite(trigPin, HIGH);                                  //trigPin High
                  delayMicroseconds(1000);                                      //delay 1000 microseconds
                  digitalWrite(trigPin, LOW);                                   //trigPin LOW
                  duration = pulseIn(echoPin, HIGH);
                  distance = (duration/2) / 29.1;
                  delay(200);                                                   //delay 0.2s
                  Serial.println("no object detected");                         //serial print no object detected

                }

                delay(1000);
                digitalWrite(GREEN, LOW);                                       //turn green light off
                lcd.clear();                                                    //clear lcd panel
                servo.write(0);
                lcd.setCursor(0,0);                                             //set cursor to (0,0)
                lcd.write("Please put your");                                   //Print Please put your on LCD
                lcd.setCursor(3,1);                                             //set Cursor to (3,1)
                lcd.write("card here!");                                        //Print card here! on LCD

            }
                else{                                                           //If its the wrong card

                  Serial.print("ACCESS DENIED");                                //Console Print ACCESS DENIED

                  lcd.setCursor(0,0);                                           //set Cursor to (0,0)
                  lcd.print("No thoroughfare!");                                //Print No thoroughfare! on LCD
                  lcd.setCursor(0,1);                                           //set Cursor to (0,1)
                  lcd.write("XXXXXXXXXXXXXXXX");                                //Print XXXXXXXXXXXXXXXX on LCD

                  digitalWrite(RED, HIGH);                                      //Turn on red LED
                  delay(1500);                                                  //delay 1,5s
                  digitalWrite(RED, LOW);                                       //Turn off red LED

                  delay(150);                                                   //delay 0.15s

                  lcd.clear();                                                  //clear LCD panel

                  lcd.setCursor(0,0);                                           //set cursor to (0,0)
                  lcd.write("Please put your");                                 //Print Please put your on LCD
                  lcd.setCursor(3,1);                                           //set Cursor to (3,1)
                  lcd.write("card here!");                                      //Print card here! on LCD

              }

  }

        myRFID.AddicoreRFID_Halt();                                             //Command tag into hibernation
}
