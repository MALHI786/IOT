#include <WiFi.h>

const char *ssid = "1272";
const char *password = "12345678";

IPAddress local_IP(192, 168, 5, 2);
IPAddress gateway(192, 168, 5, 1);
IPAddress subnet(255, 255, 255, 0);

void setup()
{
  Serial.begin(115200);
  delay(1000);

  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  Serial.println("Network Created!");
  Serial.print("Network Name: ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
}

void loop()
{
}