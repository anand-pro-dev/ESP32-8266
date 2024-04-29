#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Anand";
const char* password =  "12345689";

//const char* ssid = "Airtel_gaur_9551";
//const char* password =  "air80473";
const int ledPin = 2; // Pin connected to the LED

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); // Set the LED pin as an output
  connectToWiFi();
}

void loop() {
  int productPrice = getProductPrice();
  if (productPrice == 1) {
    digitalWrite(ledPin, HIGH); // Turn on the LED
  } else {
    digitalWrite(ledPin, LOW); // Turn off the LED
  }
  delay(5000); // Wait for 10 seconds before checking again
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");
}

int getProductPrice() {
  HTTPClient http;
  http.begin("https://curd-api.onrender.com/api/products");
  int httpCode = http.GET();
  
  int productPrice = -1; // Default value if unable to fetch data
  
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    
    // Parse JSON response to extract productPrice
    int startIdx = payload.indexOf("productPrice\":") + 14;
    int endIdx = payload.indexOf(",", startIdx);
    String priceStr = payload.substring(startIdx, endIdx);
    productPrice = priceStr.toInt();
  } else {
    Serial.println("Error on HTTP request");
  }
  http.end();
  
  return productPrice;
}
