#include "ThingSpeak.h";     //Librería para el uso IoT en ThingSpeak
#include <WiFi.h>           //Librería para el uso del ESP32-WROOM-32D
#include <ArduinoJson.h>    //Librería para el uso del API WEBSERVER
#include <HTTPClient.h>     //Librería para el uso del API WEBSERVER
#include <WiFiClient.h>    //Librería para el uso del ESP32-WROOM-32D

float Sensibilidad=0.0675;

#define WIFI_SSID "Dgf"                    //Usuario para el WiFi
#define WIFI_PASSWORD "03121986"             //Contraseña WiFi
unsigned long channelID = 1684794;             //ChannelID del ThingSpeak- es el canal específicado para un usuario determinado
const char* WriteAPIKey = "H63VK5Z5O4I6AEC3";  //API KEYS es el punto de enlace con el canal del usuario thingSpeak


WiFiClient Cliente;                     //Crea un cliente para conectarse a una dirección IP de Internet

void setup() {

Serial.begin(9600);                       //Se utiliza rl monitor serial para verificar los datos envíados al thingSpeak

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);    /**********************/
  Serial.print("Conectado...");            /**********************/
  while(WiFi.status() != WL_CONNECTED){    /**********************/
    Serial.print(".");                     //Estebloque de código se utiliza para conectar al WiFi
    delay(500);                            //imprime . si está buscando la red y se conecta
    }                                      //
  Serial.print("conectado: ");             //Notificación de que se ha encontrado y conectado a la red definida arriba
  Serial.println(WiFi.localIP());          //Dirreción a la cual se ha establecido

                                           //Conexión al thingspeak
ThingSpeak.begin(Cliente);
}

void loop() {
 float ValorCorriente=get_corriente(200);//obtenemos la corriente promedio de 200 muestras
 ValorCorriente= ValorCorriente-17.23;                              //Corrección de offset
 
int sensorValue = analogRead(A0);                 //Lectura del valor analogo del ESP32 canal 0
//int sensor = analogRead(A3);                      //Lectura del valor analogo del ESP32 canal 3
float voltaje = sensorValue * ( 5 / 4095.0);      //Conversión del valor ADC
float MotorM1 = ((1.72*(voltaje))+1.4);
//float corriente = sensor * ( 3.3 / 4095.0);       //Conversión del valor ADC
String timeS = getTime();                         //Función para el API- Devuelve la hora del lugar que establecemos en la dirección HTTP- en este caso de EUROPA-LONDRES
String ValorM = "";                               //Variable para tomar la parte de los minutos solamente

int Hora;  
 Hora = timeS.toInt();              //Debido a que thingSpeak solo recive variables int float etc, debemos convertir el valor String de texto del api a una variable entera en este caso
 
 ValorM = timeS.substring(3,-3);   //tomamos sólo el valor de los minutos
int Minutos;
 Minutos = ValorM.toInt();         //Debido a que thingSpeak solo recive variables int float etc, debemos convertir el valor String de texto del api a una variable entera en este caso
   
Serial.print(" CORRIENTE: ");
Serial.print(ValorCorriente);         //Se imprime un valor de voltaje del módulo en este caso Motor-Generador 
Serial.print(" Voltaje: ");
Serial.println(MotorM1);          //Se imprime un valor de Corriente del módulo en este caso Motor-Generador
                                  //Se imprime la hora tomada del API
Serial.print("ADC:");
Serial.println(voltaje);
Serial.print(" Hora: ");
Serial.println(Hora);
Serial.print(" Minutos: ");
Serial.println(Minutos);
                              
delay(100);
                                  //Se envían los datos a los campos del canal dashboard de thingSpeak (Voltaje de motor, corriente, Potencia, Hora de London, minutos en London)
ThingSpeak.setField (1,MotorM1);
ThingSpeak.setField (2,ValorCorriente);
ThingSpeak.setField (3,Hora);
ThingSpeak.setField (4,Minutos);
ThingSpeak.writeFields (channelID,WriteAPIKey); //Credenciales para enviar la información al thingSpeak específico
Serial.println ("Datos enviados a ThingSpeak!"); //Notificación de envío de datos
delay(15000);                                 //Tiempo de espera del ThingSpeak para poder enviar información para la versión gratuita es de 15segundos

}

float get_corriente(int n_muestras)
{
  float voltajeSensorC;
  float corriente=0;
  for(int i=0;i<n_muestras;i++)
  {
    voltajeSensorC = analogRead(A3) * (5.0 / 4095.0);////lectura del sensor
    corriente=corriente+(voltajeSensorC-2.429)/Sensibilidad; //Ecuación  para obtener la corriente
  }
  corriente=(corriente/n_muestras);
  return(corriente);
}

String getTime() {
  WiFiClient client;
  HTTPClient http;
  String timeS = "";

  http.begin(client, "http://worldtimeapi.org/api/timezone/Europe/London");
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY){
    String payload = http.getString(); 
    int beginS = payload.indexOf("datetime");
    int endS = payload.indexOf("day_of_week");
    Serial.println(payload);
    timeS = payload.substring(beginS + 22, endS - 16);   
   }
 return timeS;
  }
 
