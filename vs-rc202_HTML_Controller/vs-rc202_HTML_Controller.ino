#include <vs-rc202.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <FS.h>

#define GO 0
#define LEFT 1
#define RIGHT 2
#define BACK 3
#define STOP 4
#define FUNC1 5
#define FUNC2 6

const char* ui_path = "/index.html";
const char* ssid = "YOUR SSID"; //無線ルーターのSSIDを記載
const char* password = "YOUR PASSWORD"; //無線ルーターのパスワードを記載

IPAddress ip(192, 168, 10, 15);           // for fixed IP Address
IPAddress gateway(192,168, 10, 1);        
IPAddress subnet(255, 255, 255, 0);      
IPAddress DNS(192, 168, 10, 1);          

#define BUF_SIZE 10240
uint8_t buf[BUF_SIZE];
int led_onoff_flag = 0;

WiFiClient client; 

ESP8266WebServer server(80);

int loadUI(){
  File ui = SPIFFS.open(ui_path, "r");
  if (!ui) {
    Serial.println("index.html does not exist.");
    return 0;
  }else if(ui.size() >= BUF_SIZE){
    Serial.print("Could not load file. Make file smaller.");
    return 0;
  }else{
    ui.read(buf,ui.size());
    ui.close();
    return 1;
  }
}

int home_position[1][11] = {
                      {800,0,0,0,0,0,0,0,0,0,0},
                    };

int go[6][11] = {
                  {300,0,0,800,0,0,0,0,0,0,0},
                  {300,-600,-600,800,0,0,0,0,0,0,0},
                  {300,-600,-600,0,0,0,0,0,0,0,0},
                  {300,-600,-600,-800,0,0,0,0,0,0,0},
                  {300,600,600,-800,0,0,0,0,0,0,0},
                  {300,600,600,0,0,0,0,0,0,0,0},
                };
                

int left[5][11] = {
                    {300,0,0,-600,0,0,0,0,0,0,0},
                    {300,300,0,-600,0,0,0,0,0,0,0},
                    {500,300,0,600,0,0,0,0,0,0,0},
                    {300,0,0,600,0,0,0,0,0,0,0},
                    {300,0,0,0,0,0,0,0,0,0,0},
                    };


int right[5][11] = {
                    {300,0,0,600,0,0,0,0,0,0,0},
                    {300,0,-300,600,0,0,0,0,0,0,0},
                    {500,0,-300,-600,0,0,0,0,0,0,0},
                    {300,0,0,-600,0,0,0,0,0,0,0},
                    {300,0,0,0,0,0,0,0,0,0,0},
                    };

int back[6][11] = {
                  {300,0,0,-800,0,0,0,0,0,0,0},
                  {300,-600,-600,-800,0,0,0,0,0,0,0},
                  {300,-600,-600,0,0,0,0,0,0,0,0},
                  {300,-600,-600,800,0,0,0,0,0,0,0},
                  {300,600,600,800,0,0,0,0,0,0,0},
                  {300,600,600,0,0,0,0,0,0,0,0},
                };

int f1[2][11] = {
                      {800,0,0,0,-1500,0,0,0,0,0,0},
                      {800,0,0,0,0},
                    };


int f2[2][11] = {
                      {800,0,0,0,1500,0,0,0,0,0,0},
                      {800,0,0,0,0,0,0,0,0,0,0},
                    };

void selectMotion(){
  
  switch(getMotionNumber()){
    case GO:
      playMotion(go, 6);
      break;
    case LEFT:
      playMotion(left, 5);
      break;
    case RIGHT:
      playMotion(right, 5);
      break;
    case BACK:
      playMotion(back, 6);
      break;
    case STOP:
      stopServo();
      delay(200);
      playMotionOnce(home_position, 1);
      break;
   case FUNC1:
      playMotionOnce(f1, 2);
      break;
   case FUNC2:
      playMotionOnce(f2, 2);
      break;
  }
}

//Send UI page
void handleRoot(){
  server.send(200, "text/html", (char *)buf);
}

void Go(){
  setMotionNumber(GO);
  Serial.println("Go");
  server.send(200, "text/html","Go");
  
}

void Left(){
  setMotionNumber(LEFT);
  Serial.println("Left");
  server.send(200, "text/html","Left");
}

void Right(){
  setMotionNumber(RIGHT);
  Serial.println("Right");
  server.send(200, "text/html","Right");
}

void Back(){
  setMotionNumber(BACK);
  Serial.println("Back");
  server.send(200, "text/html","Back");
}

void Stop(){
  setMotionNumber(STOP);
  Serial.println("Stop");
  server.send(200, "text/html","Stop");
}

void F1(){
  setMotionNumber(FUNC1);
  Serial.println("F1");
  server.send(200, "text/html","F1");
}

void F2(){
  setMotionNumber(FUNC2);
  Serial.println("F2");
  server.send(200, "text/html","F2");
}

void F3(){
  if(led_onoff_flag == 0){
    setLedBrightnessDirect(9,1000);   //Turn on LED
    setLedBrightnessDirect(10,1000);
    led_onoff_flag = 1;
  }else{
    setLedBrightnessDirect(9,0);   //Turn off LED
    setLedBrightnessDirect(10,0);
    led_onoff_flag = 0;
  }
  
  server.send(200, "text/html","F3");
}

void F4(){
  Serial.println("F4");
  server.send(200, "text/html","F4");
  powerOff();
}

void Sens(){

  int sens_data[3];

  //Sensor data from lpc
  int data1 = readSens(1);
  int data2 = readSens(2);
  int data3 = readSens(3);
 
  //Sonic sensor data
  double dist = getDist();

  //Power supply voltage
  int power = readPow();

  //Convert numeric to string
  String st_data1 = String(data1);
  String st_data2 = String(data2);
  String st_data3 = String(data3);  
  String st_data4 = String(dist);
  String st_data5 = String(power);
  
  String res = String(st_data1+","+st_data2+","+st_data3+","+st_data4+","+st_data5);
  server.send(200, "text/html",res);
}

void setup() {

  //debug
  Serial.begin(115200);
  delay(500);

  //Init robot lib
  initLib();
  delay(10);

  buzzerEnable(1);

  //SV1 - 4 servo mode
  servoEnable(1, 1);        //Enable SV1 PWM
  servoEnable(2, 1);        //Enable SV2 PWM
  servoEnable(3, 1);        //Enable SV3 PWM
  servoEnable(4, 1);        //Enable SV4 PWM

  //SV9 and SV10 LED mode
  servoEnable(9, 1);        //Enable SV9 PWM    
  servoEnable(10, 1);       //Enable SV10 PWM
  setLedMode(9, 1);         //Set SV9 LED mode
  setLedMode(10, 1);        //Set SV10 LED mode

  //Offset
  setServoOffset(1,-50);
  setServoOffset(2,0);
  setServoOffset(3,-70);
  setServoOffset(4,100);

  //Read html file
  SPIFFS.begin();
  if(!loadUI()){
    return;
  }

  //Connect to AP
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to Access Point");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.config(ip, gateway, subnet, DNS);
  delay(10);
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID()); 
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Set function
  server.on("/", handleRoot);
  server.on("/go/", Go);
  server.on("/left/", Left);
  server.on("/right/", Right);
  server.on("/back/", Back);
  server.on("/stop/", Stop);
  server.on("/f1/", F1);
  server.on("/f2/", F2);
  server.on("/f3/", F3);
  server.on("/f4/", F4);
  server.on("/sens/", Sens);

  //Start server
  server.begin();

  
}

void loop() {
  
  server.handleClient();
  //Play motion
  selectMotion();
  
}
