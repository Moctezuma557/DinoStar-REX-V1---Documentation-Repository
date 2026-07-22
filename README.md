<div align="center">

<!-- Sustituye la ruta por la ubicación real de tu logo en el repo -->
<img src="./assets/logo.png" alt="DinoStar Logo" width="180"/>

# REX V1

**Sistema IoT de monitoreo de venoclisis en tiempo real**

*Desarrollado por [DinoStar](https://github.com/TU_USUARIO)*

---

![Estado](https://img.shields.io/badge/Estado-En%20desarrollo-yellow?style=flat-square)
![Plataforma](https://img.shields.io/badge/Plataforma-ESP32-blue?style=flat-square)
![Protocolo](https://img.shields.io/badge/Protocolo-MQTT-purple?style=flat-square)
![Licencia](https://img.shields.io/badge/Licencia-MIT-green?style=flat-square)

</div>

---

## ¿Qué es REX V1?

REX V1 es el primer prototipo de un sistema embebido IoT diseñado para monitorear en tiempo real el goteo de equipos de venoclisis en entornos hospitalarios. El sistema detecta automáticamente cada gota mediante sensores infrarrojos, calcula la tasa de goteo por minuto y estima el tiempo restante para que la bolsa o botella de suero se termine, enviando esta información en tiempo real al personal de enfermería.

El objetivo es reducir los tiempos de respuesta del personal clínico ante situaciones como bolsas vacías, goteo anómalo o desconexiones del equipo, mejorando la seguridad y atención al paciente.

---

## Características principales

- Detección de gotas mediante sensor infrarrojo con interrupción de hardware (ISR)
- Soporte para equipos de **normogoteo** (20 gotas/mL) y **microgoteo** (60 gotas/mL)
- Cálculo en tiempo real de gotas por minuto y tiempo restante de la bolsa
- Interfaz local para el enfermero: pantalla + encoder rotativo + 2 botones
- Transmisión de datos vía **MQTT** hacia nodos agregadores (**Braquios**)
- Sistema de alertas locales y remotas ante condiciones anómalas
- Persistencia de configuración en memoria NVS del ESP32
- Arquitectura orientada a objetos, modular y escalable

---

## Arquitectura del sistema

El sistema se organiza en cuatro capas:

```
[Sensor IR] → [ESP32 - REX Unit] → (MQTT) → [Nodo Braquio] → [Servidor central] → [Dispositivo enfermera]
```

| Capa | Componente | Descripción |
|---|---|---|
| 1 | REX Unit | ESP32 por cama de paciente. Detecta, procesa y muestra datos |
| 2 | Nodo Braquio | ESP32 agregador por sala. Recibe datos de N pacientes |
| 3 | Servidor central | Broker MQTT + backend. Procesa y distribuye la información |
| 4 | Dispositivo enfermera | Dashboard en tiempo real con alertas y historial |

Para la documentación técnica completa del firmware y el diagrama de clases UML, consulta [`docs/firmware.md`](./docs/firmware.md).

---

## Hardware requerido

| Componente | Descripción |
|---|---|
| ESP32 | Microcontrolador principal (cualquier variante con WiFi) |
| Sensor IR | Par emisor/receptor infrarrojo para detección de gotas |
| Pantalla LCD/OLED | Interfaz visual para el enfermero |
| Encoder rotativo | Navegación por menús |
| 2 botones | Confirmación y cancelación de acciones |

---

## Estado del proyecto

| Etapa | Estado |
|---|---|
| Detección de gotas con sensor IR | ✅ Completado |
| Arquitectura de clases y documentación | ✅ Completado |
| Desarrollo del firmware (ESP32) | 🔄 En progreso |
| Nodo Braquio | ⏳ Pendiente |
| Servidor central y broker MQTT | ⏳ Pendiente |
| Dashboard para enfermera | ⏳ Pendiente |

---


## Créditos

**Desarrollado por:** Adrian — [DinoStar](https://github.com/Moctezuma557)  
**Proyecto:** REX V1 — Sistema IoT de monitoreo de venoclisis  
 

---


<div align="center">
<sub>DinoStar © 2025 · Hecho en Morelia, México 🦕</sub>
</div>
