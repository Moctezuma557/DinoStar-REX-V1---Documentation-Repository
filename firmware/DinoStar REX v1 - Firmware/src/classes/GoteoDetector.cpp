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

// <========================  MÉTODO simularGota() =========================>

void GoteoDetector::simularGota(){

    if(!modoSimulacion){
        Serial.println("[GoteoDetector] No se puede usar este método en modo simulación");
        return;
    } 

    digitalWrite(pinSimulacion, HIGH);
    delayMicroseconds(500);
    digitalWrite(pinSimulacion, LOW);

}

// <========================  MÉTODO setCallback =========================>

void GoteoDetector::setCallback(void (*callback)()) {
    onGotaCallback = callback;
}

// <========================  MÉTODO setModo() =========================>

void GoteoDetector::setModo(ModoGoteo modo) {
    modoGoteo = modo;
}


// <========================  MÉTODOS GETTERS =========================>


// <========================  MÉTODO getModo() =========================>

ModoGoteo GoteoDetector::getModo() const {
    return modoGoteo;
}

uint32_t GoteoDetector::getUltimaGota() const {
    return ultimaGota;
}


// <========================  MÉTODO debug() =========================>

void GoteoDetector::debug() const{

    Serial.println("---- GoteoDetector ----");
    Serial.println("  pinSensor    : " + String(pinSensor));
    Serial.println("  modoGoteo     : " + String(modoGoteo == ModoGoteo::NORMAL_GOTEO
                                              ? "NORMAL (20 gotas/mL)"
                                              : "MICRO  (60 gotas/mL)"));
    Serial.println("  ultimaGota   : " + String(ultimaGota) + " ms");
    Serial.println("  modoSimulacion: " + String(modoSimulacion));
    Serial.println("---------------------------");  
}