#ifndef DATAPACKET_H // <== Inicio de cabecera Datapacket

#define DATAPACKET_H

#include "Arduino.h"



enum class ModoGoteo {
    NORMAL_GOTEO,  // 20 gotas = 1 mL  →  1 gota = 0.05 mL
    MICRO_GOTEO    // 60 gotas = 1 mL  →  1 gota = 0.0167 mL
};

enum class EstadoSesion {
    ACTIVA,
    PAUSADA,
    FINALIZADA,
    ERROR
};

enum class TipoAlerta {
    FIN_BOLSA,
    GOTEO_LENTO,
    GOTEO_RAPIDO,
    DESCONECTADO
};


struct DataPacket {
    String    pacienteId;
    float     gotasPorMin;
    uint32_t  tiempoRestante;  // en minutos ==> Posible cambio
    float     volRestante;     // en mL
    ModoGoteo modo;
    uint32_t  timestamp;       // millis() al momento de generarse

    // Constructor con valores por defecto
    DataPacket()
        : pacienteId(""),
          gotasPorMin(0.0f),
          tiempoRestante(0),
          volRestante(0.0f),
          modo(ModoGoteo::NORMAL_GOTEO),
          timestamp(0)
    {}
};


#endif  // <== Fin de cabezera Datapacket