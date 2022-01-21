#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;


// Radio
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

//**************remote control****************
struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
    int16_t menuDown;  
    int16_t Select;    
    int16_t menuUp;  
    int16_t toggleBottom;  
    int16_t toggleTop; 
    int16_t toggle1;
    int16_t toggle2;
    int16_t mode;  
    int16_t RLR;
    int16_t RFB;
    int16_t RT;
    int16_t LLR;
    int16_t LFB;
    int16_t LT;
};

RECEIVE_DATA_STRUCTURE mydata_remote;

int RLR = 0;
int RFB = 0;
int RFBa = 0;
int RT = 0;
int LLR = 0;
int LFB = 0;
int LT = 0;

unsigned long currentMillis;
long previousMillis = 0;    // set up timers
long interval = 10;        // time constant for timer

int servo1Offset = 1610;    //    FR     higher forwards
int servo2Offset = 1400;    //    FL     lower forwards
int servo3Offset = 1500;    //    MR     higher forwards
int servo4Offset = 1450;    //    ML     lower forwards
int servo5Offset = 1500;    //    BR     higher forwards
int servo6Offset = 1470;    //    BL     lower fowards

int servo1Out;
int servo2Out;
int servo3Out;
int servo4Out;
int servo5Out;
int servo6Out;

int scaler = 1;

void setup() {

    // initialize serial communication
    Serial.begin(115200);
    
    radio.begin();
    radio.openWritingPipe(addresses[0]); // 00002
    radio.openReadingPipe(1, addresses[1]); // 00001
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();

    servo1.attach(32);
    servo2.attach(34);
    servo3.attach(36);
    servo4.attach(38);
    servo5.attach(40);
    servo6.attach(42);
    
    pinMode(5,OUTPUT);
    pinMode(6,OUTPUT);


    servo1.writeMicroseconds(servo1Offset);
    servo2.writeMicroseconds(servo2Offset);
    servo3.writeMicroseconds(servo3Offset);
    servo4.writeMicroseconds(servo4Offset);
    servo5.writeMicroseconds(servo3Offset);
    servo6.writeMicroseconds(servo4Offset);

    
}   // end of setup

// ********************* MAIN LOOP *******************************

void loop() {  

      
        currentMillis = millis();
        if (currentMillis - previousMillis >= 10) {  // start timed event
          
            previousMillis = currentMillis;


            // check for radio data
            if (radio.available()) {
                    radio.read(&mydata_remote, sizeof(RECEIVE_DATA_STRUCTURE));   
            }             

            else {
              Serial.println("no data");
            }

            Serial.println(mydata_remote.toggleTop);

            if (mydata_remote.toggleTop == 1) {
              scaler = 2;
            }

            else { scaler = 1; }

            // threshold remote data
            // some are reversed based on stick wiring in remote
            RFB = (thresholdStick(mydata_remote.RFB))*2;   
            RLR = (thresholdStick(mydata_remote.RLR))*2;
            LFB = (thresholdStick(mydata_remote.LFB))*-scaler;
            LLR = (thresholdStick(mydata_remote.LLR))*scaler; 

            servo1Out = servo1Offset + LLR + LFB;
            servo2Out = servo2Offset + LLR - LFB;
            servo1Out = constrain(servo1Out,500,2800);
            servo2Out = constrain(servo2Out,500,2800);

            servo3Out = servo3Offset + LLR + LFB;
            servo4Out = servo4Offset + LLR - LFB;
            servo3Out = constrain(servo3Out,500,2800);
            servo4Out = constrain(servo4Out,500,2800);

            servo5Out = servo5Offset + LLR + LFB;
            servo6Out = servo6Offset + LLR - LFB;
            servo5Out = constrain(servo5Out,500,2800);
            servo6Out = constrain(servo6Out,500,2800);

            servo1.writeMicroseconds(servo1Out);
            servo2.writeMicroseconds(servo2Out);
            servo3.writeMicroseconds(servo3Out);
            servo4.writeMicroseconds(servo4Out);
            servo5.writeMicroseconds(servo5Out);
            servo6.writeMicroseconds(servo6Out);

            RFB = map(RFB,-900,900,-254,254);
            RFB = constrain(RFB,-255,255);


            if (RFB > 0) {
              analogWrite(5,RFB);
              analogWrite(6,0);
            }

            else if (RFB < 0) {
              RFBa = abs(RFB);
              analogWrite(6,RFBa);
              analogWrite(5,0);
            }

            else {
              analogWrite(6,0);
              analogWrite(5,0);
            }                  
                



      
        }     // end of timed loop         

   
}       // end  of main loop
