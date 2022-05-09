#include <ESP8266WiFi.h> //Thư viện ESP8266
#include <ESP8266WebServer.h>  //Thư viện WebServer
#include <DNSServer.h> 
#include <ESP8266mDNS.h>
#include <ESP8266Ping.h>
#include <EEPROM.h> 
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
String auth;
const int ledPin = 5;
const int btnPin = 14;
BlynkTimer timer;
BlynkTimer timerCheckconnect;
void checkPhysicalButton();
int ledState = HIGH;
int btnState = HIGH;
const int buttonPin = 0;
boolean internetStatus = 0;
boolean wifimode;

DNSServer dnsServer;   
ESP8266WebServer webServer(80);
const IPAddress apIP(192, 168, 1, 1);  
String ssid;                       //Thông tin access point
String pass;
String ssidList;

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V2);
}
BLYNK_WRITE(V2) {
  ledState = param.asInt();
  digitalWrite(ledPin, ledState);
}
void checkPhysicalButton()
{
  if (digitalRead(btnPin) == LOW) {
    // btnState is used to avoid sequential toggles
    if (btnState != LOW) {

      // Toggle LED state
      ledState = !ledState;
      digitalWrite(ledPin, ledState);

      // Update Button Widget
      Blynk.virtualWrite(V2, ledState);
    }
    btnState = LOW;
  } else {
    btnState = HIGH;
  }
}
void checkInternet(){
  bool ret = Ping.ping("www.google.com");
  if(ret){
    Serial.println("Đã ping đến google thành công");
    //Blynk.begin(auth.c_str(), ssid.c_str(), pass.c_str());
    internetStatus = 1;
  }else{
    Serial.println("Mất kết nối internet");
    internetStatus = 0;
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  EEPROM.begin(512);       //Khởi động bộ nhớ EEPROM
  delay(10);
  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT_PULLUP);
  digitalWrite(ledPin, ledState);
  pinMode(buttonPin, INPUT_PULLUP); //Khai báo nút nhấn
  attachInterrupt(buttonPin, ClearEEPROM, FALLING);     //Ngắt khi chuyển từ mứ cao sang thấp
  timer.setInterval(100L, checkPhysicalButton);
  timerCheckconnect.setInterval(60000L,checkInternet);
  if(restore_config()){
    if(checkConnection()){   
       Serial.println("Ket noi wifi thanh cong!");
       bool ret = Ping.ping("www.google.com");
       if(ret){
          Serial.println("Đã ping đến google thành công");
          Blynk.begin(auth.c_str(), ssid.c_str(), pass.c_str());
          internetStatus = 1;
       }
      wifimode=0;
      if (!MDNS.begin("E-SMART")) {
        Serial.println("Error setting up MDNS responder!");
        while (1) {
          delay(1000);
        }
      }
      Serial.println("mDNS responder started");
      MDNS.addService("http","tcp",80);
      startWebServer();
      return;      
    }
  }
  WiFi.mode(WIFI_AP);          // Chế đô Access Point
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  ssid = "E-SMART";
  pass = "12345678";
  WiFi.softAP(ssid,pass); 
  Serial.print("Please connect to SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(pass);
  dnsServer.start(53, "*", apIP);
  wifimode=1;
  startWebServer();  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(internetStatus == 1){
    Blynk.run();
  }
  timer.run();
  timerCheckconnect.run();
  if(wifimode==1){
    dnsServer.processNextRequest();
  }else{
    MDNS.update();
  }
  webServer.handleClient();
}
void ClearEEPROM(){
  for (int i = 0; i < 96; ++i) {
    EEPROM.write(i, 0);               //xoa bo nho EEPROM
    digitalWrite(ledPin,!digitalRead(ledPin));
    delay(20);
  }
  EEPROM.commit();
}
//----------restore config--------
boolean restore_config(){
  Serial.println("Reading EEPROM...");
  if(EEPROM.read(0) != 0){
    Serial.println("Reading Wifi Setup...");
    ssid = "";
    pass= "";
    for (int i=0; i<32; ++i){
      ssid += char(EEPROM.read(i));
    }
    Serial.print("SSID: ");
    Serial.println(ssid);
    for (int i=32; i<96; ++i){
      pass += char(EEPROM.read(i));
    }
    Serial.print("PASSWORD: ");
    Serial.println(pass);
    for (int i=96; i<128; ++i){
      auth += char(EEPROM.read(i));
    }
    Serial.print("AUTH TOKEN: ");
    Serial.println(auth);
    WiFi.begin(ssid.c_str(), pass.c_str()); 
    return true;
  }else{
    Serial.println("Config not found!");
    return false;
  }
}
boolean checkConnection() {
  Serial.println();
  Serial.print("Check connecting to ");
  Serial.println(ssid);
  int count=0;
  while(count < 50){
    if(WiFi.status() == WL_CONNECTED){
      Serial.println();
      Serial.println("Connected!");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      return (true);
    }
    Serial.print(".");
    count++;
    delay(500);
  }
  Serial.println("Timed out.");
  return false;
}
void startWebServer() {
  if(wifimode == 1){
      int n = WiFi.scanNetworks();    //quet cac mang wifi xung quanh xem co bao nhieu mang
      delay(100);
      Serial.println("");
      for (int i = 0; i < n; ++i) {    //dua danh sach wifi vao list
        ssidList += "<option value=\"";
        ssidList += WiFi.SSID(i);
        ssidList += "\">";
        ssidList += WiFi.SSID(i);
        ssidList += "</option>";
      }
      delay(100);
      Serial.print("Starting Web Server at ");
      Serial.println(WiFi.softAPIP());
      webServer.on("/", []() {
        String s = "<h1>WIFI SETTINGS</h1><p>Please enter your password by selecting the SSID.</p>";
        s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
        s += ssidList;
        s += "</select><br>Password: <input name=\"pass\" length=64 required type=\"password\"><br>";
        s += "Auth Token: <input name=\"auth\" length=32 required ><br>";
        s += "<input type=\"submit\"></form>";
        webServer.send(200, "text/html", makePage("WIFI SETTINGS", s));
      });
      webServer.on("/setap", []() {
        ssid = urlDecode(webServer.arg("ssid"));
        Serial.print("SSID: ");
        Serial.println(ssid);
        pass = urlDecode(webServer.arg("pass"));
        Serial.print("Password: ");
        Serial.println(pass);
        auth = urlDecode(webServer.arg("auth"));
        Serial.print("Password: ");
        Serial.println(auth);
  
        Serial.println("Writing SSID to EEPROM");
        for (int i = 0; i < 128; ++i) {
          EEPROM.write(i, 0);               //xoa bo nho EEPROM
          digitalWrite(ledPin,!digitalRead(ledPin));
          delay(20);
        }
        for (int i = 0; i < ssid.length(); ++i) {
          EEPROM.write(i, ssid[i]);
        }
        Serial.println("Writing Password to EEPROM");
        for (int i = 0; i < pass.length(); ++i) {
          EEPROM.write(32 + i, pass[i]);
        }
        Serial.println("Writing auth EEPROM");
        for (int i = 0; i < auth.length(); ++i) {
          EEPROM.write(96 + i, auth[i]);
        }
        EEPROM.commit();
        String s = "<h1>Setup complete.</h1><p>Device connected to \"";
        s += ssid;
        s += "\"";
        webServer.send(200, "text/html", makePage("WIFI SETTINGS", s));
      });
  }else{
    webServer.on("/", []() {
      String s = "<h1>CONTROL PANNEL</h1></p>";
      webServer.send(200, "text/html", makePage("CONTROL PANNEL", s));
    });
  }
  webServer.on("/on", []() {
    digitalWrite(ledPin,LOW);
    String s = "<h1>CONTROL PANNEL</h1></p>";
    webServer.send(200, "text/html", makePage("CONTROL PANNEL", s));
  });
  webServer.on("/off", []() {
    digitalWrite(ledPin,HIGH);
    String s = "<h1>CONTROL PANNEL</h1></p>";
    webServer.send(200, "text/html", makePage("CONTROL PANNEL", s));
  });
  webServer.on("/restart", []() {
    ESP.restart();
  });
  webServer.on("/reset", []() {
    ClearEEPROM();
    wifimode=1;
    ESP.restart();
  });
  webServer.begin();
}

String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\" charset=\"UTF-8\">";
  s += "<title>";
  s += title;
  s += "</title></head><body>";
  s += "<h1><font color=\"blue\">ĐIỆN THÔNG MINH E-SMART</font></h1>";
  s += contents;
  s += "<p><button><a href=\"/off\" style=\"text-decoration: none\">Switch off</a></button>";
  s += "<button><a href=\"/on\" style=\"text-decoration: none\">Switch on</a></button></p>";
  if(digitalRead(ledPin)){
    s += "<p>LED OFF</p>";
  }else{
    s += "<p>LED ON</p>";
  }
  s += "<p>Click <a href=\"/restart\">restart</a> to restart again.<br>Click <a href=\"/reset\">clear</a> to clear EEPROM and restart again.<br>Click <a href=\"/\">Home</a> to back home</p>";
  s += "</body></html>";
  return s;
}
String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}
