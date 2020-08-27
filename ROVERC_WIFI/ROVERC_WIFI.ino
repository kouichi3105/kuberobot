#include <M5StickC.h>

//Webサーバー関連設定
#include <WiFi.h>
const char* ssid = "YOUR SSID"; //無線ルーターのSSIDを記載
const char* password = "YOUR PASSWORD"; //無線ルーターのパスワードを記載

IPAddress ip(192, 168, 10, 12); //IP固定用
IPAddress gateway(192,168, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress DNS(192, 168, 10, 1);

WiFiServer server(80);

const int servo_pin = 26;
int freq = 50;
int ledChannel = 0;
int resolution = 10;
extern const unsigned char m5stack_startup_music[];

//Variable to store the HTTP request
String header;

//速度設定変数
int8_t speed_sendbuff[4] = {0};

uint8_t FORWARD[4]     = {50, 50, 50, 50};
uint8_t LEFT[4]        = {-50, 50, 50, -50};
uint8_t BACKWARD[4]    = {-50, -50, -50, -50};
uint8_t RIGHT[4]       = {50, -50, -50, 50};
uint8_t ROTATE_R[4]    = {-50, 50, -50, 50};
uint8_t ROTATE_L[4]    = {50, -50, 50, -50};

float limit;

float f, b, l, r, rr, rl = 0.0;

void SetChargingCurrent(uint8_t CurrentLevel)
{
    Wire1.beginTransmission(0x34);
    Wire1.write(0x33);
    Wire1.write(0xC0 | (CurrentLevel & 0x0f));
    Wire1.endTransmission();
}

int8_t I2CWrite1Byte(uint8_t Addr, uint8_t Data)
{
    Wire.beginTransmission(0x38);
    Wire.write(Addr);
    Wire.write(Data);
    return Wire.endTransmission();
}

uint8_t I2CWritebuff(uint8_t Addr, uint8_t *Data, uint16_t Length)
{
    Wire.beginTransmission(0x38);
    Wire.write(Addr);
    for (int i = 0; i < Length; i++)
    {
        Wire.write(Data[i]);
    }
    return Wire.endTransmission();
}

uint8_t setspeed() {  // 前後左右回転それぞれを係数と掛けて足す
  for (int i = 0; i < 4; i++) {
    speed_sendbuff[i] = FORWARD[i] * f;
    speed_sendbuff[i] += BACKWARD[i] * b;
    speed_sendbuff[i] += RIGHT[i] * r;
    speed_sendbuff[i] += LEFT[i] * l;
    speed_sendbuff[i] += ROTATE_L[i] * rl;
    speed_sendbuff[i] += ROTATE_R[i] * rr;
  }
  for (int i = 0; i < 4; i++) {
    // speedが100を超えないようにリミッターをかける
    limit = 100.0 / max(
                         abs(speed_sendbuff[3]), max(
                           abs(speed_sendbuff[2]), max(
                             abs(speed_sendbuff[1]),abs(speed_sendbuff[0])
                           )
                         )
                       );
  }
  //    printf("limit = %f\n", limit);
  if (limit < 1.0) {
    for (int i = 0; i < 4; i++) {
      speed_sendbuff[i] = speed_sendbuff[i] * limit;
    }
  }

  return I2CWritebuff(0x00, (uint8_t *)speed_sendbuff, 4);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  M5.begin();
  M5.update();
  Wire.begin(0, 26, 10000);
  SetChargingCurrent(4);

  //ディスプレイの明るさ調整
  //M5.Axp.ScreenBreath(10);
  
  pinMode(GPIO_NUM_10, OUTPUT);
  digitalWrite(GPIO_NUM_10, HIGH);

  // ディスプレイ表示
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(15, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("RoverC");

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(servo_pin, ledChannel);
  ledcWrite(ledChannel, 256);//0°

  ledcWriteTone(ledChannel, 1250);
  delay(200);
  ledcWriteTone(ledChannel, 0);
  delay(100);
  ledcWriteTone(ledChannel, 1250);
  delay(200);
  ledcWriteTone(ledChannel, 0);
  delay(100);

  //Connect to AP
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  M5.Lcd.println("Connecting to Access Point");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  
  WiFi.config(ip, gateway, subnet, DNS);
  delay(10);
  
  M5.Lcd.println("Connected...");
  M5.Lcd.println("IP: ");
  M5.Lcd.println(WiFi.localIP());
  ledcWriteTone(ledChannel, 1250);
  delay(1000);
  ledcWriteTone(ledChannel, 0);
  delay(100);

  //Start server
  server.begin();

}

void loop() {
  
    //M5.Lcd.println(". ");
    WiFiClient client = server.available();             // listen for incoming clients
        if (client) {                        // if you get a client,
        M5.Lcd.println("New Client.");            // print a message out the serial port
        String currentLine = "";            // make a String to hold incoming data from the client
        while (client.connected()) {            // loop while the client's connected
            if (client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                header += c;
                M5.Lcd.write(c);            // print it out the serial monitor
                if (c == '\n') {            // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");

                        // CSS to style the on/off buttons 
                        // Feel free to change the background-color and font-size attributes to fit your preferences
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".button2 {background-color: #555555;}</style></head>");

                        // Web Page Heading
                        client.println("<body><h1>ROVERC WIFI</h1>");

                        // the content of the HTTP response follows the header:
                        client.print("<p> <a href=\"/G\"><button class=\"button\">GO</button></a></p>");
                        client.print("<p> <a href=\"/L\"><button class=\"button\">LEFT</button></a></p>");
                        client.print("<p> <a href=\"/S\"><button class=\"button\">STOP</button></a></p>");
                        client.print("<p> <a href=\"/R\"><button class=\"button\">RIGHT</button></a></p>");
                        client.print("<p> <a href=\"/B\"><button class=\"button\">BACK</button></a></p>");
                        client.print("<p> <a href=\"/TL\"><button class=\"button\">TURN R</button></a></p>");
                        client.print("<p> <a href=\"/TR\"><button class=\"button\">TURN L</button></a></p>");
                        client.print("<p> <a href=\"/E\"><button class=\"button\">END</button></a></p>");         

                        client.println("</body></html>");

                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    } else {            // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                } else if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;            // add it to the end of the currentLine
                }

                // Check to see if the client request was "GET /H" or "GET /L":
                if (currentLine.endsWith("GET /G")) {
                    M5.Lcd.fillScreen(BLACK);
                    M5.Lcd.setCursor(30, 30);
                    M5.Lcd.setTextColor(WHITE);
                    M5.Lcd.setTextSize(4);
                    M5.Lcd.println("GO");
                    M5.Lcd.println();
                    M5.Lcd.println();
                    go_forward();
                    
                }
                if (currentLine.endsWith("GET /S")) {
                    M5.Lcd.fillScreen(BLACK);
                    M5.Lcd.setCursor(30, 30);
                    M5.Lcd.setTextColor(WHITE);
                    M5.Lcd.setTextSize(4);
                    M5.Lcd.println("STOP");
                    M5.Lcd.println();
                    M5.Lcd.println();
                    go_stop();
                }

                if (currentLine.endsWith("GET /R")) {
                    M5.Lcd.fillScreen(BLACK);
                    M5.Lcd.setCursor(30, 30);
                    M5.Lcd.setTextColor(WHITE);
                    M5.Lcd.setTextSize(4);
                    M5.Lcd.println("RIGHT");
                    M5.Lcd.println();
                    M5.Lcd.println();
                    right();
                    
                }
                if (currentLine.endsWith("GET /L")) {
                    M5.Lcd.fillScreen(BLACK);
                    M5.Lcd.setCursor(30, 30);
                    M5.Lcd.setTextColor(WHITE);
                    M5.Lcd.setTextSize(4);
                    M5.Lcd.println("LEFT");
                    M5.Lcd.println();
                    M5.Lcd.println();
                    left();
                }
                if (currentLine.endsWith("GET /TR")) {
                    M5.Lcd.fillScreen(BLACK);
                    M5.Lcd.setCursor(5, 30);
                    M5.Lcd.setTextColor(WHITE);
                    M5.Lcd.setTextSize(4);
                    M5.Lcd.println("TURN R");
                    M5.Lcd.println();
                    M5.Lcd.println();
                    right_rotate();
                    
                }
                if (currentLine.endsWith("GET /TL")) {
                    M5.Lcd.fillScreen(BLACK);
                    M5.Lcd.setCursor(5, 30);
                    M5.Lcd.setTextColor(WHITE);
                    M5.Lcd.setTextSize(4);
                    M5.Lcd.println("TURN L");
                    M5.Lcd.println();
                    M5.Lcd.println();
                    left_rotate();
                }
                if (currentLine.endsWith("GET /B")) {
                    M5.Lcd.fillScreen(BLACK);
                    M5.Lcd.setCursor(30, 30);
                    M5.Lcd.setTextColor(WHITE);
                    M5.Lcd.setTextSize(4);
                    M5.Lcd.println("BACK");
                    M5.Lcd.println();
                    M5.Lcd.println();
                    go_back();
                }
                if (currentLine.endsWith("GET /E")) {
                    M5.Lcd.fillScreen(BLACK);
                    M5.Lcd.setCursor(30, 30);
                    M5.Lcd.setTextColor(WHITE);
                    M5.Lcd.setTextSize(4);
                    M5.Lcd.println("END");
                    M5.Lcd.println();
                    M5.Lcd.println();
                    go_stop();
                    break;
                }

            }
        }
        // close the connection:
        client.stop();
        client.println();
        M5.Lcd.println("Client Disconnected.");
    }
    M5.update();

}

void playMusic(const uint8_t* music_data, uint16_t sample_rate) {
  uint32_t length = strlen((char*)music_data);
  uint16_t delay_interval = ((uint32_t)1000000 / sample_rate);
    for(int i = 0; i < length; i++) {
      ledcWriteTone(ledChannel, music_data[i]*50);
      delayMicroseconds(delay_interval);
    } 
}

void go_forward(){
  // 前進
  f = 1.0;
  setspeed();
  delay(500);
}

void left(){
  // 左平行移動
  f = 0.0;
  l = 1.0;
  setspeed();
  delay(500);
}

void go_back(){
  // 後退
  l = 0.0;
  b = 1.0;
  setspeed();
  delay(500);
}

void right(){
  // 右平行移動
  b = 0.0;
  r = 1.0;
  setspeed();
  delay(500);
}

void go_stop(){
  // 停止
  f = 0.0;
  b = 0.0;
  l = 0.0;
  r = 0.0;
  rr = 0.0;
  rl = 0.0;
  setspeed();
  delay(1000);
}

void go_right(){
  // 右ナナメ前移動
  f = 0.5;
  r = 0.5;
  setspeed();
  delay(500);
}

void back_right(){
  // 右斜め後ろ移動
  f = 0.0;
  r = 0.5;
  b = 0.5;
  setspeed();
  delay(500);
}

void  back_left(){
  // 左斜め後ろ移動
  r = 0.0;
  l = 0.5;
  b = 0.5;
  setspeed();
  delay(500);
}

void go_left(){
  // 左斜め前移動
  f = 0.5;
  l = 0.5;
  b = 0.0;
  setspeed();
  delay(500);
}

void right_rotate(){
  // 右回転
  f = 0.0;
  l = 0.0;
  rr = 1.0;
  setspeed();
  delay(500);
}

void left_rotate(){
  // 左回転
  rr = 0.0;
  rl = 1.0;
  setspeed();
  delay(500);
}
