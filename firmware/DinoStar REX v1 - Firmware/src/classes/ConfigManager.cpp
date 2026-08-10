#include "ConfigManager.h" // <== Código de funcionalidad para la clase ConfigManager

// Nombre del namespace en NVS — máximo 15 caracteres
const char* ConfigManager::NAMESPACE = "rex_config";

ConfigManager::ConfigManager(){
    pacienteId = "Sin asignar";
    VolTotal = 500.0f; // <== 500 ml como valor por defecto
    modoGoteo = ModoGoteo::NORMAL_GOTEO;
};

ConfigManager::cargar(){
    pref.begin(NAMESPACE, false);

    pacienteId = pref.getString("pacienteId", "Sin asignar");
    VolTotal = pref.getFloat("VolTotal", 500.0f);
};