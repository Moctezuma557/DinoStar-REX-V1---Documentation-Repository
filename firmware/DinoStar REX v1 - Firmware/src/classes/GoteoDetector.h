






#ifndef GOTEO_DETECTOR_H  //<== Inicio de cabecera GoteoDetector
#define GOTEO_DETECTOR_H

#include <Arduino.h>
#include "DataPacket.h"

class GoteoDetector {

private:
    uint8_t   pinSensor;
    uint8_t   pinSimulacion;      // <== pin que genera el pulso (solo para simulación)
    ModoGoteo modoGoteo;
    uint32_t  ultimaGota;
    bool      modoSimulacion;     // <== true = usa pulso digital, false = sensor IR real

    // Función callback: se llama cada vez que se detecta una gota
    // Es un puntero a función que apunta a un método de DropProcessor
    void (*onGotaCallback)();

public:
    
    GoteoDetector(uint8_t pinEntrada, uint8_t pinSalida, ModoGoteo modo); //  <== Constructor para simulación (dos pines: salida y entrada)

    
    GoteoDetector(uint8_t pinEntrada, ModoGoteo modo);  // <== Constructor para hardware real (solo pin de entrada del sensor IR)

    void begin();

    
    void onGotaDetectada();  // <== Se ejecuta automáticamente por la interrupción, debe ser muy corta

    void setModo(ModoGoteo modo);
    void setCallback(void (*callback)());

    ModoGoteo getModo()      const;
    uint32_t  getUltimaGota() const;

    // Solo disponible en modo simulación
    void simularGota();

    void debug() const;
};

#endif // <== Inicio de cabecera GoteoDetector