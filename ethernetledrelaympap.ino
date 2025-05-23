#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>
#include <avr/pgmspace.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 199, 214);
IPAddress gateway(192, 168, 199, 110);
IPAddress subnet(255, 255, 255, 0);
EthernetServer servidor(80);
const int PIN_DIRECCION = 2;
const int PIN_PASO = 4;
const int PIN_RELE = 5;
const int PIN_LED = 8;
const int PIN_DHT = 7;
#define TIPO_DHT DHT11
DHT dht(PIN_DHT, TIPO_DHT);
int angulo_actual = 0;
int angulo_objetivo = 0;
bool estado_rele = false;
bool estado_led = false;
const unsigned long INTERVALO_SENSOR = 2000;
const unsigned long INTERVALO_VERIFICACION_ETHERNET = 10000;
const unsigned long INTERVALO_LOG_DHT = 1000;
unsigned long ultimo_log_dht = 0;

void enviarRespuestaCors(EthernetClient cliente) {
  cliente.println(F("HTTP/1.1 204 No Content"));
  cliente.println(F("Access-Control-Allow-Origin: *"));
  cliente.println(F("Access-Control-Allow-Methods: GET, POST, OPTIONS"));
  cliente.println(F("Access-Control-Allow-Headers: Content-Type"));
  cliente.println(F("Connection: close"));
  cliente.println();
}

void enviarRespuestaJson(EthernetClient cliente, const char* formato, ...) {
  char buffer[100];
  va_list args;
  va_start(args, formato);
  vsnprintf(buffer, sizeof(buffer), formato, args);
  va_end(args);
  cliente.println(F("HTTP/1.1 200 OK"));
  cliente.println(F("Content-Type: application/json"));
  cliente.println(F("Access-Control-Allow-Origin: *"));
  cliente.println(F("Connection: close"));
  cliente.println();
  cliente.println(buffer);
}

void enviarDatosSensor(EthernetClient cliente) {
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();
  if (isnan(temperatura) || isnan(humedad)) {
    enviarRespuestaJson(cliente, "{\"exito\":false,\"mensaje\":\"Error leyendo sensor\"}");
    return;
  }
  // Convertir flotantes a strings con 1 decimal
  char temp_str[10];
  char hum_str[10];
  dtostrf(temperatura, 4, 1, temp_str); // 4 caracteres totales, 1 decimal
  dtostrf(humedad, 4, 1, hum_str);
  // Enviar JSON con valores como strings
  enviarRespuestaJson(cliente, "{\"exito\":true,\"temperatura\":\"%s\",\"humedad\":\"%s\"}", temp_str, hum_str);
}

void moverMotor() {
  float delta = fmod((angulo_objetivo - angulo_actual + 360.0), 360.0);
  if (delta > 180.0) {
    delta = 360.0 - delta;
    digitalWrite(PIN_DIRECCION, HIGH);
    Serial.println(F("Dirección: Horario"));
  } else {
    digitalWrite(PIN_DIRECCION, LOW);
    Serial.println(F("Dirección: Antihorario"));
  }
  int pasos_necesarios = delta / 1.8;
  Serial.print(F("Pasos necesarios: "));
  Serial.println(pasos_necesarios);
  for (int i = 0; i < pasos_necesarios; i++) {
    digitalWrite(PIN_PASO, HIGH);
    delay(10);
    digitalWrite(PIN_PASO, LOW);
    delay(10);
  }
  angulo_actual = angulo_objetivo;
  Serial.print(F("Posición actualizada: "));
  Serial.println(angulo_actual);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_DIRECCION, OUTPUT);
  pinMode(PIN_PASO, OUTPUT);
  pinMode(PIN_RELE, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_RELE, LOW);
  digitalWrite(PIN_LED, LOW);
  dht.begin();
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Fallo DHCP, usando IP estática"));
    Ethernet.begin(mac, ip, gateway, gateway, subnet);
  }
  delay(1000);
  servidor.begin();
  Serial.println(F("================================================="));
  Serial.print(F("Servidor IP: "));
  Serial.println(Ethernet.localIP());
  Serial.println(F("Endpoints:"));
  Serial.println(F("- POST /interruptor1: Alternar relé"));
  Serial.println(F("- POST /interruptor2: Alternar LED"));
  Serial.println(F("- POST /interruptor3: Alternar ambos"));
  Serial.println(F("- GET /mover/[grados]: Mover motor"));
  Serial.println(F("- GET /sensor: Datos del sensor"));
  Serial.println(F("Sistema listo."));
  Serial.println(F("================================================="));
}

void loop() {
  unsigned long tiempo_actual = millis();
  if (tiempo_actual - ultimo_log_dht >= INTERVALO_LOG_DHT) {
    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();
    if (!isnan(temperatura) && !isnan(humedad)) {
      Serial.print(F("Temperatura: "));
      Serial.print(temperatura, 1);
      Serial.print(F(" °C, Humedad: "));
      Serial.print(humedad, 1);
      Serial.println(F(" %"));
    } else {
      Serial.println(F("Error leyendo sensor DHT"));
    }
    ultimo_log_dht = tiempo_actual;
  }

  EthernetClient cliente = servidor.available();
  if (cliente) {
    char solicitud[100];
    int indice_solicitud = 0;
    bool linea_en_blanco = true;
    bool es_post = false;
    bool es_get = false;
    bool es_options = false;
    bool solicitud_valida = false;
    
    while (cliente.connected() && indice_solicitud < 99) {
      if (cliente.available()) {
        char c = cliente.read();
        if (indice_solicitud < 99) solicitud[indice_solicitud++] = c;
        if (c == '\n' && linea_en_blanco) break;
        if (c == '\n') {
          linea_en_blanco = true;
          if (strncmp(solicitud, "POST", 4) == 0) {
            es_post = true;
            solicitud_valida = true;
          } else if (strncmp(solicitud, "GET", 3) == 0) {
            es_get = true;
            solicitud_valida = true;
          } else if (strncmp(solicitud, "OPTIONS", 7) == 0) {
            es_options = true;
            solicitud_valida = true;
          }
        } else if (c != '\r') {
          linea_en_blanco = false;
        }
      }
    }
    solicitud[indice_solicitud] = '\0';
    
    if (!solicitud_valida) {
      cliente.stop();
      return;
    }
    
    Serial.print(F("Cliente conectado desde: "));
    Serial.println(cliente.remoteIP());
    
    char url[20];
    int inicio_url = 0, fin_url = 0;
    for (int i = 0; i < indice_solicitud; i++) {
      if (solicitud[i] == ' ') {
        if (inicio_url == 0) inicio_url = i + 1;
        else { fin_url = i; break; }
      }
    }
    int longitud_url = fin_url - inicio_url;
    if (longitud_url >= 20) longitud_url = 19;
    strncpy(url, solicitud + inicio_url, longitud_url);
    url[longitud_url] = '\0';
    
    Serial.print(F("Endpoint solicitado: "));
    Serial.println(url);
    
    if (es_options) {
      enviarRespuestaCors(cliente);
    } else if (strcmp(url, "/interruptor1") == 0 && es_post) {
      estado_rele = !estado_rele;
      digitalWrite(PIN_RELE, estado_rele ? HIGH : LOW);
      enviarRespuestaJson(cliente, "{\"exito\":true,\"estado_rele\":%s}", estado_rele ? "true" : "false");
    } else if (strcmp(url, "/interruptor2") == 0 && es_post) {
      estado_led = !estado_led;
      digitalWrite(PIN_LED, estado_led ? HIGH : LOW);
      enviarRespuestaJson(cliente, "{\"exito\":true,\"estado_led\":%s}", estado_led ? "true" : "false");
    } else if (strcmp(url, "/interruptor3") == 0 && es_post) {
      estado_rele = !estado_rele;
      estado_led = !estado_led;
      digitalWrite(PIN_RELE, estado_rele ? HIGH : LOW);
      digitalWrite(PIN_LED, estado_led ? HIGH : LOW);
      enviarRespuestaJson(cliente, "{\"exito\":true,\"estado_rele\":%s,\"estado_led\":%s}", 
                          estado_rele ? "true" : "false", estado_led ? "true" : "false");
    } else if (strcmp(url, "/sensor") == 0 && es_get) {
      enviarDatosSensor(cliente);
    } else if (strncmp(url, "/mover/", 7) == 0 && es_get) {
      char* cadena_angulo = url + 7;
      bool es_numero = true;
      for (char* p = cadena_angulo; *p; p++) {
        if (!isdigit(*p)) { es_numero = false; break; }
      }
      if (es_numero) {
        int angulo = atoi(cadena_angulo);
        if (angulo >= 0 && angulo <= 360) {
          angulo_objetivo = angulo;
          Serial.println(F("------------------------------------------"));
          Serial.print(F("Ángulo objetivo recibido: "));
          Serial.println(angulo_objetivo);
          moverMotor();
          enviarRespuestaJson(cliente, "{\"exito\":true,\"angulo\":%d}", angulo);
          cliente.stop();
          return;
        }
      }
      enviarRespuestaJson(cliente, "{\"exito\":false,\"mensaje\":\"Ángulo inválido. Usa /mover/[0-360]\"}");
    } else {
      enviarRespuestaJson(cliente, "{\"exito\":false,\"mensaje\":\"Ruta no encontrada o método inválido\"}");
    }
    cliente.stop();
    Serial.println(F("Cliente desconectado"));
    Serial.println(F("------------------------------------------"));
  }
}