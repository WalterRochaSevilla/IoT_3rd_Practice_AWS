#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* WIFI_SSID = "CATOLAB";
const char* WIFI_PASS = "sistemas2024";

// Replace with your MQTT broker details
const char* MQTT_BROKER = "a2x84s7vsr52x5-ats.iot.us-east-2.amazonaws.com"; // changeMe
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP-32";

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUNgxLczr6oX72MXc79ZoQTyBjhoQwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDQyMzIzMjE0
NloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKCBgQIcKaHJmVgfs2L/
KnOZyOe10IMHTOd1TIKgVI+vUuQzFFs4NfEOCABqMeBR+aW9cO7WShT1sb34CVvu
t2iHqC1ruk2k+VgilgGb3KbDtKXAH+x3Z8zF9a9q/Hr89jSn+riStcyGoYYtqTUq
DOrZKZfDiwQ9qnWaw08f9VdoXEcTtbWrES1sZjKTb367puxohNf5h0Pon57k87SP
j8jYL7LFlS/0LGlwR4Db9QIsLt/c2Fd50MDIWkaMuMlHLvJa/bO58U9bz8vzWaWZ
m+oWw5KmUkDJsGbDxGR5SN8375JKz41y2RFsRv0ohU0JJqhWl3AX76NsmUWn5qJF
8ccCAwEAAaNgMF4wHwYDVR0jBBgwFoAUVvcpilZvs/XyeeWy1R6/DYEMfO8wHQYD
VR0OBBYEFL7Gdd86ctSO+IkjEaAReECDbO1OMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAL7op3CtbGsd4lcukyBQzD94mY
Au9lkOL1Uolfv9EVLqCGRdtoDXFy6BORD2PX0TG+OPJ2kA175AyZ95n0YsOUIsYY
kKR9FmrtM8PYIHmQA4kC9Zd05gU5bZhxu2dZroSDzUkn6kWT1Ws9QU3yjL9Ptc70
UzOooN0t/6VptSYGER2hLqcApVhzdeoF+di0slmW562IFDxmWDIeEUsaevi5sdgS
DolX6RcBEdc1XrrsEl6Oh0hi61yES1ka+XtzP77AUBjCrojPLaZlaYfoR+xf4WpL
seqtfcH6p8cnrsBaq2UVdkCymp6j4z6PbnPaeSKyg+Om/W5CAWxRLq3cRY9O
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAoIGBAhwpocmZWB+zYv8qc5nI57XQgwdM53VMgqBUj69S5DMU
Wzg18Q4IAGox4FH5pb1w7tZKFPWxvfgJW+63aIeoLWu6TaT5WCKWAZvcpsO0pcAf
7HdnzMX1r2r8evz2NKf6uJK1zIahhi2pNSoM6tkpl8OLBD2qdZrDTx/1V2hcRxO1
tasRLWxmMpNvfrum7GiE1/mHQ+ifnuTztI+PyNgvssWVL/QsaXBHgNv1Aiwu39zY
V3nQwMhaRoy4yUcu8lr9s7nxT1vPy/NZpZmb6hbDkqZSQMmwZsPEZHlI3zfvkkrP
jXLZEWxG/SiFTQkmqFaXcBfvo2yZRafmokXxxwIDAQABAoIBAAlh6J+t88UQWtVq
7PqRffLJ/5OD7k9qgtVY5wMPykGn9wl97ueyLn0L2+3iR04cnKJSo6JihmwZPK2l
LS7hknkazPryvC5DwYukYe5wJQLel9XEmFkjVb8Dp2v8rgumc+H6G87kTJpLGHAW
hTilX6Hu1d6YQK0bybYcOOSMMrRMPrvSFGgwf/K3xX8c9ky+9+j7th+dvrrsu86I
43Hl1p8MHjCwyF/HZCe+M2EABsklK8XusrNoTkOdEiYF+SNO+Wrii+5axWrGHXFe
8oNQU+VM/q99Yb3UyFrLJu4paX9DKFyQaAOSSaCI9WdDmHoaULzQNtg5d9rCllVC
TKExBhkCgYEAz+bo7cSxCg2wYqWRvHGzn5ZG3TCcA5cXD3fA8i++5MaVKPqVGF30
bDTgIDN6uVU6ZfIaOPwMjVybmy3YJyolfm0nMNZqXgBQ/lEmrJvbqkmRzKiEb/HZ
2bkiizlghAVJFzPG1cK7kNi2CtFxr2r8YT5tH8+Yve4RkffQEJQVprsCgYEAxaOJ
lop52gDymBtPQO+tkqqN9+L6/mJ66Ub4CZ5jthvwe86Cm+wkggz63LbMxB4DKhU4
qCu27Kj6+qE3mTh7yR7l9qK6Ckjb5NNyrDf+jEfrRZmbFgPtFh8yDD38H+LpDoPA
aYWZu6MfSQ2CmNOhQ130Lo9NZ9HtKjIz7ROf3mUCgYBMs+GOD3pNvj0sN++rubtO
jo/4fT2iKXUtlL4P3EOufgF8C+9D/S33Wg6ycqu9gs8323mHOp6a7ObP+rr7wBXz
Lew+nWzC35UCLwh4uEvz6I6tFiM+ddks9ZvkB7gvHRAZSlX6eYzf5MRM+3bZBDy2
1+REzz+IGzRKSUZvR2MxjwKBgQCSgHtJADT7CKDLZ9MDy6bahVrfWX28e7uomFbc
qnA3WdaVr6rZcd5OGNkMKuT2q1ilJT3uSxk5rLj+d+sCxP/dQe4tOMq3PPLjWGo0
hv1FdzpLSWXQl/By6YTr1jbCXC1t+PP0RHiEwpI6UK1yI83yZf7PVKKIRvCKytP6
9QUYkQKBgQCvK4uVLo+sUsI34koOV3M2YHYac0dmymbgeBivR0IWGBLoREAa9s5n
EqYjM63RuetfTW8FdeZy/759I+qHyQlXrHlPH0drvnPzoQfB0qXSkQE+RcQRgJYl
1iMSDLl+sdrUzkwz6sKysVsIDOe8ytmPWqAX78k12MoV4OAlPmr5+A==
-----END RSA PRIVATE KEY-----
)KEY";

// Replace with your think shadow details
const char* UPDATE_TOPIC = "$aws/things/iot_object/shadow/update";              // publish changeMe
const char* UPDATE_DELTA_TOPIC = "$aws/things/iot_object/shadow/update/delta";  // subscribe changeMe

WiFiClientSecure wiFiClient;
PubSubClient client(wiFiClient);

StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;
StaticJsonDocument<JSON_OBJECT_SIZE(4)> outputDoc;
char outputBuffer[128];

byte ledBuiltIn = 0;

void reportLedBuiltIn() {
  outputDoc["state"]["reported"]["led_builtin"] = ledBuiltIn;
  serializeJson(outputDoc, outputBuffer);
  client.publish(UPDATE_TOPIC, outputBuffer);
}

void setLedBuiltIn() {
  if (ledBuiltIn) digitalWrite(LED_BUILTIN, HIGH);
  else digitalWrite(LED_BUILTIN, LOW);
  reportLedBuiltIn();
}

// Callback function to handle messages received from the subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) message += String((char) payload[i]);
  Serial.println("Message from topic " + String(topic) + ":" + message);
  DeserializationError err = deserializeJson(inputDoc, payload);
  if (!err) {
    if (String(topic) == UPDATE_DELTA_TOPIC) {
      ledBuiltIn = inputDoc["state"]["led_builtin"].as<int8_t>();
      setLedBuiltIn();
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  setupWiFi();

  wiFiClient.setCACert(AMAZON_ROOT_CA1);
  wiFiClient.setCertificate(CERTIFICATE);
  wiFiClient.setPrivateKey(PRIVATE_KEY);

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
}

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CLIENT_ID)) {
      Serial.println("connected");
      // Subscribe to a topic
      client.subscribe(UPDATE_DELTA_TOPIC);
      Serial.println("Subscribed to " + String(UPDATE_DELTA_TOPIC));
      delay(100);
      reportLedBuiltIn();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
