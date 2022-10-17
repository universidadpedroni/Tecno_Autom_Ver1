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


blink parpadeo(LED_BUILTIN);
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire);
DHT dht(DHT_PIN, DHT11);

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


void UpdateAndShow(unsigned long interval,int cuenta)
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
    display.printf("Temperatura: %.1fC\nHumedad: %.1f\nValor ADC: %d\nCuenta: %d\n",
                    dht.readTemperature(),
                    dht.readHumidity(),
                    analogRead(GPIO_NUM_15),
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
  parpadeo.on();
  Serial.println(F("Iniciando Entradas Digitales"));
  pinMode(PULSA1_PIN,INPUT_PULLUP);
  pinMode(PULSA2_PIN, INPUT_PULLUP);
  Serial.println(F("Iniciando Display"));
  displayInit();
  Serial.println(F("Iniciando DHT11"));
  dhtInit();
  Serial.println(F("Configurando ADC..."));
  analogReadResolution(12);
  Serial.println(F("Setup Terminado"));
  Serial.println(F("PINOUT: https://www.mischianti.org/2021/02/17/doit-esp32-dev-kit-v1-high-resolution-pinout-and-specs/"));
  delay(5000);
  
}

void loop() {
  int cuenta = 0;
  static int detectado = 0;
  parpadeo.update(BLINK_OK);
  cuenta = checkButton(PULSA2_PIN);
  if (cuenta !=0) detectado = cuenta;
  UpdateAndShow(1000, detectado);
   
}