#include <LiquidCrystal.h>
#include <IRremote.h>

// Pin  schermo LCD
LiquidCrystal lcd(6, 10, 11, 12, 13, 7);

// Pin sensori
#define TMP_SENSOR A1
#define HUMIDITY_SENSOR A0
#define IR_RECEIVER 8

// Pin ventole
#define FAN 9

// Pin led
#define LED_RED 2
#define LED_ORANGE 3
#define LED_GREEN 4
#define LED_BLUE 5

IRrecv irrecv(IR_RECEIVER);
decode_results results;
bool manualMode=false;

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.print("Avvio...");
    
    irrecv.enableIRIn();
    
    pinMode(FAN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_ORANGE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    
    delay(2000);
    lcd.clear();
}

void loop() {
    int rawTemp=analogRead(TMP_SENSOR);
    float voltage=rawTemp*(5.0 / 1023.0);
    float temp=(voltage-0.5)*100.0; // Conversione TMP in gradi Celsius
    int wallHumidity=analogRead(HUMIDITY_SENSOR);

    Serial.print("Temp: "); Serial.print(temp);
    Serial.print(" C, Umidita' muro: "); Serial.println(wallHumidity);

    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(temp); lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("Muro:"); lcd.print(wallHumidity);
    
    // Logica controllo umidità e temperatura
    if(temp > 25 || wallHumidity > 700){
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_ORANGE, LOW);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(FAN, HIGH);
    }else if(temp > 22 || wallHumidity > 500){
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_ORANGE, HIGH);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(FAN, LOW);
    }else{
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_ORANGE, LOW);
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(FAN, LOW);
    }

    if(irrecv.decode(&results)){
        if(results.value == 0xFFA25D){ // Sostituire con il codice del telecomando utilizzato
            manualMode=!manualMode;
        }
        irrecv.resume();
    }
    
    if(manualMode){
        digitalWrite(FAN, HIGH);
        digitalWrite(LED_BLUE, HIGH);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_ORANGE, LOW);
        digitalWrite(LED_GREEN, LOW);
        
        // Se si raggiungono le condizioni ideali, vengono spente automaticamente le ventole
        if(temp <= 22 && wallHumidity <= 500){
            digitalWrite(FAN, LOW);
            digitalWrite(LED_BLUE, LOW);
        }
    }
    
    delay(2000);
}
