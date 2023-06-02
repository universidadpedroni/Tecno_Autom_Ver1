#include <Arduino.h>
#include <config.h>
#include <blink.h>
// oLED Display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Sensor DHT11
#include <DHT.h>
// Pulsador
#include <checkButton.h>
// Encoder
#include <Encoder.h>
// Wifi
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

const char* ssid = "MyWiFi_S9";
const char* password = "HelpUsObiJuan";
#define CONNECT_TIME 10000

String ledState;  // Stores LED state

AsyncWebServer server(80);
blink parpadeo(LED_BUILTIN);
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire);
DHT dht(DHT_PIN, DHT11);
Encoder myEnc(PIN_ENC_A, PIN_ENC_B);


/* Los archivos de la página web se encuentran en la carpeta /data. Hay que subirlos manualmente */

String procesamientoFrontEnd(const String& var){
  
  if(var == "STATE"){
    digitalRead(LED_BUILTIN) == HIGH? ledState = "ON" : ledState = "OFF";
    Serial.printf("Estado del LED: %s\n", ledState.c_str());  
    return ledState;
  }
  return String();
 
}


void spiffsInit(){
  if(!SPIFFS.begin(true)){
    Serial.println("Error al inicializar SPIFFs. El sistema se detendra");
    while(1){
      delay(100);
    }
  }
  else{
    Serial.println("SPIFFS inicializado con exito");
    Serial.println("Listado de archivos disponibles:");
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while(file){
      Serial.printf("Archivo: %s\n", file.name());
      file.close();
      file = root.openNextFile();
    }
    root.close();
    
  }

}

void spiffsCheckFiles(char * nombreArchivo){
  SPIFFS.exists(nombreArchivo)? Serial.printf("Archivo %s encontrado\n", nombreArchivo) : Serial.printf("Archivo %s no encontrado\n", nombreArchivo);
  
}

void wifiInit(){
  unsigned long tiempoDeInicio = millis();
  Serial.printf("Intentando conectar a la red Wifi %s...", ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED && (tiempoDeInicio + CONNECT_TIME > millis())){
    Serial.print(".");
    delay(1000);
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.print("\nIP del servidor:");
    Serial.println(WiFi.localIP());
  }
  else{
    Serial.println("No se pudo conectar al Wifi. Llamen a los Avengers");
  }
  
}

void serverInitBootstrap(){
  Serial.print("Inicializando servidor usando Boostrap Framework");
  //Ruta a index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS,"/indexWithBoostrap.html",String(), false, procesamientoFrontEnd);
  });
  server.on("/src/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/bootstrap.bundle.min.js", "text/javascript");
  });
 
  server.on("/src/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/jquery-3.6.3.min.js", "text/javascript");
  });
 
  server.on("/src/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/bootstrap.min.css", "text/css");
  });

  // Ruta para encender el LED
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_BUILTIN, HIGH);
    request->send(SPIFFS, "/indexWithBoostrap.html", String(), false,procesamientoFrontEnd);
  });

  // Ruta para apagar el LED
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_BUILTIN, LOW);
    request->send(SPIFFS, "/indexWithBoostrap.html", String(), false,procesamientoFrontEnd);
  });

  server.begin();



}

void serverInit(){
  Serial.print("Inicializando servidor...");
  //Ruta a index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS,"/index.html",String(), false, procesamientoFrontEnd);
  });

  //Ruta a style.css
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/style.css","text/css");
  });

  // Ruta para encender el LED
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_BUILTIN, HIGH);
    request->send(SPIFFS, "/index.html", String(), false,procesamientoFrontEnd);
  });

  // Ruta para apagar el LED
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_BUILTIN, LOW);
    request->send(SPIFFS, "/index.html", String(), false,procesamientoFrontEnd);
  });

  server.begin();
  Serial.println("listo");
}


void dhtInit(){
  dht.begin();
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  Serial.printf("Temperatura: %d°C\tHumedad: %d%\n",
                (int)temp,
                (int)hum);
}

void displayInit(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //display.begin()
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Hola Mundo!"));
  display.println(F("Datos de Compilacion:"));
  display.printf("Fecha %s\n",__DATE__);
  display.printf("Hora: %s\n",__TIME__);
  display.display();
}

void displayWiFi(){
  display.clearDisplay();
  display.setCursor(0,0);
  if(WiFi.status() == WL_CONNECTED){
    display.println(F("Conectado al Wifi"));
    display.println(F("IP:"));
    display.println(WiFi.localIP());

  }
  else{
    display.println(F("No se pudo conectar a WiFi"));
  }
  display.display();

}

void myEncInit(){
  myEnc.write(0);
}

void displayUpdateAndShow(unsigned long interval, int cuenta, float valorADC)
{
	static unsigned long previousMillis = 0;        // will store last time LED was updated
	//const long interval = 1000;           // interval at which to blink (milliseconds)
	unsigned long currentMillis = millis();
	static bool estadoPin=false;
	
	if(currentMillis - previousMillis > interval) 
	{
		previousMillis = currentMillis;
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Temperatura: %.1fC\nHumedad: %.1f\nADC: %.2f\nEncoder: %d\n",
                    dht.readTemperature(),
                    dht.readHumidity(),
                    valorADC,
                    cuenta); 
    display.display();
	}
    
}

void setup() {
  delay(1000);
  Serial.begin(BAUDRATE);
  Serial.println(F("Hola Mundo!"));
  Serial.println(F("Hardware Test"));
  Serial.println(F("Iniciando Led"));
  parpadeo.init();
  parpadeo.off();
  Serial.println(F("Iniciando Entradas Digitales"));
  pinMode(PIN_ENC_PUSH,INPUT_PULLUP);
  pinMode(PIN_LED_R, OUTPUT);
  digitalWrite(PIN_LED_R, HIGH);
  Serial.println(F("Iniciando Display"));
  displayInit();
  Serial.println(F("Iniciando DHT11"));
  dhtInit();
  Serial.println(F("Configurando ADC..."));
  analogReadResolution(12);
  Serial.println(F("Inicializando Encoder"));
  myEncInit();
  Serial.println(F("Inicializando SPIFFS"));
  spiffsInit();
  Serial.println(F("Inicializando WiFi"));
  wifiInit();
  displayWiFi();
  delay(2000);
  if (digitalRead(PIN_ENC_PUSH)){
    Serial.println("Usando servidor con Boostrap Framework");
    serverInitBootstrap();
  }
  else{
    Serial.println("Usando servidor sin Boostrap");
    serverInit();

  }


  Serial.println(F("Setup Terminado"));
  Serial.println(F("PINOUT: https://www.mischianti.org/2021/02/17/doit-esp32-dev-kit-v1-high-resolution-pinout-and-specs/"));
  delay(5000);
  digitalWrite(PIN_LED_R, LOW);
}

void loop() {
  int cuenta = myEnc.read() /4;
  float valorADC = (float) map(analogRead(PIN_ADC),0 ,4095, 4095, 0) * 3.3 / 4095.0;
  parpadeo.update(BLINK_OK);
  int pulsador = checkButton(PIN_ENC_PUSH);
  if (pulsador !=0) Serial.printf("Valor del pulsador: %d\n", pulsador);
  if( pulsador !=0) digitalWrite(PIN_LED_R, !digitalRead(PIN_LED_R));
  displayUpdateAndShow(100, cuenta, valorADC);
   
}