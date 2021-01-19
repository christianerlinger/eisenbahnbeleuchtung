#include "FastLED.h"

#define NUM_LEDS 7
#define DATA_PIN 10
#define CLOCK_PIN 9

// offset (von 0) für den Pin (also schalter 1 hängt auf pin 3 z.B.)
#define LIGHTSWITCH_PIN_OFFSET 2

// anzahl der taster die für lichter zuständig sind
#define NUM_LIGHTSWITCHES 5

#define BAHNHOF 0
#define KIRCHE 1
#define GASTHAUS 2
#define BAUERNHOF 3
#define WOHNHAUS 4

#define DEBOUNCE_TIME 200



struct lightswitch {
  int current_state;
  int pin_number;
  unsigned long state_change_time;
  int led_count;
};

CRGB leds[NUM_LEDS];

lightswitch switches[NUM_LIGHTSWITCHES];


void setup() {
  FastLED.addLeds<WS2801,RGB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);

  for(int i = 0; i < NUM_LEDS; i++)
  {
   leds[i] = CRGB::White;    
  }


  for(int i = 0; i < NUM_LIGHTSWITCHES; i++)
  {
    switches[i].current_state = HIGH;
    switches[i].state_change_time = 0;
    switches[i].pin_number = LIGHTSWITCH_PIN_OFFSET + i;
    pinMode(switches[i].pin_number,INPUT);

    if(i == GASTHAUS || i == WOHNHAUS || i == BAUERNHOF) // gebäude mit einem LED
    {
      switches[i].led_count = 1;
    }
    else if(i == KIRCHE || i == BAHNHOF) // gebäude mit 2 LEDs
    {
      switches[i].led_count = 2;
    }
    else //failsafe
    {
      switches[i].led_count = 0;
    }
  }

}

void toggleSwitch(lightswitch &sw)
{
  sw.state_change_time = 0; //statuszeit verwerfen
  sw.current_state = !sw.current_state; //den aktuellen status toggeln

  CRGB currColor;
  
  if(sw.current_state) //wenn der aktuelle status HIGH ist aufdrehen, sonst abdrehen
  {
     currColor = CRGB::White;
  }
  else
  {
    currColor = CRGB::Black;
  }

  // jetzt abhängig von der startposition des switches X lichter auf die neue farbe setzen
  for(int i = sw.pin_number - LIGHTSWITCH_PIN_OFFSET; i < switches[i].led_count; i++)
  {
    leds[i] = currColor;
  }
}

void loop() 
{  
  //millisekunden seitdem der arduino gestartet wurde; nach 50 tagen wieder 0...wird aber eher nicht 50 tage permanent...
  unsigned long currTime = millis(); 
  
  for(int i = 0; i < NUM_LIGHTSWITCHES; i++)
  {
      //pin status auslesen
      int currPinStatus = digitalRead(switches[i].pin_number); //status des tasters auslesen
      if(currPinStatus == HIGH) //taster gedrückt
      {
        // es wurde erst gedrückt
        if(switches[i].state_change_time == 0)
        {
          switches[i].state_change_time = currTime; //zeitpunkt merken
        }
        else if(currTime - switches[i].state_change_time > DEBOUNCE_TIME) // entprellen
        {
          toggleSwitch(switches[i]);
        }
      } 
      else
      {
        //wenn sich der status nicht geändert hat wurde entweder nix gedrückt oder zu kurz gedrückt (geprellt) => zeitpunkt löschen
        switches[i].state_change_time = 0;
      }
      
  }

  FastLED.show();
}
