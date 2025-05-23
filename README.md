# 🤖 RoboticaEthernetArduino

- Aparicio Llanquipacha Gabriel
- Arancibia Aguilar Daniel Andree
- Moises Llanos

**Control remoto de LED, sensor DHT11 y motor paso a paso vía Arduino con Ethernet Shield**

Este proyecto implementa un sistema de control robótico que permite gestionar remotamente un LED, un relé para activar un foco conectado a la corriente eléctrica, un motor paso a paso y monitorear un sensor de temperatura y humedad DHT11 a través de una interfaz web. El sistema utiliza un Arduino con Ethernet Shield actuando como servidor HTTP.

## 🌟 Características del Proyecto

- **🌐 Control Web**: Interfaz HTML para control remoto
- **🔌 Conectividad Ethernet**: Comunicación mediante shield W5100
- **💡 Control de LED**: Encendido/apagado de LED integrado
- **⚡ Control de Relé**: Activación de foco conectado a 110V/220V
- **🌡️ Sensor DHT11**: Lectura de temperatura y humedad
- **⚙️ Motor Paso a Paso**: Control de posición angular (0° a 360°)

## 📦 Estructura del Proyecto

```
📁 RoboticaEthernetArduino/
├── 📄 Examen.html                    # Interfaz web de control
├── 📄 ethernetledrelaympap.ino       # Código fuente para Arduino
└── 📄 README.md                      # Este archivo
```

## 🧰 Lista de Componentes

| Componente | Cantidad | Especificaciones |
|------------|----------|------------------|
| **Arduino UNO** | 1 | Microcontrolador ATmega328P |
| **Ethernet Shield W5100** | 1 | Para conectividad de red |
| **Módulo Relé 5V** | 1 | Para control del foco de 110V/220V |
| **Motor Paso a Paso NENA 23** | 1 | Con driver ULN2003 |
| **Sensor DHT11** | 1 | Sensor de temperatura y humedad |
| **LED** | 1 | LED indicador |
| **Resistencia 220Ω** | 1 | Para el LED |
| **Foco incandescente** | 1 | Conectado a través del relé |
| **Fuente externa variable** | 1 | Para alimentar el motor paso a paso |
| **Protoboard y cables** | Varios | Para conexiones |

## 🔌 Conexiones del Circuito

### Tabla de Conexiones

| Pin Arduino | Componente | Conexión | Descripción |
|-------------|------------|----------|-------------|
| **Digital 2** | Driver ULN2003 | IN1 | Control motor paso a paso |
| **Digital 4** | Driver ULN2003 | IN2 | Control motor paso a paso |
| **Digital 5** | Módulo Relé | IN | Control del relé para el foco |
| **Digital 7** | Sensor DHT11 | DATA | Lectura de temperatura y humedad |
| **Digital 8** | LED | Ánodo (+) | LED indicador (con resistencia 220Ω) |
| **5V** | Varios | VCC | Alimentación para módulos |
| **GND** | Varios | GND | Tierra común |

### Alimentación Externa
- **Motor Paso a Paso**: Requiere fuente externa de 12V conectada al driver ULN2003
- **Relé y Foco**: El relé controla directamente un foco conectado a un enchufe de 110V/220V
- **Tierra Común**: Conectar GND de la fuente externa con GND del Arduino

![Texto alternativo](imagenes/Proyecto.jpeg)

## 🌐 Interfaz Web (Examen.html)

La interfaz web proporciona los siguientes controles:

### Controles Disponibles
1. **Control de Relé**: Botón para encender/apagar el foco
2. **Control de LED**: Botón para encender/apagar el LED
3. **Control Simultáneo**: Botón para activar relé y LED al mismo tiempo
4. **Control de Motor**: Campo de entrada para especificar ángulo (0° a 360°)
5. **Monitor de Sensor**: Visualización en tiempo real de temperatura y humedad

### Configuración de IP
En el archivo `Examen.html`, modificar la IP del Arduino:
```javascript
const IP_ARDUINO = "http://192.168.199.214";
```

## 🔗 API del Arduino (ethernetledrelaympap.ino)

El Arduino actúa como servidor HTTP y responde a las siguientes rutas:

### Endpoints Disponibles

| Método | Ruta | Función |
|--------|------|---------|
| **POST** | `/interruptor1` | Alterna el estado del relé (foco) |
| **POST** | `/interruptor2` | Alterna el estado del LED |
| **POST** | `/interruptor3` | Activa simultáneamente relé y LED |
| **GET** | `/sensor` | Devuelve datos de temperatura y humedad |
| **GET** | `/mover/[grados]` | Mueve el motor al ángulo especificado |

### Ejemplos de Respuesta

**Sensor DHT11** (`GET /sensor`):
```json
{
  "exito": true,
  "temperatura": "25.3",
  "humedad": "61.4"
}
```

**Control de Motor** (`GET /mover/90`):
Mueve el motor a la posición de 90 grados desde su posición actual.

## ⚙️ Configuración e Instalación

### 1. Configuración del Arduino

1. **Configurar la IP en el código**:
   ```cpp
   IPAddress ip(192, 168, 199, 214);
   ```

2. **Cargar el código**: Subir `ethernetledrelaympap.ino` al Arduino usando el IDE de Arduino

3. **Verificar conexión**: El monitor serie debe mostrar la inicialización exitosa

### 2. Configuración de la Interfaz Web

1. **Abrir `Examen.html`** en un navegador web
2. **Verificar conectividad**: Los botones deben responder y mostrar el estado de los componentes
3. **Monitoreo automático**: Los datos del sensor se actualizan cada segundo

## 🎛️ Funcionamiento del Sistema

### Control de Relé (Foco)
- El relé se conecta directamente a un enchufe de corriente
- Al activarse, enciende el foco conectado
- Controlado desde la interfaz web mediante botón

### Control del LED
- LED conectado al pin 13 con resistencia limitadora
- Funciona como indicador visual del sistema
- Control independiente desde la interfaz web

## 🔄 Funcionamiento Paso a Paso del Sistema

### 1. Inicialización del Sistema

**Arranque del Arduino:**
1. **Configuración de red**: Arduino configura su IP estática (192.168.199.214) y se conecta a la red Ethernet
2. **Inicialización de pines**: Se configuran los pines digitales como entrada (sensor) o salida (relé, LED, motor)
3. **Configuración del servidor HTTP**: Arduino inicia el servidor web en el puerto 80
4. **Estado inicial**: Todos los componentes se encuentran en estado OFF/desactivado
5. **Verificación**: El monitor serie muestra "Servidor iniciado" confirmando la inicialización exitosa

**Preparación del sensor DHT11:**
1. El sensor se inicializa y realiza su primera lectura de calibración
2. Se establece el intervalo de lectura (cada 2 segundos mínimo por limitaciones del sensor)

### 2. Proceso de Control desde la Interfaz Web

**Carga de la página web (Examen.html):**
1. **Conexión inicial**: El navegador carga la página HTML y establece conexión con la IP del Arduino
2. **Verificación de conectividad**: JavaScript intenta conectarse al Arduino mediante fetch API
3. **Inicialización de elementos**: Se configuran los botones, campos de entrada y áreas de visualización
4. **Inicio de monitoreo**: Se inicia el timer para actualización automática del sensor cada segundo

**Flujo de control de componentes:**

#### Control del Relé (Foco):
1. **Usuario presiona botón**: Click en "Controlar Relé" en la interfaz web
2. **Envío de petición**: JavaScript envía POST request a `/interruptor1`
3. **Recepción en Arduino**: Arduino recibe la petición HTTP en el puerto 80
4. **Procesamiento**: El código analiza la URL y identifica el comando para el relé
5. **Cambio de estado**: Arduino alterna el estado del pin 5 (HIGH/LOW)
6. **Activación física**: El relé conmuta sus contactos, activando/desactivando el circuito del foco
7. **Respuesta HTTP**: Arduino envía confirmación de estado al navegador
8. **Actualización visual**: La interfaz web actualiza el estado del botón

#### Control del LED:
1. **Acción del usuario**: Click en "Controlar LED"
2. **Petición HTTP**: POST a `/interruptor2`
3. **Procesamiento interno**: Arduino recibe y procesa la petición
4. **Control de pin**: Cambio de estado del pin 13 (digitalWrite HIGH/LOW)
5. **Efecto físico**: LED se enciende/apaga según el nuevo estado
6. **Confirmación**: Arduino responde con el estado actual del LED

#### Control Simultáneo:
1. **Activación múltiple**: Usuario presiona "Activar Ambos"
2. **Procesamiento conjunto**: POST a `/interruptor3`
3. **Secuencia de activación**:
   - Primero se activa el relé (pin 5 = HIGH)
   - Inmediatamente después se activa el LED (pin 13 = HIGH)
4. **Respuesta unificada**: Arduino confirma la activación de ambos componentes

### 3. Control del Motor Paso a Paso

**Proceso detallado de movimiento:**
1. **Entrada de usuario**: Usuario ingresa ángulo deseado (0-360°) y presiona "Mover Motor"
2. **Validación**: JavaScript verifica que el valor esté en el rango válido
3. **Envío de comando**: GET request a `/mover/[ángulo]` (ejemplo: `/mover/90`)
4. **Recepción y parsing**: Arduino extrae el valor del ángulo de la URL
5. **Cálculo de pasos**:
   - Arduino conoce la posición actual del motor (variable global)
   - Calcula la diferencia entre posición actual y deseada
   - Determina la dirección de giro (horario/antihorario) para el menor recorrido
   - Convierte grados a pasos: `pasos = (ángulo * 2048) / 360`
6. **Secuencia de movimiento**:
   - Arduino envía pulsos secuenciales a los pines 2, 3, 4, 5 del driver ULN2003
   - Cada secuencia mueve el motor un paso (0.176°)
   - El proceso se repite hasta alcanzar la posición deseada
7. **Actualización de posición**: Arduino actualiza la variable de posición actual
8. **Confirmación**: Se envía respuesta HTTP confirmando el movimiento completado

**Secuencia de pasos del motor (patrón de activación):**
```
Paso 1: IN1=HIGH, IN2=LOW,  IN3=LOW,  IN4=LOW
Paso 2: IN1=LOW,  IN2=HIGH, IN3=LOW,  IN4=LOW
Paso 3: IN1=LOW,  IN2=LOW,  IN3=HIGH, IN4=LOW
Paso 4: IN1=LOW,  IN2=LOW,  IN3=LOW,  IN4=HIGH
```

### 4. Monitoreo del Sensor DHT11

**Ciclo continuo de lectura:**
1. **Timer automático**: Cada segundo, JavaScript ejecuta función de actualización
2. **Petición de datos**: GET request a `/sensor`
3. **Lectura física del sensor**:
   - Arduino activa el pin 7 para comunicarse con DHT11
   - Envía señal de inicio al sensor
   - DHT11 responde con 40 bits de datos (16 para humedad + 16 para temperatura + 8 checksum)
   - Arduino decodifica los bits y calcula valores reales
4. **Validación de datos**: Verificación de checksum para asegurar lectura correcta
5. **Formateo de respuesta**: Arduino crea JSON con temperatura y humedad
6. **Envío de datos**: Respuesta HTTP con los valores actuales
7. **Actualización visual**: JavaScript actualiza los elementos HTML con los nuevos valores
8. **Reinicio del ciclo**: Timer reinicia para la siguiente lectura en 1 segundo

**Formato de comunicación con DHT11:**
- **Señal de inicio**: 18ms LOW + 40μs HIGH
- **Respuesta del sensor**: 40 bits de datos + timing específico
- **Interpretación**: Los primeros 16 bits = humedad, siguientes 16 bits = temperatura

### 5. Gestión de Errores y Estados

**Manejo de fallos de comunicación:**
1. **Timeout de red**: Si no hay respuesta en 5 segundos, JavaScript muestra error de conectividad
2. **Datos corruptos del sensor**: Si checksum falla, Arduino mantiene última lectura válida
3. **Límites del motor**: Si se solicita ángulo fuera de rango, Arduino ignora el comando
4. **Sobrecarga de peticiones**: Arduino procesa peticiones secuencialmente para evitar conflictos

**Estados del sistema:**
- **IDLE**: Sistema esperando comandos
- **PROCESSING**: Ejecutando comando recibido
- **MOVING**: Motor en movimiento (no acepta nuevos comandos de motor)
- **READING**: Leyendo sensor DHT11
- **ERROR**: Estado de error temporal (se recupera automáticamente)

### 6. Comunicación HTTP Détallada

**Estructura de peticiones:**

### Motor Paso a Paso
- Alimentado por fuente externa de 5V
- Control de posición angular preciso
- Rango de movimiento: 0° a 360°
- El sistema calcula automáticamente el movimiento mínimo requerido

### Sensor DHT11
- Mide temperatura y humedad ambiental
- Actualización automática cada segundo
- Datos mostrados en tiempo real en la interfaz web

## 🔧 Especificaciones Técnicas

### Motor Paso a Paso 28BYJ-48
- **Pasos por revolución**: 2048 (con reductor interno)
- **Ángulo por paso**: 0.176°
- **Alimentación**: 5V DC (fuente externa)

### Sensor DHT11
- **Rango temperatura**: 0°C a 50°C
- **Rango humedad**: 20% a 90% RH
- **Precisión**: ±2°C, ±5% RH

### Conectividad
- **Protocolo**: HTTP sobre Ethernet
- **Puerto**: 80
- **IP por defecto**: 192.168.199.214

**Proyecto desarrollado para control robótico mediante Arduino y Ethernet Shield**
