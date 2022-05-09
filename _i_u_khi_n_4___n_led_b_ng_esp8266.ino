/*
 * This code will configure ESP8266 in station mode and it will then act as a web server after connecting to a WiFi Router. It will then turn On/Off 4 LEDs as per input from the connected client
 */
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
 
/*Specifying the SSID and Password of the WiFi*/
 
const char *ssid = "le van tuan";
const char *password = "hoangtu14082000  ";
 
//Specifying the Webserver instance to connect with HTTP Port: 80
ESP8266WebServer server(80);
 
//Specifying the Pins connected from LED1 to LED4
uint8_t led1_pin=D0;
uint8_t led2_pin=D1;
uint8_t led3_pin=D2;
uint8_t led4_pin=D5;
 
//Specifying the boolean variables indicating the status of LED1 to LED4
bool led1_status=false, led2_status=false, led3_status=false, led4_status=false;
 
void setup() {
  //Starting the serial communication channel
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
 
 //Output mode for the LED Pins
  pinMode(led1_pin,OUTPUT);
  pinMode(led2_pin,OUTPUT);
  pinMode(led3_pin,OUTPUT);
  pinMode(led4_pin,OUTPUT);
   
  //Connecting to the local WiFi network
  WiFi.begin(ssid,password);
 
  //Keep checking the WiFi status until it is connected to the wifi network
 
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi Connected with IP Address: ");
  Serial.println(WiFi.localIP());
 
  //Specifying the functions which will be executed upon corresponding GET request from the client
  server.on("/",handle_OnConnect);
  server.on("/led1on",handle_led1on);
  server.on("/led1off",handle_led1off);
  server.on("/led2on",handle_led2on);
  server.on("/led2off",handle_led2off);
  server.on("/led3on",handle_led3on);
  server.on("/led3off",handle_led3off);
  server.on("/led4on",handle_led4on);
  server.on("/led4off",handle_led4off);
  server.onNotFound(handle_NotFound);
   
  //Starting the Server
  server.begin();
  Serial.println("HTTP Server Started");
}
 
void loop() {
 
  if(WiFi.status()==WL_CONNECTED)//Check if ESP8266 is still connected to the internet
  {
    //Assign the server to handle the clients
    server.handleClient();
 
    //Turn the LEDs ON/OFF as per their status set by the connected client
   
    //LED1
    if(led1_status==false)
    {
      digitalWrite(led1_pin,LOW);
    }
    else
    {
      digitalWrite(led1_pin,HIGH);
    }
   
    //LED2
    if(led2_status==false)
    {
      digitalWrite(led2_pin,LOW);
    }
    else
    {
      digitalWrite(led2_pin,HIGH);
    }
   
    //LED3
    if(led3_status==false)
    {
      digitalWrite(led3_pin,LOW);
    }
    else
    {
      digitalWrite(led3_pin,HIGH);
    }
   
    //LED4
    if(led4_status==false)
    {
      digitalWrite(led4_pin,LOW);
    }
    else
    {
      digitalWrite(led4_pin,HIGH);
    }
  }
  else
  {
    Serial.println("WiFi Disconnected!!!");
    Serial.print("Trying to establish the connection...");
     
    //Keep checking the WiFi status until it is connected to the wifi network
    while(WiFi.status()!=WL_CONNECTED)
    {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi Connected with IP Address: ");
    Serial.println(WiFi.localIP());
  }
}
 
void handle_OnConnect()
{
  Serial.println("Client Connected");
  server.send(200, "text/html", HTML()); 
}
 
void handle_led1on()
{
  Serial.println("LED1 ON");
  led1_status=true;
  server.send(200, "text/html", HTML());
}
 
void handle_led1off()
{
  Serial.println("LED1 OFF");
  led1_status=false;
  server.send(200, "text/html", HTML());
}
 
void handle_led2on()
{
  Serial.println("LED2 ON");
  led2_status=true;
  server.send(200, "text/html", HTML());
}
 
void handle_led2off()
{
  Serial.println("LED2 OFF");
  led2_status=false;
  server.send(200, "text/html", HTML());
}
 
void handle_led3on()
{
  Serial.println("LED3 ON");
  led3_status=true;
  server.send(200, "text/html", HTML());
}
 
void handle_led3off()
{
  Serial.println("LED3 OFF");
  led3_status=false;
  server.send(200, "text/html", HTML());
}
 
void handle_led4on()
{
  Serial.println("LED4 ON");
  led4_status=true;
  server.send(200, "text/html", HTML());
}
 
void handle_led4off()
{
  Serial.println("LED4 OFF");
  led4_status=false;
  server.send(200, "text/html", HTML());
}
 
void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}
 
String HTML()
{
  String msg="<!DOCTYPE html> <html>\n";
  msg+="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  msg+="<title>LED Control</title>\n";
  msg+="<style>html{font-family:Helvetica; display:inline-block; margin:0px auto; text-align:center;}\n";
  msg+="body{margin-top: 50px;} h1{color: #444444; margin: 50px auto 30px;} h3{color:#444444; margin-bottom: 50px;}\n";
  msg+=".button{display:block; width:80px; background-color:#f48100; border:none; color:white; padding: 13px 30px; text-decoration:none; font-size:25px; margin: 0px auto 35px; cursor:pointer; border-radius:4px;}\n";
  msg+=".button-on{background-color:#f48100;}\n";
  msg+=".button-on:active{background-color:#f48100;}\n";
  msg+=".button-off{background-color:#26282d;}\n";
  msg+=".button-off:active{background-color:#26282d;}\n";
  msg+="</style>\n";
  msg+="</head>\n";
  msg+="<body>\n";
  msg+="<h1>ESP8266 Web Server</h1>\n";
  msg+="<h3>Using Station (STA) Mode</h3>\n";
   
  if(led1_status==false)
  {
    msg+="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";    
  }
  else
  {
    msg+="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";
  }
 
  if(led2_status==false)
  {
    msg+="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";    
  }
  else
  {
    msg+="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";
  }
 
  if(led3_status==false)
  {
    msg+="<p>LED3 Status: OFF</p><a class=\"button button-on\" href=\"/led3on\">ON</a>\n";    
  }
  else
  {
    msg+="<p>LED3 Status: ON</p><a class=\"button button-off\" href=\"/led3off\">OFF</a>\n";
  }
 
  if(led4_status==false)
  {
    msg+="<p>LED4 Status: OFF</p><a class=\"button button-on\" href=\"/led4on\">ON</a>\n";    
  }
  else
  {
    msg+="<p>LED4 Status: ON</p><a class=\"button button-off\" href=\"/led4off\">OFF</a>\n";
  }
   
  msg+="</body>\n";
  msg+="</html>\n";
  return msg;
}
