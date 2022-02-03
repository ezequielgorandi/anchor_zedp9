#include "Firebase.h"
#include "Boat.hpp"

#include <Firebase_ESP_Client.h>
// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>
void firebaseTask(void *pvParameters)
{
  Boat *boat = (Boat *)pvParameters;
  UBaseType_t uxHighWaterMark;
  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  Serial.print("Firebase:");
  Serial.println(uxHighWaterMark);
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

  int count = 0;
  while (1)
  {
    if (Firebase.ready())
    {
      count = count + 10;
      Serial.printf("Set float... %s\n", Firebase.RTDB.setInt(&fbdo, F("/test/float"), count) ? "ok" : fbdo.errorReason().c_str());
      Firebase.RTDB.setBool(&fbdo, F("/anchor/fixed"), boat->anchor.isFixed());
    }
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    Serial.print("Firebase:");
    Serial.println(uxHighWaterMark);
    delay(5000);
  }
}
