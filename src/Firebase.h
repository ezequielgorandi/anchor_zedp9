// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyBz5o3pwZH-5YESvoOWtAhJKWSxmMi6lOE"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://anchor-5ea71-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "jose@gmail.com"
#define USER_PASSWORD "unPerrito"

void firebaseTask(void *pvParameters);
void firebaseTask2(void *pvParameters);