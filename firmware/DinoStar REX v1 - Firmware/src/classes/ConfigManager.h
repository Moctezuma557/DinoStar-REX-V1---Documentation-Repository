#ifndef CONFIG_MANAGER_h // <== Inicio de cabecera ConfigManager
#define CONFIG_MANAGER_h


//Si alguien se detiene a leer esto espero que seas testigo de la primeras lineas de código que escribi para este
//proyecto. No importa si eres programadoR, ingeniero en biomedica o alguien de la salud que por curiosidad quizo ver el 
//codigo de esta cosa, solo quiero que sepas que agradezco a todos su apoyo. Gracias por leer 

//Por cierto, recuerda:
//1- SI EL PROGRAMA COMPILA Y FUNCIONA NO LO TOQUES.
//
//2- SOLO YO, CLAUDE Y DIOS SABEN COMO FUNCIONA ESTO.
//
//3- SI VAS A AÑADIR, ELIMINAR O MODIFICAR ALGUN CÓDIGO RESA POR QUE NO
//   ROMPA ALGO.
//
//4- NO POR SER DE TECNOLOGIAS NO PUEDES SALVAR VIDAS, LOS MEDICOS Y ENFERMEROS SALVAN
//   A UNO A LA VEZ, NOSOTROS CREAMOS SISTEMAS QUE PUEDEN SALVAR A MILES, Y ESO NOS HACE
//   IMPORTANTES. CREE EN TI.






// Librerias

#include <Arduino.h>
#include <Preferences.h>
#include "DataPacket.h"

class ConfigManager
{

private:
    Preferences prefs;

    String pacienteId;
    float volTotal;
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

    void setPacienteId(const String &id); //       Recordar que para que los datos se guarden en la memoria ram del esp
    void setVolTotal(float vol);          //       es necesario que despues de cambiar los datos se guarden usando el metodo
    void setModoGoteo(ModoGoteo modo);    //       guardar() de esta clase para que los datos persistan.



    void resetearDefault(); // < == Regresa a los valores por defecto

    void debug() const; //  <== imprime la configuracion actual, no esta incluida en la documentacion pero me va a servir despues
    
};

#endif // <== Fin de cabezera ConfigManager