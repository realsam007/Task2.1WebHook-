#include <WiFiNINA.h>
#include "secrets.h"
#include "DHT.h"
#include "ThingSpeak.h" 

#define DHT_PIN 2      
#define DHT_TYPE DHT22 

char ssid[] = SECRET_SSID;   // Network SSID
char password[] = SECRET_PASS; // Network password
WiFiClient client;           // WiFi client object

unsigned long channelNumber = SECRET_CH_ID;
const char *apiWriteKey = SECRET_WRITE_APIKEY;

DHT dht(DHT_PIN, DHT_TYPE);

String statusMessage = "";

void setup() {
  Serial.begin(115200);  
  while (!Serial) {
    ; 
  }
  
  dht.begin();          
  ThingSpeak.begin(client);  // Initialize ThingSpeak communication
}

void loop() {
  // Ensure connection to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password); // Attempt to connect to WiFi
      Serial.print(".");
      delay(5000);                // Wait before trying again
    }
    Serial.println("\nConnected to WiFi.");
  }
  
  // Reading humidity and temperature
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();  
  // Read temperature in Celsius

  // Validate sensor readings
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Error reading from DHT sensor."));
    return;
  }

  // Update ThingSpeak fields
  ThingSpeak.setField(1, humidity);
  ThingSpeak.setField(2, temperature);
  
  // Set the status message
  statusMessage = "Data updated successfully";
  ThingSpeak.setStatus(statusMessage);
  
  // Write data to ThingSpeak
  int responseCode = ThingSpeak.writeFields(channelNumber, apiWriteKey);
  if (responseCode == 200) {
    Serial.println("Successfully updated ThingSpeak channel.");
  } else {
    Serial.println("Error updating ThingSpeak channel. HTTP error code: " + String(responseCode));
  }
  
  // Wait 60 seconds before the next update
  delay(60000);
}
