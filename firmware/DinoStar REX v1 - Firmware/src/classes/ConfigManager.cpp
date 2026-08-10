#include "ConfigManager.h" // <== Código de funcionalidad para la clase ConfigManager

// Nombre del namespace en NVS — máximo 15 caracteres
const char* ConfigManager::NAMESPACE = "rex_config";



// <========================  CONSTRUCTOR =========================>

ConfigManager::ConfigManager(){
    pacienteId = "Sin asignar";
    volTotal = 500.0f; // <== 500 ml como valor por defecto
    modoGoteo = ModoGoteo::NORMAL_GOTEO;
};


// <========================  MÉTODO CARGAR =========================>
void ConfigManager::cargar(){
    prefs.begin(NAMESPACE, true); // <== true para solo lectura

    pacienteId = prefs.getString("pacienteId", "Sin asignar");
    volTotal = prefs.getFloat("volTotal", 500.0f);
    modoGoteo  = static_cast<ModoGoteo>(prefs.getInt("modoGoteo", 0));

    prefs.end();

    Serial.println("[ConfigManager] Configuración cargada desde memoria");
    debug();
};

// <========================  MÉTODO GUARDAR =========================>

void ConfigManager::guardar(){
    prefs.begin(NAMESPACE, false); // <== false para lectura/escritura

    prefs.putString("pacienteId", pacienteId);
    prefs.putFloat("volTotal", volTotal);
    prefs.putInt("modoGoteo", static_cast<int>(modoGoteo));

    prefs.end();

    Serial.println("[ConfigManager] Configuración guardada en memoria");
};

String ConfigManager::getPacienteId() const {
    return pacienteId;
};


float ConfigManager::getVolTotal() const {
    return volTotal;
};

ModoGoteo ConfigManager::getModoGoteo() const {
    return modoGoteo;
};

void ConfigManager::setPacienteId(const String &id) {
    pacienteId = id;
};

void ConfigManager::setVolTotal(float vol) {
    volTotal = vol;
};

void ConfigManager::setModoGoteo(ModoGoteo modo) {
    modoGoteo = modo;
};

void ConfigManager::resetearDefault(){
    pacienteId = "Sin asignar";
    volTotal = 500.0f; 
    modoGoteo = ModoGoteo::NORMAL_GOTEO;
    guardar();

    Serial.println("[ConfigManager] Valores reseteados a valores por defecto");
};

void ConfigManager::debug() const {
    Serial.println("---- ConfigManager ----");
    Serial.println("  pacienteId : " + pacienteId);
    Serial.println("  volTotal   : " + String(volTotal) + " mL");
    Serial.println("  modoGoteo  : " + String(modoGoteo == ModoGoteo::NORMAL_GOTEO
                                               ? "NORMAL (20 gotas/mL)"
                                               : "MICRO  (60 gotas/mL)"));
    Serial.println("-----------------------");
}