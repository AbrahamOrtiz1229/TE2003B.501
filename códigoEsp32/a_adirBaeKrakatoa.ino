//Incluir librerias para funcionamiento del código.
#include <WiFi.h>
#include <PubSubClient.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
//Libreria para teclado matricial
#include <Keypad.h>

//Definiciones para funcionamiento del teclado matricial
const byte FILAS = 4; // Número de filas del teclado
const byte COLUMNAS = 4; // Número de columnas del teclado
char tecla; //Leer la tecla
static char numero[4]; // Buffer para almacenar el número
static byte indice = 0; // Índice para almacenar los dígitos
int numeroEntero; //Poner número ya casteado
String temp; //Temporal para manejar datos
String charValue; //Char para lectura de datos

// Define los pines utilizados para las filas y columnas
byte pinesFilas[FILAS] = {23, 22, 19, 18};
byte pinesColumnas[COLUMNAS] = {5, 17, 16, 4};

char teclas[FILAS][COLUMNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// Crea un objeto Keypad
Keypad keypad = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);


// Replace the next variables with your SSID/Password combination
const char* ssid = "Eleazar";
const char* pass = "12345678";


// Insert Firebase project API Key
#define API_KEY "AIzaSyBz72ASb-I7Ng50LqE4t8t4kRFO0uj3M5U"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://krakatoa-2b78a-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//Definicion de variables
unsigned long sendDataPrevMillis = 0; //Contador de milis
int intValue; //Lectura de datos
int intValue1; //Lectura de datos para actualizar
float floatValue;

bool signupOK = false; //Condición de cuando se tiene todo listo.
int count = 0;  //Contador

//Se inicia la conexión con wifi y la de serial.
void setup() {
  Serial.begin(9600);
  delay(10);
  setup_wifi();
}

//Función que inicia la conexión a internet e inicia la base de datos.
void setup_wifi() {
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  //* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    signupOK = true;
  }


  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}


void loop() {
  //Para lectura de lo que preciona el teclado
  char tecla = keypad.getKey();
  //Solamente se cumple cuando se presiona el keypad
  if (tecla != NO_KEY) {
  
    // Solo aceptar dígitos del 0 al 9
    if (tecla >= '0' && tecla <= '9') {
      static char numero[4]; // Buffer para almacenar el número
      static byte indice = 0; // Índice para almacenar los dígitos
  
      // Agregar el dígito al buffer
      numero[indice] = tecla;
      indice++;
  
      // Verificar si se ha ingresado un número completo de 3 dígitos
      if (indice == 3) {
        numero[3] = '\0'; // Agregar el caracter nulo al final del número
        int numeroEntero = atoi(numero); // Convertir el número a entero
  
        // Hacer algo con el número entero (por ejemplo, imprimirlo)
        if (Firebase.ready() ) {
      
         // Write an Int number on the database path test/int
          if (Firebase.RTDB.setInt(&fbdo, "music/num", numeroEntero)){
    
          }

      }
 
        // Reiniciar el buffer y el índice
        memset(numero, 0, sizeof(numero));
        indice = 0;
      }
    }
  }
  else if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 50 || sendDataPrevMillis == 0)){
    // Leer datos
      sendDataPrevMillis = millis(); //Se le pone al timer los millis que lleva
      unsigned char cambiar = Serial.read();
      if (Firebase.RTDB.getString(&fbdo, "/movimiento/mover")) { //Se lee el dato
        //Se guarda el dato si es un string y se hace char para imprimir en serial y transmitir al robot
        if (fbdo.dataType() == "string") {
          charValue = fbdo.stringData();
          char receivedChar = charValue.charAt(0);
          Serial.println(receivedChar);
        }
      }
      if(cambiar == 'p'){//Detecta proximidad que transmite el bot
        if (Firebase.RTDB.getInt(&fbdo, "music/num")) {
          //Se guarda el dato si es int y se le suma uno para poder actualizar la misma celda
          if (fbdo.dataType() == "int") {
            intValue1 = fbdo.intData();
            intValue1 += 1;
            if (Firebase.RTDB.setInt(&fbdo, "music/num", intValue1)){}
          }
        }
      }

      else if(cambiar == 'c'){//Detecta choque que transmite el bot
        //Se guarda el dato si es int y se le resta uno para poder actualizar la misma celda
        if (Firebase.RTDB.getInt(&fbdo, "music/num")) {
          if (fbdo.dataType() == "int") {
            intValue1 = fbdo.intData();
            intValue1 -= 1;
            if (Firebase.RTDB.setInt(&fbdo, "music/num", intValue1)){}
          }
        }
      }
        
   }
}   
