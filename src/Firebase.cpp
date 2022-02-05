#include "Firebase.h"
#include "Boat.hpp"

#include <Firebase_ESP_Client.h>
// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

String dirAnchor = "/anchor";
String dirBoat = "/boat";
String fileFixed = "/fixed";
String fileDistance = "/distance";
String fileLatitude = "/latitude"; 
String fileLongitude = "/longitude";

typedef struct 
{
  int isFixed = -1;
} saved_t;

void firebaseTask(void *pvParameters)
{
  Boat *boat = (Boat *)pvParameters;
  saved_t saved;
  UBaseType_t uxHighWaterMark;
  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  Serial.print("Firebase:");
  Serial.println(uxHighWaterMark);
  
  //- Authentication
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;
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
  int count = 0;
  while (1)
  {
    if (Firebase.ready())
    {
      count = count + 10;
      Firebase.RTDB.setBool(&fbdo, dirAnchor+fileFixed, boat->anchor.isFixed());
      Firebase.RTDB.setFloat(&fbdo, dirAnchor+fileDistance, count+boat->getAnchorDistance());
      Firebase.RTDB.setFloat(&fbdo, dirBoat+fileLatitude, boat->gps.getPosition().latitude);
      Firebase.RTDB.setFloat(&fbdo, dirBoat+fileLongitude, boat->gps.getPosition().longitude);
    }
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    Serial.print("Firebase:");
    Serial.println(uxHighWaterMark);
    delay(1000);
  }
}
