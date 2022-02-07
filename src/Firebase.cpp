#include "Firebase.h"
#include "Boat.hpp"

#include <Firebase_ESP_Client.h>
// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

const String dirAnchor = "/anchor";
const String dirBoat = "/boat";
const String fileFixed = "/fixed";
const String fileDistance = "/distance";
const String fileLatitude = "/latitude";
const String fileLongitude = "/longitude";
const String fileAccuracy = "/accuracy";
const String fileUser = "/user";
const String fileTime = "/time";
const String fileTimestamp = "/timestamp";

QueueHandle_t pushAnchorDataQueue = NULL;


void firebaseTask(void *pvParameters)
{
  Boat *boat = (Boat *)pvParameters;
  UBaseType_t uxHighWaterMark;
  pushAnchorDataQueue = xQueueCreate(1, sizeof(bool));
  String uid;
  //- Authentication
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;

  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  Serial.print("Firebase:");
  Serial.println(uxHighWaterMark);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  // Or use legacy authenticate method
  // config.database_url = DATABASE_URL;
  // config.signer.tokens.legacy_token = "<database secret>";
  // To connect without auth in Test Mode, see Authentications/TestMode/TestMode.ino
  //////////////////////////////////////////////////////////////////////////////////////////////
  // Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  // otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////
  Firebase.begin(&config, &auth);
  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
  //- End of Authentication

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "")
  {
    Serial.print('.');
    delay(1000);
  }
  uid = "/" + String(auth.token.uid.c_str());
  Serial.print(uid);

  //Init anchor status
  Firebase.RTDB.setBool(&fbdo, fileUser + uid + dirAnchor + fileFixed, boat->anchor.isFixed());


  uint timer = 0; // Cuenta segundos para transmitir datos
  bool retryPushing = false;
  while (1)
  {
    // Todo: Enviar solo los valores del ancla que cambian. Permite ahorrar muchos datos.
    if (Firebase.ready())
    {
      // Sends anchor data when a new message is received.
      bool push;
      if (xQueueReceive(pushAnchorDataQueue, &push, 0) || retryPushing == true) // Hay datos que enviar. Trabaja por polling
      {
        Serial.println("Sending Data...");
        Serial.println(retryPushing);
        bool success = true; // si alguno falla, se vuelve falso
        success &= Firebase.RTDB.setBool(&fbdo, fileUser + uid + dirAnchor + fileFixed, boat->anchor.isFixed());
        success &= Firebase.RTDB.setInt(&fbdo, fileUser + uid + dirAnchor + fileTimestamp, boat->anchor.getPosition().epoch);
        if (boat->anchor.isFixed())
        {
          success &= Firebase.RTDB.setFloat(&fbdo, fileUser + uid + dirBoat + fileDistance, boat->getAnchorDistance());
          success &= Firebase.RTDB.setFloat(&fbdo, fileUser + uid + dirAnchor + fileLatitude, boat->anchor.getPosition().latitude);
          success &= Firebase.RTDB.setFloat(&fbdo, fileUser + uid + dirAnchor + fileLatitude, boat->anchor.getPosition().longitude);
          success &= Firebase.RTDB.setFloat(&fbdo, fileUser + uid + dirAnchor + fileAccuracy, boat->anchor.getPosition().accuracy);
        }
        if (success == true)
        {
          retryPushing = false;
        }
        else
        {
          retryPushing = true;
        }
      }

      // Sends boat data every minute.
      if (timer == 60)
      {
        timer = 0;
        Firebase.RTDB.setTimestamp(&fbdo, fileUser + uid + "/timestamp");
        Firebase.RTDB.setFloat(&fbdo, fileUser + uid + dirBoat + fileLatitude, boat->gps.getPosition().latitude);
        Firebase.RTDB.setFloat(&fbdo, fileUser + uid + dirBoat + fileLongitude, boat->gps.getPosition().longitude);
        Firebase.RTDB.setFloat(&fbdo, fileUser + uid + dirBoat + fileAccuracy, boat->gps.getPosition().accuracy);
        if (boat->anchor.isFixed())
        {
          Firebase.RTDB.setFloat(&fbdo, fileUser + uid + dirBoat + fileDistance, boat->getAnchorDistance());
        }
      }
    }
    timer++;
    delay(1000);
  }
}
