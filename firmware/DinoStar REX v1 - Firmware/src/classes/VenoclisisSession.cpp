#include "VenoclisisSession.h"


// <========================  CONSTRUCTOR =========================>

VenoclisisSession::VenoclisisSession(){
    pacienteId = "Sin asignar";
    inicioSesion = 0;
    volTotal = 0.00f;
    estado = EstadoSesion::FINALIZADA;
};





// <========================  MÉTODO Iniciar =========================>
void VenoclisisSession::iniciar() {
    if (estado == EstadoSesion::FINALIZADA) {
        inicioSesion = millis();
        estado = EstadoSesion::ACTIVA;
    } else {
        Serial.println("[VenoclisisSession] NO se puede iniciar, ya hay una sesión en curso");
    }
}


// <========================  MÉTODO Pausar =========================>
void VenoclisisSession::pausar() {

    if(estado == EstadoSesion::ACTIVA){
        estado = EstadoSesion::PAUSADA;
    } 
    else {
        Serial.println("[VenoclisisSession] NO se puede pausar una sesión que no está activa");
    }
};


// <========================  MÉTODO Terminar =========================>

void VenoclisisSession::terminar() {
    if (estado == EstadoSesion::ACTIVA || estado == EstadoSesion::PAUSADA) {
        estado = EstadoSesion::FINALIZADA;
    } else {
        Serial.println("[VenoclisisSession] NO se puede terminar una sesión que no está activa");
    }
};

// <========================  MÉTODO isActiva =========================>

bool VenoclisisSession::isActiva() {
    return (estado == EstadoSesion::ACTIVA || estado == EstadoSesion::PAUSADA);
}



// <========================  MÉTODOS get =========================>

String VenoclisisSession::getPacienteId() const {
    return pacienteId;
};

uint32_t VenoclisisSession::getInicioSesion() const {
    return inicioSesion;
};

float VenoclisisSession::getVolTotal() const {
    return volTotal;
};

EstadoSesion VenoclisisSession::getEstado() const {
    return estado;
};

void VenoclisisSession::configurar(const String& id, float vol) {
    pacienteId = id;
    volTotal = vol;
};

void VenoclisisSession::debug() const {
    String estadoStr;
    switch(estado) {
        case EstadoSesion::ACTIVA:     estadoStr = "ACTIVA";     break;
        case EstadoSesion::PAUSADA:    estadoStr = "PAUSADA";    break;
        case EstadoSesion::FINALIZADA: estadoStr = "FINALIZADA"; break;
        case EstadoSesion::ERROR:      estadoStr = "ERROR";      break;
        default:                       estadoStr = "DESCONOCIDO";
    }

    Serial.println("---- VenoclisisSession ----");
    Serial.println("  pacienteId   : " + pacienteId);
    Serial.println("  inicioSesion : " + String(inicioSesion) + " ms");
    Serial.println("  volTotal     : " + String(volTotal) + " mL");
    Serial.println("  estado       : " + estadoStr);
    Serial.println("---------------------------");
}