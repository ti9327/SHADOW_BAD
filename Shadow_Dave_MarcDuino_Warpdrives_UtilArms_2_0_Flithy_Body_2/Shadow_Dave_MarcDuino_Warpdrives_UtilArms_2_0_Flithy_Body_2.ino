// =======================================================================================
//        SHADOW_MD:  Small Handheld Arduino Droid Operating Wand + MarcDuino
// =======================================================================================
//                          Last Revised Date: 10/05/14
//                             Written By: KnightShade
//                        Inspired by the PADAWAN by danf
// =======================================================================================
// Latest
//         This program is free software: you can redistribute it and/or modify it .
//         This program is distributed in the hope that it will be useful,
//         but WITHOUT ANY WARRANTY; without even the implied warranty of
//         MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// =======================================================================================
//   Note: You will need a Arduino Mega 1280/2560 to run this sketch,
//   as a normal Arduino (Uno, Duemilanove etc.) doesn't have enough SRAM and FLASH
//
//   This is written to be a UNIVERSAL Sketch - supporting multiple controller options
//      - Single PS3 Move Navigation
//      - Pair of PS3 Move Navigation
//      - Android Phone (Limited Controls)
//      Future Planned Enhancements:
//      - XBox 360 Controller  (Why not, these also uses the USB Host Shield)
//      - PS3 Dual Shock Controller
//      - PS4 Dual Shock Controller
//
//   PS3 Bluetooth library - developed by Kristian Lauszus (kristianl@tkjelectronics.com)
//   For more information visit my blog: http://blog.tkjelectronics.dk/ or
//
//   Holoprojector Support:
//      Legacy Holoprojector Support:  was based on Padawan, using a second Arduino (Teeces)
//          This used a Padawan Dome sketch that was loaded to the Teeces Logics.
//          It leveraged the EasyTransfer libraries by Bill Porter
//          Legacy support will likely be deprecated and removed in time
//      Long Term Holoprojector Support:
//          SHADOW control will be isolated from particular logic hardware.
//          We will migrate to I2C dome commands with PWM support:
//          Holoprojector Servos and LEDs will be driven by:
//            http://www.adafruit.com/product/815
//            This can drive 6 servos, and 3 LEDs.  PWM will allow for LED brightness "flicker"
//
//   Sabertooth (Foot Drive):
//         Set Sabertooth 2x32 or 2x25 Dip Switches: 1 and 2 Down, All Others Up
//
//   SyRen 10 Dome Drive:
//         For SyRen packetized Serial Set Switches: 1, 2 and 4 Down, All Others Up
//         NOTE:  Support for SyRen Simple Serial has been removed, due to problems.
//         Please contact DimensionEngineering to get an RMA to flash your firmware
//         Some place a 10K ohm resistor between S1 & GND on the SyRen 10 itself
//
// =======================================================================================
//
// ---------------------------------------------------------------------------------------
//                          User Settings
// ---------------------------------------------------------------------------------------

//Primary Controller
String PS3MoveNavigatonPrimaryMAC = "00:07:04:0A:B4:3F"; //If using multiple controlers, designate a primary


byte drivespeed1 = 75;   //set these 3 to whatever speeds work for you. 0-stop, 127-full speed.
byte drivespeed2 = 75;  //Recommend beginner: 50 to 75, experienced: 100 to 127, I like 100.

byte turnspeed = 50; //50;     // the higher this number the faster it will spin in place, lower - easier to control.
// Recommend beginner: 40 to 50, experienced: 50 $ up, I like 75

byte domespeed = 50;    // If using a speed controller for the dome, sets the top speed
// Use a number up to 127 for serial

byte ramping = 4; //3;   // Ramping - the lower this number the longer R2 will take to speedup or slow down,
// change this by increments of 1

int footDriveSpeed = 0;

byte joystickFootDeadZoneRange = 15;  // For controllers that centering problems, use the lowest number with no drift
byte joystickDomeDeadZoneRange = 10;  // For controllers that centering problems, use the lowest number with no drift

byte driveDeadBandRange = 10;     // Used to set the Sabertooth DeadZone for foot motors

int invertTurnDirection = -1;   //This may need to be set to 1 for some configurations

byte domeAutoSpeed = 60;     // Speed used when dome automation is active (1- 127)
int time360DomeTurn = 1250;  // milliseconds for dome to complete 360 turn at domeAutoSpeed

//#define TEST_CONROLLER   //Support coming soon
#define SHADOW_DEBUG       //uncomment this for console DEBUG output
#define SHADOW_VERBOSE     //uncomment this for console VERBOSE output

#define MD_BODY_CONNECTED

// ---------------------------------------------------------------------------------------
//                          MarcDuino Button Settings
// ---------------------------------------------------------------------------------------
// Std MarcDuino Function Codes:
//     1 = Close All Panels
//     2 = Scream - all panels open
//     3 = Wave, One Panel at a time
//     4 = Fast (smirk) back and forth wave
//     5 = Wave 2, Open progressively all panels, then close one by one
//     6 = Beep cantina - w/ marching ants panel action
//     7 = Faint / Short Circuit
//     8 = Cantina Dance - orchestral, rhythmic panel dance
//     9 = Leia message
//    10 = Disco
//    11 = Quite mode reset (panel close, stop holos, stop sounds)
//    12 = Full Awake mode reset (panel close, rnd sound, holo move,holo lights off)
//    13 = Mid Awake mode reset (panel close, rnd sound, stop holos)
//    14 = Full Awake+ reset (panel close, rnd sound, holo move, holo lights on)
//    15 = Scream, with all panels open (NO SOUND)
//    16 = Panel Wiggle (aka Jazz "Hands")
//    17 = Scream, with all panels open
//    18 = Wave, one panel at a time (NO SOUND)
//    19 = Fast (smirk) back and forth (NO SOUND)
//    20 = Wave 2 (Open progressively, then close one by one) (NO SOUND)
//    21 = Marching Ants (NO SOUND)
//    22 = Faint/Short Circuit (NO SOUND)
//    23 = Rhythmic cantina dance (NO SOUND)
//    24 = Bye Bye Wave
//    25 = Random Holo Movement On (All) - No other actions
//    26 = Holo Lights On (All)
//    27 = Holo Lights Off (All)
//    28 = Holo reset (motion off, lights off)
//    29 = Volume Up
//    30 = Volume Down
//    31 = Volume Max
//    32 = Volume Mid
//    33 = Open All Dome Panels
//    34 = Open Top Dome Panels
//    35 = Open Bottom Dome Panels
//    36 = Close All Dome Panels
//    37 = Open Dome Panel #1
//    38 = Close Dome Panel #1
//    39 = Open Dome Panel #2
//    40 = Close Dome Panel #2
//    41 = Open Dome Panel #3
//    42 = Close Dome Panel #3
//    43 = Open Dome Panel #4
//    44 = Close Dome Panel #4
//    45 = Open Dome Panel #5
//    46 = Close Dome Panel #5
//    47 = Open Dome Panel #6
//    48 = Close Dome Panel #6
//    49 = Open Dome Panel #7
//    50 = Close Dome Panel #7
//    51 = Open Dome Panel #8
//    52 = Close Dome Panel #8
//    53 = Open Dome Panel #9
//    54 = Close Dome Panel #9
//    55 = Open Dome Panel #10
//    56 = Close Dome Panel #10
//   *** MAGIC PANEL LIGHTING COMMANDS
//    57 = Magic Panel ON
//    58 = Magic Panel OFF
//    59 = Magic Panel Flicker (10 seconds) 
//    60 = Scream (bank 6 sound 1)
//    61 = Faint/short circuit (bank 6 sound 3)
//    62 = Leia message (bank 7 sound 1)
//    63 = Short Cantina music (bank 8 sound 1)
//    64 = Star Wars (bank 8 sound 2)
//    65 = Imperial March (bank 8 sound 3)
//    66 = Long Cantina music (bank 8 sound 5)
//    67 = Disco Star Wars music (bank 8 sound 6)
//   *** BODY PANEL OPTIONS ASSUME SECOND MARCDUINO MASTER BOARD ON MEGA ADK SERIAL #3 ***
//    68 = Open All Body Panels
//    69 = Close All Body Panels
//    70 = Open Body Panel #1
//    71 = Close Body Panel #1
//    72 = Open Body Panel #2
//    73 = Close Body Panel #2
//    74 = Open Body Panel #3
//    75 = Close Body Panel #3
//    76 = Open Body Panel #4
//    77 = Close Body Panel #4
//    78 = Open Body Panel #5
//    79 = Close Body Panel #5
//    80 = Open Body Panel #6
//    81 = Close Body Panel #6
//    82 = Open Body Panel #7
//    83 = Close Body Panel #7
//    84 = Open Body Panel #8
//    85 = Close Body Panel #8
//    86 = Open Body Panel #9
//    87 = Close Body Panel #9
//    88 = Open Body Panel #10
//    89 = Close Body Panel #10
//
// Std MarcDuino Logic Display Functions (For custom functions)
//     1 = Display normal random sequence
//     2 = Short circuit (10 second display sequence)
//     3 = Scream (flashing light display sequence)
//     4 = Leia (34 second light sequence)
//     5 = Display “Star Wars”
//     6 = March light sequence
//     7 = Spectrum, bar graph display sequence
//     8 = Display custom text
//
// Std MarcDuino Panel Functions (For custom functions)
//     1 = Panels stay closed (normal position)
//     2 = Scream sequence, all panels open
//     3 = Wave panel sequence
//     4 = Fast (smirk) back and forth panel sequence
//     5 = Wave 2 panel sequence, open progressively all panels, then close one by one)
//     6 = Marching ants panel sequence
//     7 = Faint / short circuit panel sequence
//     8 = Rhythmic cantina panel sequence
//     9 = Custom Panel Sequence


//----------------------------------------------------
// CONFIGURE: The FOOT Navigation Controller Buttons
//----------------------------------------------------

//---------------------------------
// CONFIGURE: Arrow Up
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnUP_MD_func = 11;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnUP_cust_MP3_num = 1;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_use_DP1 = false;
int btnUP_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_use_DP2 = false;
int btnUP_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_use_DP3 = false;
int btnUP_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_use_DP4 = false;
int btnUP_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_use_DP5 = false;
int btnUP_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_use_DP6 = false;
int btnUP_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_use_DP7 = false;
int btnUP_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_use_DP8 = false;
int btnUP_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_use_DP9 = false;
int btnUP_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_use_DP10 = false;
int btnUP_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnLeft_MD_func = 16;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnLeft_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_use_DP1 = false;
int btnLeft_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_use_DP2 = false;
int btnLeft_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_use_DP3 = false;
int btnLeft_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_use_DP4 = false;
int btnLeft_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_use_DP5 = false;
int btnLeft_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_use_DP6 = false;
int btnLeft_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_use_DP7 = false;
int btnLeft_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_use_DP8 = false;
int btnLeft_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_use_DP9 = false;
int btnLeft_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_use_DP10 = false;
int btnLeft_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnRight_MD_func = 13;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnRight_cust_MP3_num = 2;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_use_DP1 = false;
int btnRight_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_use_DP2 = false;
int btnRight_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_use_DP3 = false;
int btnRight_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_use_DP4 = false;
int btnRight_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_use_DP5 = false;
int btnRight_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_use_DP6 = false;
int btnRight_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_use_DP7 = false;
int btnRight_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_use_DP8 = false;
int btnRight_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_use_DP9 = false;
int btnRight_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_use_DP10 = false;
int btnRight_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnDown_MD_func = 7;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0, 1, 2   or 182 - 200
int btnDown_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_use_DP1 = false;
int btnDown_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_use_DP2 = false;
int btnDown_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_use_DP3 = false;
int btnDown_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_use_DP4 = false;
int btnDown_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_use_DP5 = false;
int btnDown_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_use_DP6 = false;
int btnDown_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_use_DP7 = false;
int btnDown_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_use_DP8 = false;
int btnDown_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_use_DP9 = false;
int btnDown_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_use_DP10 = false;
int btnDown_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow UP + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_L2_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnUP_L2_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnUP_L2_cust_MP3_num = 205;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_L2_use_DP1 = false;
int btnUP_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_L2_use_DP2 = false;
int btnUP_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_L2_use_DP3 = false;
int btnUP_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_L2_use_DP4 = false;
int btnUP_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_L2_use_DP5 = false;
int btnUP_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_L2_use_DP6 = false;
int btnUP_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_L2_use_DP7 = false;
int btnUP_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_L2_use_DP8 = false;
int btnUP_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_L2_use_DP9 = false;
int btnUP_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_L2_use_DP10 = false;
int btnUP_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_L2_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnLeft_L2_MD_func = 9;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnLeft_L2_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_L2_use_DP1 = false;
int btnLeft_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_L2_use_DP2 = false;
int btnLeft_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_L2_use_DP3 = false;
int btnLeft_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_L2_use_DP4 = false;
int btnLeft_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_L2_use_DP5 = false;
int btnLeft_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_L2_use_DP6 = false;
int btnLeft_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_L2_use_DP7 = false;
int btnLeft_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_L2_use_DP8 = false;
int btnLeft_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_L2_use_DP9 = false;
int btnLeft_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_L2_use_DP10 = false;
int btnLeft_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_L2_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnRight_L2_MD_func = 8;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnRight_L2_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_L2_use_DP1 = false;
int btnRight_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_L2_use_DP2 = false;
int btnRight_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_L2_use_DP3 = false;
int btnRight_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_L2_use_DP4 = false;
int btnRight_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_L2_use_DP5 = false;
int btnRight_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_L2_use_DP6 = false;
int btnRight_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_L2_use_DP7 = false;
int btnRight_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_L2_use_DP8 = false;
int btnRight_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_L2_use_DP9 = false;
int btnRight_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_L2_use_DP10 = false;
int btnRight_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_L2_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnDown_L2_MD_func = 4;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnDown_L2_cust_MP3_num = 102;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_L2_use_DP1 = false;
int btnDown_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_L2_use_DP2 = false;
int btnDown_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_L2_use_DP3 = false;
int btnDown_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_L2_use_DP4 = false;
int btnDown_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_L2_use_DP5 = false;
int btnDown_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_L2_use_DP6 = false;
int btnDown_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_L2_use_DP7 = false;
int btnDown_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_L2_use_DP8 = false;
int btnDown_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_L2_use_DP9 = false;
int btnDown_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_L2_use_DP10 = false;
int btnDown_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow UP + CIRCLE
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_CIRCLE_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnUP_CIRCLE_MD_func = 2;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnUP_CIRCLE_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_CIRCLE_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_CIRCLE_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_CIRCLE_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_CIRCLE_use_DP1 = false;
int btnUP_CIRCLE_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_CIRCLE_use_DP2 = false;
int btnUP_CIRCLE_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_CIRCLE_use_DP3 = false;
int btnUP_CIRCLE_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_CIRCLE_use_DP4 = false;
int btnUP_CIRCLE_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_CIRCLE_use_DP5 = false;
int btnUP_CIRCLE_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_CIRCLE_use_DP6 = false;
int btnUP_CIRCLE_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_CIRCLE_use_DP7 = false;
int btnUP_CIRCLE_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_CIRCLE_use_DP8 = false;
int btnUP_CIRCLE_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_CIRCLE_use_DP9 = false;
int btnUP_CIRCLE_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_CIRCLE_use_DP10 = false;
int btnUP_CIRCLE_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left + CIRCLE
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_CIRCLE_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnLeft_CIRCLE_MD_func = 4;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnLeft_CIRCLE_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_CIRCLE_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_CIRCLE_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_CIRCLE_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_CIRCLE_use_DP1 = false;
int btnLeft_CIRCLE_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_CIRCLE_use_DP2 = false;
int btnLeft_CIRCLE_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_CIRCLE_use_DP3 = false;
int btnLeft_CIRCLE_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_CIRCLE_use_DP4 = false;
int btnLeft_CIRCLE_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_CIRCLE_use_DP5 = false;
int btnLeft_CIRCLE_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_CIRCLE_use_DP6 = false;
int btnLeft_CIRCLE_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_CIRCLE_use_DP7 = false;
int btnLeft_CIRCLE_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_CIRCLE_use_DP8 = false;
int btnLeft_CIRCLE_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_CIRCLE_use_DP9 = false;
int btnLeft_CIRCLE_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_CIRCLE_use_DP10 = false;
int btnLeft_CIRCLE_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right + CIRCLE
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_CIRCLE_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnRight_CIRCLE_MD_func = 7;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnRight_CIRCLE_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_CIRCLE_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_CIRCLE_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_CIRCLE_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_CIRCLE_use_DP1 = false;
int btnRight_CIRCLE_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_CIRCLE_use_DP2 = false;
int btnRight_CIRCLE_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_CIRCLE_use_DP3 = false;
int btnRight_CIRCLE_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_CIRCLE_use_DP4 = false;
int btnRight_CIRCLE_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_CIRCLE_use_DP5 = false;
int btnRight_CIRCLE_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_CIRCLE_use_DP6 = false;
int btnRight_CIRCLE_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_CIRCLE_use_DP7 = false;
int btnRight_CIRCLE_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_CIRCLE_use_DP8 = false;
int btnRight_CIRCLE_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_CIRCLE_use_DP9 = false;
int btnRight_CIRCLE_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_CIRCLE_use_DP10 = false;
int btnRight_CIRCLE_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down + CIRCLE
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_CIRCLE_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnDown_CIRCLE_MD_func = 10;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnDown_CIRCLE_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_CIRCLE_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_CIRCLE_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_CIRCLE_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_CIRCLE_use_DP1 = false;
int btnDown_CIRCLE_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_CIRCLE_use_DP2 = false;
int btnDown_CIRCLE_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_CIRCLE_use_DP3 = false;
int btnDown_CIRCLE_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_CIRCLE_use_DP4 = false;
int btnDown_CIRCLE_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_CIRCLE_use_DP5 = false;
int btnDown_CIRCLE_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_CIRCLE_use_DP6 = false;
int btnDown_CIRCLE_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_CIRCLE_use_DP7 = false;
int btnDown_CIRCLE_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_CIRCLE_use_DP8 = false;
int btnDown_CIRCLE_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_CIRCLE_use_DP9 = false;
int btnDown_CIRCLE_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_CIRCLE_use_DP10 = false;
int btnDown_CIRCLE_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow UP + PS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_PS_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnUP_PS_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnUP_PS_cust_MP3_num = 183;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_PS_cust_LD_type = 5;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_PS_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_PS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_PS_use_DP1 = false;
int btnUP_PS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_PS_use_DP2 = false;
int btnUP_PS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_PS_use_DP3 = false;
int btnUP_PS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_PS_use_DP4 = false;
int btnUP_PS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_PS_use_DP5 = false;
int btnUP_PS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_PS_use_DP6 = false;
int btnUP_PS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_PS_use_DP7 = false;
int btnUP_PS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_PS_use_DP8 = false;
int btnUP_PS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_PS_use_DP9 = false;
int btnUP_PS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_PS_use_DP10 = false;
int btnUP_PS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left + PS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_PS_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnLeft_PS_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnLeft_PS_cust_MP3_num = 186;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_PS_cust_LD_type = 8;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_PS_cust_LD_text = " GARY R2D2 ";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_PS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_PS_use_DP1 = false;
int btnLeft_PS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_PS_use_DP2 = false;
int btnLeft_PS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_PS_use_DP3 = false;
int btnLeft_PS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_PS_use_DP4 = false;
int btnLeft_PS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_PS_use_DP5 = false;
int btnLeft_PS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_PS_use_DP6 = false;
int btnLeft_PS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_PS_use_DP7 = false;
int btnLeft_PS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_PS_use_DP8 = false;
int btnLeft_PS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_PS_use_DP9 = false;
int btnLeft_PS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_PS_use_DP10 = false;
int btnLeft_PS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right + PS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_PS_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnRight_PS_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnRight_PS_cust_MP3_num = 185;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_PS_cust_LD_type = 8;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_PS_cust_LD_text = "DEATH STAR BITE ME";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_PS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_PS_use_DP1 = false;
int btnRight_PS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_PS_use_DP2 = false;
int btnRight_PS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_PS_use_DP3 = false;
int btnRight_PS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_PS_use_DP4 = false;
int btnRight_PS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_PS_use_DP5 = false;
int btnRight_PS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_PS_use_DP6 = false;
int btnRight_PS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_PS_use_DP7 = false;
int btnRight_PS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_PS_use_DP8 = false;
int btnRight_PS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_PS_use_DP9 = false;
int btnRight_PS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_PS_use_DP10 = false;
int btnRight_PS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down + PS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_PS_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnDown_PS_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnDown_PS_cust_MP3_num = 184;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_PS_cust_LD_type = 6;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_PS_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_PS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_PS_use_DP1 = false;
int btnDown_PS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_PS_use_DP2 = false;
int btnDown_PS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_PS_use_DP3 = false;
int btnDown_PS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_PS_use_DP4 = false;
int btnDown_PS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_PS_use_DP5 = false;
int btnDown_PS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_PS_use_DP6 = false;
int btnDown_PS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_PS_use_DP7 = false;
int btnDown_PS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_PS_use_DP8 = false;
int btnDown_PS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_PS_use_DP9 = false;
int btnDown_PS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_PS_use_DP10 = false;
int btnDown_PS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Up + L1
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_L1_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnUP_L1_MD_func = 10;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnUP_L1_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_L1_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_L1_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_L1_cust_panel = 5;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_L1_use_DP1 = false;
int btnUP_L1_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_L1_use_DP2 = false;
int btnUP_L1_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_L1_use_DP3 = false;
int btnUP_L1_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_L1_use_DP4 = false;
int btnUP_L1_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_L1_use_DP5 = false;
int btnUP_L1_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_L1_use_DP6 = false;
int btnUP_L1_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_L1_use_DP7 = false;
int btnUP_L1_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_L1_use_DP8 = false;
int btnUP_L1_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_L1_use_DP9 = false;
int btnUP_L1_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_L1_use_DP10 = false;
int btnUP_L1_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left + L1
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_L1_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnLeft_L1_MD_func = 3;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnLeft_L1_cust_MP3_num = 190;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_L1_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_L1_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_L1_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_L1_use_DP1 = false;
int btnLeft_L1_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_L1_use_DP2 = false;
int btnLeft_L1_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_L1_use_DP3 = false;
int btnLeft_L1_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_L1_use_DP4 = false;
int btnLeft_L1_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_L1_use_DP5 = false;
int btnLeft_L1_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_L1_use_DP6 = false;
int btnLeft_L1_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_L1_use_DP7 = false;
int btnLeft_L1_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_L1_use_DP8 = false;
int btnLeft_L1_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_L1_use_DP9 = false;
int btnLeft_L1_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_L1_use_DP10 = false;
int btnLeft_L1_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right + L1
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_L1_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnRight_L1_MD_func = 5;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnRight_L1_cust_MP3_num = 186;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_L1_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_L1_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_L1_cust_panel = 8;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_L1_use_DP1 = false;
int btnRight_L1_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_L1_use_DP2 = false;
int btnRight_L1_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_L1_use_DP3 = false;
int btnRight_L1_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_L1_use_DP4 = false;
int btnRight_L1_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_L1_use_DP5 = false;
int btnRight_L1_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_L1_use_DP6 = false;
int btnRight_L1_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_L1_use_DP7 = false;
int btnRight_L1_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_L1_use_DP8 = false;
int btnRight_L1_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_L1_use_DP9 = false;
int btnRight_L1_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_L1_use_DP10 = false;
int btnRight_L1_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down + L1
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_L1_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnDown_L1_MD_func = 9;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnDown_L1_cust_MP3_num = 188;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_L1_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_L1_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_L1_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_L1_use_DP1 = false;
int btnDown_L1_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_L1_use_DP2 = false;
int btnDown_L1_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_L1_use_DP3 = false;
int btnDown_L1_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_L1_use_DP4 = false;
int btnDown_L1_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_L1_use_DP5 = false;
int btnDown_L1_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_L1_use_DP6 = false;
int btnDown_L1_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_L1_use_DP7 = false;
int btnDown_L1_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_L1_use_DP8 = false;
int btnDown_L1_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_L1_use_DP9 = false;
int btnDown_L1_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_L1_use_DP10 = false;
int btnDown_L1_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Cross + L1
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnCross_L1_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnCross_L1_MD_func = 29;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnCross_L1_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnCross_L1_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnCross_L1_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnCross_L1_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnCross_L1_use_DP1 = false;
int btnCross_L1_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnCross_L1_use_DP2 = false;
int btnCross_L1_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnCross_L1_use_DP3 = false;
int btnCross_L1_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnCross_L1_use_DP4 = false;
int btnCross_L1_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnCross_L1_use_DP5 = false;
int btnCross_L1_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnCross_L1_use_DP6 = false;
int btnCross_L1_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnCross_L1_use_DP7 = false;
int btnCross_L1_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnCross_L1_use_DP8 = false;
int btnCross_L1_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnCross_L1_use_DP9 = false;
int btnCross_L1_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnCross_L1_use_DP10 = false;
int btnCross_L1_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Circle + L1
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnCircle_L1_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnCircle_L1_MD_func = 30;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnCircle_L1_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnCircle_L1_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnCircle_L1_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnCircle_L1_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnCircle_L1_use_DP1 = false;
int btnCircle_L1_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnCircle_L1_use_DP2 = false;
int btnCircle_L1_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnCircle_L1_use_DP3 = false;
int btnCircle_L1_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnCircle_L1_use_DP4 = false;
int btnCircle_L1_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnCircle_L1_use_DP5 = false;
int btnCircle_L1_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnCircle_L1_use_DP6 = false;
int btnCircle_L1_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnCircle_L1_use_DP7 = false;
int btnCircle_L1_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnCircle_L1_use_DP8 = false;
int btnCircle_L1_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnCircle_L1_use_DP9 = false;
int btnCircle_L1_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnCircle_L1_use_DP10 = false;
int btnCircle_L1_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Circle + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnCircle_L2_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnCircle_L2_MD_func = 11;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnCircle_L2_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnCircle_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnCircle_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnCircle_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnCircle_L2_use_DP1 = false;
int btnCircle_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnCircle_L2_use_DP2 = false;
int btnCircle_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnCircle_L2_use_DP3 = false;
int btnCircle_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnCircle_L2_use_DP4 = false;
int btnCircle_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnCircle_L2_use_DP5 = false;
int btnCircle_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnCircle_L2_use_DP6 = false;
int btnCircle_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnCircle_L2_use_DP7 = false;
int btnCircle_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnCircle_L2_use_DP8 = false;
int btnCircle_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnCircle_L2_use_DP9 = false;
int btnCircle_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnCircle_L2_use_DP10 = false;
int btnCircle_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Up + L1 + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_L1_L2_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnUP_L1_L2_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnUP_L1_L2_cust_MP3_num = 190;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_L1_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_L1_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_L1_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_L1_L2_use_DP1 = false;
int btnUP_L1_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_L1_L2_use_DP2 = false;
int btnUP_L1_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_L1_L2_use_DP3 = false;
int btnUP_L1_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_L1_L2_use_DP4 = false;
int btnUP_L1_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_L1_L2_use_DP5 = false;
int btnUP_L1_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_L1_L2_use_DP6 = false;
int btnUP_L1_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_L1_L2_use_DP7 = false;
int btnUP_L1_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_L1_L2_use_DP8 = false;
int btnUP_L1_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_L1_L2_use_DP9 = false;
int btnUP_L1_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_L1_L2_use_DP10 = false;
int btnUP_L1_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right + L1 + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_L1_L2_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnRight_L1_L2_MD_func = 8;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnRight_L1_L2_cust_MP3_num = 184;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_L1_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_L1_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_L1_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_L1_L2_use_DP1 = false;
int btnRight_L1_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_L1_L2_use_DP2 = false;
int btnRight_L1_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_L1_L2_use_DP3 = false;
int btnRight_L1_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_L1_L2_use_DP4 = false;
int btnRight_L1_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_L1_L2_use_DP5 = false;
int btnRight_L1_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_L1_L2_use_DP6 = false;
int btnRight_L1_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_L1_L2_use_DP7 = false;
int btnRight_L1_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_L1_L2_use_DP8 = false;
int btnRight_L1_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_L1_L2_use_DP9 = false;
int btnRight_L1_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_L1_L2_use_DP10 = false;
int btnRight_L1_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down + L1 + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_L1_L2_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnDown_L1_L2_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnDown_L1_L2_cust_MP3_num = 204;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_L1_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_L1_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_L1_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_L1_L2_use_DP1 = false;
int btnDown_L1_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_L1_L2_use_DP2 = false;
int btnDown_L1_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_L1_L2_use_DP3 = false;
int btnDown_L1_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_L1_L2_use_DP4 = false;
int btnDown_L1_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_L1_L2_use_DP5 = false;
int btnDown_L1_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_L1_L2_use_DP6 = false;
int btnDown_L1_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_L1_L2_use_DP7 = false;
int btnDown_L1_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_L1_L2_use_DP8 = false;
int btnDown_L1_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_L1_L2_use_DP9 = false;
int btnDown_L1_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_L1_L2_use_DP10 = false;
int btnDown_L1_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left + L1 + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_L1_L2_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnLeft_L1_L2_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnLeft_L1_L2_cust_MP3_num = 6;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_L1_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_L1_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_L1_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_L1_L2_use_DP1 = false;
int btnLeft_L1_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_L1_L2_use_DP2 = false;
int btnLeft_L1_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_L1_L2_use_DP3 = false;
int btnLeft_L1_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_L1_L2_use_DP4 = false;
int btnLeft_L1_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_L1_L2_use_DP5 = false;
int btnLeft_L1_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_L1_L2_use_DP6 = false;
int btnLeft_L1_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_L1_L2_use_DP7 = false;
int btnLeft_L1_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_L1_L2_use_DP8 = false;
int btnLeft_L1_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_L1_L2_use_DP9 = false;
int btnLeft_L1_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_L1_L2_use_DP10 = false;
int btnLeft_L1_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Cross + L1 + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnCross_L1_L2_type = 2;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnCross_L1_L2_MD_func = 8;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnCross_L1_L2_cust_MP3_num = 201;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnCross_L1_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnCross_L1_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnCross_L1_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnCross_L1_L2_use_DP1 = false;
int btnCross_L1_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnCross_L1_L2_use_DP2 = false;
int btnCross_L1_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnCross_L1_L2_use_DP3 = false;
int btnCross_L1_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnCross_L1_L2_use_DP4 = false;
int btnCross_L1_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnCross_L1_L2_use_DP5 = false;
int btnCross_L1_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnCross_L1_L2_use_DP6 = false;
int btnCross_L1_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnCross_L1_L2_use_DP7 = false;
int btnCross_L1_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnCross_L1_L2_use_DP8 = false;
int btnCross_L1_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnCross_L1_L2_use_DP9 = false;
int btnCross_L1_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnCross_L1_L2_use_DP10 = false;
int btnCross_L1_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_L1_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Circle + L1 + L2
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnCircle_L1_L2_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnCircle_L1_L2_MD_func = 30;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnCircle_L1_L2_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnCircle_L1_L2_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnCircle_L1_L2_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnCircle_L1_L2_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnCircle_L1_L2_use_DP1 = false;
int btnCircle_L1_L2_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_L2_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnCircle_L1_L2_use_DP2 = false;
int btnCircle_L1_L2_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_L2_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnCircle_L1_L2_use_DP3 = false;
int btnCircle_L1_L2_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_L2_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnCircle_L1_L2_use_DP4 = false;
int btnCircle_L1_L2_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_L2_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnCircle_L1_L2_use_DP5 = false;
int btnCircle_L1_L2_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_L2_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnCircle_L1_L2_use_DP6 = false;
int btnCircle_L1_L2_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_L2_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnCircle_L1_L2_use_DP7 = false;
int btnCircle_L1_L2_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_L2_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnCircle_L1_L2_use_DP8 = false;
int btnCircle_L1_L2_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_L2_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnCircle_L1_L2_use_DP9 = false;
int btnCircle_L1_L2_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_L2_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnCircle_L1_L2_use_DP10 = false;
int btnCircle_L1_L2_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_L1_L2_DP10_stay_open_time = 5; // in seconds (1 to 30)


//---------------------------------
// CONFIGURE: Cross
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnCross_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnCross_MD_func = 40;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnCross_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnCross_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnCross_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnCross_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnCross_use_DP1 = false;
int btnCross_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnCross_use_DP2 = false;
int btnCross_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnCross_use_DP3 = false;
int btnCross_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnCross_use_DP4 = false;
int btnCross_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnCross_use_DP5 = false;
int btnCross_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnCross_use_DP6 = false;
int btnCross_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnCross_use_DP7 = false;
int btnCross_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnCross_use_DP8 = false;
int btnCross_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnCross_use_DP9 = false;
int btnCross_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnCross_use_DP10 = false;
int btnCross_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnCross_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Circle
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnCircle_type = 1;

// IF Std MarcDuino Function (type=1)
// Enter MarcDuino Function Code (1 - 75) (See Above)
int btnCircle_MD_func = 38;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200
int btnCircle_cust_MP3_num = 0;

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnCircle_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnCircle_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnCircle_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnCircle_use_DP1 = false;
int btnCircle_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnCircle_use_DP2 = false;
int btnCircle_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnCircle_use_DP3 = false;
int btnCircle_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnCircle_use_DP4 = false;
int btnCircle_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnCircle_use_DP5 = false;
int btnCircle_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnCircle_use_DP6 = false;
int btnCircle_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnCircle_use_DP7 = false;
int btnCircle_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnCircle_use_DP8 = false;
int btnCircle_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnCircle_use_DP9 = false;
int btnCircle_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnCircle_use_DP10 = false;
int btnCircle_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnCircle_DP10_stay_open_time = 5; // in seconds (1 to 30)

//----------------------------------------------------
// CONFIGURE: The DOME Navigation Controller Buttons
//----------------------------------------------------

//---------------------------------
// CONFIGURE: Arrow Up
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnUP_MD_func = 6;

//---------------------------------
// CONFIGURE: Arrow Left
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnLeft_MD_func = 56;

//---------------------------------
// CONFIGURE: Arrow Right
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnRight_MD_func = 57;

//---------------------------------
// CONFIGURE: Arrow Down
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnDown_MD_func = 19;

//---------------------------------
// CONFIGURE: Arrow UP + CROSS
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnUP_L2_MD_func = 28;

//---------------------------------
// CONFIGURE: Arrow Left + CROSS
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnLeft_L2_MD_func = 33;

//---------------------------------
// CONFIGURE: Arrow Right + CROSS
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnRight_L2_MD_func = 30;

//---------------------------------
// CONFIGURE: Arrow Down + CROSS
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnDown_L2_MD_func = 29;

//---------------------------------
// CONFIGURE: Arrow UP + CIRCLE
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnUP_CIRCLE_MD_func = 22;

//---------------------------------
// CONFIGURE: Arrow Left + CIRCLE
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnLeft_CIRCLE_MD_func = 23;

//---------------------------------
// CONFIGURE: Arrow Right + CIRCLE
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnRight_CIRCLE_MD_func = 24;

//---------------------------------
// CONFIGURE: Arrow Down + CIRCLE
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnDown_CIRCLE_MD_func = 25;

//---------------------------------
// CONFIGURE: Arrow UP + PS
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnUP_PS_MD_func = 38;

//---------------------------------
// CONFIGURE: Arrow Left + PS
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnLeft_PS_MD_func = 40;

//---------------------------------
// CONFIGURE: Arrow Right + PS
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnRight_PS_MD_func = 41;

//---------------------------------
// CONFIGURE: Arrow Down + PS
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnDown_PS_MD_func = 39;

//---------------------------------
// CONFIGURE: Arrow Up + L1
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnUP_L1_MD_func = 34;

//---------------------------------
// CONFIGURE: Arrow Left + L1
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnLeft_L1_MD_func = 36;

//---------------------------------
// CONFIGURE: Arrow Right + L1
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnRight_L1_MD_func = 37;

//---------------------------------
// CONFIGURE: Arrow Down + L1
//---------------------------------
// Enter MarcDuino Function Code (1 - 75) (See Above)
int FTbtnDown_L1_MD_func = 35;

// ---------------------------------------------------------------------------------------
//               SYSTEM VARIABLES - USER CONFIG SECTION COMPLETED
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
//                          Drive Controller Settings
// ---------------------------------------------------------------------------------------

int motorControllerBaudRate = 9600;   // Set the baud rate for the Syren motor controller
                                      // for packetized options are: 2400, 9600, 19200 and 38400

#define SYREN_ADDR         129      // Serial Address for Dome Syren
#define SABERTOOTH_ADDR    128      // Serial Address for Foot Sabertooth

int marcDuinoBaudRate = 9600; // Set the baud rate for the Marcduino Connection

#define ENABLE_UHS_DEBUGGING 1

// ---------------------------------------------------------------------------------------
//                          Sound Settings
// ---------------------------------------------------------------------------------------
//#define EXTRA_SOUNDS

// ---------------------------------------------------------------------------------------
//                          LED Settings
// ---------------------------------------------------------------------------------------

//Coin Slot LED Contribution by Dave C.
//TODO:  Move PINS to upper part of Mega for Shield purposes
#define numberOfCoinSlotLEDs 3
int COIN_SLOT_LED_PINS[] = { 2, 3, 4 }; // LED pins to use.
long nextCoinSlotLedFlash[numberOfCoinSlotLEDs]; // Array indicating which LED to flash next.
int coinSlotLedState[numberOfCoinSlotLEDs]; // Array indicating the state of the LED's.


// ---------------------------------------------------------------------------------------
//                          Libraries
// ---------------------------------------------------------------------------------------
#include <PS3BT.h>
#include <usbhub.h>

// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <Sabertooth.h>
#include <Servo.h>
#include <LedControl.h>

// ---------------------------------------------------------------------------------------
//                          Utility Arm Settings
// ---------------------------------------------------------------------------------------

//Utility Arm Contribution by Dave C.
int utilArmClosedPos = 0;    // variable to store the servo closed position
int utilArmOpenPos = 140;    // variable to store the servo Opened position

// Check value, open = true, closed = false
boolean isUtilArmTopOpen = false;
boolean isUtilArmBottomOpen = false;

const int UTIL_ARM_TOP = 1;
const int UTIL_ARM_BOTTOM = 2;

Servo FireExtinguisherServo; // create servo object to control a servo

int fireExtinguisherClosedPos = 100;    // variable to store the servo closed position 
int fireExtinguisherOpenPos = 75; // variable to store the servo Opened position

const int PanelClosedPos = 0;    // variable to store the servo closed position
const int PanelOpenPos = 140;    // variable to store the servo closed position

boolean isPanelsOpen = false;

const int DoorsClosedPos = 0;    // variable to store the servo closed position
const int DoorsOpenPos = 140;    // variable to store the servo closed position

boolean isDoorsOpen = false;


// ---------------------------------------------------------------------------------------
//                          Variables
// ---------------------------------------------------------------------------------------

long previousDomeMillis = millis();
long previousFootMillis = millis();
long previousMarcDuinoMillis = millis();
long currentMillis = millis();
int serialLatency = 25;   //This is a delay factor in ms to prevent queueing of the Serial data.
//25ms seems appropriate for HardwareSerial, values of 50ms or larger are needed for Softare Emulation

int marcDuinoButtonCounter = 0;
//int speedToggleButtonCounter = 0;
//int domeToggleButtonCounter = 0;

Sabertooth *ST = new Sabertooth(SABERTOOTH_ADDR, Serial2);
Sabertooth *SyR = new Sabertooth(SYREN_ADDR, Serial2);

///////Set-up for USB and Bluetooth Devices////////////////////////////
USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
PS3BT *PS3Nav = new PS3BT(&Btd);
PS3BT *PS3Nav2 = new PS3BT(&Btd);

//Used for PS3 Fault Detection
uint32_t msgLagTime = 0;
uint32_t lastMsgTime = 0;
uint32_t currentTime = 0;
uint32_t lastLoopTime = 0;
int badPS3Data = 0;
int badPS3DataDome = 0;

boolean firstMessage = true;
String output = "";

boolean isFootMotorStopped = true;
boolean isDomeMotorStopped = true;

boolean isPS3NavigatonInitialized = false;
boolean isSecondaryPS3NavigatonInitialized = false;

boolean isStickEnabled = true;

boolean WaitingforReconnect = false;
boolean WaitingforReconnectDome = false;

boolean mainControllerConnected = false;
boolean domeControllerConnected = false;

byte isAutomateDomeOn = false;
//unsigned long automateMillis = 0;
//byte automateDelay = random(5,20);// set this to min and max seconds between sounds
//int domeAutomationTurnDirection = 20;

// Dome Automation Variables
boolean domeAutomation = false;
int domeTurnDirection = 1;  // 1 = positive turn, -1 negative turn
float domeTargetPosition = 0; // (0 - 359) - degrees in a circle, 0 = home
unsigned long domeStopTurnTime = 0;    // millis() when next turn should stop
unsigned long domeStartTurnTime = 0;  // millis() when next turn should start
int domeStatus = 0;  // 0 = stopped, 1 = prepare to turn, 2 = turning

boolean isQuietModeOn = true;

byte action = 0;
unsigned long DriveMillis = 0;

// ---------------------------------------------------------------------------------------
//                    Panel Management Variables
// ---------------------------------------------------------------------------------------
boolean runningCustRoutine = false;

int DP1_Status = 0;  // 0 = closed, 1 = waiting to open, 2 = open
unsigned long DP1_start = 0;
int DP1_s_delay = 0;
int DP1_o_time = 0;

int DP2_Status = 0;  // 0 = closed, 1 = waiting to open, 2 = open
unsigned long DP2_start = 0;
int DP2_s_delay = 0;
int DP2_o_time = 0;

int DP3_Status = 0;  // 0 = closed, 1 = waiting to open, 2 = open
unsigned long DP3_start = 0;
int DP3_s_delay = 0;
int DP3_o_time = 0;

int DP4_Status = 0;  // 0 = closed, 1 = waiting to open, 2 = open
unsigned long DP4_start = 0;
int DP4_s_delay = 0;
int DP4_o_time = 0;

int DP5_Status = 0;  // 0 = closed, 1 = waiting to open, 2 = open
unsigned long DP5_start = 0;
int DP5_s_delay = 0;
int DP5_o_time = 0;

int DP6_Status = 0;  // 0 = closed, 1 = waiting to open, 2 = open
unsigned long DP6_start = 0;
int DP6_s_delay = 0;
int DP6_o_time = 0;

int DP7_Status = 0;  // 0 = closed, 1 = waiting to open, 2 = open
unsigned long DP7_start = 0;
int DP7_s_delay = 0;
int DP7_o_time = 0;

int DP8_Status = 0;  // 0 = closed, 1 = waiting to open, 2 = open
unsigned long DP8_start = 0;
int DP8_s_delay = 0;
int DP8_o_time = 0;

int DP9_Status = 0;  // 0 = closed, 1 = waiting to open, 2 = open
unsigned long DP9_start = 0;
int DP9_s_delay = 0;
int DP9_o_time = 0;

int DP10_Status = 0;  // 0 = closed, 1 = waiting to open, 2 = open
unsigned long DP10_start = 0;
int DP10_s_delay = 0;
int DP10_o_time = 0;

// =======================================================================================
//                          Main Program
// =======================================================================================

void setup()
{
  //Debug Serial for use with USB Debugging
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

  //The Arduino Mega has three additional serial ports:
  // - Serial1 on pins 19 (RX) and 18 (TX),
  // - Serial2 on pins 17 (RX) and 16 (TX),
  // - Serial3 on pins 15 (RX) and 14 (TX).

  //Setup for Serial2:: Motor Controllers - Syren (Dome) and Sabertooth (Feet)
  Serial2.begin(motorControllerBaudRate);
  SyR->autobaud();
  SyR->setTimeout(300);      //DMB:  How low can we go for safety reasons?  multiples of 100ms

  //Setup for Sabertooth / Foot Motors
  ST->autobaud();          // Send the autobaud command to the Sabertooth controller(s).
  ST->setTimeout(300);      //DMB:  How low can we go for safety reasons?  multiples of 100ms
  ST->setDeadband(driveDeadBandRange);
  ST->stop();

  //Setup for Serial1:: MarcDuino Dome Control Board
  Serial1.begin(marcDuinoBaudRate);

//#ifdef MD_BODY_CONNECTED
  //Setup for Serial3:: Optional MarcDuino Control Board for Body Panels
  Serial3.begin(marcDuinoBaudRate);
//#endif

  randomSeed(analogRead(0));  // random number seed for dome automation

  // NOTE: *Not all* Sabertooth controllers need the autobaud command.
  //       It doesn't hurt anything, but V2 controllers use an
  //       EEPROM setting (changeable with the function setBaudRate) to set
  //       the baud rate instead of detecting with autobaud.
  //
  //       If you have a 2x12, 2x25 V2, 2x60 or SyRen 50, you can remove
  //       the autobaud line and save yourself two seconds of startup delay.

  //Close all panels
  closePanels();

  // Set volume on MP3 Trigger to Mid
  Serial1.print("$m\r");
  
  //Setup for Coin Slot LEDs
  for (int i = 0; i < numberOfCoinSlotLEDs; i++)
  {
    pinMode(COIN_SLOT_LED_PINS[i], OUTPUT);
    coinSlotLedState[i] = LOW;
    digitalWrite(COIN_SLOT_LED_PINS[i], LOW); // all LEDs off
    nextCoinSlotLedFlash[i] = millis() + random(100, 1000);
  }
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
  //Useful to enable with serial console when having controller issues.
#ifdef TEST_CONROLLER
  testPS3Controller();
#endif

  //LOOP through functions from highest to lowest priority.

  if ( !readUSB() )
  {
    //We have a fault condition that we want to ensure that we do NOT process any controller data!!!
    printOutput();
    return;
  }
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
  marcDuinoControl();
  flashCoinSlotLEDs();
  printOutput();

  // If running a custom MarcDuino Panel Routine - Call Function
  if (runningCustRoutine)
  {
    custMarcDuinoPanel();
  }
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
  for (int8_t i = 5; i > 0; i--)
  {
    if (Btd.disc_bdaddr[i] < 0x10)
    {
      btAddress += "0";
    }
    btAddress += String(Btd.disc_bdaddr[i], HEX);
    btAddress += (":");
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

void automateDome()
{
  //automate dome movement
  if (isAutomateDomeOn)
  {
    long rndNum;
    int domeSpeed;
    if (domeStatus == 0)  // Dome is currently stopped - prepare for a future turn
    {
      if (domeTargetPosition == 0)  // Dome is currently in the home position - prepare to turn away
      {
        domeStartTurnTime = millis() + (random(3, 10) * 1000);
        rndNum = random(5, 354);
        domeTargetPosition = rndNum;  // set the target position to a random degree of a 360 circle - shaving off the first and last 5 degrees

        if (domeTargetPosition < 180)  // Turn the dome in the positive direction
        {
          domeTurnDirection = 1;
          domeStopTurnTime = domeStartTurnTime + ((domeTargetPosition / 360) * time360DomeTurn);
        }
        else  // Turn the dome in the negative direction
        {
          domeTurnDirection = -1;
          domeStopTurnTime = domeStartTurnTime + (((360 - domeTargetPosition) / 360) * time360DomeTurn);
        }
      }
      else  // Dome is not in the home position - send it back to home
      {
        domeStartTurnTime = millis() + (random(3, 10) * 1000);

        if (domeTargetPosition < 180)
        {
          domeTurnDirection = -1;
          domeStopTurnTime = domeStartTurnTime + ((domeTargetPosition / 360) * time360DomeTurn);
        }
        else
        {
          domeTurnDirection = 1;
          domeStopTurnTime = domeStartTurnTime + (((360 - domeTargetPosition) / 360) * time360DomeTurn);
        }

        domeTargetPosition = 0;

      }

      domeStatus = 1;  // Set dome status to preparing for a future turn

#ifdef SHADOW_DEBUG
      output += "Dome Automation: Initial Turn Set\r\n";
      output +=  "Current Time: ";
      output +=  millis();
      output += "\r\n Next Start Time: ";
      output += domeStartTurnTime;
      output += "\r\n";
      output += "Next Stop Time: ";
      output += domeStopTurnTime;
      output += "\r\n";
      output += "Dome Target Position: ";
      output += domeTargetPosition;
      output += "\r\n";
#endif
    }

    if (domeStatus == 1)  // Dome is prepared for a future move - start the turn when ready
    {
      if (domeStartTurnTime < millis())
      {
        domeStatus = 2;

#ifdef SHADOW_DEBUG
        output += "Dome Automation: Ready To Start Turn\r\n";
#endif
      }
    }

    if (domeStatus == 2) // Dome is now actively turning until it reaches its stop time
    {
      if (domeStopTurnTime > millis())
      {
        domeSpeed = domeAutoSpeed * domeTurnDirection;
        SyR->motor(domeSpeed);

#ifdef SHADOW_DEBUG
        output += "Turning Now!!\r\n";
#endif
      }
      else  // turn completed - stop the motor
      {
        domeStatus = 0;
        SyR->stop();

#ifdef SHADOW_DEBUG
        output += "STOP TURN!!\r\n";
#endif
      }
    }
  }
}

// =======================================================================================
// //////////////////////////Process PS3 Controller Fault Detection///////////////////////
// =======================================================================================
boolean criticalFaultDetect()
{
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

    if (msgLagTime > 300 && !isFootMotorStopped)
    {
#ifdef SHADOW_DEBUG
      output += "It has been 100ms since we heard from the PS3 Controller\r\n";
      output += "Shut downing motors, and watching for a new PS3 message\r\n";
#endif
      ST->stop();
      SyR->stop();
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
    if (!PS3Nav->getStatus(Plugged) && !PS3Nav->getStatus(Unplugged))
    {
      // We don't have good data from the controller.
      //Wait 10ms, Update USB, and try again
      delay(10);
      Usb.Task();
      if (!PS3Nav->getStatus(Plugged) && !PS3Nav->getStatus(Unplugged))
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
    SyR->stop();
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
    if (!PS3Nav2->getStatus(Plugged) && !PS3Nav2->getStatus(Unplugged))
    {
      // We don't have good data from the controller.
      //Wait 15ms, Update USB, and try again
      delay(15);
      Usb.Task();
      if (!PS3Nav2->getStatus(Plugged) && !PS3Nav2->getStatus(Unplugged))
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
  int stickSpeed = 0;
  int turnnum = 0;

  if (isPS3NavigatonInitialized)
  {
    // Additional fault control.  Do NOT send additional commands to Sabertooth if no controllers have initialized.
    if (!isStickEnabled)
    {
#ifdef SHADOW_VERBOSE
      if ( abs(myPS3->getAnalogHat(LeftHatY) - 128) > joystickFootDeadZoneRange)
      {
        output += "Drive Stick is disabled\r\n";
      }
#endif
      if (!isFootMotorStopped)
      {
        ST->stop();
        isFootMotorStopped = true;
        footDriveSpeed = 0;

      #ifdef SHADOW_VERBOSE
        output += "\r\n***Foot Motor STOPPED***\r\n";
      #endif
      }

      return false;
    } else if (!myPS3->PS3NavigationConnected)
    {
      if (!isFootMotorStopped)
      {
        ST->stop();
        isFootMotorStopped = true;
        footDriveSpeed = 0;

      #ifdef SHADOW_VERBOSE
        output += "\r\n***Foot Motor STOPPED***\r\n";
      #endif
      }

      return false;
    } else if ( myPS3->getButtonPress(L1) && (!myPS3->getButtonPress(L2)) )
    {
      if (!isFootMotorStopped)
      {
        ST->stop();
        isFootMotorStopped = true;
        footDriveSpeed = 0;

#ifdef SHADOW_VERBOSE
        output += "\r\n***Foot Motor STOPPED***\r\n";
#endif
      }

      return false;
    } else
    {
      int joystickPosition = myPS3->getAnalogHat(LeftHatY);
      isFootMotorStopped = false;
      if ((myPS3->getButtonPress(L2)) && (!myPS3->getButtonPress(L1)))
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

      if ( abs(joystickPosition - 128) < joystickFootDeadZoneRange)
      {
        // This is RAMP DOWN code when stick is now at ZERO but prior FootSpeed > 20
        if (abs(footDriveSpeed) > 50)
        {
          if (footDriveSpeed > 0)
          {
            footDriveSpeed -= 2;
          } else
          {
            footDriveSpeed += 2;
          }

#ifdef SHADOW_VERBOSE
          output += "ZERO FAST RAMP: footSpeed: ";
          output += footDriveSpeed;
          output += "\nStick Speed: ";
          output += stickSpeed;
          output += "\n\r";
#endif

        } else if (abs(footDriveSpeed) > 20)
        {
          if (footDriveSpeed > 0)
          {
            footDriveSpeed -= 1;
          } else
          {
            footDriveSpeed += 1;
          }

#ifdef SHADOW_VERBOSE
          output += "ZERO MID RAMP: footSpeed: ";
          output += footDriveSpeed;
          output += "\nStick Speed: ";
          output += stickSpeed;
          output += "\n\r";
#endif

        } else
        {
          footDriveSpeed = 0;
        }

      } else
      {
        isFootMotorStopped = false;

        if (footDriveSpeed < stickSpeed)
        {
          if ((stickSpeed - footDriveSpeed) > (ramping + 1))
          {
            footDriveSpeed += ramping;

#ifdef SHADOW_VERBOSE
            output += "RAMPING UP: footSpeed: ";
            output += footDriveSpeed;
            output += "\nStick Speed: ";
            output += stickSpeed;
            output += "\n\r";
#endif
          } else
            footDriveSpeed = stickSpeed;
        } else if (footDriveSpeed > stickSpeed)
        {
          if ((footDriveSpeed - stickSpeed) > (ramping + 1))
          {
            footDriveSpeed -= ramping;

#ifdef SHADOW_VERBOSE
            output += "RAMPING DOWN: footSpeed: ";
            output += footDriveSpeed;
            output += "\nStick Speed: ";
            output += stickSpeed;
            output += "\n\r";
#endif
          } else
            footDriveSpeed = stickSpeed;
        } else
        {
          footDriveSpeed = stickSpeed;
        }
      }

      turnnum = (myPS3->getAnalogHat(LeftHatX));

      //TODO:  Is there a better algorithm here?
      if ( abs(footDriveSpeed) > 50)
        turnnum = (map(myPS3->getAnalogHat(LeftHatX), 54, 200, -(turnspeed / 4), (turnspeed / 4)));
      else if (turnnum <= 200 && turnnum >= 54)
        turnnum = (map(myPS3->getAnalogHat(LeftHatX), 54, 200, -(turnspeed / 3), (turnspeed / 3)));
      else if (turnnum > 200)
        turnnum = (map(myPS3->getAnalogHat(LeftHatX), 201, 255, turnspeed / 3, turnspeed));
      else if (turnnum < 54)
        turnnum = (map(myPS3->getAnalogHat(LeftHatX), 0, 53, -turnspeed, -(turnspeed / 3)));


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


        ST->turn(turnnum * invertTurnDirection);
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
  if ( (controllerNumber == 1 && myPS3->getButtonPress(L1)) || ( controllerNumber == 2 && !myPS3->getButtonPress(L1) && !myPS3->getButtonPress(L2) )  )
  {
    int joystickPosition = myPS3->getAnalogHat(LeftHatX);
    domeRotationSpeed = (map(joystickPosition, 0, 255, -domespeed, domespeed));
    if ( abs(joystickPosition - 128) < joystickDomeDeadZoneRange )
      domeRotationSpeed = 0;

    if (domeRotationSpeed != 0 && isAutomateDomeOn == true)  // Turn off dome automation if manually moved
    {
      isAutomateDomeOn = false;
      domeStatus = 0;
      domeTargetPosition = 0;

#ifdef SHADOW_DEBUG
      output += "Dome Automation OFF\r\n";
#endif

    }
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
  // 4.) Reduce the time-out of the SyRen - just better for safety!

  currentMillis = millis();
  if ( (!isDomeMotorStopped || domeRotationSpeed != 0) && ((currentMillis - previousDomeMillis) > (2 * serialLatency) )  )
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

void ps3utilityArms(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{
  if (!(myPS3->getButtonPress(L1) || myPS3->getButtonPress(L2) || myPS3->getButtonPress(PS)))
  {
    if (myPS3->getButtonClick(CROSS))
    {
#ifdef SHADOW_DEBUG
      output += "Opening/Closing top utility arm\r\n";
#endif

      waveUtilArm(UTIL_ARM_TOP);
    }
    if (myPS3->getButtonClick(CIRCLE))
    {
#ifdef SHADOW_DEBUG
      output += "Opening/Closing bottom utility arm\r\n";
#endif

      waveUtilArm(UTIL_ARM_BOTTOM);
    }
  }
}

void utilityArms()
{
  if (PS3Nav->PS3NavigationConnected) ps3utilityArms(PS3Nav, 1);
  if (PS3Nav2->PS3NavigationConnected) ps3utilityArms(PS3Nav2, 2);
}

void ps3ToggleSettings(PS3BT* myPS3 = PS3Nav)
{
  if (myPS3->getButtonPress(PS) && myPS3->getButtonClick(L3))
  {
    //Quick Shutdown of PS3 Controller
    output += "\r\nDisconnecting the controller.\r\n";
    myPS3->disconnect();
  }


  // enable / disable Drive stick & play sound
  if (myPS3->getButtonPress(PS) && myPS3->getButtonClick(CROSS))
  {
#ifdef SHADOW_DEBUG
    output += "Disabling the DriveStick\r\n";
#endif
    isStickEnabled = false;
  }

  if (myPS3->getButtonPress(PS) && myPS3->getButtonClick(CIRCLE))
  {
#ifdef SHADOW_DEBUG
    output += "Enabling the DriveStick\r\n";
#endif
    isStickEnabled = true;
  }

  if ((myPS3->getButtonPress(L2) && !myPS3->getButtonPress(L1)) && myPS3->getButtonClick(CROSS))
  {
    if (isAutomateDomeOn)
    {
      #ifdef SHADOW_DEBUG
        output += "Disabling the Dome Automation\r\n";
      #endif
      isAutomateDomeOn = false;
      domeStatus = 0;
      domeTargetPosition = 0;
      SyR->stop();
      action = 0;
      Serial1.print("$93\r");
    }
    else
    {
      #ifdef SHADOW_DEBUG
        output += "Enabling the Dome Automation\r\n";
      #endif
      isAutomateDomeOn = true;
      Serial1.print("$92\r");
    }
  }

  if ((myPS3->getButtonPress(L2) && !myPS3->getButtonPress(L1)) && myPS3->getButtonClick(CIRCLE))
  {
    if (isQuietModeOn)
    {
#ifdef SHADOW_DEBUG
      output += "Enabling Sound Automation/Mid-Awake Mode\r\n";
#endif
      btnCircle_L2_MD_func = 14;
      isQuietModeOn = false;
    }
    else
    {
#ifdef SHADOW_DEBUG
      output += "Disabling Sound Automation/Mid-Awake Mode\r\n";
#endif
      btnCircle_L2_MD_func = 11;
      isQuietModeOn = true;
    }

    marcDuinoButtonPush(btnCircle_L2_type, btnCircle_L2_MD_func, btnCircle_L2_cust_MP3_num, btnCircle_L2_cust_LD_type, btnCircle_L2_cust_LD_text, btnCircle_L2_cust_panel,
                        btnCircle_L2_use_DP1,
                        btnCircle_L2_DP1_open_start_delay,
                        btnCircle_L2_DP1_stay_open_time,
                        btnCircle_L2_use_DP2,
                        btnCircle_L2_DP2_open_start_delay,
                        btnCircle_L2_DP2_stay_open_time,
                        btnCircle_L2_use_DP3,
                        btnCircle_L2_DP3_open_start_delay,
                        btnCircle_L2_DP3_stay_open_time,
                        btnCircle_L2_use_DP4,
                        btnCircle_L2_DP4_open_start_delay,
                        btnCircle_L2_DP4_stay_open_time,
                        btnCircle_L2_use_DP5,
                        btnCircle_L2_DP5_open_start_delay,
                        btnCircle_L2_DP5_stay_open_time,
                        btnCircle_L2_use_DP6,
                        btnCircle_L2_DP6_open_start_delay,
                        btnCircle_L2_DP6_stay_open_time,
                        btnCircle_L2_use_DP7,
                        btnCircle_L2_DP7_open_start_delay,
                        btnCircle_L2_DP7_stay_open_time,
                        btnCircle_L2_use_DP8,
                        btnCircle_L2_DP8_open_start_delay,
                        btnCircle_L2_DP8_stay_open_time,
                        btnCircle_L2_use_DP9,
                        btnCircle_L2_DP9_open_start_delay,
                        btnCircle_L2_DP9_stay_open_time,
                        btnCircle_L2_use_DP10,
                        btnCircle_L2_DP10_open_start_delay,
                        btnCircle_L2_DP10_stay_open_time);
  }

  if ((myPS3->getButtonPress(L2) && myPS3->getButtonPress(L1)) && myPS3->getButtonClick(CROSS))
  {
    marcDuinoButtonPush(btnCross_L1_L2_type, btnCross_L1_L2_MD_func, btnCross_L1_L2_cust_MP3_num, btnCross_L1_L2_cust_LD_type, btnCross_L1_L2_cust_LD_text, btnCross_L1_L2_cust_panel,
                        btnCross_L1_L2_use_DP1,
                        btnCross_L1_L2_DP1_open_start_delay,
                        btnCross_L1_L2_DP1_stay_open_time,
                        btnCross_L1_L2_use_DP2,
                        btnCross_L1_L2_DP2_open_start_delay,
                        btnCross_L1_L2_DP2_stay_open_time,
                        btnCross_L1_L2_use_DP3,
                        btnCross_L1_L2_DP3_open_start_delay,
                        btnCross_L1_L2_DP3_stay_open_time,
                        btnCross_L1_L2_use_DP4,
                        btnCross_L1_L2_DP4_open_start_delay,
                        btnCross_L1_L2_DP4_stay_open_time,
                        btnCross_L1_L2_use_DP5,
                        btnCross_L1_L2_DP5_open_start_delay,
                        btnCross_L1_L2_DP5_stay_open_time,
                        btnCross_L1_L2_use_DP6,
                        btnCross_L1_L2_DP6_open_start_delay,
                        btnCross_L1_L2_DP6_stay_open_time,
                        btnCross_L1_L2_use_DP7,
                        btnCross_L1_L2_DP7_open_start_delay,
                        btnCross_L1_L2_DP7_stay_open_time,
                        btnCross_L1_L2_use_DP8,
                        btnCross_L1_L2_DP8_open_start_delay,
                        btnCross_L1_L2_DP8_stay_open_time,
                        btnCross_L1_L2_use_DP9,
                        btnCross_L1_L2_DP9_open_start_delay,
                        btnCross_L1_L2_DP9_stay_open_time,
                        btnCross_L1_L2_use_DP10,
                        btnCross_L1_L2_DP10_open_start_delay,
                        btnCross_L1_L2_DP10_stay_open_time);
  }

  if ((myPS3->getButtonPress(L2) && myPS3->getButtonPress(L1)) && myPS3->getButtonClick(CIRCLE))
  {
    wavePanels();
  }

  if ((myPS3->getButtonPress(L1) && !myPS3->getButtonPress(L2)) && myPS3->getButtonClick(LEFT))
  {
    waveDoors();
  }
}

// =======================================================================================
// This is the main MarcDuino Button Management Function
// =======================================================================================
void marcDuinoButtonPush(int type, int MD_func, int MP3_num, int LD_type, String LD_text, int panel_type,
                         boolean use_DP1,
                         int DP1_str_delay,
                         int DP1_open_time,
                         boolean use_DP2,
                         int DP2_str_delay,
                         int DP2_open_time,
                         boolean use_DP3,
                         int DP3_str_delay,
                         int DP3_open_time,
                         boolean use_DP4,
                         int DP4_str_delay,
                         int DP4_open_time,
                         boolean use_DP5,
                         int DP5_str_delay,
                         int DP5_open_time,
                         boolean use_DP6,
                         int DP6_str_delay,
                         int DP6_open_time,
                         boolean use_DP7,
                         int DP7_str_delay,
                         int DP7_open_time,
                         boolean use_DP8,
                         int DP8_str_delay,
                         int DP8_open_time,
                         boolean use_DP9,
                         int DP9_str_delay,
                         int DP9_open_time,
                         boolean use_DP10,
                         int DP10_str_delay,
                         int DP10_open_time)
{

  String strMDCmd;

  if (type == 1)  // Std Marcduino Function Call Configured
  {

    switch (MD_func)
    {
      case 1:
        Serial1.print(":SE00\r");
        Serial3.print(":SE00\r");
        break;

      case 2:
        Serial1.print(":SE01\r");
        Serial3.print(":SE01\r");
        break;

      case 3:
        Serial1.print(":SE02\r");
        Serial3.print(":SE02\r");
        break;

      case 4:
        Serial1.print(":SE03\r");
        break;

      case 5:
        Serial1.print(":SE04\r");
        break;

      case 6:
        Serial1.print(":SE05\r");
        break;

      case 7:
        //Serial1.print(":SE06\r");
        Serial1.print("&25,\"A007|10\r:SE06\r");
        break;

      case 8:
        Serial1.print(":SE07\r");
        Serial3.print(":SE07\r");
        break;

      case 9:
        //Serial1.print(":SE08\r");
        Serial1.print("&25,\"S1:SE08\r");
        break;

      case 10:
        Serial1.print(":SE09\r");
        Serial3.print(":SE09\r");
        break;

      case 11:
        Serial1.print(":SE10\r");
        Serial3.print(":SE10\r");
        break;

      case 12:
        Serial1.print(":SE11\r");
        break;

      case 13:
        Serial1.print(":SE13\r");
        break;

      case 14:
        Serial1.print(":SE14\r");
        break;

      case 15:
        Serial1.print(":SE15\r");
        break;

 /*     case 16:
        Serial1.print(":SE16\r");
        break;*/
        
      case 16:
        Serial1.print("&25,\"A006|10\r:SE16\r");
        Serial3.print(":SE16\r");
        break;
        
      case 17:
        Serial1.print(":SE51\r");
        break;

      case 18:
        Serial1.print(":SE52\r");
        break;

      case 19:
        Serial1.print(":SE53\r");
        break;

      case 20:
        Serial1.print(":SE54\r");
        break;

      case 21:
        Serial1.print(":SE55\r");
        break;

      case 22:
        Serial1.print(":SE56\r");
        break;

      case 23:
        Serial1.print(":SE57\r");
        break;

      case 24:
        Serial1.print(":SE58\r");
        break;

      case 25:
        Serial1.print("*RD00\r");
        break;

      case 26:
        Serial1.print("*ON00\r");
        break;

      case 27:
        Serial1.print("*OF00\r");
        break;

      case 28:
        Serial1.print("*ST00\r");
        break;

      case 29:
        Serial1.print("$+\r");
        break;

      case 30:
        Serial1.print("$-\r");
        break;

      case 31:
        Serial1.print("$f\r");
        break;

      case 32:
        Serial1.print("$m\r");
        break;

      case 33:
        Serial1.print(":OP00\r");
        Serial3.print(":OP00\r");
        break;

      case 34:
        Serial1.print(":OP11\r");
        break;

      case 35:
        Serial1.print(":OP12\r");
        break;

      case 36:
        Serial1.print(":CL00\r");
        Serial3.print(":CL00\r");
        break;

      case 37:
        Serial1.print(":OP01\r");
        break;

      case 38:
        Serial1.print(":CL01\r");
        break;

      case 39:
        Serial1.print(":OP02\r");
        break;

      case 40:
        Serial1.print(":CL02\r");
        break;

      case 41:
        Serial1.print(":OP03\r");
        break;

      case 42:
        Serial1.print(":CL03\r");
        break;

      case 43:
        Serial1.print(":OP04\r");
        break;

      case 44:
        Serial1.print(":CL04\r");
        break;

      case 45:
        Serial1.print(":OP05\r");
        break;

      case 46:
        Serial1.print(":CL05\r");
        break;

      case 47:
        Serial1.print(":OP06\r");
        break;

      case 48:
        Serial1.print(":CL06\r");
        break;

      case 49:
        Serial1.print(":OP07\r");
        break;

      case 50:
        Serial1.print(":CL07\r");
        break;

      case 51:
        Serial1.print(":OP08\r");
        break;

      case 52:
        Serial1.print(":CL08\r");
        break;

      case 53:
        Serial1.print(":OP09\r");
        break;

      case 54:
        Serial1.print(":CL09\r");
        break;

      case 55:
        Serial1.print(":OP10\r");
        break;

      case 56:
        Serial1.print(":CL10\r");
        break;

      case 57:
        Serial1.print("*MO99\r");
        break;

      case 58:
        Serial1.print("*MO00\r");
        break;

      case 59:
        Serial1.print("*MF10\r");
        break;

      case 60:
        Serial1.print("$S\r");
        break;

      case 61:
        Serial1.print("$F\r");
        break;

      case 62:
        Serial1.print("$L\r");
        break;

      case 63:
        Serial1.print("$c\r");
        break;

      case 64:
        Serial1.print("$W\r");
        break;

      case 65:
        Serial1.print("$M\r");
        break;

      case 66:
        Serial1.print("$C\r");
        break;

      case 67:
        Serial1.print("$D\r");
        break;

#ifdef MD_BODY_CONNECTED
      case 68:
        Serial3.print(":OP00\r");
        break;

      case 69:
        Serial3.print(":CL00\r");
        break;

      case 70:
        Serial3.print(":OP01\r");
        break;

      case 71:
        Serial3.print(":CL01\r");
        break;

      case 72:
        Serial3.print(":OP02\r");
		    Serial3.print(":OP05\r");
        break;

      case 73:
        Serial3.print(":CL02\r");
		    Serial3.print(":CL05\r");
        break;

      case 74:
        Serial3.print(":OP03\r");
        break;

      case 75:
        Serial3.print(":CL03\r");
        break;

      case 76:
        Serial3.print(":OP04\r");
        break;

      case 77:
        Serial3.print(":CL04\r");
        break;

      case 78:
        Serial3.print(":OP05\r");
        break;

      case 79:
        Serial3.print(":CL05\r");
        break;

      case 80:
        Serial3.print(":OP06\r");
        break;

      case 81:
        Serial3.print(":CL06\r");
        break;

      case 82:
        Serial3.print(":OP07\r");
        break;

      case 83:
        Serial3.print(":CL07\r");
        break;

      case 84:
        Serial3.print(":OP08\r");
        break;

      case 85:
        Serial3.print(":CL08\r");
        break;

      case 86:
        Serial3.print(":OP09\r");
        break;

      case 87:
        Serial3.print(":CL09\r");
        break;

      case 88:
        Serial3.print(":OP10\r");
        break;

      case 89:
        Serial3.print(":CL10\r");
        break;
#endif

    }

  }  // End Std Marcduino Function Calls


  if (type == 2) // Custom Button Configuration
  {

    if ((MP3_num > 101 && MP3_num < 226) || (MP3_num >= 1 && MP3_num < 7)) // Valid Custom Sound Range Selected - Play Custom Sound Selection
    {

      switch (MP3_num)
      {
        case 1:
          // Play Random sound from Generic sound bank (bank #1)
          strMDCmd = "$1";
          strMDCmd += String(random(1, 26));
          strMDCmd += "\r";
          Serial1.print(strMDCmd);
          break;
        case 2:
          // Play Random sound from Chatty sound bank (bank #2)
          strMDCmd = "$2";
          strMDCmd += String(random(1, 26));
          strMDCmd += "\r";
          Serial1.print(strMDCmd);
          break;
        case 3:
          // Play Random sound from Happy sound bank (bank #1)
          strMDCmd = "$3";
          strMDCmd += String(random(1, 26));
          strMDCmd += "\r";
          Serial1.print(strMDCmd);
          break;
        case 4:
          // Play Random sound from Sad sound bank (bank #2)
          strMDCmd = "$4";
          strMDCmd += String(random(1, 8));
          strMDCmd += "\r";
          Serial1.print(strMDCmd);
          break;
        case 5:
          // Play Random sound from Sound bank (bank #9)
          strMDCmd = "$9";
          strMDCmd += String(random(5, 18));
          strMDCmd += "\r";
          Serial1.print(strMDCmd);
          break;
        case 6:
          // Play Random Blaster sound from Sound bank (bank #9)
          strMDCmd = "$9";
          strMDCmd += String(random(19, 26));
          strMDCmd += "\r";
          Serial1.print(strMDCmd);
          break;

        case 102:
          Serial1.print("$52\r");
          break;

        case 176:
          Serial1.print("$81\r");
          break;

        case 177:
          Serial1.print("$82\r");
          break;

        case 178:
          Serial1.print("$83\r");
          break;

        case 179:
          Serial1.print("$84\r");
          break;
        
        case 180:
          Serial1.print("$85\r");
          break;
          
        case 181:
          Serial1.print("$86\r");
          break;
          
        case 182:
          Serial1.print("$87\r");
          break;

        case 183:
          Serial1.print("$88\r");
          break;

        case 184:
          Serial1.print("$89\r");
          break;

        case 185:
          Serial1.print("$810\r");
          break;

        case 186:
          Serial1.print("$811\r");
          break;

        case 187:
          Serial1.print("$812\r");
          break;

        case 188:
          Serial1.print("$813\r");
          break;

        case 189:
          Serial1.print("$814\r");
          break;

        case 190:
          Serial1.print("$815\r");
          break;

        case 191:
          Serial1.print("$816\r");
          break;

        case 192:
          Serial1.print("$817\r");
          break;

        case 193:
          Serial1.print("$818\r");
          break;

        case 194:
          Serial1.print("$819\r");
          break;

        case 195:
          Serial1.print("$820\r");
          break;

        case 196:
          Serial1.print("$821\r");
          break;

        case 197:
          Serial1.print("$822\r");
          break;

        case 198:
          Serial1.print("$823\r");
          break;

        case 199:
          Serial1.print("$824\r");
          break;

        case 200:
          Serial1.print("$825\r");
          break;

        case 201:
          Serial1.print("$91\r");
          break;

        case 202:
          Serial1.print("$92\r");
          break;

        case 203:
          Serial1.print("$93\r");
          break;

        case 204:
          Serial1.print("$94\r");
          break;

        case 205:
          Serial1.print("$95\r");
          break;

        case 206:
          Serial1.print("$96\r");
          break;

        case 207:
          Serial1.print("$97\r");
          break;

        case 208:
          Serial1.print("$98\r");
          break;

        case 209:
          Serial1.print("$99\r");
          break;

        case 210:
          Serial1.print("$910\r");
          break;

        case 211:
          Serial1.print("$911\r");
          break;

        case 212:
          Serial1.print("$912\r");
          break;

        case 213:
          Serial1.print("$913\r");
          break;

        case 214:
          Serial1.print("$914\r");
          break;

        case 215:
          Serial1.print("$915\r");
          break;

        case 216:
          Serial1.print("$916\r");
          break;

        case 217:
          Serial1.print("$917\r");
          break;

        case 218:
          Serial1.print("$918\r");
          break;

        case 219:
          Serial1.print("$919\r");
          break;

        case 220:
          Serial1.print("$920\r");
          break;

        case 221:
          Serial1.print("$921\r");
          break;

        case 222:
          Serial1.print("$922\r");
          break;

        case 223:
          Serial1.print("$923\r");
          break;

        case 224:
          Serial1.print("$924\r");
          break;

        case 225:
          Serial1.print("$925\r");
          break;
      }

    }

    if (panel_type > 0 && panel_type < 10) // Valid panel type selected - perform custom panel functions
    {

      // Reset the custom panel flags
      DP1_Status = 0;
      DP2_Status = 0;
      DP3_Status = 0;
      DP4_Status = 0;
      DP5_Status = 0;
      DP6_Status = 0;
      DP7_Status = 0;
      DP8_Status = 0;
      DP9_Status = 0;
      DP10_Status = 0;

      if (panel_type > 1)
      {
        Serial1.print(":CL00\r");  // close all the panels prior to next custom routine
        delay(50); // give panel close command time to process before starting next panel command
      }

      switch (panel_type)
      {

        case 1:
          Serial1.print(":CL00\r");
          break;

        case 2:
          Serial1.print(":SE51\r");
          break;

        case 3:
          Serial1.print(":SE52\r");
          break;

        case 4:
          Serial1.print(":SE53\r");
          break;

        case 5:
          Serial1.print(":SE54\r");
          break;

        case 6:
          Serial1.print(":SE55\r");
          break;

        case 7:
          Serial1.print(":SE56\r");
          break;

        case 8:
          Serial1.print(":SE57\r");
          break;

        case 9: // This is the setup section for the custom panel routines

          runningCustRoutine = true;

          // Configure Dome Panel #1
          if (use_DP1)
          {

            DP1_Status = 1;
            DP1_start = millis();

            if (DP1_str_delay < 31)
            {

              DP1_s_delay = DP1_str_delay;

            } else
            {
              DP1_Status = 0;
            }

            if (DP1_open_time > 0 && DP1_open_time < 31)
            {

              DP1_o_time = DP1_open_time;

            } else
            {
              DP1_Status = 0;
            }

          }

          // Configure Dome Panel #2
          if (use_DP2)
          {

            DP2_Status = 1;
            DP2_start = millis();

            if (DP2_str_delay < 31)
            {

              DP2_s_delay = DP2_str_delay;

            } else
            {
              DP2_Status = 0;
            }

            if (DP2_open_time > 0 && DP2_open_time < 31)
            {

              DP2_o_time = DP2_open_time;

            } else
            {
              DP2_Status = 0;
            }

          }


          // Configure Dome Panel #3
          if (use_DP3)
          {

            DP3_Status = 1;
            DP3_start = millis();

            if (DP3_str_delay < 31)
            {

              DP3_s_delay = DP3_str_delay;

            } else
            {
              DP3_Status = 0;
            }

            if (DP3_open_time > 0 && DP3_open_time < 31)
            {

              DP3_o_time = DP3_open_time;

            } else
            {
              DP3_Status = 0;
            }

          }

          // Configure Dome Panel #4
          if (use_DP4)
          {

            DP4_Status = 1;
            DP4_start = millis();

            if (DP4_str_delay < 31)
            {

              DP4_s_delay = DP4_str_delay;

            } else
            {
              DP4_Status = 0;
            }

            if (DP4_open_time > 0 && DP4_open_time < 31)
            {

              DP4_o_time = DP4_open_time;

            } else
            {
              DP4_Status = 0;
            }

          }

          // Configure Dome Panel #5
          if (use_DP5)
          {

            DP5_Status = 1;
            DP5_start = millis();

            if (DP5_str_delay < 31)
            {

              DP5_s_delay = DP5_str_delay;

            } else
            {
              DP5_Status = 0;
            }

            if (DP5_open_time > 0 && DP5_open_time < 31)
            {

              DP5_o_time = DP5_open_time;

            } else
            {
              DP5_Status = 0;
            }

          }

          // Configure Dome Panel #6
          if (use_DP6)
          {

            DP6_Status = 1;
            DP6_start = millis();

            if (DP6_str_delay < 31)
            {

              DP6_s_delay = DP6_str_delay;

            } else
            {
              DP6_Status = 0;
            }

            if (DP6_open_time > 0 && DP6_open_time < 31)
            {

              DP6_o_time = DP6_open_time;

            } else
            {
              DP6_Status = 0;
            }

          }

          // Configure Dome Panel #7
          if (use_DP7)
          {

            DP7_Status = 1;
            DP7_start = millis();

            if (DP7_str_delay < 31)
            {

              DP7_s_delay = DP7_str_delay;

            } else
            {
              DP7_Status = 0;
            }

            if (DP7_open_time > 0 && DP7_open_time < 31)
            {

              DP7_o_time = DP7_open_time;

            } else
            {
              DP7_Status = 0;
            }

          }

          // Configure Dome Panel #8
          if (use_DP8)
          {

            DP8_Status = 1;
            DP8_start = millis();

            if (DP8_str_delay < 31)
            {

              DP8_s_delay = DP8_str_delay;

            } else
            {
              DP8_Status = 0;
            }

            if (DP8_open_time > 0 && DP8_open_time < 31)
            {

              DP8_o_time = DP8_open_time;

            } else
            {
              DP8_Status = 0;
            }

          }

          // Configure Dome Panel #9
          if (use_DP9)
          {

            DP9_Status = 1;
            DP9_start = millis();

            if (DP9_str_delay < 31)
            {

              DP9_s_delay = DP9_str_delay;

            } else
            {
              DP9_Status = 0;
            }

            if (DP9_open_time > 0 && DP9_open_time < 31)
            {

              DP9_o_time = DP9_open_time;

            } else
            {
              DP9_Status = 0;
            }

          }

          // Configure Dome Panel #10
          if (use_DP10)
          {

            DP10_Status = 1;
            DP10_start = millis();

            if (DP10_str_delay < 31)
            {

              DP10_s_delay = DP10_str_delay;

            } else
            {
              DP10_Status = 0;
            }

            if (DP10_open_time > 0 && DP10_open_time < 31)
            {

              DP10_o_time = DP10_open_time;

            } else
            {
              DP10_Status = 0;
            }

          }

          // If every dome panel config failed to work - reset routine flag to false
          if (DP1_Status + DP2_Status + DP3_Status + DP4_Status + DP5_Status + DP6_Status + DP7_Status + DP8_Status + DP9_Status + DP10_Status == 0)
          {

            runningCustRoutine = false;

          }

          break;
      }
    }


    if (LD_type > 0 && LD_type < 9) // Valid Logic Display Selected - Display Custom Logic Display
    {

      if (panel_type > 1 && panel_type < 10)  // If a custom panel movement was selected - need to briefly pause before changing light sequence to avoid conflict)
      {
        delay(30);
      }

      switch (LD_type)
      {

        case 1:
          Serial1.print("@0T1\r");
          break;

        case 2:
          Serial1.print("@0T4\r");
          break;

        case 3:
          Serial1.print("@0T5\r");
          break;

        case 4:
          Serial1.print("@0T6\r");
          break;

        case 5:
          Serial1.print("@0T10\r");
          break;

        case 6:
          Serial1.print("@0T11\r");
          break;

        case 7:
          Serial1.print("@0T92\r");
          break;

        case 8:
          Serial1.print("@0T100\r");
          delay(50);
          String custString = "@0M";
          custString += LD_text;
          custString += "\r";
          Serial1.print(custString);
          break;
      }
    }

  }

}

void processmarcDuinoCommand(char marcDuinoCommand)
{
  if (marcDuinoCommand != '+' && marcDuinoCommand != '-')
  {
    isQuietModeOn = false;
  }

  switch (marcDuinoCommand)
  {
    case '+':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnCross - Volume Up\r\n";
#endif
      marcDuinoButtonPush(btnCross_L1_type, btnCross_L1_MD_func, btnCross_L1_cust_MP3_num, btnCross_L1_cust_LD_type, btnCross_L1_cust_LD_text, btnCross_L1_cust_panel,
                          btnCross_L1_use_DP1,
                          btnCross_L1_DP1_open_start_delay,
                          btnCross_L1_DP1_stay_open_time,
                          btnCross_L1_use_DP2,
                          btnCross_L1_DP2_open_start_delay,
                          btnCross_L1_DP2_stay_open_time,
                          btnCross_L1_use_DP3,
                          btnCross_L1_DP3_open_start_delay,
                          btnCross_L1_DP3_stay_open_time,
                          btnCross_L1_use_DP4,
                          btnCross_L1_DP4_open_start_delay,
                          btnCross_L1_DP4_stay_open_time,
                          btnCross_L1_use_DP5,
                          btnCross_L1_DP5_open_start_delay,
                          btnCross_L1_DP5_stay_open_time,
                          btnCross_L1_use_DP6,
                          btnCross_L1_DP6_open_start_delay,
                          btnCross_L1_DP6_stay_open_time,
                          btnCross_L1_use_DP7,
                          btnCross_L1_DP7_open_start_delay,
                          btnCross_L1_DP7_stay_open_time,
                          btnCross_L1_use_DP8,
                          btnCross_L1_DP8_open_start_delay,
                          btnCross_L1_DP8_stay_open_time,
                          btnCross_L1_use_DP9,
                          btnCross_L1_DP9_open_start_delay,
                          btnCross_L1_DP9_stay_open_time,
                          btnCross_L1_use_DP10,
                          btnCross_L1_DP10_open_start_delay,
                          btnCross_L1_DP10_stay_open_time);
      break;
    case '-':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnCircle_L1 - Volume Down\r\n";
#endif
      marcDuinoButtonPush(btnCircle_L1_type, btnCircle_L1_MD_func, btnCircle_L1_cust_MP3_num, btnCircle_L1_cust_LD_type, btnCircle_L1_cust_LD_text, btnCircle_L1_cust_panel,
                          btnCircle_L1_use_DP1,
                          btnCircle_L1_DP1_open_start_delay,
                          btnCircle_L1_DP1_stay_open_time,
                          btnCircle_L1_use_DP2,
                          btnCircle_L1_DP2_open_start_delay,
                          btnCircle_L1_DP2_stay_open_time,
                          btnCircle_L1_use_DP3,
                          btnCircle_L1_DP3_open_start_delay,
                          btnCircle_L1_DP3_stay_open_time,
                          btnCircle_L1_use_DP4,
                          btnCircle_L1_DP4_open_start_delay,
                          btnCircle_L1_DP4_stay_open_time,
                          btnCircle_L1_use_DP5,
                          btnCircle_L1_DP5_open_start_delay,
                          btnCircle_L1_DP5_stay_open_time,
                          btnCircle_L1_use_DP6,
                          btnCircle_L1_DP6_open_start_delay,
                          btnCircle_L1_DP6_stay_open_time,
                          btnCircle_L1_use_DP7,
                          btnCircle_L1_DP7_open_start_delay,
                          btnCircle_L1_DP7_stay_open_time,
                          btnCircle_L1_use_DP8,
                          btnCircle_L1_DP8_open_start_delay,
                          btnCircle_L1_DP8_stay_open_time,
                          btnCircle_L1_use_DP9,
                          btnCircle_L1_DP9_open_start_delay,
                          btnCircle_L1_DP9_stay_open_time,
                          btnCircle_L1_use_DP10,
                          btnCircle_L1_DP10_open_start_delay,
                          btnCircle_L1_DP10_stay_open_time);
      break;
    case '1':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnUP\r\n";
#endif
      marcDuinoButtonPush(btnUP_type, btnUP_MD_func, btnUP_cust_MP3_num, btnUP_cust_LD_type, btnUP_cust_LD_text, btnUP_cust_panel,
                          btnUP_use_DP1,
                          btnUP_DP1_open_start_delay,
                          btnUP_DP1_stay_open_time,
                          btnUP_use_DP2,
                          btnUP_DP2_open_start_delay,
                          btnUP_DP2_stay_open_time,
                          btnUP_use_DP3,
                          btnUP_DP3_open_start_delay,
                          btnUP_DP3_stay_open_time,
                          btnUP_use_DP4,
                          btnUP_DP4_open_start_delay,
                          btnUP_DP4_stay_open_time,
                          btnUP_use_DP5,
                          btnUP_DP5_open_start_delay,
                          btnUP_DP5_stay_open_time,
                          btnUP_use_DP6,
                          btnUP_DP6_open_start_delay,
                          btnUP_DP6_stay_open_time,
                          btnUP_use_DP7,
                          btnUP_DP7_open_start_delay,
                          btnUP_DP7_stay_open_time,
                          btnUP_use_DP8,
                          btnUP_DP8_open_start_delay,
                          btnUP_DP8_stay_open_time,
                          btnUP_use_DP9,
                          btnUP_DP9_open_start_delay,
                          btnUP_DP9_stay_open_time,
                          btnUP_use_DP10,
                          btnUP_DP10_open_start_delay,
                          btnUP_DP10_stay_open_time);
      break;
    case '2':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnRight.\r\n";
#endif
      marcDuinoButtonPush(btnRight_type, btnRight_MD_func, btnRight_cust_MP3_num, btnRight_cust_LD_type, btnRight_cust_LD_text, btnRight_cust_panel,
                          btnRight_use_DP1,
                          btnRight_DP1_open_start_delay,
                          btnRight_DP1_stay_open_time,
                          btnRight_use_DP2,
                          btnRight_DP2_open_start_delay,
                          btnRight_DP2_stay_open_time,
                          btnRight_use_DP3,
                          btnRight_DP3_open_start_delay,
                          btnRight_DP3_stay_open_time,
                          btnRight_use_DP4,
                          btnRight_DP4_open_start_delay,
                          btnRight_DP4_stay_open_time,
                          btnRight_use_DP5,
                          btnRight_DP5_open_start_delay,
                          btnRight_DP5_stay_open_time,
                          btnRight_use_DP6,
                          btnRight_DP6_open_start_delay,
                          btnRight_DP6_stay_open_time,
                          btnRight_use_DP7,
                          btnRight_DP7_open_start_delay,
                          btnRight_DP7_stay_open_time,
                          btnRight_use_DP8,
                          btnRight_DP8_open_start_delay,
                          btnRight_DP8_stay_open_time,
                          btnRight_use_DP9,
                          btnRight_DP9_open_start_delay,
                          btnRight_DP9_stay_open_time,
                          btnRight_use_DP10,
                          btnRight_DP10_open_start_delay,
                          btnRight_DP10_stay_open_time);
      break;
    case '3':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnDown\r\n";
#endif
      marcDuinoButtonPush(btnDown_type, btnDown_MD_func, btnDown_cust_MP3_num, btnDown_cust_LD_type, btnDown_cust_LD_text, btnDown_cust_panel,
                          btnDown_use_DP1,
                          btnDown_DP1_open_start_delay,
                          btnDown_DP1_stay_open_time,
                          btnDown_use_DP2,
                          btnDown_DP2_open_start_delay,
                          btnDown_DP2_stay_open_time,
                          btnDown_use_DP3,
                          btnDown_DP3_open_start_delay,
                          btnDown_DP3_stay_open_time,
                          btnDown_use_DP4,
                          btnDown_DP4_open_start_delay,
                          btnDown_DP4_stay_open_time,
                          btnDown_use_DP5,
                          btnDown_DP5_open_start_delay,
                          btnDown_DP5_stay_open_time,
                          btnDown_use_DP6,
                          btnDown_DP6_open_start_delay,
                          btnDown_DP6_stay_open_time,
                          btnDown_use_DP7,
                          btnDown_DP7_open_start_delay,
                          btnDown_DP7_stay_open_time,
                          btnDown_use_DP8,
                          btnDown_DP8_open_start_delay,
                          btnDown_DP8_stay_open_time,
                          btnDown_use_DP9,
                          btnDown_DP9_open_start_delay,
                          btnDown_DP9_stay_open_time,
                          btnDown_use_DP10,
                          btnDown_DP10_open_start_delay,
                          btnDown_DP10_stay_open_time);
      break;
    case '4':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnLeft\r\n";
#endif
      marcDuinoButtonPush(btnLeft_type, btnLeft_MD_func, btnLeft_cust_MP3_num, btnLeft_cust_LD_type, btnLeft_cust_LD_text, btnLeft_cust_panel,
                          btnLeft_use_DP1,
                          btnLeft_DP1_open_start_delay,
                          btnLeft_DP1_stay_open_time,
                          btnLeft_use_DP2,
                          btnLeft_DP2_open_start_delay,
                          btnLeft_DP2_stay_open_time,
                          btnLeft_use_DP3,
                          btnLeft_DP3_open_start_delay,
                          btnLeft_DP3_stay_open_time,
                          btnLeft_use_DP4,
                          btnLeft_DP4_open_start_delay,
                          btnLeft_DP4_stay_open_time,
                          btnLeft_use_DP5,
                          btnLeft_DP5_open_start_delay,
                          btnLeft_DP5_stay_open_time,
                          btnLeft_use_DP6,
                          btnLeft_DP6_open_start_delay,
                          btnLeft_DP6_stay_open_time,
                          btnLeft_use_DP7,
                          btnLeft_DP7_open_start_delay,
                          btnLeft_DP7_stay_open_time,
                          btnLeft_use_DP8,
                          btnLeft_DP8_open_start_delay,
                          btnLeft_DP8_stay_open_time,
                          btnLeft_use_DP9,
                          btnLeft_DP9_open_start_delay,
                          btnLeft_DP9_stay_open_time,
                          btnLeft_use_DP10,
                          btnLeft_DP10_open_start_delay,
                          btnLeft_DP10_stay_open_time);
      break;
    case '5':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnUP_L2.\r\n";
#endif
      marcDuinoButtonPush(btnUP_L2_type, btnUP_L2_MD_func, btnUP_L2_cust_MP3_num, btnUP_L2_cust_LD_type, btnUP_L2_cust_LD_text, btnUP_L2_cust_panel,
                          btnUP_L2_use_DP1,
                          btnUP_L2_DP1_open_start_delay,
                          btnUP_L2_DP1_stay_open_time,
                          btnUP_L2_use_DP2,
                          btnUP_L2_DP2_open_start_delay,
                          btnUP_L2_DP2_stay_open_time,
                          btnUP_L2_use_DP3,
                          btnUP_L2_DP3_open_start_delay,
                          btnUP_L2_DP3_stay_open_time,
                          btnUP_L2_use_DP4,
                          btnUP_L2_DP4_open_start_delay,
                          btnUP_L2_DP4_stay_open_time,
                          btnUP_L2_use_DP5,
                          btnUP_L2_DP5_open_start_delay,
                          btnUP_L2_DP5_stay_open_time,
                          btnUP_L2_use_DP6,
                          btnUP_L2_DP6_open_start_delay,
                          btnUP_L2_DP6_stay_open_time,
                          btnUP_L2_use_DP7,
                          btnUP_L2_DP7_open_start_delay,
                          btnUP_L2_DP7_stay_open_time,
                          btnUP_L2_use_DP8,
                          btnUP_L2_DP8_open_start_delay,
                          btnUP_L2_DP8_stay_open_time,
                          btnUP_L2_use_DP9,
                          btnUP_L2_DP9_open_start_delay,
                          btnUP_L2_DP9_stay_open_time,
                          btnUP_L2_use_DP10,
                          btnUP_L2_DP10_open_start_delay,
                          btnUP_L2_DP10_stay_open_time);
      break;
    case '6':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnRight_L2.\r\n";
#endif
      marcDuinoButtonPush(btnRight_L2_type, btnRight_L2_MD_func, btnRight_L2_cust_MP3_num, btnRight_L2_cust_LD_type, btnRight_L2_cust_LD_text, btnRight_L2_cust_panel,
                          btnRight_L2_use_DP1,
                          btnRight_L2_DP1_open_start_delay,
                          btnRight_L2_DP1_stay_open_time,
                          btnRight_L2_use_DP2,
                          btnRight_L2_DP2_open_start_delay,
                          btnRight_L2_DP2_stay_open_time,
                          btnRight_L2_use_DP3,
                          btnRight_L2_DP3_open_start_delay,
                          btnRight_L2_DP3_stay_open_time,
                          btnRight_L2_use_DP4,
                          btnRight_L2_DP4_open_start_delay,
                          btnRight_L2_DP4_stay_open_time,
                          btnRight_L2_use_DP5,
                          btnRight_L2_DP5_open_start_delay,
                          btnRight_L2_DP5_stay_open_time,
                          btnRight_L2_use_DP6,
                          btnRight_L2_DP6_open_start_delay,
                          btnRight_L2_DP6_stay_open_time,
                          btnRight_L2_use_DP7,
                          btnRight_L2_DP7_open_start_delay,
                          btnRight_L2_DP7_stay_open_time,
                          btnRight_L2_use_DP8,
                          btnRight_L2_DP8_open_start_delay,
                          btnRight_L2_DP8_stay_open_time,
                          btnRight_L2_use_DP9,
                          btnRight_L2_DP9_open_start_delay,
                          btnRight_L2_DP9_stay_open_time,
                          btnRight_L2_use_DP10,
                          btnRight_L2_DP10_open_start_delay,
                          btnRight_L2_DP10_stay_open_time);
      break;
    case '7':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnDown_L2.\r\n";
#endif
      marcDuinoButtonPush(btnDown_L2_type, btnDown_L2_MD_func, btnDown_L2_cust_MP3_num, btnDown_L2_cust_LD_type, btnDown_L2_cust_LD_text, btnDown_L2_cust_panel,
                          btnDown_L2_use_DP1,
                          btnDown_L2_DP1_open_start_delay,
                          btnDown_L2_DP1_stay_open_time,
                          btnDown_L2_use_DP2,
                          btnDown_L2_DP2_open_start_delay,
                          btnDown_L2_DP2_stay_open_time,
                          btnDown_L2_use_DP3,
                          btnDown_L2_DP3_open_start_delay,
                          btnDown_L2_DP3_stay_open_time,
                          btnDown_L2_use_DP4,
                          btnDown_L2_DP4_open_start_delay,
                          btnDown_L2_DP4_stay_open_time,
                          btnDown_L2_use_DP5,
                          btnDown_L2_DP5_open_start_delay,
                          btnDown_L2_DP5_stay_open_time,
                          btnDown_L2_use_DP6,
                          btnDown_L2_DP6_open_start_delay,
                          btnDown_L2_DP6_stay_open_time,
                          btnDown_L2_use_DP7,
                          btnDown_L2_DP7_open_start_delay,
                          btnDown_L2_DP7_stay_open_time,
                          btnDown_L2_use_DP8,
                          btnDown_L2_DP8_open_start_delay,
                          btnDown_L2_DP8_stay_open_time,
                          btnDown_L2_use_DP9,
                          btnDown_L2_DP9_open_start_delay,
                          btnDown_L2_DP9_stay_open_time,
                          btnDown_L2_use_DP10,
                          btnDown_L2_DP10_open_start_delay,
                          btnDown_L2_DP10_stay_open_time);
      break;
    case '8':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnLeft_L2.\r\n";
#endif
      marcDuinoButtonPush(btnLeft_L2_type, btnLeft_L2_MD_func, btnLeft_L2_cust_MP3_num, btnLeft_L2_cust_LD_type, btnLeft_L2_cust_LD_text, btnLeft_L2_cust_panel,
                          btnLeft_L2_use_DP1,
                          btnLeft_L2_DP1_open_start_delay,
                          btnLeft_L2_DP1_stay_open_time,
                          btnLeft_L2_use_DP2,
                          btnLeft_L2_DP2_open_start_delay,
                          btnLeft_L2_DP2_stay_open_time,
                          btnLeft_L2_use_DP3,
                          btnLeft_L2_DP3_open_start_delay,
                          btnLeft_L2_DP3_stay_open_time,
                          btnLeft_L2_use_DP4,
                          btnLeft_L2_DP4_open_start_delay,
                          btnLeft_L2_DP4_stay_open_time,
                          btnLeft_L2_use_DP5,
                          btnLeft_L2_DP5_open_start_delay,
                          btnLeft_L2_DP5_stay_open_time,
                          btnLeft_L2_use_DP6,
                          btnLeft_L2_DP6_open_start_delay,
                          btnLeft_L2_DP6_stay_open_time,
                          btnLeft_L2_use_DP7,
                          btnLeft_L2_DP7_open_start_delay,
                          btnLeft_L2_DP7_stay_open_time,
                          btnLeft_L2_use_DP8,
                          btnLeft_L2_DP8_open_start_delay,
                          btnLeft_L2_DP8_stay_open_time,
                          btnLeft_L2_use_DP9,
                          btnLeft_L2_DP9_open_start_delay,
                          btnLeft_L2_DP9_stay_open_time,
                          btnLeft_L2_use_DP10,
                          btnLeft_L2_DP10_open_start_delay,
                          btnLeft_L2_DP10_stay_open_time);
      break;
    case '9':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnUP_L1.\r\n";
#endif
      marcDuinoButtonPush(btnUP_L1_type, btnUP_L1_MD_func, btnUP_L1_cust_MP3_num, btnUP_L1_cust_LD_type, btnUP_L1_cust_LD_text, btnUP_L1_cust_panel,
                          btnUP_L1_use_DP1,
                          btnUP_L1_DP1_open_start_delay,
                          btnUP_L1_DP1_stay_open_time,
                          btnUP_L1_use_DP2,
                          btnUP_L1_DP2_open_start_delay,
                          btnUP_L1_DP2_stay_open_time,
                          btnUP_L1_use_DP3,
                          btnUP_L1_DP3_open_start_delay,
                          btnUP_L1_DP3_stay_open_time,
                          btnUP_L1_use_DP4,
                          btnUP_L1_DP4_open_start_delay,
                          btnUP_L1_DP4_stay_open_time,
                          btnUP_L1_use_DP5,
                          btnUP_L1_DP5_open_start_delay,
                          btnUP_L1_DP5_stay_open_time,
                          btnUP_L1_use_DP6,
                          btnUP_L1_DP6_open_start_delay,
                          btnUP_L1_DP6_stay_open_time,
                          btnUP_L1_use_DP7,
                          btnUP_L1_DP7_open_start_delay,
                          btnUP_L1_DP7_stay_open_time,
                          btnUP_L1_use_DP8,
                          btnUP_L1_DP8_open_start_delay,
                          btnUP_L1_DP8_stay_open_time,
                          btnUP_L1_use_DP9,
                          btnUP_L1_DP9_open_start_delay,
                          btnUP_L1_DP9_stay_open_time,
                          btnUP_L1_use_DP10,
                          btnUP_L1_DP10_open_start_delay,
                          btnUP_L1_DP10_stay_open_time);
      break;
    case '0':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnRight_L1.\r\n";
#endif
      marcDuinoButtonPush(btnRight_L1_type, btnRight_L1_MD_func, btnRight_L1_cust_MP3_num, btnRight_L1_cust_LD_type, btnRight_L1_cust_LD_text, btnRight_L1_cust_panel,
                          btnRight_L1_use_DP1,
                          btnRight_L1_DP1_open_start_delay,
                          btnRight_L1_DP1_stay_open_time,
                          btnRight_L1_use_DP2,
                          btnRight_L1_DP2_open_start_delay,
                          btnRight_L1_DP2_stay_open_time,
                          btnRight_L1_use_DP3,
                          btnRight_L1_DP3_open_start_delay,
                          btnRight_L1_DP3_stay_open_time,
                          btnRight_L1_use_DP4,
                          btnRight_L1_DP4_open_start_delay,
                          btnRight_L1_DP4_stay_open_time,
                          btnRight_L1_use_DP5,
                          btnRight_L1_DP5_open_start_delay,
                          btnRight_L1_DP5_stay_open_time,
                          btnRight_L1_use_DP6,
                          btnRight_L1_DP6_open_start_delay,
                          btnRight_L1_DP6_stay_open_time,
                          btnRight_L1_use_DP7,
                          btnRight_L1_DP7_open_start_delay,
                          btnRight_L1_DP7_stay_open_time,
                          btnRight_L1_use_DP8,
                          btnRight_L1_DP8_open_start_delay,
                          btnRight_L1_DP8_stay_open_time,
                          btnRight_L1_use_DP9,
                          btnRight_L1_DP9_open_start_delay,
                          btnRight_L1_DP9_stay_open_time,
                          btnRight_L1_use_DP10,
                          btnRight_L1_DP10_open_start_delay,
                          btnRight_L1_DP10_stay_open_time);
      break;
    case 'A':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnDown_L1.\r\n";
#endif
      marcDuinoButtonPush(btnDown_L1_type, btnDown_L1_MD_func, btnDown_L1_cust_MP3_num, btnDown_L1_cust_LD_type, btnDown_L1_cust_LD_text, btnDown_L1_cust_panel,
                          btnDown_L1_use_DP1,
                          btnDown_L1_DP1_open_start_delay,
                          btnDown_L1_DP1_stay_open_time,
                          btnDown_L1_use_DP2,
                          btnDown_L1_DP2_open_start_delay,
                          btnDown_L1_DP2_stay_open_time,
                          btnDown_L1_use_DP3,
                          btnDown_L1_DP3_open_start_delay,
                          btnDown_L1_DP3_stay_open_time,
                          btnDown_L1_use_DP4,
                          btnDown_L1_DP4_open_start_delay,
                          btnDown_L1_DP4_stay_open_time,
                          btnDown_L1_use_DP5,
                          btnDown_L1_DP5_open_start_delay,
                          btnDown_L1_DP5_stay_open_time,
                          btnDown_L1_use_DP6,
                          btnDown_L1_DP6_open_start_delay,
                          btnDown_L1_DP6_stay_open_time,
                          btnDown_L1_use_DP7,
                          btnDown_L1_DP7_open_start_delay,
                          btnDown_L1_DP7_stay_open_time,
                          btnDown_L1_use_DP8,
                          btnDown_L1_DP8_open_start_delay,
                          btnDown_L1_DP8_stay_open_time,
                          btnDown_L1_use_DP9,
                          btnDown_L1_DP9_open_start_delay,
                          btnDown_L1_DP9_stay_open_time,
                          btnDown_L1_use_DP10,
                          btnDown_L1_DP10_open_start_delay,
                          btnDown_L1_DP10_stay_open_time);
      break;
    case 'B':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnLeft_L1.\r\n";
#endif
      marcDuinoButtonPush(btnLeft_L1_type, btnLeft_L1_MD_func, btnLeft_L1_cust_MP3_num, btnLeft_L1_cust_LD_type, btnLeft_L1_cust_LD_text, btnLeft_L1_cust_panel,
                          btnLeft_L1_use_DP1,
                          btnLeft_L1_DP1_open_start_delay,
                          btnLeft_L1_DP1_stay_open_time,
                          btnLeft_L1_use_DP2,
                          btnLeft_L1_DP2_open_start_delay,
                          btnLeft_L1_DP2_stay_open_time,
                          btnLeft_L1_use_DP3,
                          btnLeft_L1_DP3_open_start_delay,
                          btnLeft_L1_DP3_stay_open_time,
                          btnLeft_L1_use_DP4,
                          btnLeft_L1_DP4_open_start_delay,
                          btnLeft_L1_DP4_stay_open_time,
                          btnLeft_L1_use_DP5,
                          btnLeft_L1_DP5_open_start_delay,
                          btnLeft_L1_DP5_stay_open_time,
                          btnLeft_L1_use_DP6,
                          btnLeft_L1_DP6_open_start_delay,
                          btnLeft_L1_DP6_stay_open_time,
                          btnLeft_L1_use_DP7,
                          btnLeft_L1_DP7_open_start_delay,
                          btnLeft_L1_DP7_stay_open_time,
                          btnLeft_L1_use_DP8,
                          btnLeft_L1_DP8_open_start_delay,
                          btnLeft_L1_DP8_stay_open_time,
                          btnLeft_L1_use_DP9,
                          btnLeft_L1_DP9_open_start_delay,
                          btnLeft_L1_DP9_stay_open_time,
                          btnLeft_L1_use_DP10,
                          btnLeft_L1_DP10_open_start_delay,
                          btnLeft_L1_DP10_stay_open_time);
      break;
    case 'C':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnUP_L1_L2.\r\n";
#endif
      marcDuinoButtonPush(btnUP_L1_L2_type, btnUP_L1_L2_MD_func, btnUP_L1_L2_cust_MP3_num, btnUP_L1_L2_cust_LD_type, btnUP_L1_L2_cust_LD_text, btnUP_L1_L2_cust_panel,
                          btnUP_L1_L2_use_DP1,
                          btnUP_L1_L2_DP1_open_start_delay,
                          btnUP_L1_L2_DP1_stay_open_time,
                          btnUP_L1_L2_use_DP2,
                          btnUP_L1_L2_DP2_open_start_delay,
                          btnUP_L1_L2_DP2_stay_open_time,
                          btnUP_L1_L2_use_DP3,
                          btnUP_L1_L2_DP3_open_start_delay,
                          btnUP_L1_L2_DP3_stay_open_time,
                          btnUP_L1_L2_use_DP4,
                          btnUP_L1_L2_DP4_open_start_delay,
                          btnUP_L1_L2_DP4_stay_open_time,
                          btnUP_L1_L2_use_DP5,
                          btnUP_L1_L2_DP5_open_start_delay,
                          btnUP_L1_L2_DP5_stay_open_time,
                          btnUP_L1_L2_use_DP6,
                          btnUP_L1_L2_DP6_open_start_delay,
                          btnUP_L1_L2_DP6_stay_open_time,
                          btnUP_L1_L2_use_DP7,
                          btnUP_L1_L2_DP7_open_start_delay,
                          btnUP_L1_L2_DP7_stay_open_time,
                          btnUP_L1_L2_use_DP8,
                          btnUP_L1_L2_DP8_open_start_delay,
                          btnUP_L1_L2_DP8_stay_open_time,
                          btnUP_L1_L2_use_DP9,
                          btnUP_L1_L2_DP9_open_start_delay,
                          btnUP_L1_L2_DP9_stay_open_time,
                          btnUP_L1_L2_use_DP10,
                          btnUP_L1_L2_DP10_open_start_delay,
                          btnUP_L1_L2_DP10_stay_open_time);

      break;
    case 'D':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnRight_L1_L2\r\n";
#endif
      marcDuinoButtonPush(btnRight_L1_L2_type, btnRight_L1_L2_MD_func, btnRight_L1_L2_cust_MP3_num, btnRight_L1_L2_cust_LD_type, btnRight_L1_L2_cust_LD_text, btnRight_L1_L2_cust_panel,
                          btnRight_L1_L2_use_DP1,
                          btnRight_L1_L2_DP1_open_start_delay,
                          btnRight_L1_L2_DP1_stay_open_time,
                          btnRight_L1_L2_use_DP2,
                          btnRight_L1_L2_DP2_open_start_delay,
                          btnRight_L1_L2_DP2_stay_open_time,
                          btnRight_L1_L2_use_DP3,
                          btnRight_L1_L2_DP3_open_start_delay,
                          btnRight_L1_L2_DP3_stay_open_time,
                          btnRight_L1_L2_use_DP4,
                          btnRight_L1_L2_DP4_open_start_delay,
                          btnRight_L1_L2_DP4_stay_open_time,
                          btnRight_L1_L2_use_DP5,
                          btnRight_L1_L2_DP5_open_start_delay,
                          btnRight_L1_L2_DP5_stay_open_time,
                          btnRight_L1_L2_use_DP6,
                          btnRight_L1_L2_DP6_open_start_delay,
                          btnRight_L1_L2_DP6_stay_open_time,
                          btnRight_L1_L2_use_DP7,
                          btnRight_L1_L2_DP7_open_start_delay,
                          btnRight_L1_L2_DP7_stay_open_time,
                          btnRight_L1_L2_use_DP8,
                          btnRight_L1_L2_DP8_open_start_delay,
                          btnRight_L1_L2_DP8_stay_open_time,
                          btnRight_L1_L2_use_DP9,
                          btnRight_L1_L2_DP9_open_start_delay,
                          btnRight_L1_L2_DP9_stay_open_time,
                          btnRight_L1_L2_use_DP10,
                          btnRight_L1_L2_DP10_open_start_delay,
                          btnRight_L1_L2_DP10_stay_open_time);

      break;
    case 'E':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnDown_L1_L2.\r\n";
#endif
      marcDuinoButtonPush(btnDown_L1_L2_type, btnDown_L1_L2_MD_func, btnDown_L1_L2_cust_MP3_num, btnDown_L1_L2_cust_LD_type, btnDown_L1_L2_cust_LD_text, btnDown_L1_L2_cust_panel,
                          btnDown_L1_L2_use_DP1,
                          btnDown_L1_L2_DP1_open_start_delay,
                          btnDown_L1_L2_DP1_stay_open_time,
                          btnDown_L1_L2_use_DP2,
                          btnDown_L1_L2_DP2_open_start_delay,
                          btnDown_L1_L2_DP2_stay_open_time,
                          btnDown_L1_L2_use_DP3,
                          btnDown_L1_L2_DP3_open_start_delay,
                          btnDown_L1_L2_DP3_stay_open_time,
                          btnDown_L1_L2_use_DP4,
                          btnDown_L1_L2_DP4_open_start_delay,
                          btnDown_L1_L2_DP4_stay_open_time,
                          btnDown_L1_L2_use_DP5,
                          btnDown_L1_L2_DP5_open_start_delay,
                          btnDown_L1_L2_DP5_stay_open_time,
                          btnDown_L1_L2_use_DP6,
                          btnDown_L1_L2_DP6_open_start_delay,
                          btnDown_L1_L2_DP6_stay_open_time,
                          btnDown_L1_L2_use_DP7,
                          btnDown_L1_L2_DP7_open_start_delay,
                          btnDown_L1_L2_DP7_stay_open_time,
                          btnDown_L1_L2_use_DP8,
                          btnDown_L1_L2_DP8_open_start_delay,
                          btnDown_L1_L2_DP8_stay_open_time,
                          btnDown_L1_L2_use_DP9,
                          btnDown_L1_L2_DP9_open_start_delay,
                          btnDown_L1_L2_DP9_stay_open_time,
                          btnDown_L1_L2_use_DP10,
                          btnDown_L1_L2_DP10_open_start_delay,
                          btnDown_L1_L2_DP10_stay_open_time);

      break;
    case 'F':
#ifdef SHADOW_DEBUG
      output += "MarcDuino Button ";
      output += marcDuinoCommand;
      output += " - btnLeft_L1_L2.\r\n";
#endif
      marcDuinoButtonPush(btnLeft_L1_L2_type, btnLeft_L1_L2_MD_func, btnLeft_L1_L2_cust_MP3_num, btnLeft_L1_L2_cust_LD_type, btnLeft_L1_L2_cust_LD_text, btnLeft_L1_L2_cust_panel,
                          btnLeft_L1_L2_use_DP1,
                          btnLeft_L1_L2_DP1_open_start_delay,
                          btnLeft_L1_L2_DP1_stay_open_time,
                          btnLeft_L1_L2_use_DP2,
                          btnLeft_L1_L2_DP2_open_start_delay,
                          btnLeft_L1_L2_DP2_stay_open_time,
                          btnLeft_L1_L2_use_DP3,
                          btnLeft_L1_L2_DP3_open_start_delay,
                          btnLeft_L1_L2_DP3_stay_open_time,
                          btnLeft_L1_L2_use_DP4,
                          btnLeft_L1_L2_DP4_open_start_delay,
                          btnLeft_L1_L2_DP4_stay_open_time,
                          btnLeft_L1_L2_use_DP5,
                          btnLeft_L1_L2_DP5_open_start_delay,
                          btnLeft_L1_L2_DP5_stay_open_time,
                          btnLeft_L1_L2_use_DP6,
                          btnLeft_L1_L2_DP6_open_start_delay,
                          btnLeft_L1_L2_DP6_stay_open_time,
                          btnLeft_L1_L2_use_DP7,
                          btnLeft_L1_L2_DP7_open_start_delay,
                          btnLeft_L1_L2_DP7_stay_open_time,
                          btnLeft_L1_L2_use_DP8,
                          btnLeft_L1_L2_DP8_open_start_delay,
                          btnLeft_L1_L2_DP8_stay_open_time,
                          btnLeft_L1_L2_use_DP9,
                          btnLeft_L1_L2_DP9_open_start_delay,
                          btnLeft_L1_L2_DP9_stay_open_time,
                          btnLeft_L1_L2_use_DP10,
                          btnLeft_L1_L2_DP10_open_start_delay,
                          btnLeft_L1_L2_DP10_stay_open_time);
      break;

    default:
#ifdef SHADOW_DEBUG
      output += "Invalid Sound Command\r\n";
#endif

  }
}

void ps3marcDuinoControl(PS3BT* myPS3 = PS3Nav, int controllerNumber = 1)
{

  if (!(myPS3->getButtonPress(L1) || myPS3->getButtonPress(L2) || myPS3->getButtonPress(PS)))
  {
    if (myPS3->getButtonClick(UP))          processmarcDuinoCommand('1');
    else if (myPS3->getButtonClick(RIGHT))  processmarcDuinoCommand('2');
    else if (myPS3->getButtonClick(DOWN))   processmarcDuinoCommand('3');
    else if (myPS3->getButtonClick(LEFT))   processmarcDuinoCommand('4');
  }
  else if ((myPS3->getButtonPress(L2)) && (!(myPS3->getButtonPress(L1) || myPS3->getButtonPress(PS))))
  {
    if (myPS3->getButtonClick(UP))          processmarcDuinoCommand('5');
    else if (myPS3->getButtonClick(RIGHT))  processmarcDuinoCommand('6');
    else if (myPS3->getButtonClick(DOWN))   processmarcDuinoCommand('7');
    else if (myPS3->getButtonClick(LEFT))   processmarcDuinoCommand('8');
  }
  else if ((myPS3->getButtonPress(L1)) && (!(myPS3->getButtonPress(L2) || myPS3->getButtonPress(PS))))
  {
    if (myPS3->getButtonClick(UP))          processmarcDuinoCommand('9');
    else if (myPS3->getButtonClick(RIGHT))  processmarcDuinoCommand('0');
    else if (myPS3->getButtonClick(DOWN))   processmarcDuinoCommand('A');
    //else if (myPS3->getButtonClick(LEFT))   processmarcDuinoCommand('B');
    else if (myPS3->getButtonClick(CROSS))  processmarcDuinoCommand('+');
    else if (myPS3->getButtonClick(CIRCLE)) processmarcDuinoCommand('-');
  }
  else if ((myPS3->getButtonPress(L1) && myPS3->getButtonPress(L2)) && (!(myPS3->getButtonPress(PS))))
  {
    if (myPS3->getButtonClick(UP))          processmarcDuinoCommand('C');
    else if (myPS3->getButtonClick(RIGHT))  processmarcDuinoCommand('D');
    else if (myPS3->getButtonClick(DOWN))   processmarcDuinoCommand('E');
    else if (myPS3->getButtonClick(LEFT))   processmarcDuinoCommand('F');
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
  if ((millis() - previousDomeMillis) < (2 * serialLatency) ) return;

  int domeRotationSpeed = 0;
  int ps3NavControlSpeed = 0;
  int ps3Nav2ControlSpeed = 0;
  if (PS3Nav->PS3NavigationConnected) ps3NavControlSpeed = ps3DomeDrive(PS3Nav, 1);
  if (PS3Nav2->PS3NavigationConnected) ps3Nav2ControlSpeed = ps3DomeDrive(PS3Nav2, 2);

  //In a two controller system, give dome priority to the secondary controller.
  //Only allow the "Primary" controller dome control if the Secondary is NOT spinnning it

  if ( abs(ps3Nav2ControlSpeed) > 0 )
  {
    domeRotationSpeed = ps3Nav2ControlSpeed;
  } else
  {
    domeRotationSpeed = ps3NavControlSpeed;
  }
  rotateDome(domeRotationSpeed, "Controller Move");
}

void toggleSettings()
{
  if (PS3Nav->PS3NavigationConnected) ps3ToggleSettings(PS3Nav);
  if (PS3Nav2->PS3NavigationConnected) ps3ToggleSettings(PS3Nav2);
}

void marcDuinoControl()
{
  if (PS3Nav->PS3NavigationConnected) ps3marcDuinoControl(PS3Nav, 1);
  if (PS3Nav2->PS3NavigationConnected) ps3marcDuinoControl(PS3Nav2, 2);
}


void openUtilArm(int arm)
{
  //When passed a position - this can "partially" open the arms.
  //Great for more interaction
  moveUtilArm(arm, utilArmOpenPos);
}

void closeUtilArm(int arm)
{
  moveUtilArm(arm, utilArmClosedPos);
}

void waveUtilArm(int arm)
{
  switch (arm)
  {
    case UTIL_ARM_TOP:
      if (isUtilArmTopOpen == false) {
        openUtilArm(UTIL_ARM_TOP);
      } else {
        closeUtilArm(UTIL_ARM_TOP);
      }
      break;
    case UTIL_ARM_BOTTOM:
      if (isUtilArmBottomOpen == false) {
        openUtilArm(UTIL_ARM_BOTTOM);
      } else {
        closeUtilArm(UTIL_ARM_BOTTOM);
      }
      break;
  }
}

void moveUtilArm(int arm, int position)
{
  switch (arm)
  {
    case UTIL_ARM_TOP:
      if (position == utilArmClosedPos)
      {
        btnCross_MD_func = 77;
        isUtilArmTopOpen = false;
      } else
      {
        btnCross_MD_func = 76;
        isUtilArmTopOpen = true;
      }
     marcDuinoButtonPush (btnCross_type, btnCross_MD_func, btnCross_cust_MP3_num, btnCross_cust_LD_type, btnCross_cust_LD_text, btnCross_cust_panel,
                          btnCross_use_DP1,
                          btnCross_DP1_open_start_delay,
                          btnCross_DP1_stay_open_time,
                          btnCross_use_DP2,
                          btnCross_DP2_open_start_delay,
                          btnCross_DP2_stay_open_time,
                          btnCross_use_DP3,
                          btnCross_DP3_open_start_delay,
                          btnCross_DP3_stay_open_time,
                          btnCross_use_DP4,
                          btnCross_DP4_open_start_delay,
                          btnCross_DP4_stay_open_time,
                          btnCross_use_DP5,
                          btnCross_DP5_open_start_delay,
                          btnCross_DP5_stay_open_time,
                          btnCross_use_DP6,
                          btnCross_DP6_open_start_delay,
                          btnCross_DP6_stay_open_time,
                          btnCross_use_DP7,
                          btnCross_DP7_open_start_delay,
                          btnCross_DP7_stay_open_time,
                          btnCross_use_DP8,
                          btnCross_DP8_open_start_delay,
                          btnCross_DP8_stay_open_time,
                          btnCross_use_DP9,
                          btnCross_DP9_open_start_delay,
                          btnCross_DP9_stay_open_time,
                          btnCross_use_DP10,
                          btnCross_DP10_open_start_delay,
                          btnCross_DP10_stay_open_time);
      break;
    case UTIL_ARM_BOTTOM:
      if (position == utilArmClosedPos)
      {
        btnCircle_MD_func = 75;
        isUtilArmBottomOpen = false;
      } else
      {
        btnCircle_MD_func = 74;
        isUtilArmBottomOpen = true;
      }
      marcDuinoButtonPush(btnCircle_type, btnCircle_MD_func, btnCircle_cust_MP3_num, btnCircle_cust_LD_type, btnCircle_cust_LD_text, btnCircle_cust_panel,
                          btnCircle_use_DP1,
                          btnCircle_DP1_open_start_delay,
                          btnCircle_DP1_stay_open_time,
                          btnCircle_use_DP2,
                          btnCircle_DP2_open_start_delay,
                          btnCircle_DP2_stay_open_time,
                          btnCircle_use_DP3,
                          btnCircle_DP3_open_start_delay,
                          btnCircle_DP3_stay_open_time,
                          btnCircle_use_DP4,
                          btnCircle_DP4_open_start_delay,
                          btnCircle_DP4_stay_open_time,
                          btnCircle_use_DP5,
                          btnCircle_DP5_open_start_delay,
                          btnCircle_DP5_stay_open_time,
                          btnCircle_use_DP6,
                          btnCircle_DP6_open_start_delay,
                          btnCircle_DP6_stay_open_time,
                          btnCircle_use_DP7,
                          btnCircle_DP7_open_start_delay,
                          btnCircle_DP7_stay_open_time,
                          btnCircle_use_DP8,
                          btnCircle_DP8_open_start_delay,
                          btnCircle_DP8_stay_open_time,
                          btnCircle_use_DP9,
                          btnCircle_DP9_open_start_delay,
                          btnCircle_DP9_stay_open_time,
                          btnCircle_use_DP10,
                          btnCircle_DP10_open_start_delay,
                          btnCircle_DP10_stay_open_time);
      break;
  }
}

void openPanels()
{
  //When passed a position - this can "partially" open the arms.
  //Great for more interaction
  movePanels(PanelOpenPos);
}

void closePanels()
{
  movePanels(PanelClosedPos);
}

void wavePanels()
{
  if (isPanelsOpen == false) {
    #ifdef SHADOW_DEBUG
      output += "Panels Closed, Open them.\r\n";
    #endif
    openPanels();
    isPanelsOpen = true;
  } else {
    #ifdef SHADOW_DEBUG
      output += "Panels Open, Open them.\r\n";
    #endif
    closePanels();
    isPanelsOpen = false;
  }
}

void movePanels(int position)
{
  switch (position)
  {
    case PanelOpenPos:
      #ifdef SHADOW_DEBUG
          output += "Opening All Panels\r\n";
      #endif
      btnCircle_L1_L2_MD_func = 33;
      break;
    case PanelClosedPos:
      #ifdef SHADOW_DEBUG
          output += "Closing All Panels\r\n";
      #endif
      btnCircle_L1_L2_MD_func = 36;
      break;
  }
  
  marcDuinoButtonPush(btnCircle_L1_L2_type, btnCircle_L1_L2_MD_func, btnCircle_L1_L2_cust_MP3_num, btnCircle_L1_L2_cust_LD_type, btnCircle_L1_L2_cust_LD_text, btnCircle_L1_L2_cust_panel,
                    btnCircle_L1_L2_use_DP1,
                    btnCircle_L1_L2_DP1_open_start_delay,
                    btnCircle_L1_L2_DP1_stay_open_time,
                    btnCircle_L1_L2_use_DP2,
                    btnCircle_L1_L2_DP2_open_start_delay,
                    btnCircle_L1_L2_DP2_stay_open_time,
                    btnCircle_L1_L2_use_DP3,
                    btnCircle_L1_L2_DP3_open_start_delay,
                    btnCircle_L1_L2_DP3_stay_open_time,
                    btnCircle_L1_L2_use_DP4,
                    btnCircle_L1_L2_DP4_open_start_delay,
                    btnCircle_L1_L2_DP4_stay_open_time,
                    btnCircle_L1_L2_use_DP5,
                    btnCircle_L1_L2_DP5_open_start_delay,
                    btnCircle_L1_L2_DP5_stay_open_time,
                    btnCircle_L1_L2_use_DP6,
                    btnCircle_L1_L2_DP6_open_start_delay,
                    btnCircle_L1_L2_DP6_stay_open_time,
                    btnCircle_L1_L2_use_DP7,
                    btnCircle_L1_L2_DP7_open_start_delay,
                    btnCircle_L1_L2_DP7_stay_open_time,
                    btnCircle_L1_L2_use_DP8,
                    btnCircle_L1_L2_DP8_open_start_delay,
                    btnCircle_L1_L2_DP8_stay_open_time,
                    btnCircle_L1_L2_use_DP9,
                    btnCircle_L1_L2_DP9_open_start_delay,
                    btnCircle_L1_L2_DP9_stay_open_time,
                    btnCircle_L1_L2_use_DP10,
                    btnCircle_L1_L2_DP10_open_start_delay,
                    btnCircle_L1_L2_DP10_stay_open_time);
}

void openDoors()
{
  //When passed a position - this can "partially" open the arms.
  //Great for more interaction
  moveDoors(DoorsOpenPos);
}

void closeDoors()
{
  moveDoors(DoorsClosedPos);
}

void waveDoors()
{
  if (isDoorsOpen == false) {
    #ifdef SHADOW_DEBUG
      output += "Doors Closed, Open them.\r\n";
    #endif
    openDoors();
    isDoorsOpen = true;
  } else {
    #ifdef SHADOW_DEBUG
      output += "Doors Open, Open them.\r\n";
    #endif
    closeDoors();
    isDoorsOpen = false;
  }
}

void moveDoors(int position)
{
  switch (position)
  {
    case DoorsOpenPos:
      #ifdef SHADOW_DEBUG
          output += "Opening All Panels\r\n";
      #endif
      btnLeft_L1_MD_func = 72;
      break;
    case DoorsClosedPos:
      #ifdef SHADOW_DEBUG
          output += "Closing All Panels\r\n";
      #endif
      btnLeft_L1_MD_func = 73;
      break;
  }
  
  marcDuinoButtonPush(btnLeft_L1_type, btnLeft_L1_MD_func, btnLeft_L1_cust_MP3_num, btnLeft_L1_cust_LD_type, btnLeft_L1_cust_LD_text, btnLeft_L1_cust_panel,
                    btnLeft_L1_use_DP1,
                    btnLeft_L1_DP1_open_start_delay,
                    btnLeft_L1_DP1_stay_open_time,
                    btnLeft_L1_use_DP2,
                    btnLeft_L1_DP2_open_start_delay,
                    btnLeft_L1_DP2_stay_open_time,
                    btnLeft_L1_use_DP3,
                    btnLeft_L1_DP3_open_start_delay,
                    btnLeft_L1_DP3_stay_open_time,
                    btnLeft_L1_use_DP4,
                    btnLeft_L1_DP4_open_start_delay,
                    btnLeft_L1_DP4_stay_open_time,
                    btnLeft_L1_use_DP5,
                    btnLeft_L1_DP5_open_start_delay,
                    btnLeft_L1_DP5_stay_open_time,
                    btnLeft_L1_use_DP6,
                    btnLeft_L1_DP6_open_start_delay,
                    btnLeft_L1_DP6_stay_open_time,
                    btnLeft_L1_use_DP7,
                    btnLeft_L1_DP7_open_start_delay,
                    btnLeft_L1_DP7_stay_open_time,
                    btnLeft_L1_use_DP8,
                    btnLeft_L1_DP8_open_start_delay,
                    btnLeft_L1_DP8_stay_open_time,
                    btnLeft_L1_use_DP9,
                    btnLeft_L1_DP9_open_start_delay,
                    btnLeft_L1_DP9_stay_open_time,
                    btnLeft_L1_use_DP10,
                    btnLeft_L1_DP10_open_start_delay,
                    btnLeft_L1_DP10_stay_open_time);
}

// =======================================================================================
//          Flash Coin Slot LED Function
// =======================================================================================
void flashCoinSlotLEDs()
{
  for (int i = 0; i < numberOfCoinSlotLEDs; i++)
  {
    if (millis() > nextCoinSlotLedFlash[i])
    {
      if (coinSlotLedState[i] == LOW) coinSlotLedState[i] = HIGH;
      else coinSlotLedState[i] = LOW;
      digitalWrite(COIN_SLOT_LED_PINS[i], coinSlotLedState[i]);
      nextCoinSlotLedFlash[i] = millis() + random(100, 1000) ; // next toggle random time
    }
  }
}

// =======================================================================================
// This function handles the processing of custom MarcDuino panel routines
// =======================================================================================
void custMarcDuinoPanel()
{

  // Open & Close Logic: Dome Panel #1
  if (DP1_Status == 1)
  {

    if ((DP1_start + (DP1_s_delay * 1000)) < millis())
    {

      Serial1.print(":OP01\r");
      DP1_Status = 2;
    }

  }

  if (DP1_Status == 2)
  {

    if ((DP1_start + ((DP1_s_delay + DP1_o_time) * 1000)) < millis())
    {

      Serial1.print(":CL01\r");
      DP1_Status = 0;
    }

  }

  // Open & Close Logic: Dome Panel #2
  if (DP2_Status == 1)
  {

    if ((DP2_start + (DP2_s_delay * 1000)) < millis())
    {

      Serial1.print(":OP02\r");
      DP2_Status = 2;
    }

  }

  if (DP2_Status == 2)
  {

    if ((DP2_start + ((DP2_s_delay + DP2_o_time) * 1000)) < millis())
    {

      Serial1.print(":CL02\r");
      DP2_Status = 0;
    }

  }

  // Open & Close Logic: Dome Panel #3
  if (DP3_Status == 1)
  {

    if ((DP3_start + (DP3_s_delay * 1000)) < millis())
    {

      Serial1.print(":OP03\r");
      DP3_Status = 2;
    }

  }

  if (DP3_Status == 2)
  {

    if ((DP3_start + ((DP3_s_delay + DP3_o_time) * 1000)) < millis())
    {

      Serial1.print(":CL03\r");
      DP3_Status = 0;
    }

  }

  // Open & Close Logic: Dome Panel #4
  if (DP4_Status == 1)
  {

    if ((DP4_start + (DP4_s_delay * 1000)) < millis())
    {

      Serial1.print(":OP04\r");
      DP4_Status = 2;
    }

  }

  if (DP4_Status == 2)
  {

    if ((DP4_start + ((DP4_s_delay + DP4_o_time) * 1000)) < millis())
    {

      Serial1.print(":CL04\r");
      DP4_Status = 0;
    }

  }

  // Open & Close Logic: Dome Panel #5
  if (DP5_Status == 1)
  {

    if ((DP5_start + (DP5_s_delay * 1000)) < millis())
    {

      Serial1.print(":OP05\r");
      DP5_Status = 2;
    }

  }

  if (DP5_Status == 2)
  {

    if ((DP5_start + ((DP5_s_delay + DP5_o_time) * 1000)) < millis())
    {

      Serial1.print(":CL05\r");
      DP5_Status = 0;
    }

  }

  // Open & Close Logic: Dome Panel #6
  if (DP6_Status == 1)
  {

    if ((DP6_start + (DP6_s_delay * 1000)) < millis())
    {

      Serial1.print(":OP06\r");
      DP6_Status = 2;
    }

  }

  if (DP6_Status == 2)
  {

    if ((DP6_start + ((DP6_s_delay + DP6_o_time) * 1000)) < millis())
    {

      Serial1.print(":CL06\r");
      DP6_Status = 0;
    }

  }

  // Open & Close Logic: Dome Panel #7
  if (DP7_Status == 1)
  {

    if ((DP7_start + (DP7_s_delay * 1000)) < millis())
    {

      Serial1.print(":OP07\r");
      DP7_Status = 2;
    }

  }

  if (DP7_Status == 2)
  {

    if ((DP7_start + ((DP7_s_delay + DP7_o_time) * 1000)) < millis())
    {

      Serial1.print(":CL07\r");
      DP7_Status = 0;
    }

  }

  // Open & Close Logic: Dome Panel #8
  if (DP8_Status == 1)
  {

    if ((DP8_start + (DP8_s_delay * 1000)) < millis())
    {

      Serial1.print(":OP08\r");
      DP8_Status = 2;
    }

  }

  if (DP8_Status == 2)
  {

    if ((DP8_start + ((DP8_s_delay + DP8_o_time) * 1000)) < millis())
    {

      Serial1.print(":CL08\r");
      DP8_Status = 0;
    }

  }

  // Open & Close Logic: Dome Panel #9
  if (DP9_Status == 1)
  {

    if ((DP9_start + (DP9_s_delay * 1000)) < millis())
    {

      Serial1.print(":OP09\r");
      DP9_Status = 2;
    }

  }

  if (DP9_Status == 2)
  {

    if ((DP9_start + ((DP9_s_delay + DP9_o_time) * 1000)) < millis())
    {

      Serial1.print(":CL09\r");
      DP9_Status = 0;
    }

  }

  // Open & Close Logic: Dome Panel #10
  if (DP10_Status == 1)
  {

    if ((DP10_start + (DP10_s_delay * 1000)) < millis())
    {

      Serial1.print(":OP10\r");
      DP10_Status = 2;
    }

  }

  if (DP10_Status == 2)
  {

    if ((DP10_start + ((DP10_s_delay + DP10_o_time) * 1000)) < millis())
    {

      Serial1.print(":CL10\r");
      DP10_Status = 0;
    }

  }

  // If all the panels have now closed - close out the custom routine
  if (DP1_Status + DP2_Status + DP3_Status + DP4_Status + DP5_Status + DP6_Status + DP7_Status + DP8_Status + DP9_Status + DP10_Status == 0)
  {

    runningCustRoutine = false;

  }
}

// =======================================================================================
//          Print Output Function
// =======================================================================================

void printOutput()
{
  if (output != "")
  {
    if (Serial) Serial.println(output);
    output = ""; // Reset output string
  }
}
