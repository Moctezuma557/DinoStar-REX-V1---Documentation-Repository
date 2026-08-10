#include <Arduino.h>

#include "classes/ConfigManager.h"

ConfigManager conf;

void setup(){

  Serial.begin(115200);
    
    // Espera hasta que el monitor serial esté conectado
    while(!Serial) { delay(10); }
    delay(500);
    
  Serial.println("DinoStar REX v1 firmware --> Iniciando....");

  



  conf.cargar();

  conf.setPacienteId("cama-03");
  conf.setVolTotal(500.0f);
  conf.setModoGoteo(ModoGoteo::NORMAL_GOTEO);

  conf.guardar();

  Serial.println("\n>>> Recargando desde NVS...");
  conf.cargar();
}

void loop(){
  
}