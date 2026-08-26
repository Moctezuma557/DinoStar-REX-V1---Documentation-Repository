
/*
    
 ██████████    ███                       █████████   █████                       
░░███░░░░███  ░░░                       ███░░░░░███ ░░███                        
 ░███   ░░███ ████  ████████    ██████ ░███    ░░░  ███████    ██████   ████████ 
 ░███    ░███░░███ ░░███░░███  ███░░███░░█████████ ░░░███░    ░░░░░███ ░░███░░███
 ░███    ░███ ░███  ░███ ░███ ░███ ░███ ░░░░░░░░███  ░███      ███████  ░███ ░░░ 
 ░███    ███  ░███  ░███ ░███ ░███ ░███ ███    ░███  ░███ ███ ███░░███  ░███     
 ██████████   █████ ████ █████░░██████ ░░█████████   ░░█████ ░░████████ █████    
░░░░░░░░░░   ░░░░░ ░░░░ ░░░░░  ░░░░░░   ░░░░░░░░░     ░░░░░   ░░░░░░░░ ░░░░░     
                                                                                 
                                                                                 
           
                                                                      
   ▄████████    ▄████████ ▀████    ▐████▀ 
  ███    ███   ███    ███   ███▌   ████▀  
  ███    ███   ███    █▀     ███  ▐███    
 ▄███▄▄▄▄██▀  ▄███▄▄▄        ▀███▄███▀    
▀▀███▀▀▀▀▀   ▀▀███▀▀▀        ████▀██▄     
▀███████████   ███    █▄    ▐███  ▀███    
  ███    ███   ███    ███  ▄███     ███▄  
  ███    ███   ██████████ ████       ███▄ 
  ███    ███                              


        
*/ 
//                                              ____
//  ___                                      .-~. /_"-._
//`-._~-.                                  / /_ "~o\  :Y
//      \  \                                / : \~x.  ` ')
//      ]  Y                              /  |  Y< ~-.__j
//     /   !                        _.--~T : l  l<  /.-~
//    /   /                 ____.--~ .   ` l /~\ \<|Y
//   /   /             .-~~"        /| .    ',-~\ \L|
//  /   /             /     .^   \ Y~Y \.^>/l_   "--'
// /   Y           .-"(  .  l__  j_j l_/ /~_.-~    .
//Y    l          /    \  )    ~~~." / `/"~ / \.__/l_
//|     \     _.-"      ~-{__     l  :  l._Z~-.___.--~
//|      ~---~           /   ~~"---\_  ' __[>
//l  .                _.^   ___     _>-y~
// \  \     .      .-~   .-~   ~>--"  /
//  \  ~---"            /     ./  _.-'
//   "-.,_____.,_  _.--~\     _.-~
//               ~~     (   _}       -Row
//                      `. ~(
//                        )  \
//                  /,`--'~\--'~\
//                  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~





#include <Arduino.h>

#define PIN_SALIDA  18
#define PIN_ENTRADA 19

volatile uint32_t contadorPulsos = 0;

void IRAM_ATTR onPulso() {
    contadorPulsos++;
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    delay(500);

    Serial.println("=== REX V1 - Prueba simulacion de gotas ===\n");

    pinMode(PIN_SALIDA,  OUTPUT);
    pinMode(PIN_ENTRADA, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_ENTRADA), onPulso, RISING);

    digitalWrite(PIN_SALIDA, LOW);
}

void loop() {
    static uint32_t ultimaGota      = 0;
    static uint32_t ultimaImpresion = 0;

    // Simula una gota cada 2 segundos (30 gotas/min)
    if (millis() - ultimaGota >= 2000) {
        ultimaGota = millis();

        // Un solo pulso digital: sube y baja
        digitalWrite(PIN_SALIDA, HIGH);
        delayMicroseconds(500);  // 0.5ms es suficiente para que la interrupción lo detecte
        digitalWrite(PIN_SALIDA, LOW);

        Serial.println("Gota simulada!");
    }

    // Imprime el contador cada 5 segundos
    if (millis() - ultimaImpresion >= 5000) {
        ultimaImpresion = millis();
        Serial.println("Pulsos detectados: " + String(contadorPulsos));
    }
}