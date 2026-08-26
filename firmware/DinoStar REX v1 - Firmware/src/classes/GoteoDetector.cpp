#include "GoteoDetector.h"

// ==================== [CONSTRUCTOR SIMULACION] - INICIO ====================
GoteoDetector::GoteoDetector(uint8_t pinEntrada, uint8_t pinSalida, ModoGoteo modo) {
    pinSensor    = pinEntrada;
    pinSimulacion = pinSalida;
    modoGoteo    = modo;
    ultimaGota   = 0;
    modoSimulacion = true;
    onGotaCallback = nullptr;  // sin callback por ahora
}


// ==================== [CONSTRUCTOR HARDWARE REAL] - INICIO ====================
GoteoDetector::GoteoDetector(uint8_t pinEntrada, ModoGoteo modo) {
    pinSensor      = pinEntrada;
    pinSimulacion  = 0;
    modoGoteo      = modo;
    ultimaGota     = 0;
    modoSimulacion = false;
    onGotaCallback = nullptr;
}

// <========================  MÉTODO Begin() =========================>

void GoteoDetector::begin() {
    pinMode(pinSensor, INPUT);

    if(modoSimulacion){
        pinMode(pinSimulacion, OUTPUT);
        digitalWrite(pinSimulacion, LOW);
    }

    Serial.println("[GoteoDetector] Inicializado en modo: " +
                   String(modoSimulacion ? "SIMULACION" : "HARDWARE REAL"));
}

// <========================  MÉTODO onGotaDetectada() =========================>

void IRAM_ATTR GoteoDetector:: onGotaDetectada() {

    ultimaGota = millis();

    if (onGotaCallback != nullptr){
        onGotaCallback();
    }

}