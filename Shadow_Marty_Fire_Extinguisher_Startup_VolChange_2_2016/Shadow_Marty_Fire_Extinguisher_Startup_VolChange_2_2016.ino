// =======================================================================================
// //////////////////SHADOW :  Small Handheld Arduino Droid Operating Wand////////////////
// =======================================================================================
//                          Last Revised Date: 08/26/14
//                             Written By: KnightShade
//                     Heavily Influenced by PADAWAN by danf
//
//   Note: You will need a Arduino Mega 1280/2560 to run this sketch,
//   as a normal Arduino (Uno, Duemilanove etc.) doesn't have enough SRAM and FLASH
//
//   This is written to be a UNIVERSAL Sketch - supporting multiple controllers
//      - Single PS3 Move Navigation
//      - Pair of PS3 Move Navigation
//      - Android Phone (Limited Controls)
//      Future Planned Enhancements:
//      - XBox 360 Controller  (Why not, also uses the USB Host Shield)
//      - PS3 Dual Shock Controller
//      - PS4 Dual Shock Controller
//
//   PS3 Bluetooth library - developed by Kristian Lauszus (kristianl@tkjelectronics.com)
//   For more information visit my blog: http://blog.tkjelectronics.dk/ or
//
//
//    Tabled:  I had difficulties with making this a universal skets to *concurrently* support both PS2 and PS3 controllers.
//          I may experiment again in the future:
//      - Wireless PS2 Controller
//
//   Designed to be used with a second Arduino running the Padawan Dome code
//              EasyTransfer and PS2X_lib libraries by Bill Porter
//
//
//         Set Sabertooth 2x25 Dip Switches 1 and 2 Down, All Others Up
//         For SyRen packetized Serial Set Switches 1 and 2 Down, All Others Up
//         NOTE:  Support for SyRen Simple Serial has been removed, due to problems.
//         Please contact DimensionEngineering to get an RMA to flash your firmware
//         Some place a 10K ohm resistor between S1 & GND on the SyRen 10 itself
//
//         This program is free software: you can redistribute it and/or modify it .
//         This program is distributed in the hope that it will be useful,
//         but WITHOUT ANY WARRANTY; without even the implied warranty of
//         MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//************************** Set speed and turn here************************************//
String PS3MoveNavigatonPrimaryMAC = "00:06:F7:8F:36:7B"; //If using multiple controlers, designate a primary

byte drivespeed1 = 70;   //set these 3 to whatever speeds work for you. 0-stop, 127-full speed.
byte drivespeed2 = 127;  //Recommend beginner: 50 to 75, experienced: 100 to 127, I like 100.
byte drivespeed3 = 0;    //Set to 0 if you only want 2 speeds.

byte turnspeed = 75; //50;     // the higher this number the faster it will spin in place, lower - easier to control.
                         // Recommend beginner: 40 to 50, experienced: 50 $ up, I like 75

byte domespeed1 = 60;    // If using a speed controller for the dome (X & O Buttons), sets the top speed
                         // Use a number up to 127 for serial
                         
byte domespeed2 = 90;    // If using a speed controller for the dome (joystick), sets the top speed
                         // Use a number up to 127 for serial                         

byte ramping = 6; //3;        // Ramping- the lower this number the longer R2 will take to speedup or slow down,
                         // change this by increments of 1

byte joystickFootDeadZoneRange = 15;  // For controllers that centering problems, use the lowest number with no drift
byte joystickDomeDeadZoneRange = 10;  // For controllers that centering problems, use the lowest number with no drift
byte driveDeadBandRange = 10;     // Used to set the Sabertooth DeadZone for foot motors

int motorControllerBaudRate = 9600; // Set the baud rate for the Syren motor controller
                                    // for packetized options are: 2400, 9600, 19200 and 38400

/*
//DMB:  I've been unable to get the PS2 and PS3 controllers to work on the same arduino.
//      Tabling that functionality until after FanExpo
//      May Explore a PS3 DualShock instead
#define PS2_CLK        13  //BLUE
#define PS2_CMD        11  //ORANGE ----BROWN
#define PS2_SEL        10  //YELLOW ----ORANGE
#define PS2_DAT        12  //BROWN  -----YELLOW
#define PS2_PRESSURES   true
#define PS2_RUMBLE      true
*/

#define DOME_PIN          8
#define SYREN_PIN         5 //A0 //5
#define SABERTOOTH_PIN    6 //A1 //6

#define SYREN_ADDR         129
#define SABERTOOTH_ADDR    128

#define UAT 9
#define UAB 11
#define FireExtinguisherPin 12

#define numberOfLEDs 3

//Uncomment one line based on your sound system
#define SOUND_MP3TRIGGER
//#define SOUND_CFSOUNDIII
//#define SOUND_ROGUE_RMP3   //Support coming soon
//#define SOUND_RASBERRYPI   //Support coming soon

//#define TEST_CONROLLER   //Support coming soon
#define SHADOW_DEBUG       //uncomment this for console DEBUG output
#define SHADOW_VERBOSE     //uncomment this for console VERBOSE output




///////include libs and declare variables////////////////////////////
#include <PS3BT.h>
#include <SPP.h>
#include <usbhub.h>
// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <Sabertooth.h>
#include <Servo.h>
#include <EasyTransfer.h>
#include <LedControl.h>
//I left this library, as I *may* want to add support for both controllers
//#include <PS2X_lib.h>  //for v1.8

//This is the traditional sound controler that has been used with PADAWAN
#ifdef SOUND_MP3TRIGGER
#include <MP3Trigger.h>
MP3Trigger trigger;
#endif

//Custom written Libraryy for the old CFSoundIII to emulate 12 button remote
//CFSoundIII needs a supporting CFSOUND.BAS version running on the CFSoundIII 
#ifdef SOUND_CFSOUNDIII
#include <CFSoundIII.h>
CFSoundIII cfSound;
#endif

//#ifdef  SOUND_ROGUE_RMP3
//TODO:add rMP3 support
//#endif
//#ifdef  SOUND_RASBERRYPI
//TODO:add Raspberry Pi Sound support
//#endif

Servo UArmTopServo;  // create servo object to control a servo 
Servo UArmBottomServo;  // create servo object to control a servo
Servo FireExtinguisherServo; // create servo object to control a servo

int utilArmClosedPos = 0;    // variable to store the servo closed position 
int utilArmOpenPos = 140;    // variable to store the servo Opened position 

int fireExtinguisherClosedPos = 100;    // variable to store the servo closed position 
int fireExtinguisherOpenPos = 75; // variable to store the servo Opened position

// Check value, open = true, closed = false
boolean UArmTopCheck = false;    
boolean UArmBottomCheck = false;

int UArmBottomPos = 0;
int UArmTopPos = 0;

long nextFlash[numberOfLEDs]; // Array indicating which LED to flash next.
int ledPin[] = { 2, 3, 4 }; // LED pins to use.
int ledState[numberOfLEDs]; // Array indicating the state of the LED's.

long previousDomeMillis = millis();
long previousFootMillis = millis();
long currentMillis = millis();
int serialLatency = 25;   //This is a delay factor in ms to prevent queueing of the Serial data.
                          //25ms seems approprate for HardwareSerial, values of 50ms or larger are needed for Softare Emulation

//DMB:Sabertooth ST(SABERTOOTH_ADDR, STSerial);
//DMB:Sabertooth SyR(SYREN_ADDR, SyRSerial);
Sabertooth *ST=new Sabertooth(SABERTOOTH_ADDR, Serial2);
Sabertooth *SyR=new Sabertooth(SYREN_ADDR, Serial2);


byte drivespeed = drivespeed1;


EasyTransfer ET;//create object
struct SEND_DATA_STRUCTURE
{
    //put your variable definitions here for the data you want to send
    //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
    int hpx; // hp movement
    int hpy; // hp movement
    int hpl; // hp light
    int hpa; // hp automation
    int dsp; // 100=no change, 0=random, 1=alarm, 4=whistle, 5=leia, 6=short circut, 10=EQ, 11=alarm2,
};         // 21=speed1, 22=speed2, 23=speed3, 24=logics+, 25=logics-

SEND_DATA_STRUCTURE domeData;//give a name to the group of data

///////Setup for USB and Bluetooth Devices////////////////////////////
USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
PS3BT *PS3Nav=new PS3BT(&Btd);
PS3BT *PS3Nav2=new PS3BT(&Btd);
//Used for PS3 Fault Detection
uint32_t msgLagTime = 0;
uint32_t lastMsgTime = 0;
uint32_t currentTime = 0;
int badPS3Data = 0;

SPP SerialBT(&Btd,"Astromech:R2D2MM","1524"); // Create a BT Serial device(defaults: "Arduino" and the pin to "0000" if not set)
boolean firstMessage = true;
String output = "";

uint32_t lastLoopTime = 0;


boolean isFootMotorStopped = true;
boolean isDomeMotorStopped = true;

///boolean isPS2ControllerInitialized = false;
boolean isPS3NavigatonInitialized = false;
boolean isSecondaryPS3NavigatonInitialized = false;


/*
//Left these so that I can go back and add PS2 support to for testing
PS2X ps2x; // create PS2 Controller Class
int ps2xError = 0; // part of the ps2x lib
byte ps2xType = 0; // part of the ps2x lib
*/

byte vol = 40; // 0 = full volume, 255 off
boolean isStickEnabled = true;
byte isAutomateDomeOn = false;
unsigned long automateMillis = 0;
byte automateDelay = random(5,15);// set this to min and max seconds between sounds
int turnDirection = 20;
byte action = 0;
unsigned long DriveMillis = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
    if (Usb.Init() == -1)
    {
        Serial.print(F("\r\nOSC did not start"));
        while (1); //halt
    }
    Serial.print(F("\r\nBluetooth Library Started"));
    output.reserve(200); // Reserve 200 bytes for the output string

    //Setup for PS3
    PS3Nav->attachOnInit(onInitPS3); // onInit() is called upon a new connection - you can call the function whatever you like
    PS3Nav2->attachOnInit(onInitPS3Nav2); 

    

    //Setup for Syren / Dome
    Serial2.begin(motorControllerBaudRate);
//DMB:    SyRSerial.begin(domeBaudeRate);

        SyR->autobaud();
        //TODO:  DMB:  How low can we go for safety reasons?  multiples of 100ms
        SyR->setTimeout(300);

    //Setup for Sabertooth / Foot Motors
//DMB:    STSerial.begin(9600);   // 9600 is the default baud rate for Sabertooth packet serial.
    ST->autobaud();          // Send the autobaud command to the Sabertooth controller(s).
    // NOTE: *Not all* Sabertooth controllers need this command.
    //       It doesn't hurt anything, but V2 controllers use an
    //       EEPROM setting (changeable with the function setBaudRate) to set
    //       the baud rate instead of detecting with autobaud.
    //
    //       If you have a 2x12, 2x25 V2, 2x60 or SyRen 50, you can remove
    //       the autobaud line and save yourself two seconds of startup delay.

    //TODO:  DMB:  How low can we go for safety reasons?
    ST->setTimeout(300);
    ST->setDeadband(driveDeadBandRange);
    ST->stop(); 


    //The Arduino Mega has three additional serial ports: 
    // - Serial1 on pins 19 (RX) and 18 (TX), 
    // - Serial2 on pins 17 (RX) and 16 (TX), 
    // - Serial3 on pins 15 (RX) and 14 (TX). 

    //Setup for Sound
    #ifdef SOUND_CFSOUNDIII
      cfSound.setup(&Serial1,2400);    
    #endif
    #ifdef SOUND_MP3TRIGGER
      trigger.setup(&Serial1);
      trigger.setVolume(vol);
    #endif
    
    UArmTopServo.attach(UAT);  
    UArmBottomServo.attach(UAB);

    UArmTopClose();
    UArmBottomClose();
    
    FireExtinguisherServo.attach(FireExtinguisherPin);
    FireExtinguisherServo.write(fireExtinguisherClosedPos);
    
    for(int i = 0; i<numberOfLEDs; i++){
      pinMode(ledPin[i],OUTPUT);
      ledState[i] = LOW;
      digitalWrite(ledPin[i], LOW); // all LEDs off
      nextFlash[i] = millis() +random(100, 1000);
    }     

    //Setup for Dome Data Transfer
    Serial3.begin(57600);//start the library, pass in the data details and the name of the serial port.
    ET.begin(details(domeData), &Serial3);

    trigger.play(56);
}

boolean readUSB()
{
    //The more devices we have connected to the USB or BlueTooth, the more often Usb.Task need to be called to eliminate latency.
    Usb.Task();
    if (PS3Nav->PS3NavigationConnected ) Usb.Task();
    if (PS3Nav2->PS3NavigationConnected ) Usb.Task();
    if (criticalFaultDetect())
    {
      //We have a fault condition that we want to ensure that we do NOT process any controller data!!!
      return false;
    }
    
    if (criticalFaultDetectNav2())
    { 
      //We have a fault condition that we want to ensure that we do NOT process any controller data!!!
      return false;
    }
    return true;
}

void loop()
{
    //The more devices we have connected to the USB or BlueTooth, the more often Usb.Task need to be called to eliminate latency.
    initAndroidTerminal();
    
    //Useful to enable with serial console when having controller issues.
    #ifdef TEST_CONROLLER
      testPS3Controller();
    #endif

    if ( !readUSB() )
    {
      //We have a fault condition that we want to ensure that we do NOT process any controller data!!!
      return;
    }
    
    //LOOP through functions from highest to lowest priority.
    footMotorDrive();

    if ( !readUSB() )
    {
      //We have a fault condition that we want to ensure that we do NOT process any controller data!!!
      return;
    }

    automateDome();
    domeDrive();

    utilityArms();
    toggleSettings();
    soundControl();
    //ledFlash();
    flushAndroidTerminal();
}


void onInitPS3()
{
    String btAddress = getLastConnectedBtMAC();
    PS3Nav->setLedOn(LED1);
    isPS3NavigatonInitialized = true;
    badPS3Data = 0;
    #ifdef SHADOW_DEBUG
      output += "\r\nBT Address of Last connected Device when Primary PS3 Connected: ";
      output += btAddress;
      if (btAddress == PS3MoveNavigatonPrimaryMAC)
      {
          output += "\r\nWe have our primary controller connected.\r\n";
      }
      else
      {
          output += "\r\nWe have a controller connected, but it is not designated as \"primary\".\r\n";
      }
    #endif
}

void onInitPS3Nav2()
{
    String btAddress = getLastConnectedBtMAC();
    PS3Nav2->setLedOn(LED1);
    isSecondaryPS3NavigatonInitialized = true;
    badPS3Data = 0;
    if (btAddress == PS3MoveNavigatonPrimaryMAC) swapPS3NavControllers();
    #ifdef SHADOW_DEBUG
      output += "\r\nBT Address of Last connected Device when Secondary PS3 Connected: ";
      output += btAddress;
      if (btAddress == PS3MoveNavigatonPrimaryMAC)
      {
          output += "\r\nWe have our primary controller connecting out of order.  Swapping locations\r\n";
      }
      else
      {
          output += "\r\nWe have a secondary controller connected.\r\n";
      }
    #endif
}

String getLastConnectedBtMAC()
{
    String btAddress = "";
    for(int8_t i = 5; i > 0; i--)
    {
        if (Btd.disc_bdaddr[i]<0x10)
        {
            btAddress +="0";
        }
        btAddress += String(Btd.disc_bdaddr[i], HEX);
        btAddress +=(":");
    }
    btAddress += String(Btd.disc_bdaddr[0], HEX);
    btAddress.toUpperCase();
    return btAddress; 
}

void swapPS3NavControllers()
{
    PS3BT* temp = PS3Nav;
    PS3Nav = PS3Nav2;
    PS3Nav2 = temp;
    //Correct the status for Initialization
    boolean tempStatus = isPS3NavigatonInitialized;
    isPS3NavigatonInitialized = isSecondaryPS3NavigatonInitialized;
    isSecondaryPS3NavigatonInitialized = tempStatus;
    //Must relink the correct onInit calls
    PS3Nav->attachOnInit(onInitPS3);
    PS3Nav2->attachOnInit(onInitPS3Nav2); 
}


void initAndroidTerminal()
{
    //Setup for Bluetooth Serial Monitoring
    if (SerialBT.connected)
    {
        if (firstMessage)
        {
            firstMessage = false;
            SerialBT.println(F("Hello from S.H.A.D.O.W.")); // Send welcome message
        }
        //TODO:  Process input from the SerialBT
        //if (SerialBT.available())
        //    Serial.write(SerialBT.read());
    }
    else
    {
        firstMessage = true;
    }
}

void flushAndroidTerminal()
{
    if (output != "")
    {
        if (Serial) Serial.println(output);
        if (SerialBT.connected)
            SerialBT.println(output);
            SerialBT.send();
        output = ""; // Reset output string
    }
}


void automateDome()
{
  /////////////automate
    if (isAutomateDomeOn)
    {
        //TODO:  We have other conditions where the dome is moving
        //if (PS3Nav->getAnalogHat(LeftHatX)!=128)
        if ( abs(PS3Nav->getAnalogHat(LeftHatY)-128) > joystickFootDeadZoneRange)
        {
            automateMillis = millis();
        }
        currentMillis = millis();
        if (currentMillis - automateMillis > (automateDelay*1000))
        {
            automateMillis = millis();
            action = random(1,5);
            if (action>1)
            {
              #ifdef SOUND_MP3TRIGGER
              (trigger.play(random(32,52)));
              #endif
            }
            if (action<4)
            {
                  Serial.println("automation of Dome");			
                  rotateDome(turnDirection,"Automation");
                  delay(500);
                  rotateDome(0,"Automation");
                if (turnDirection>0)
                {
                    turnDirection = -45;
                }
                else
                {
                    turnDirection = 45;
                }
            }
            automateDelay = random(5,15);// set this to min and max seconds between sounds
        }
    }
}



// =======================================================================================
// //////////////////////////Process PS3 Controller Fault Detection///////////////////////
// =======================================================================================
boolean criticalFaultDetect()
{
    /*
    if(ps2xError != 0) isPS2ControllerInitialized = false;
    if(isPS2ControllerInitialized && ps2x.Analog(PSS_RX) ==255 && ps2x.Analog(PSS_RY) ==255 && ps2x.Analog(PSS_LX)==255 &&ps2x.Analog(PSS_LY)==255)
    {
        ST->stop();
        SyR->stop();
        isPS2ControllerInitialized = false;    
    }
    */

    if (PS3Nav->PS3NavigationConnected || PS3Nav->PS3Connected)
    {
        lastMsgTime = PS3Nav->getLastMessageTime();
        currentTime = millis();
        if ( currentTime >= lastMsgTime)
        {
          msgLagTime = currentTime - lastMsgTime;
        } else
        {
             #ifdef SHADOW_DEBUG
               output += "Waiting for PS3Nav Controller Data\r\n";
             #endif
             badPS3Data++;
             msgLagTime = 0;
        }
        
        if (msgLagTime > 100 && !isFootMotorStopped)
        {
            #ifdef SHADOW_DEBUG
              output += "It has been 100ms since we heard from the PS3 Controller\r\n";
              output += "Shut downing motors, and watching for a new PS3 message\r\n";
            #endif
            ST->stop();
            //rotateDome(turnDirection, "Fault: Lag");
            isFootMotorStopped = true;
            return true;
        }
        if ( msgLagTime > 30000 )
        {
            #ifdef SHADOW_DEBUG
              output += "It has been 30s since we heard from the PS3 Controller\r\n";
              output += "msgLagTime:";
              output += msgLagTime;
              output += "  lastMsgTime:";
              output += lastMsgTime;
              output += "  millis:";
              output += millis();            
              output += "\r\nDisconnecting the controller.\r\n";
            #endif
            PS3Nav->disconnect();
        }

        //Check PS3 Signal Data
        if(!PS3Nav->getStatus(Plugged) && !PS3Nav->getStatus(Unplugged))
        {
            // We don't have good data from the controller.
            //Wait 10ms, Update USB, and try again
            delay(10);
            Usb.Task();
            if(!PS3Nav->getStatus(Plugged) && !PS3Nav->getStatus(Unplugged))
            {
                badPS3Data++;
                #ifdef SHADOW_DEBUG
                    output += "\r\nInvalid data from PS3 Controller.";
                #endif
                return true;
            }
        }
        else if (badPS3Data > 0)
        {
            //output += "\r\nPS3 Controller  - Recovered from noisy connection after: ";
            //output += badPS3Data;
            badPS3Data = 0;
        }
        if ( badPS3Data > 10 )
        {
            #ifdef SHADOW_DEBUG
                output += "Too much bad data coming fromo the PS3 Controller\r\n";
                output += "Disconnecting the controller.\r\n";
            #endif
            PS3Nav->disconnect();
        }
    }
    else if (!isFootMotorStopped)
    {
        #ifdef SHADOW_DEBUG      
            output += "No Connected Controllers were found\r\n";
            output += "Shuting downing motors, and watching for a new PS3 message\r\n";
        #endif
        ST->stop();
        //rotateDome(turnDirection,"Fault:  No controllers");
        isFootMotorStopped = true;
        return true;
    }
    return false;
}


// =======================================================================================
// //////////////////////////END of PS3 Controller Fault Detection///////////////////////
// =======================================================================================

// =======================================================================================
// //////////////////////////Process of PS3 Secondary Controller Fault Detection//////////
// =======================================================================================
boolean criticalFaultDetectNav2()
{
  if (PS3Nav2->PS3NavigationConnected || PS3Nav2->PS3Connected)
  {
    lastMsgTime = PS3Nav2->getLastMessageTime();
    currentTime = millis();
    
    if ( currentTime >= lastMsgTime)
    {
      msgLagTime = currentTime - lastMsgTime;
    } 
    else
    {
      #ifdef SHADOW_DEBUG
        output += "Waiting for PS3Nav Secondary Controller Data\r\n";
      #endif
      badPS3Data++;
      msgLagTime = 0;
    }
    
    if ( msgLagTime > 10000 )
    {
      #ifdef SHADOW_DEBUG
        output += "It has been 10s since we heard from the PS3 secondary Controller\r\n";
        output += "msgLagTime:";
        output += msgLagTime;
        output += " lastMsgTime:";
        output += lastMsgTime;
        output += " millis:";
        output += millis(); 
        output += "\r\nDisconnecting the secondary controller.\r\n";
      #endif
      SyR->stop();
      PS3Nav2->disconnect();
      return true;
    }
    
    //Check PS3 Signal Data
    if(!PS3Nav2->getStatus(Plugged) && !PS3Nav2->getStatus(Unplugged))
    {
      // We don't have good data from the controller.
      //Wait 15ms, Update USB, and try again
      delay(15);
      Usb.Task();
      if(!PS3Nav2->getStatus(Plugged) && !PS3Nav2->getStatus(Unplugged))
      {
        badPS3Data++;
        #ifdef SHADOW_DEBUG
          output += "\r\nInvalid data from PS3 Secondary Controller.";
        #endif
        return true;
      }
    }
    else if (badPS3Data > 0)
    {
      badPS3Data = 0;
    }
  
    if ( badPS3Data > 10 )
    {
      #ifdef SHADOW_DEBUG
        output += "Too much bad data coming from the PS3 Secondary Controller\r\n";
        output += "Disconnecting the controller.\r\n";
      #endif
      SyR->stop();
      PS3Nav2->disconnect();
      return true;
    }
  }
  
  return false;
}
// =======================================================================================
// //////////////////////////END of PS3 Secondary Controller Fault Detection//////////////
// =======================================================================================

boolean ps3FootMotorDrive(PS3BT* myPS3 = PS3Nav)
{
  int footDriveSpeed = 0;
  int stickSpeed = 0;
  int turnnum = 0;

  if (isPS3NavigatonInitialized)
  {
      // Additional fault control.  Do NOT send additional commands to Sabertooth if no controllers have initialized.
      if (!isStickEnabled)
      {
            #ifdef SHADOW_VERBOSE
              if ( abs(myPS3->getAnalogHat(LeftHatY)-128) > joystickFootDeadZoneRange)
              {
                output += "Drive Stick is disabled\r\n";
              }
            #endif
          ST->stop();
          isFootMotorStopped = true;
      } else if (!myPS3->PS3NavigationConnected)
      {
          ST->stop();
          isFootMotorStopped = true;
      } else if ( myPS3->getButtonPress(L1) )
      {
          //TODO:  Does this need to change this when we support dual controller, or covered by improved isStickEnabled
          ST->stop();
          isFootMotorStopped = true;
      } else
      {
          int joystickPosition = myPS3->getAnalogHat(LeftHatY);
          isFootMotorStopped = false;
          if (myPS3->getButtonPress(L2))
          {
            int throttle = 0;
            if (joystickPosition < 127)
            {
                throttle = joystickPosition - myPS3->getAnalogButton(L2);
            } else
            {
                throttle = joystickPosition + myPS3->getAnalogButton(L2);
            }
            stickSpeed = (map(throttle, -255, 510, -drivespeed2, drivespeed2));                
          } else 
          {
            stickSpeed = (map(joystickPosition, 0, 255, -drivespeed1, drivespeed1));
          }          

          if ( abs(joystickPosition-128) < joystickFootDeadZoneRange)
          {
              footDriveSpeed = 0;
          } else if (footDriveSpeed < stickSpeed)
          {
              if (stickSpeed-footDriveSpeed<(ramping+1))
                  footDriveSpeed+=ramping;
              else
                  footDriveSpeed = stickSpeed;
          }
          else if (footDriveSpeed > stickSpeed)
          {
              if (footDriveSpeed-stickSpeed<(ramping+1))
                  footDriveSpeed-=ramping;
              else
                  footDriveSpeed = stickSpeed;  
          }
          
          turnnum = (myPS3->getAnalogHat(LeftHatX));

          //TODO:  Is there a better algorithm here?  
          if ( abs(footDriveSpeed) > 50)
              turnnum = (map(myPS3->getAnalogHat(LeftHatX), 54, 200, -(turnspeed/4), (turnspeed/4)));
          else if (turnnum <= 200 && turnnum >= 54)
              turnnum = (map(myPS3->getAnalogHat(LeftHatX), 54, 200, -(turnspeed/3), (turnspeed/3)));
          else if (turnnum > 200)
              turnnum = (map(myPS3->getAnalogHat(LeftHatX), 201, 255, turnspeed/3, turnspeed));
          else if (turnnum < 54)
              turnnum = (map(myPS3->getAnalogHat(LeftHatX), 0, 53, -turnspeed, -(turnspeed/3)));


          currentMillis = millis();
          if ( (currentMillis - previousFootMillis) > serialLatency  )
          {

          #ifdef SHADOW_VERBOSE      
          if ( footDriveSpeed < -driveDeadBandRange || footDriveSpeed > driveDeadBandRange)
          {
            output += "Driving Droid at footSpeed: ";
            output += footDriveSpeed;
            output += "!  DriveStick is Enabled\r\n";
            output += "Joystick: ";              
            output += myPS3->getAnalogHat(LeftHatX);
            output += "/";              
            output += myPS3->getAnalogHat(LeftHatY);
            output += " turnnum: ";              
            output += turnnum;
            output += "/";              
            output += footDriveSpeed;
            output += " Time of command: ";              
            output += millis();
          }
          #endif

          //TODO:  DMB:  R2 was turning Backwards.  Possible wiring issue in comparison to RC Mode?
          //Fixed by negating turnnum - add a multiplier at the top of code. 
          //ST->turn(turnnum);
          ST->turn(turnnum);
          ST->drive(footDriveSpeed);
          // The Sabertooth won't act on mixed mode packet serial commands until
          // it has received power levels for BOTH throttle and turning, since it
          // mixes the two together to get diff-drive power levels for both motors.
           previousFootMillis = currentMillis;
          return true; //we sent a foot command   
          }
      }
  }
  return false;
}

int ps3DomeDrive(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{
    int domeRotationSpeed = 0;
    if ( (controllerNumber==1 && myPS3->getButtonPress(L1)) || ( controllerNumber==2 && !myPS3->getButtonPress(L1) && !myPS3->getButtonPress(L2) )  )
    {
        int joystickPosition = myPS3->getAnalogHat(LeftHatX);
        domeRotationSpeed = (map(joystickPosition, 0, 255, -domespeed2, domespeed2));
        if ( abs(joystickPosition-128) < joystickDomeDeadZoneRange ) 
          domeRotationSpeed = 0;
    }
    return domeRotationSpeed;
}
 
void rotateDome(int domeRotationSpeed, String mesg)
{
    //Constantly sending commands to the SyRen (Dome) is causing foot motor delay.
    //Lets reduce that chatter by trying 3 things:
    // 1.) Eliminate a constant stream of "don't spin" messages (isDomeMotorStopped flag)
    // 2.) Add a delay between commands sent to the SyRen (previousDomeMillis timer)
    // 3.) Switch to real UART on the MEGA (Likely the *CORE* issue and solution)
    // 4.) Reduce the timout of the SyRen - just better for safety!
    
    currentMillis = millis();
    if ( (!isDomeMotorStopped || domeRotationSpeed != 0) && ((currentMillis - previousDomeMillis) > (2*serialLatency) )  )
    {
      #ifdef SHADOW_VERBOSE      
        output += "DEBUG:  Dome Rotation called by: ";
        output += mesg;
        if (domeRotationSpeed < 0)
        {
          output += "  Spinning Dome Left at speed: "; 
        } else if (domeRotationSpeed > 0)
        {
          output += "  Spinning Dome Right at speed: "; 
        } else
        {
          output += "  Stopping Dome Spin speed: "; 
        }    
        output += domeRotationSpeed; 
        output += "\r\n";
      #endif
      if (domeRotationSpeed != 0)
      {
        isDomeMotorStopped = false;
      } else
      {
        isDomeMotorStopped = true;
      }
      previousDomeMillis = currentMillis;      
      SyR->motor(domeRotationSpeed);
    }
}

void ps3Holoprojector(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{
  ////turn hp light on & off
    if( !(myPS3->getButtonPress(PS)) && myPS3->getButtonClick(L3))
    {
        if(domeData.hpl == 1)
        {
            #ifdef SHADOW_DEBUG      
              output += "Turning Off Holo Light\r\n";
            #endif
            domeData.hpl = 0;    //if hp light is on, turn it off
            domeData.dsp = 100;
            ET.sendData();
        }
        else
        {
            #ifdef SHADOW_DEBUG      
              output += "Turning On Holo Light\r\n";
            #endif
            domeData.hpl = 1;    //turn hp light on
            domeData.dsp = 100;
            ET.sendData();
        }
    }

    if (myPS3->getButtonPress(L1) &&controllerNumber == 2)
    {
            //TODO:  Analog stick - can we granually control holo?
            if(myPS3->getAnalogHat(LeftHatY) < 50)
            {
                output += "Move Holo Up\r\n";
                domeData.hpy = 30;
                domeData.dsp = 100;
                ET.sendData();
            }        
            if(myPS3->getAnalogHat(LeftHatY) > 200)
            {
                output += "Move Holo Down\r\n";
                domeData.hpy = 150;
                domeData.dsp = 100;
                ET.sendData();
            }
            if(myPS3->getAnalogHat(LeftHatX) < 50)
            {
                output += "Move Holo Left\r\n";
                domeData.hpx = 30;
                domeData.dsp = 100;
                ET.sendData();
            }
            if(myPS3->getAnalogHat(LeftHatX) > 200)
            {
                output += "Move Holo Right\r\n";
                domeData.hpx = 150;
                domeData.dsp = 100;
                ET.sendData();
            }
            if (!(myPS3->getAnalogHat(LeftHatY) < 50||myPS3->getAnalogHat(LeftHatY) > 200))
            {
                //output += "Move Holo V-Center\r\n";
                domeData.hpy = 0;
                domeData.dsp = 100;
                ET.sendData();
            }
            if (!(myPS3->getAnalogHat(LeftHatX) < 50||myPS3->getAnalogHat(LeftHatX) > 200))
            {
                //output += "Move Holo H-Center\r\n";
                domeData.hpx = 0;
                domeData.dsp = 100;
                ET.sendData();
            }          
    }
}

void ps3utilityArms(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{
  if (!(myPS3->getButtonPress(L1)||myPS3->getButtonPress(L2)||myPS3->getButtonPress(PS)))
  {
    if(myPS3->getButtonClick(CROSS))
    {
        #ifdef SHADOW_DEBUG
          output += "Openning/Closing top utility arm\r\n";
        #endif
        
        UArmTopOpenClose();
    }
    if(myPS3->getButtonClick(CIRCLE))
    {
        #ifdef SHADOW_DEBUG
          output += "Openning/Closing bottom utility arm\r\n";
        #endif
        
         UArmBottomOpenClose();
    }
  }
}

void utilityArms()
{
  if (PS3Nav->PS3NavigationConnected) ps3utilityArms(PS3Nav,1);
  if (PS3Nav2->PS3NavigationConnected) ps3utilityArms(PS3Nav2,2);
}

void ps3ToggleSettings(PS3BT* myPS3 = PS3Nav)
{
    if(myPS3->getButtonPress(PS)&&myPS3->getButtonClick(L3))
    {
      //Quick Shutdown of PS3 Controller
      output += "\r\nDisconnecting the controller.\r\n";
      myPS3->disconnect();
    }

  
  //// enable / disable right stick & play sound
    if(myPS3->getButtonPress(PS)&&myPS3->getButtonClick(CROSS))
    {
        #ifdef SHADOW_DEBUG
          output += "Disiabling the DriveStick\r\n";
        #endif
        isStickEnabled = false;
//        trigger.play(52);
    }
    if(myPS3->getButtonPress(PS)&&myPS3->getButtonClick(CIRCLE))
    {
        #ifdef SHADOW_DEBUG
          output += "Enabling the DriveStick\r\n";
        #endif
        isStickEnabled = true;
///        trigger.play(53);
    }


////turn hp automation or automate on & off
//    if(myPS3->getButtonPress(L1)&&myPS3->getButtonClick(CIRCLE))
//    {
//        #ifdef SHADOW_DEBUG
//          output += "Enabling the Holo Automation\r\n";
//        #endif
//        //Turn On HP Automation
//        domeData.hpa = 1;
//        domeData.dsp = 100;
//        ET.sendData();
//    }
//    if(myPS3->getButtonPress(L1)&&myPS3->getButtonClick(CROSS))
//    {
//        #ifdef SHADOW_DEBUG
//          output += "Disabling the Holo Automation\r\n";
//        #endif
//        //Turn Off HP Automation
//        domeData.hpa = 0;
//        domeData.dsp = 100;
//        ET.sendData();
//    }

    if(myPS3->getButtonPress(L2)&&myPS3->getButtonClick(CROSS))
    {
        if(isAutomateDomeOn)
        {
          #ifdef SHADOW_DEBUG
            output += "Disabling the Dome Automation\r\n";        
          #endif
          isAutomateDomeOn = false;
          trigger.play(66);
        }
        else
        {
          #ifdef SHADOW_DEBUG
            output += "Enabling the Dome Automation\r\n";
          #endif
          isAutomateDomeOn = true;
          trigger.play(65);
        }
        action = 0;
    }
    if(myPS3->getButtonPress(L2)&&myPS3->getButtonClick(CIRCLE))
    {
      #ifdef SHADOW_DEBUG
        output += "Pulsing Fire Extinguish\r\n";
      #endif
      
      fireExtinguisher();
      
//        #ifdef SHADOW_DEBUG
//          output += "Enabling the Dome Automation\r\n";
//        #endif
//        isAutomateDomeOn = true;
////        trigger.play(52);
    }


    /*
    ////Logic display brightness
        if(ps2x.ButtonPressed(PSB_PAD_UP))
        {
            if(ps2x.Button(PSB_L1))
            {
                domeData.dsp = 24;
                ET.sendData();
            }
        }
        if(ps2x.ButtonPressed(PSB_PAD_DOWN))
        {
            if(ps2x.Button(PSB_L1))
            {
                domeData.dsp = 25;
                ET.sendData();
            }
        }
    */


////Change drivespeed
    /*
        if(ps2x.ButtonPressed(PSB_R3)) //right joystick
        {
            if(drivespeed == drivespeed1)//if in lowest speed
            {
                drivespeed = drivespeed2;    //change to medium speed and play sound 3-tone
                trigger.play(53);
                domeData.dsp = 22;
                ET.sendData();
                domeData.dsp = 0;
            }
            else if(drivespeed == drivespeed2 && (drivespeed3!=0))//if in medium speed
            {
                drivespeed = drivespeed3;    //change to high speed and play sound scream
                trigger.play(1);
                domeData.dsp = 23;
                ET.sendData();
                domeData.dsp = 0;
            }
            else////////////////////////////////////////we must be in high speed
            {
                drivespeed = drivespeed1;    //change to low speed and play sound 2-tone
                trigger.play(52);
                domeData.dsp = 21;
                ET.sendData();
                domeData.dsp = 0;
            }
        }
    */
	
}

void processSoundCommand(char soundCommand)
{
    #ifdef SOUND_CFSOUNDIII
    //cfSound.playfile("happy.wav");  
    //cfSound.setVolume(20);
    switch (soundCommand) 
    {
        case '+':
            #ifdef SHADOW_DEBUG    
              output += "Volume Up\r\n";
            #endif
            cfSound.volumeUp();
        break;
        case '-':    
            #ifdef SHADOW_DEBUG
              output += "Volume Down\r\n";
            #endif
            cfSound.volumeDown();
        break;
        case '9': 
            #ifdef SHADOW_DEBUG   
              output += "Sound Random OFF/Bank\r\n";
            #endif
            cfSound.sendButton('9');
        break;
        case '0':    
            #ifdef SHADOW_DEBUG
              output += "Sound Random ON/Bank\r\n";
            #endif    
            cfSound.sendButton('0');
        break;
        case '1':    
        case '2':    
        case '3':    
        case '4':    
        case '5':    
        case '6':    
        case '7':    
        case '8':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += "\r\n";
            #endif
            cfSound.sendButton(soundCommand);
        break;
        default:
            #ifdef SHADOW_DEBUG
              output += "Invalid Sound Command\r\n";
            #endif
        break;
    }
    #endif

    #ifdef SOUND_MP3TRIGGER
    switch (soundCommand) 
    {
        case '+':
            #ifdef SHADOW_DEBUG    
              output += "Volume Up\r\n";
            #endif
            if (vol>0)
            {
                vol -= 10;
                trigger.setVolume(vol);
            }
        break;
        case '-':   
            #ifdef SHADOW_DEBUG 
              output += "Volume Down\r\n";
            #endif
            if (vol<255)
            {
                vol += 10;
                trigger.setVolume(vol);
            }
        break;
        
        case '1':  
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Random Sentence\r\n";
          #endif
          //Play Random Sentence
          trigger.play(random(32,52)); 
          break;
        case '2':   
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Wolf Whistle.\r\n";
          #endif        
          // Play Wolf Whistle
          trigger.play(4);
          break;
        case '3':    
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Doo Doo\r\n";
          #endif        
          //Play Doo Doo
          trigger.play(3);
          break;
        case '4':    
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Chortle\r\n";
          #endif        
          //Play Chortle
          trigger.play(2);
          break;
        case '5':    
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Random Misc.\r\n";
          #endif        
          // Play Random Misc
          trigger.play(random(17,25));
          break;
        case '6':    
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Scream.\r\n";
          #endif   
          //Play Scream.     
          trigger.play(1);
          break;
        case '7':    
          #ifdef SHADOW_DEBUG    
            output += "Sound Button ";
            output += soundCommand;
            output += " - Play Cantina Song.\r\n";
          #endif        
          //Play Cantina Song
          trigger.play(10);
          break;
        case '8':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Leia.\r\n";
            #endif
            //Play Leia
            trigger.play(5);
        break;
        case '9':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Star Wars Theme.\r\n";
            #endif
            //Play Star Wars Theme
            trigger.play(9);
        break;
        case '0':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Disco Star Wars\r\n";
            #endif
            //Play Gangnam Style
            trigger.play(64);
        break;
        case 'A':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Happy\r\n";
            #endif
            //Play Happy
            trigger.play(57);
        break;
        case 'B':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Uptown Funk\r\n";
            #endif
            //Play Uptown Funk
            trigger.play(55);
        break;
        case 'C':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play Everything is Awesome\r\n";
            #endif
            //Play Everything is Awesome
            trigger.play(62);
        break;
        case 'D':
            #ifdef SHADOW_DEBUG    
              output += "Sound Button ";
              output += soundCommand;
              output += " - Play What Does the Fox Say\r\n";
            #endif
            //Play What Does the Fox Say
            trigger.play(63);
        break;
        default:
            #ifdef SHADOW_DEBUG
              output += "Invalid Sound Command\r\n";
            #endif
            trigger.play(60);
  }
  #endif
}

void ps3soundControl(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{
  if (!(myPS3->getButtonPress(L1)||myPS3->getButtonPress(L2)||myPS3->getButtonPress(PS)))
  {
    if (myPS3->getButtonClick(UP))          processSoundCommand('1');    
    else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('2');    
    else if (myPS3->getButtonClick(DOWN))   processSoundCommand('3');    
    else if (myPS3->getButtonClick(LEFT))   processSoundCommand('4');    
  } 
  else if (myPS3->getButtonPress(L2))
  {
    if (myPS3->getButtonClick(UP))          processSoundCommand('5');    
    else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('6');    
    else if (myPS3->getButtonClick(DOWN))   processSoundCommand('7');    
    else if (myPS3->getButtonClick(LEFT))   processSoundCommand('8');    
  } 
  else if (myPS3->getButtonPress(L1))
  {
    if (myPS3->getButtonClick(UP))          processSoundCommand('9');
    else if (myPS3->getButtonClick(RIGHT))  processSoundCommand('0');     
    else if (myPS3->getButtonClick(DOWN))   processSoundCommand('A');    
    else if (myPS3->getButtonClick(LEFT))   processSoundCommand('B');
    else if (myPS3->getButtonClick(CROSS))     processSoundCommand('+');
    else if (myPS3->getButtonClick(CIRCLE))   processSoundCommand('-');
  }
}



void footMotorDrive()
{
  //Flood control prevention
  if ((millis() - previousFootMillis) < serialLatency) return;  
  if (PS3Nav->PS3NavigationConnected) ps3FootMotorDrive(PS3Nav);
  //TODO:  Drive control must be mutually exclusive - for safety
  //Future: I'm not ready to test that before FanExpo
  //if (PS3Nav2->PS3NavigationConnected) ps3FootMotorDrive(PS3Nav2);
}  

void domeDrive()
{
  //Flood control prevention
  //This is intentionally set to double the rate of the Foot Motor Latency
  if ((millis() - previousDomeMillis) < (2*serialLatency) ) return;  
  

  int domeRotationSpeed = 0;
  int ps3NavControlSpeed = 0;
  int ps3Nav2ControlSpeed = 0;
  if (PS3Nav->PS3NavigationConnected) ps3NavControlSpeed = ps3DomeDrive(PS3Nav,1);
  if (PS3Nav2->PS3NavigationConnected) ps3Nav2ControlSpeed = ps3DomeDrive(PS3Nav2,2);

  //In a two controller system, give dome priority to the secondary controller.
  //Only allow the "Primary" controller dome control if the Secondary is NOT spinnning it
  
  if ( abs(ps3Nav2ControlSpeed) > 0 )
  {
    domeRotationSpeed = ps3Nav2ControlSpeed;
  } else
  {
    domeRotationSpeed = ps3NavControlSpeed; 
  }
  rotateDome(domeRotationSpeed,"Controller Move");
}  

void holoprojector()
{
   if (PS3Nav->PS3NavigationConnected) ps3Holoprojector(PS3Nav,1);
   if (PS3Nav2->PS3NavigationConnected) ps3Holoprojector(PS3Nav2,2);
}  

void toggleSettings()
{
   if (PS3Nav->PS3NavigationConnected) ps3ToggleSettings(PS3Nav);
   if (PS3Nav2->PS3NavigationConnected) ps3ToggleSettings(PS3Nav2);
}  

void soundControl()
{
   if (PS3Nav->PS3NavigationConnected) ps3soundControl(PS3Nav,1);
   if (PS3Nav2->PS3NavigationConnected) ps3soundControl(PS3Nav2,2);

    // Read from cfsound, send to port USB Serial & BT Serial:
    #ifdef SOUND_CFSOUNDIII
    if (Serial1.available()) 
    {
      int inByte = Serial1.read();
      Serial.write(inByte);         
      if (SerialBT.connected)
      {
        SerialBT.write(inByte);
      }
    }
    #endif
}  

void UArmTopOpen(){
  UArmTopServo.write(utilArmOpenPos);
  UArmTopCheck = true;
}

void UArmTopClose(){
  UArmTopServo.write(utilArmClosedPos);
  UArmTopCheck = false;
}

void UArmTopOpenClose(){
  if(UArmTopCheck == false){
    UArmTopOpen();
  } else {
    UArmTopClose();
  }
}

void UArmBottomOpen(){
  UArmBottomServo.write(utilArmOpenPos);
  UArmBottomCheck = true;
}

void UArmBottomClose(){
  UArmBottomServo.write(utilArmClosedPos);
  UArmBottomCheck = false;
}

void UArmBottomOpenClose(){
  if(UArmBottomCheck == false){
    UArmBottomOpen();
  } else {
    UArmBottomClose();
  }
}

void fireExtinguisher()
{
  FireExtinguisherServo.write(fireExtinguisherOpenPos);
  delay(500);
  FireExtinguisherServo.write(fireExtinguisherClosedPos); 
}

void ledFlash()
{
  for(int i = 0; i<numberOfLEDs; i++)
  {
    if(millis() > nextFlash[i])
    {
      if(ledState[i] == LOW) ledState[i] = HIGH; 
      else ledState[i] = LOW;
      digitalWrite(ledPin[i],ledState[i]);
      nextFlash[i] = millis()+random(100, 1000) ; // next toggle random time
    } 
  }
}

#ifdef TEST_CONROLLER
void testPS3Controller(PS3BT* myPS3 = PS3Nav)
{
    if (myPS3->PS3Connected || myPS3->PS3NavigationConnected) {
        if (myPS3->getButtonPress(PS) && (myPS3->getAnalogHat(LeftHatX) > 137 || myPS3->getAnalogHat(LeftHatX) < 117 || myPS3->getAnalogHat(LeftHatY) > 137 || myPS3->getAnalogHat(LeftHatY) < 117 || myPS3->getAnalogHat(RightHatX) > 137 || myPS3->getAnalogHat(RightHatX) < 117 || myPS3->getAnalogHat(RightHatY) > 137 || myPS3->getAnalogHat(RightHatY) < 117)) {     
            output += "LeftHatX: ";
            output += myPS3->getAnalogHat(LeftHatX);
            output += "\tLeftHatY: ";
            output += myPS3->getAnalogHat(LeftHatY);
            if (myPS3->PS3Connected) { // The Navigation controller only have one joystick
                output += "\tRightHatX: ";
                output += myPS3->getAnalogHat(RightHatX);
                output += "\tRightHatY: ";
                output += myPS3->getAnalogHat(RightHatY);
            }
        }
        //Analog button values can be read from almost all buttons
        if (myPS3->getButtonPress(PS) && (myPS3->getAnalogButton(L2) || myPS3->getAnalogButton(R2)))
        {
            if (output != "")
                output += "\r\n";
            output += "z: ";
            output += myPS3->getAnalogButton(L2);
            if (myPS3->PS3Connected) {
                output += "\tR2: ";
-                output += myPS3->getAnalogButton(R2);
            }
        }
        if (myPS3->getButtonClick(L2)) {
            output += " - L2";
            //myPS3->disconnect();
        }
        if (myPS3->getButtonClick(R2)) {
            output += " - R2";
            //myPS3->disconnect();
        }
        if (output != "") {
            Serial.println(output);
            if (SerialBT.connected)
                SerialBT.println(output);
            output = ""; // Reset output string
        }
        if (myPS3->getButtonClick(PS)) {
            output += " - PS";
            //myPS3->disconnect();
        }
        else {
            if (myPS3->getButtonClick(TRIANGLE))
                output += " - Traingle";
            if (myPS3->getButtonClick(CIRCLE))
                output += " - Circle";
            if (myPS3->getButtonClick(CROSS))
                output += " - Cross";
            if (myPS3->getButtonClick(SQUARE))
                output += " - Square";

            if (myPS3->getButtonClick(UP)) {
                output += " - Up";
                if (myPS3->PS3Connected) {
                    myPS3->setLedOff();
                    myPS3->setLedOn(LED4);
                }
            }
            if (myPS3->getButtonClick(RIGHT)) {
                output += " - Right";
                if (myPS3->PS3Connected) {
                    myPS3->setLedOff();
                    myPS3->setLedOn(LED1);
                }
            }
            if (myPS3->getButtonClick(DOWN)) {
                output += " - Down";
                if (myPS3->PS3Connected) {
                    myPS3->setLedOff();
                    myPS3->setLedOn(LED2);
                }
            }
            if (myPS3->getButtonClick(LEFT)) {
                output += " - Left";
                if (myPS3->PS3Connected) {
                    myPS3->setLedOff();
                    myPS3->setLedOn(LED3);
                }
            }

            if (myPS3->getButtonClick(L1))
                output += " - L1";
            if (myPS3->getButtonClick(L3))
                output += " - L3";
            if (myPS3->getButtonClick(R1))
                output += " - R1";
            if (myPS3->getButtonClick(R3))
                output += " - R3";

            if (myPS3->getButtonClick(SELECT)) {
                output += " - Select";
                myPS3->printStatusString();
            }
            if (myPS3->getButtonClick(START)) {
                output += " - Start";
            }
        }
    }          
}
#endif


