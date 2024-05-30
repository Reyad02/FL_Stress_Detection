#include <Arduino.h>
#include "NeuralNetwork.h"
#include "model_data.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <rBase64.h>

NeuralNetwork *nn;

const char *ssid = "";
const char *password = "";
const char *serverUrl = "http://192.168.0.119:5001";
DynamicJsonDocument doc(2 * converted_model_tflite_len);
String response;
HTTPClient http;
const char *modelb64 = NULL;
char *model = NULL;

uint8_t count = 10;

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop()
{
  if (count == 10)
  {
    http.begin(String(serverUrl) + "/model");
    int httpResponseCode = http.GET();

    if (httpResponseCode == HTTP_CODE_OK)
    {
      response = http.getString();
      deserializeJson(doc, String(response));
      modelb64 = doc["model_encoded"];
      rbase64.decode(modelb64);
      model = rbase64.result();
      delete[] nn;
      nn = NULL;
      nn = new NeuralNetwork(model);
    }
    else
    {
      Serial.print("Error sending data. HTTP response code: ");
      Serial.println(httpResponseCode);
    }
    count = 0;
  }

  http.begin(String(serverUrl) + "/data");
  float *weights = nn->getWeights();
  char base64String[1024];
  rbase64.encode((uint8_t *)weights, sizeof(float) * nn->getWeights(), base64String);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(String("{\"weights\":\"" + String(base64String) + "\"}"));
  if (httpResponseCode == HTTP_CODE_OK)
  {
    response = http.getString();
    Serial.println(response);
  }
  else
  {
    Serial.print("Error sending data. HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  delay(10000);
  count++;
}