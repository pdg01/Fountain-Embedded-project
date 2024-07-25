// Include the libraries:
#include <Adafruit_Sensor.h>
#include <DHT.h>

//Asignare pini
#define moistPin A0
#define DHTPIN 8
#define flowPin 3    
#define BUTTON_PIN 4
#define RELAY_PIN 5
#define LED_PIN 6

//Declarare si initializare variabile 
double flowRate;    
volatile int count = 0; 
byte lastButtonState = LOW;
byte relayState = LOW;
int moistVal = 0;
double volume_initial,volume_final;
// Initialize DHT sensor for normal 16mhz Arduino:
#define DHTTYPE DHT11 
DHT dht = DHT(DHTPIN, DHTTYPE);

void setup() {
  //Configurare pini
  pinMode(moistPin, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(flowPin, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
  //Metoda intreruperii pentru debimetru
  attachInterrupt(digitalPinToInterrupt(flowPin), Flow, RISING);

  // Setup sensor:
  dht.begin();
}

void loop() {

// Actionare electrovalva prin intermediul releului
byte buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;
    if (buttonState == LOW) {
      relayState = (relayState == HIGH) ? LOW : HIGH;
      digitalWrite(RELAY_PIN, relayState);
      digitalWrite(LED_PIN, relayState);
    }
  }

  //Sensor DHT11:
  // Citire umiditate
  float h = dht.readHumidity();
  // Citire temperatura Celsius
  float t = dht.readTemperature();
  // Citire temperatura Fahrenheit
  float f = dht.readTemperature(true);
  
  // Acomodare senzori
  if (isnan(h) || isnan(t) || isnan(f)) {
    return;
  }

  // Compute heat index in Fahrenheit (default):
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius:
  float hic = dht.computeHeatIndex(t, h, false);


  Serial.print("Umiditate: ");
  Serial.print(h);
  Serial.print(" & ");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" \xC2\xB0");
  Serial.print("C | ");
  Serial.print(f);
  Serial.print(" \xC2\xB0");
  Serial.print("F ");
  

  moistVal = analogRead(moistPin);
  int percent = 2.718282 * 2.718282 * (.008985 * moistVal + 0.207762); //calculate percent for probes about 1 - 1.5 inches apart
  Serial.print(percent);
  Serial.println("% Umiditate sol ");

  //Debimetru
  count = 0;      //Resetare counter
  interrupts();   //Activare intreruperi
  
  delay (1000);   //Wait 1 second
  noInterrupts(); //Dezactivare intreruperi

  //Calculare debit
  flowRate = (count * 2.25);        //Take counted pulses in the last second and multiply by 2.25mL
  flowRate = flowRate * 60;         //Transformare secunde in minute
  flowRate = flowRate / 1000;       //Transformare mL in L, pentru valoarea Litru / Minut
  volume_initial = flowRate/6;
  volume_final = flowRate - volume_initial;
  digitalRead(flowPin);
  Serial.print("Debitul apei: ");
  Serial.print(flowRate);         //Afisare flowRate
  Serial.print("L/min\n");
  Serial.print("Volumul apei: ");
  Serial.print(volume_final);
  Serial.print("L\n");
  delay(5000);
}

void Flow()
{
   count++; //Incrementare counter
} 
