#ifndef CONFIG_MANAGER_h // <== Inicio de cabecera ConfigManager
#define CONFIG_MANAGER_h

// Librerias

#include <Arduino.h>
#include <Preferences.h>
#include "DataPacket.h"

class ConfigManager
{

private:
    Preferences pref;

    String pacienteId;
    float VolTotal;
    ModoGoteo modoGoteo;

    static const char *NAMESPACE;

public:
    ConfigManager();

    void cargar();

    void guardar();

    // Getters

    String getPacienteId() const;
    float getVolTotal() const;
    ModoGoteo getModoGoteo() const;

    // Setters

    void SetPacienteId(const String &id); //       Recordar que para que los datos se guarden en la memoria ram del esp
    void SetVolTotal(float vol);          //       es necesario que despues de cambiar los datos se guarden usando el metodo
    void SetModoGoteo(ModoGoteo modo);    //       guardar() de esta clase para que los datos persistan.



    void resetearDefault(); // < == Regresa a los valores por defecto

    void debug() const; //  <== imprime la configuracion actual, no esta incluida en la documentacion pero me va a servir despues
    
};

#endif // <== Fin de cabezera ConfigManager