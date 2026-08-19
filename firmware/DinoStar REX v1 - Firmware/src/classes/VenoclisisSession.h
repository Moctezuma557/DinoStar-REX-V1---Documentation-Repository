#ifndef VENOCLISIS_SESSION_H // <== Inicio de cabecera Datapacket
#define VENOCLISIS_SESSION_H 

#include <Arduino.h>
#include "DataPacket.h"



class VenoclisisSession
{
    private:
        String pacienteId;
        uint32_t inicioSesion;
        float volTotal;
        EstadoSesion estado;


    public:

        VenoclisisSession();

        void iniciar();  // <== Iniciar sesion
        void pausar();   // <== Pausa una sesion
        void terminar(); // <== Termina una sesion
        bool isActiva(); // <== Devuleve true o false si hay una sesion activa o no

        String getPacienteId() const;
        uint32_t getInicioSesion() const;
        float getVolTotal() const;
        EstadoSesion getEstado() const;

        void configurar(const String& id, float vol);

        void debug() const;
    
};

#endif 