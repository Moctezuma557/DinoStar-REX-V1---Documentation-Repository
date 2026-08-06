#include <Arduino.h>

#include "classes/DataPacket.h";

void setup(){

  Serial.begin(115200);
  Serial.println("DinoStar REX v1 firmware --> Iniciando....");

  DataPacket pkt;
  pkt.pacienteId = "123456789";
  pkt.gotasPorMin = 20.0f;
  pkt.tiempoRestante = 10;
  pkt.volRestante = 0.0f;
  pkt.modo = ModoGoteo::NORMAL_GOTEO;
  pkt.timestamp = millis();

  Serial.println(pkt.pacienteId);
  Serial.println(pkt.gotasPorMin);
  Serial.println(pkt.tiempoRestante);
  Serial.println(pkt.volRestante);
 
  Serial.println(pkt.timestamp);  
}

void loop(){
  
}