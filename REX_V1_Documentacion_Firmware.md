# REX V1 — Documentación técnica del firmware
**DinoStar** · Proyecto REX · Prototipo V1  
Sistema IoT de monitoreo de venoclisis basado en ESP32

---

## Índice

1. [Visión general del sistema](#1-visión-general-del-sistema)
2. [Arquitectura de clases](#2-arquitectura-de-clases)
3. [Tipos de datos y enumeraciones](#3-tipos-de-datos-y-enumeraciones)
4. [Clases del firmware](#4-clases-del-firmware)
   - [GoteoDetector](#41-goteodetector)
   - [DropProcessor](#42-dropprocessor)
   - [VenoclisisSession](#43-venoclipsissession)
   - [DataPacket](#44-datapacket)
   - [DisplayController](#45-displaycontroller)
   - [MQTTPublisher](#46-mqttpublisher)
   - [ConfigManager](#47-configmanager)
   - [AlertManager](#48-alertmanager)
   - [REXController](#49-rexcontroller)
5. [Flujo de datos principal](#5-flujo-de-datos-principal)
6. [Relaciones entre clases](#6-relaciones-entre-clases)
7. [Convenciones del proyecto](#7-convenciones-del-proyecto)

---

## 1. Visión general del sistema

REX V1 es un sistema embebido que monitorea en tiempo real el goteo de un equipo de venoclisis (suero, medicamentos, etc.) conectado a un paciente. El firmware corre en un **ESP32** y se divide en tres responsabilidades principales:

| Responsabilidad | Descripción |
|---|---|
| **Detección** | Leer el sensor IR y detectar cada gota que cae |
| **Procesamiento** | Calcular gotas por minuto y tiempo restante de la bolsa |
| **Comunicación** | Mostrar datos en pantalla y enviarlos por red vía MQTT |

El ESP32 envía los datos a un nodo agregador llamado **Braquio**, que a su vez los reenvía a un servidor central donde la enfermera puede ver el estado de todos sus pacientes en tiempo real.

---

## 2. Arquitectura de clases

El firmware está dividido en **9 clases**, cada una con una responsabilidad única y bien definida. Esto significa que ninguna clase hace demasiadas cosas: si algo falla o necesita modificarse, sabes exactamente dónde ir.

```
┌─────────────────────────────────────────────────────┐
│                   REXController                     │
│           (orquestador principal)                   │
│                                                     │
│  ┌──────────────┐  ┌──────────────┐  ┌───────────┐  │
│  │GoteoDetector │→ │DropProcessor │→ │DataPacket │  │
│  └──────────────┘  └──────────────┘  └───────────┘  │
│                           │                         │
│  ┌──────────────┐  ┌──────┴───────┐  ┌───────────┐  │
│  │DisplayCtrlr  │  │MQTTPublisher │  │AlertMngr  │  │
│  └──────────────┘  └──────────────┘  └───────────┘  │
│                                                     │
│  ┌──────────────┐  ┌──────────────┐                 │
│  │VenoclisisSes │  │ConfigManager │                 │
│  └──────────────┘  └──────────────┘                 │
└─────────────────────────────────────────────────────┘
```

**Principio clave:** Ninguna clase habla directamente con otra excepto a través de `REXController`. Esto evita dependencias cruzadas y hace el código más fácil de mantener y escalar.

---

## 3. Tipos de datos y enumeraciones

Antes de entrar a las clases, es importante entender los tipos de datos que se usan en todo el proyecto.

### Tipos enteros de tamaño fijo

En microcontroladores como el ESP32, se usan tipos de tamaño fijo en lugar de `int` genérico, porque el tamaño de `int` puede variar entre plataformas. Aquí se usan:

| Tipo | Bits | Rango de valores | Uso en REX V1 |
|---|---|---|---|
| `uint8_t` | 8 | 0 – 255 | Números de pin GPIO |
| `uint16_t` | 16 | 0 – 65,535 | Contadores medianos |
| `uint32_t` | 32 | 0 – 4,294,967,295 | Timestamps en milisegundos |
| `float` | 32 | Decimal con precisión ~7 dígitos | Volumen, gotas/min |
| `bool` | 1 | `true` / `false` | Banderas de estado |

> La `u` al inicio significa *unsigned* (sin signo), es decir, no puede ser negativo. Un pin GPIO nunca es negativo, así que `uint8_t` es el tipo correcto para eso.

### enum ModoGoteo

Define el tipo de equipo de venoclisis conectado. Afecta directamente el cálculo del volumen porque cambia cuántas gotas equivalen a 1 mL.

```cpp
enum class ModoGoteo {
    NORMAL_GOTEO,  // 20 gotas = 1 mL  →  1 gota = 0.05 mL
    MICRO_GOTEO    // 60 gotas = 1 mL  →  1 gota = 0.0167 mL
};
```

El enfermero selecciona este modo al inicio de cada sesión desde la pantalla.

### enum EstadoSesion

Representa en qué etapa se encuentra la sesión activa de venoclisis.

```cpp
enum class EstadoSesion {
    ACTIVA,       // Goteo en curso, sistema midiendo
    PAUSADA,      // El enfermero pausó manualmente
    FINALIZADA,   // La bolsa se terminó o se cerró la sesión
    ERROR         // Algo salió mal (sensor desconectado, etc.)
};
```

### enum TipoAlerta

Clasifica el tipo de problema detectado por `AlertManager`.

```cpp
enum class TipoAlerta {
    FIN_BOLSA,        // El tiempo restante llegó a cero
    GOTEO_LENTO,      // El goteo bajó demasiado del objetivo
    GOTEO_RAPIDO,     // El goteo subió demasiado del objetivo
    DESCONECTADO      // No se detectan gotas por un tiempo prolongado
};
```

---

## 4. Clases del firmware

---

### 4.1 GoteoDetector

**¿Qué hace?**  
Es la clase que tiene contacto directo con el hardware. Su única responsabilidad es escuchar el sensor infrarrojo y detectar el momento exacto en que cae una gota. No hace cálculos, no sabe de WiFi, no sabe de pantallas: solo detecta y avisa.

**¿Por qué existe como clase separada?**  
Porque si en el futuro se cambia el sensor IR por uno ultrasónico o capacitivo, solo se modifica esta clase. El resto del sistema no se toca.

**¿Cómo funciona internamente?**  
Usa una **interrupción de hardware (ISR)** del ESP32. Esto significa que cuando el sensor detecta una gota, el ESP32 pausa lo que esté haciendo, ejecuta `onGotaDetectada()`, y luego regresa. Esto garantiza que no se pierde ninguna gota, sin importar qué tan ocupado esté el procesador.

**Atributos:**

| Atributo | Tipo | Descripción |
|---|---|---|
| `pinSensor` | `uint8_t` | Número del pin GPIO donde está conectado el sensor IR |
| `modoGoteo` | `ModoGoteo` | Modo del equipo (normal o micro). Puede afectar la lógica de debounce |
| `ultimaGota` | `uint32_t` | Timestamp en ms del momento en que cayó la última gota. Sirve para detectar si el sistema se detuvo |
| `callback` | función | Función que se llama cada vez que se detecta una gota. Apunta a un método de `DropProcessor` |

**Métodos:**

| Método | Retorna | Descripción |
|---|---|---|
| `begin()` | `void` | Configura el pin como entrada y registra la interrupción de hardware. Se llama una sola vez en `setup()` |
| `onGotaDetectada()` | `void` | Se ejecuta automáticamente cuando el sensor dispara. Guarda el timestamp y llama al callback. Debe ser muy corta y rápida |
| `setModo(m)` | `void` | Cambia el modo de goteo en caliente, sin necesidad de reiniciar |

**Ejemplo de uso:**

```cpp
GoteoDetector detector(PIN_SENSOR_IR, ModoGoteo::NORMAL_GOTEO);
detector.begin();
// A partir de aquí, onGotaDetectada() se llama automáticamente
// cada vez que el sensor detecta una gota
```

---

### 4.2 DropProcessor

**¿Qué hace?**  
Recibe la notificación de cada gota desde `GoteoDetector` y realiza todos los cálculos matemáticos del sistema: cuántas gotas por minuto está teniendo el equipo y cuánto tiempo falta para que se termine la bolsa o botella.

**¿Cómo calcula las gotas por minuto?**  
Usa una **ventana deslizante de timestamps**. Guarda los tiempos de las últimas N gotas en un arreglo. Cuando llega una gota nueva, calcula cuánto tiempo pasó desde la primera gota de la ventana hasta la última, y con eso obtiene la tasa actual. Esto es más preciso que simplemente contar gotas en un minuto fijo.

**¿Cómo calcula el tiempo restante?**  
Con esta fórmula:

```
volumenPorGota = (modoGoteo == NORMAL) ? 0.05 mL : 0.0167 mL
volRestante = volTotal - (totalGotas * volumenPorGota)
tiempoRestante = volRestante / (gotasPorMin * volumenPorGota)
```

**Atributos:**

| Atributo | Tipo | Descripción |
|---|---|---|
| `ventana[]` | `uint32_t[]` | Arreglo circular con los timestamps de las últimas N gotas |
| `modoGoteo` | `ModoGoteo` | Define cuántos mL equivale cada gota |
| `volTotal` | `float` | Volumen total de la bolsa en mL (lo configura el enfermero) |
| `volRestante` | `float` | Volumen estimado que queda en la bolsa |

**Métodos:**

| Método | Retorna | Descripción |
|---|---|---|
| `registrarGota()` | `void` | Recibe el evento de gota desde `GoteoDetector` y lo agrega a la ventana |
| `getGotasPorMin()` | `float` | Calcula y retorna la tasa actual de goteo |
| `getTiempoRestante()` | `uint32_t` | Retorna los minutos estimados que quedan |
| `getSnapshot()` | `DataPacket` | Empaqueta todos los datos actuales en un `DataPacket` listo para enviarse o mostrarse |

---

### 4.3 VenoclisisSession

**¿Qué hace?**  
Guarda el estado y la información de la sesión actual de venoclisis. Una "sesión" es el periodo desde que el enfermero coloca una nueva bolsa hasta que se termina. Si se cambia la bolsa, se crea una nueva sesión.

**¿Por qué es importante?**  
Porque el sistema necesita saber cuándo empezó la sesión, quién es el paciente, y en qué estado está. Con esto se pueden generar logs e historial clínico en el servidor.

**Atributos:**

| Atributo | Tipo | Descripción |
|---|---|---|
| `pacienteId` | `String` | Identificador único del paciente. Lo ingresa el enfermero al inicio |
| `inicioSesion` | `uint32_t` | Timestamp del momento en que se inició la sesión |
| `volTotal` | `float` | Volumen total registrado para esta sesión en mL |
| `estado` | `EstadoSesion` | Estado actual: ACTIVA, PAUSADA, FINALIZADA o ERROR |

**Métodos:**

| Método | Retorna | Descripción |
|---|---|---|
| `iniciar()` | `void` | Marca el inicio de la sesión, guarda el timestamp |
| `pausar()` | `void` | Cambia el estado a PAUSADA. El sistema deja de contar gotas |
| `terminar()` | `void` | Cierra la sesión y cambia el estado a FINALIZADA |
| `isActiva()` | `bool` | Retorna `true` si la sesión está en curso y midiendo |

---

### 4.4 DataPacket

**¿Qué hace?**  
Es una estructura de datos simple (no tiene lógica propia) que agrupa toda la información de un instante de medición. Funciona como el "sobre" en el que se empaquetan los datos para enviarlos por MQTT o mostrarlos en pantalla.

**¿Por qué existe?**  
Porque varias clases necesitan los mismos datos al mismo tiempo: `MQTTPublisher` los necesita para enviarlos, `DisplayController` los necesita para mostrarlos, y `AlertManager` los necesita para evaluarlos. En lugar de pasar cada dato por separado, se pasan todos juntos en un `DataPacket`.

**Atributos:**

| Atributo | Tipo | Descripción |
|---|---|---|
| `pacienteId` | `String` | ID del paciente al que pertenece esta lectura |
| `gotasPorMin` | `float` | Tasa de goteo en el momento de la captura |
| `tiempoRestante` | `uint32_t` | Minutos estimados que quedan en la bolsa |
| `modo` | `ModoGoteo` | Modo de goteo activo en este momento |
| `timestamp` | `uint32_t` | Momento exacto en que se generó este paquete |

---

### 4.5 DisplayController

**¿Qué hace?**  
Maneja toda la interacción física entre el sistema y el enfermero: muestra los datos en la pantalla LCD/OLED, lee el encoder rotativo y los dos botones, y traduce las acciones del enfermero en eventos que el sistema puede procesar.

**¿Por qué existe como clase separada?**  
Porque la interfaz de usuario puede cambiar (diferente pantalla, diferente resolución, botones adicionales) sin afectar la lógica de negocio del sistema.

**Atributos:**

| Atributo | Tipo | Descripción |
|---|---|---|
| `pantalla` | `LCDDriver` | Objeto del driver de la pantalla |
| `encoder` | `RotaryEncoder` | Objeto del encoder rotativo |
| `boton1` | `Button` | Botón de confirmación / acción principal |
| `boton2` | `Button` | Botón de cancelar / acción secundaria |

**Métodos:**

| Método | Retorna | Descripción |
|---|---|---|
| `render(pkt)` | `void` | Recibe un `DataPacket` y actualiza la pantalla con los valores actuales |
| `leerEntrada()` | `UIEvent` | Revisa si el enfermero movió el encoder o presionó algún botón. Retorna el evento correspondiente |
| `mostrarAlerta(msg)` | `void` | Muestra un mensaje de alerta en pantalla, opcionalmente con parpadeo |

---

### 4.6 MQTTPublisher

**¿Qué hace?**  
Se encarga de la comunicación WiFi con el broker MQTT. Toma los `DataPacket` generados por `DropProcessor` y los publica en el topic correspondiente para que el nodo Braquio los reciba.

**Estructura de topics sugerida:**

```
rex/paciente/{id}/data       → datos normales de goteo
rex/paciente/{id}/alert      → alertas urgentes
rex/braquio/{id}/heartbeat   → señal de vida del dispositivo
```

**Atributos:**

| Atributo | Tipo | Descripción |
|---|---|---|
| `client` | `WiFiClient` | Cliente WiFi del ESP32 |
| `broker` | `String` | IP o hostname del broker MQTT (ej. `192.168.1.100`) |
| `topicBase` | `String` | Prefijo base del topic (ej. `rex/paciente/cama-03`) |

**Métodos:**

| Método | Retorna | Descripción |
|---|---|---|
| `connect()` | `bool` | Establece conexión con el broker. Retorna `true` si fue exitosa |
| `publish(pkt)` | `void` | Serializa el `DataPacket` a JSON y lo publica en el topic correspondiente |
| `reconnect()` | `void` | Intenta reconectarse si se pierde la conexión. Se llama desde el loop principal |

---

### 4.7 ConfigManager

**¿Qué hace?**  
Guarda y carga los parámetros de configuración que el enfermero ingresa al inicio de cada sesión. Usa la memoria **NVS (Non-Volatile Storage)** del ESP32, que persiste aunque el dispositivo se reinicie o se quede sin luz.

**¿Por qué es importante?**  
Si el ESP32 se reinicia accidentalmente en medio de una sesión, al volver a encender el sistema puede recuperar los parámetros (ID del paciente, volumen, modo) sin que el enfermero tenga que volver a configurarlo.

**Atributos:**

| Atributo | Tipo | Descripción |
|---|---|---|
| `volTotal` | `float` | Volumen total de la bolsa configurado para la sesión actual |
| `modoGoteo` | `ModoGoteo` | Modo de goteo configurado |
| `pacienteId` | `String` | ID del paciente activo |

**Métodos:**

| Método | Retorna | Descripción |
|---|---|---|
| `cargar()` | `void` | Lee los parámetros desde la memoria NVS al arrancar |
| `guardar()` | `void` | Escribe los parámetros actuales en la memoria NVS |
| `get/set()` | varios | Getters y setters para cada parámetro individual |
| `resetearDefault()` | `void` | Establece los valores por defecto en la memoria|
| `debug()` | `void` | Imprime en monitor la configuración actual|

---

### 4.8 AlertManager

**¿Qué hace?**  
Evalúa continuamente los datos del sistema y determina si algo está fuera de los límites normales. Cuando detecta un problema, dispara una alerta: muestra un mensaje en pantalla a través de `DisplayController` y publica un mensaje de alerta por MQTT a través de `MQTTPublisher`.

**¿Qué condiciones evalúa?**

| Condición | Tipo de alerta |
|---|---|
| El tiempo restante llega a cero | `FIN_BOLSA` |
| Las gotas/min caen por debajo del umbral mínimo | `GOTEO_LENTO` |
| Las gotas/min suben por encima del umbral máximo | `GOTEO_RAPIDO` |
| No se detectan gotas por más de N segundos | `DESCONECTADO` |

**Atributos:**

| Atributo | Tipo | Descripción |
|---|---|---|
| `umbrales` | `Umbrales` | Estructura con los valores mínimos y máximos configurados |
| `activa` | `bool` | Indica si hay una alerta activa en este momento |

**Métodos:**

| Método | Retorna | Descripción |
|---|---|---|
| `evaluar(pkt)` | `void` | Recibe un `DataPacket` y revisa si alguna condición se cumple |
| `emitirAlerta(t)` | `void` | Dispara la alerta del tipo indicado hacia pantalla y MQTT |
| `silenciar()` | `void` | El enfermero puede silenciar la alerta desde los botones |
| `isActiva()` | `bool` | Retorna `true` si hay una alerta sin resolver actualmente |

---

### 4.9 REXController

**¿Qué hace?**  
Es el cerebro y orquestador del sistema completo. Es la única clase que conoce a todas las demás y las coordina. Vive directamente en `setup()` y `loop()` de Arduino. Su trabajo es inicializar todo al arranque y luego mantener el ciclo principal corriendo de forma ordenada.

**¿Por qué es el orquestador?**  
Porque ninguna otra clase sabe de la existencia de las demás. `GoteoDetector` no sabe que existe `MQTTPublisher`. `DropProcessor` no sabe que existe `DisplayController`. Solo `REXController` conecta todo. Esto hace que cada clase sea independiente y fácil de probar por separado.

**Atributos (referencias a las demás clases):**

| Atributo | Tipo | Descripción |
|---|---|---|
| `detector` | `GoteoDetector*` | Referencia al detector de gotas |
| `proc` | `DropProcessor*` | Referencia al procesador de cálculos |
| `display` | `DisplayController*` | Referencia al controlador de pantalla |
| `mqtt` | `MQTTPublisher*` | Referencia al publicador MQTT |

**Métodos:**

| Método | Retorna | Descripción |
|---|---|---|
| `setup()` | `void` | Instancia y configura todas las clases. Carga la configuración desde NVS. Conecta al WiFi y al broker MQTT |
| `loop()` | `void` | Ciclo principal: lee gotas, actualiza cálculos, refresca pantalla, publica datos, evalúa alertas |

**Pseudocódigo del loop principal:**

```
cada ciclo del loop():
  1. Leer entrada del enfermero (encoder/botones)
  2. Si hay evento UI → procesar cambio de parámetro
  3. Si la sesión está activa:
     a. Obtener snapshot de DropProcessor → DataPacket
     b. Enviar DataPacket a DisplayController → actualiza pantalla
     c. Enviar DataPacket a AlertManager → evalúa condiciones
     d. Cada N segundos → enviar DataPacket a MQTTPublisher
  4. Verificar conexión MQTT → reconectar si es necesario
```

---

## 5. Flujo de datos principal

Este es el camino que recorre la información desde que cae una gota hasta que llega a la pantalla de la enfermera:

```
[Sensor IR]
    │ interrupción de hardware
    ▼
[GoteoDetector.onGotaDetectada()]
    │ callback con timestamp
    ▼
[DropProcessor.registrarGota()]
    │ calcula gotas/min y tiempo restante
    ▼
[DataPacket]  ←── snapshot de todos los datos
    │
    ├──→ [DisplayController.render()] → pantalla del aparatito
    ├──→ [AlertManager.evaluar()]     → ¿hay problema?
    └──→ [MQTTPublisher.publish()]    → broker MQTT
                                              │
                                              ▼
                                        [Nodo Braquio]
                                              │
                                              ▼
                                      [Servidor central]
                                              │
                                              ▼
                                    [Dispositivo enfermera]
```

---

## 6. Relaciones entre clases

| Clase A | Relación | Clase B | Descripción |
|---|---|---|---|
| `REXController` | contiene y usa | todas las demás | Es el único que las instancia y coordina |
| `GoteoDetector` | notifica a | `DropProcessor` | Vía callback cada vez que cae una gota |
| `DropProcessor` | genera | `DataPacket` | A través de `getSnapshot()` |
| `MQTTPublisher` | publica | `DataPacket` | Lo serializa a JSON y lo envía al broker |
| `DisplayController` | recibe | `DataPacket` | Lo renderiza en pantalla |
| `AlertManager` | evalúa | `DataPacket` | Compara valores contra umbrales |
| `ConfigManager` | alimenta a | `VenoclisisSession` | Provee los parámetros de configuración |
| `AlertManager` | dispara alertas en | `DisplayController` y `MQTTPublisher` | Notificación local y remota simultánea |

---

## 7. Convenciones del proyecto

### Nombres
- Clases: `PascalCase` → `GoteoDetector`, `DropProcessor`
- Métodos y atributos: `camelCase` → `registrarGota()`, `ultimaGota`
- Enums: `UPPER_SNAKE_CASE` → `NORMAL_GOTEO`, `FIN_BOLSA`
- Constantes: `UPPER_SNAKE_CASE` → `PIN_SENSOR_IR`, `MQTT_BROKER_IP`

### Modificaciones al código
Para identificar cambios sobre el código base, se usan bloques comentados:

```cpp
// ===== INICIO [descripcion del cambio] =====
// ... código modificado o añadido ...
// ===== FIN [descripcion del cambio] =====
```

### Archivos del proyecto (estructura sugerida)
```
REX_V1/
├── src/
│   ├── GoteoDetector.h / .cpp
│   ├── DropProcessor.h / .cpp
│   ├── VenoclisisSession.h / .cpp
│   ├── DataPacket.h
│   ├── DisplayController.h / .cpp
│   ├── MQTTPublisher.h / .cpp
│   ├── ConfigManager.h / .cpp
│   ├── AlertManager.h / .cpp
│   └── REXController.h / .cpp
├── main.cpp
└── config.h   ← constantes globales (pins, broker IP, etc.)
```

---

*Documentación escrita para REX V1 · DinoStar*  
*Se actualizara este documento cada vez que se agregue o modifique una clase.*
