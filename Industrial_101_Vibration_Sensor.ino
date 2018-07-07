/*
THE FOLLOWING INITIATES A CUSTOMER PYTHON SCRIP FOR SENDING UDP STRINGS...
- YOU NEED TO ENSURE THAT THE PYTHON SCRIPT IS LOADED INTO THE OTHER PORTION OF THE CONTROLLER.
https://www.arduino.cc/en/Tutorial/ShellCommands - for receiving commands from python
*/

/*
(1)=> REQUIRED FOR BRIDGE COMMS
(2)=> SERIAL TECH POINT
(3)=> ACC READING POINT

-Need to use bridge to communicate between processors (Arduino and Python)

Resources:
https://www.arduino.cc/en/Tutorial/ADXL3xx
https://www.arduino.cc/en/Tutorial/Bridge
*/

#include <Process.h> // (1)
#include <Bridge.h> // (1)
#include <BridgeServer.h>
#include <BridgeClient.h>

BridgeServer server; // (1) 

int Device_ID = 1;
int Increment_Test = 0;
int Cycle = 0;
String Serial_Comms = "";
int Transmission_Counter = 0; // might not need
int Transmission_Set_Point = 100;

int ypin = A1;                                                                                        // (3) y-axis
int zpin = A2;                                                                                        // (3) z-axis (only on 3-axis models)

int Cycle_Counter = 0;                                                                                // (3) sample cycle counter
const int Samples_Per_Cycle = 9000;  // default 9000                                                  // (3) set sample size
int Sample_Counter = 0;                                                                               // (3) session counter for transmission

int Cycle_Counter_Intensity = 0;   
const int Samples_Per_Cycle_Intensity = 10; // (3) used for filtering intensity readings

String Automatic_Set_Point_Programmed = "No";

int xAxis_Pin = A0;                                                                                   // (3) x-axis sensor pin
int xAxis_Extract_Sensor_Value = 0;                                                                   // (3) value extract from the analogue pin ~0->1000                                                                // vibration trigger setup point value
float xAxis_Vibration_Set_Point = 396;                                                                // (3) vibration trigger setup point value
int xAxis_Vibration_Tolerance = 10;                                                                   // (3) allowable vibration variation +/- of the set point
int xAxis_Vibration_Counter_Quantitative = 0;                                                         // (3) vibration counter for qty occurences
float xAxis_Vibration_Memory_Total_Intensity = 0;                                                     // (3) vibration int total variable
float xAxis_Last_Calculated_Intensity = 0;                                                            // (3) memory for last int reading

int yAxis_Pin = A1;                                                                                   // (3) x-axis sensor pin
int yAxis_Extract_Sensor_Value = 0;                                                                   // (3) value extract from the analogue pin ~0->1000                                                                // vibration trigger setup point value
float yAxis_Vibration_Set_Point = 396;                                                                // (3) vibration trigger setup point value
int yAxis_Vibration_Tolerance = 10;                                                                   // (3) allowable vibration variation +/- of the set point
int yAxis_Vibration_Counter_Quantitative = 0;                                                         // (3) vibration counter for qty occurences
float yAxis_Vibration_Memory_Total_Intensity = 0;                                                     // (3) vibration int total variable
float yAxis_Last_Calculated_Intensity = 0;

int zAxis_Pin = A2;                                                                                   // (3) x-axis sensor pin
int zAxis_Extract_Sensor_Value = 0;                                                                   // (3) value extract from the analogue pin ~0->1000                                                                // vibration trigger setup point value
float zAxis_Vibration_Set_Point = 396;                                                                // (3) vibration trigger setup point value
int zAxis_Vibration_Tolerance = 10;                                                                   // (3) allowable vibration variation +/- of the set point
int zAxis_Vibration_Counter_Quantitative = 0;                                                         // (3) vibration counter for qty occurences
float zAxis_Vibration_Memory_Total_Intensity = 0;                                                     // (3) vibration int total variable
float zAxis_Last_Calculated_Intensity = 0;

int Automatic_Set_Point_Counter = 0;
const int Automatic_Set_Point_Samples = 50;
float xAxis_Automatic_Set_Point_Readings[Automatic_Set_Point_Samples]; 
float yAxis_Automatic_Set_Point_Readings[Automatic_Set_Point_Samples]; 
float zAxis_Automatic_Set_Point_Readings[Automatic_Set_Point_Samples]; 
float xAxis_Automatic_Set_Point_Total = 0;
float yAxis_Automatic_Set_Point_Total = 0;
float zAxis_Automatic_Set_Point_Total = 0;


void setup() {

delay(50);                                                                                            // (3) wait 5 seconds 

Serial.begin(9600);                                                                                   // (2) // open serial connection
Bridge.begin();                                                                                       // (1) // open bridge connection between processor

Serial.println("[Device: " + String(Device_ID) + "]");                                                //
Serial.println("Bridge connection established...");                                                   // (1) 
Serial.println("System Starting... in 5s");                                                           // send start info

// NEED TO WAIT 30 SECONDS
// START CALIBRATION PROCEDURE - FOR INTENSITY VALUE (VALUE DEPENDS ON PHYSICAL ALIGNMENT

// server.listenOnLocalhost();                                                                        // UNDER DEVELOPEMENT
// server.begin();                                                                                    // UNDER DEVELOPEMENT

Automatic_Set_Point_Programmed = "Recording";

delay(5000);                                                                                          // wait 5 seconds  
}

void loop() {

// ///////////////////// INCOMING BRIDGE DATA

//  BridgeClient client = server.accept();                                                            // (1) UNDER DEVELOPMENT
//  if (client) {                                                                                     // (1) UNDER DEVELOPMENT
//  process(client);                                                                                  // (1) UNDER DEVELOPMENT
//  client.stop();                                                                                    // (1) UNDER DEVELOPMENT
//  }                                                                                                 // (1) UNDER DEVELOPMENT
//  delay(50);                                                                                        // (1) UNDER DEVELOPMENT


// ///////////////////// GET READINGS
xAxis_Extract_Sensor_Value = analogRead(xAxis_Pin);                                                   // (3) retrieve the reading from the sensor
yAxis_Extract_Sensor_Value = analogRead(yAxis_Pin);                                                   // (3) retrieve the reading from the sensor
zAxis_Extract_Sensor_Value = analogRead(zAxis_Pin);  

if( Automatic_Set_Point_Programmed == "Running"){

  
// ///////////////////// GENERAL BORDER CALCULATIONS
int xAxis_Allowable_Low = xAxis_Vibration_Set_Point - xAxis_Vibration_Tolerance;                      // (3) determine lower threshold value
int xAxis_Allowable_High = xAxis_Vibration_Set_Point + xAxis_Vibration_Tolerance;                     // (3) determine upper threshold

int yAxis_Allowable_Low = yAxis_Vibration_Set_Point - yAxis_Vibration_Tolerance;                      // (3) determine lower threshold value
int yAxis_Allowable_High = yAxis_Vibration_Set_Point + yAxis_Vibration_Tolerance;  

int zAxis_Allowable_Low = zAxis_Vibration_Set_Point - zAxis_Vibration_Tolerance;                      // (3) determine lower threshold value
int zAxis_Allowable_High = zAxis_Vibration_Set_Point + zAxis_Vibration_Tolerance;  

// ///////////////////// COUNT OCCURENCES
if(xAxis_Extract_Sensor_Value > xAxis_Allowable_High || xAxis_Extract_Sensor_Value < xAxis_Allowable_Low){        // (3) if value is outside of the allowable threshold
xAxis_Vibration_Counter_Quantitative = xAxis_Vibration_Counter_Quantitative + 1;                                  // (3) increment occurrence counter
}

if(yAxis_Extract_Sensor_Value > yAxis_Allowable_High || yAxis_Extract_Sensor_Value < yAxis_Allowable_Low){        // (3) if value is outside of the allowable threshold
yAxis_Vibration_Counter_Quantitative = yAxis_Vibration_Counter_Quantitative + 1;                                  // (3) increment occurrence counter
}

if(zAxis_Extract_Sensor_Value > zAxis_Allowable_High || zAxis_Extract_Sensor_Value < zAxis_Allowable_Low){        // (3) if value is outside of the allowable threshold
zAxis_Vibration_Counter_Quantitative = zAxis_Vibration_Counter_Quantitative + 1;                                  // (3) increment occurrence counter
}
                                                           
// ///////////////////// DETERMINE INTENSITY (UN BAISED)
float xAxis_Determined_Amount = 0;                                                                    // (3) declare variable for intensity value reading
if(xAxis_Extract_Sensor_Value > xAxis_Vibration_Set_Point){                                           // (3) if value in in (+) region
  xAxis_Determined_Amount = xAxis_Extract_Sensor_Value - xAxis_Vibration_Set_Point;
}
if(xAxis_Extract_Sensor_Value < xAxis_Vibration_Set_Point){                                           // (3) if value is in (-) region
  xAxis_Determined_Amount = xAxis_Vibration_Set_Point - xAxis_Extract_Sensor_Value;
}

float yAxis_Determined_Amount = 0;                                                                    // (3) declare variable for intensity value reading
if(yAxis_Extract_Sensor_Value > yAxis_Vibration_Set_Point){                                           // (3) if value in in (+) region
  yAxis_Determined_Amount = yAxis_Extract_Sensor_Value - yAxis_Vibration_Set_Point;
}
if(yAxis_Extract_Sensor_Value < yAxis_Vibration_Set_Point){                                           // (3) if value is in (-) region
  yAxis_Determined_Amount = yAxis_Vibration_Set_Point - yAxis_Extract_Sensor_Value;
}

float zAxis_Determined_Amount = 0;                                                                    // (3) declare variable for intensity value reading
if(zAxis_Extract_Sensor_Value > zAxis_Vibration_Set_Point){                                           // (3) if value in in (+) region
  zAxis_Determined_Amount = zAxis_Extract_Sensor_Value - zAxis_Vibration_Set_Point;
}
if(zAxis_Extract_Sensor_Value <zAxis_Vibration_Set_Point){                                           // (3) if value is in (-) region
  zAxis_Determined_Amount = zAxis_Vibration_Set_Point - zAxis_Extract_Sensor_Value;
}

// ///////////////////// RECORD INTENSITY / CYCLE
xAxis_Vibration_Memory_Total_Intensity = xAxis_Vibration_Memory_Total_Intensity + xAxis_Determined_Amount;        // (3) save value in total variable
yAxis_Vibration_Memory_Total_Intensity = yAxis_Vibration_Memory_Total_Intensity + yAxis_Determined_Amount;        // (3) save value in total variable
zAxis_Vibration_Memory_Total_Intensity = zAxis_Vibration_Memory_Total_Intensity + zAxis_Determined_Amount;        // (3) save value in total variable


// ///////////////////// DETERMINE INTENSITY AVERAGE
if(Cycle_Counter > Samples_Per_Cycle){                                                                // (3) once cycle value hit
xAxis_Last_Calculated_Intensity = xAxis_Vibration_Memory_Total_Intensity/Samples_Per_Cycle;           // (3) determine intensity average
xAxis_Vibration_Memory_Total_Intensity = 0;                                                           // (3) reset intensity variable memory
}

if(Cycle_Counter > Samples_Per_Cycle){                                                                // (3) once cycle value hit
yAxis_Last_Calculated_Intensity = yAxis_Vibration_Memory_Total_Intensity/Samples_Per_Cycle;           // (3) determine intensity average
yAxis_Vibration_Memory_Total_Intensity = 0;                                                           // (3) reset intensity variable memory
}

if(Cycle_Counter > Samples_Per_Cycle){                                                                // (3) once cycle value hit
zAxis_Last_Calculated_Intensity = zAxis_Vibration_Memory_Total_Intensity/Samples_Per_Cycle;           // (3) determine intensity average
zAxis_Vibration_Memory_Total_Intensity = 0;                                                           // (3) reset intensity variable memory
}

// ///////////////////// SEND CYCLE DATA
if(Cycle_Counter > Samples_Per_Cycle){                                                                // (3) once the sample cycle is completed
Send_Values(
  Sample_Counter,
  xAxis_Vibration_Counter_Quantitative,
  xAxis_Last_Calculated_Intensity,
  yAxis_Vibration_Counter_Quantitative,
  yAxis_Last_Calculated_Intensity,
  zAxis_Vibration_Counter_Quantitative,
  zAxis_Last_Calculated_Intensity,
  Device_ID
  );      // (3) activate the send data fnuction
xAxis_Vibration_Counter_Quantitative = 0;                                                             // (3) reset the vibration counter stat
yAxis_Vibration_Counter_Quantitative = 0;                                                             // (3) reset the vibration counter stat
zAxis_Vibration_Counter_Quantitative = 0; 

Cycle_Counter = 0;                                                                                    // (3) reset the cycle memory
Sample_Counter = Sample_Counter + 1;                                                                  // (3) increment the transmitted counter

}
}

Cycle_Counter = Cycle_Counter + 1;                                                                    // increment cycle counter


// ///////////////////// RESETING / CALIBRATING RATING DEVICE

if(Automatic_Set_Point_Programmed == "Recording"){
if(Automatic_Set_Point_Counter < Automatic_Set_Point_Samples){
Serial.println("Recording values=>" + String(Automatic_Set_Point_Counter) + ":" + String(xAxis_Extract_Sensor_Value));  
xAxis_Automatic_Set_Point_Readings[Automatic_Set_Point_Counter] = xAxis_Extract_Sensor_Value;
yAxis_Automatic_Set_Point_Readings[Automatic_Set_Point_Counter] = yAxis_Extract_Sensor_Value;
zAxis_Automatic_Set_Point_Readings[Automatic_Set_Point_Counter] = zAxis_Extract_Sensor_Value;

Automatic_Set_Point_Counter = Automatic_Set_Point_Counter + 1;
}  
}

if((Automatic_Set_Point_Counter >= Automatic_Set_Point_Samples) && (Automatic_Set_Point_Programmed == "Recording")){
Automatic_Set_Point_Programmed = "Calculating";  

for (int i=0; i < Automatic_Set_Point_Samples; i++){
xAxis_Automatic_Set_Point_Total = xAxis_Automatic_Set_Point_Total + xAxis_Automatic_Set_Point_Readings[i];
yAxis_Automatic_Set_Point_Total = yAxis_Automatic_Set_Point_Total + yAxis_Automatic_Set_Point_Readings[i];
zAxis_Automatic_Set_Point_Total = zAxis_Automatic_Set_Point_Total + zAxis_Automatic_Set_Point_Readings[i];

}

xAxis_Vibration_Set_Point = xAxis_Automatic_Set_Point_Total/float(Automatic_Set_Point_Samples);  
yAxis_Vibration_Set_Point = yAxis_Automatic_Set_Point_Total/float(Automatic_Set_Point_Samples);  
zAxis_Vibration_Set_Point = zAxis_Automatic_Set_Point_Total/float(Automatic_Set_Point_Samples);  

String Trans = "CSP (X) =>"+String(xAxis_Vibration_Set_Point)+"CSP (Y) =>"+String(xAxis_Vibration_Set_Point);
Serial.println(Trans);
Automatic_Set_Point_Programmed = "Running";
}

//delay(1000);

}


void Send_Values(int Sample_Counter, int xAxis_Vibration_Counter_Quantitative, float xAxis_Last_Calc, int yAxis_Vibration_Counter_Quantitative, float yAxis_Last_Calc, int zAxis_Vibration_Counter_Quantitative, float zAxis_Last_Calc, int Device_ID){

Serial_Comms = "";
Serial_Comms = Serial_Comms + "Device#:" + String(Device_ID) + ";";                                               // (3) device id
Serial_Comms = Serial_Comms + "Cycle#:" + String(Sample_Counter) + ";";                                           // (3) cycle #
Serial_Comms = Serial_Comms + "X-Axis Vibration Qty:" + String(xAxis_Vibration_Counter_Quantitative) + ";";       // (3) vibration amount
Serial_Comms = Serial_Comms + "X-Axis Vibration Int:" + String(xAxis_Last_Calculated_Intensity) + ";";            // (3) vibration intensity
Serial_Comms = Serial_Comms + "Y-Axis Vibration Qty:" + String(yAxis_Vibration_Counter_Quantitative) + ";";       // (3) vibration amount
Serial_Comms = Serial_Comms + "Y-Axis Vibration Int:" + String(yAxis_Last_Calculated_Intensity) + ";";            // (3) vibration intensity
Serial_Comms = Serial_Comms + "Z-Axis Vibration Qty:" + String(zAxis_Vibration_Counter_Quantitative) + ";";       // (3) vibration amount
Serial_Comms = Serial_Comms + "Z-Axis Vibration Int:" + String(zAxis_Last_Calculated_Intensity) + ";";            // (3) vibration intensity

Process UdpCommand;                                                                                   // (1) establish udp processes
UdpCommand.runShellCommand("python /mnt/sda1/udp.py");                                                // (1) run udp python script
Bridge.put("MEASUREMENT", Serial_Comms);                                                              // (1) send data to linux processor
Serial.println(Serial_Comms);                                                                         // send data to serial connection

}

// void process(BridgeClient client) {                                                                // (1) UNDER DEVELOPMENT
//  String command = client.readStringUntil('/');                                                     // (1) UNDER DEVELOPMENT
//  if (command == "digital") {  }                                                                    // (1) UNDER DEVELOPMENT
//  if (command == "analog") {  }                                                                     // (1) UNDER DEVELOPMENT
// }                                                                                                  // (1) UNDER DEVELOPMENT


