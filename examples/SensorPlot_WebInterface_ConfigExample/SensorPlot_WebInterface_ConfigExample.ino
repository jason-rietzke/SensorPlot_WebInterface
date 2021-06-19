#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <sensorplot_webinterface.h>


ESP8266WebServer server(80);
SensorPlot_WebInterface webInterface = SensorPlot_WebInterface();

int interfaceCallback(String input) {
  // use the input for example as a password and trigger some action
  // or use it as a command and execute accordingly
  // when the input was valid you should return a 1
  // in case of a invalid input return a 0
  if (input == "password") {
    // do something
    return 1;
  }
  return 0;
}


void sensorReading(float *measurements, int *measurementsCount, int maxMeasurements, int *measurementsTimestamp, float sensorInput) {
  if (*measurementsCount < maxMeasurements) {
    *measurementsCount += 1;
    measurements[(*measurementsCount - 1)] = sensorInput;
  } else {
    *measurementsCount = maxMeasurements;
    for(int i = 0; i < (*measurementsCount - 1); i++) {
      measurements[i] = measurements[i + 1];
    }
    measurements[(*measurementsCount - 1)] = sensorInput;
  }
  *measurementsTimestamp = millis();
}


// You can input up to 32 Plots which will be displayed as graphs on the WebInterface

// Graph 1
int measurementsCount1 = 0;
float measurements1[128] = {};
int measurementsTimestamp1 = millis();
int cycleDuration1 = 60; // duration in seconds
void sensorReading1() {
  float sensorInput = (float) 0;    // <- sensor reading for first input
  sensorReading(measurements1, &measurementsCount1, 128, &measurementsTimestamp1, sensorInput);
}

// Graph 2
int measurementsCount2 = 0;
float measurements2[128] = {};
int measurementsTimestamp2 = millis();
int cycleDuration2 = 60; // duration in seconds
void sensorReading2() {
  float sensorInput = (float) 0;    // <- sensor reading for second input
  sensorReading(measurements2, &measurementsCount2, 128, &measurementsTimestamp2, sensorInput);
}

void configWebInterface() {
  // Graph 1
  String name1 = "CO2 Concentration";
  String unit1 = "ppm";
  int good1 = 1200;
  int bad1 = 2000;
  int min1 = 400;
  int max1 = 3000;
  int stepsize1 = 200;
  int cycleStepsize1 = 600;
  webInterface.addPlot(name1, unit1, cycleDuration1, good1, bad1, min1, max1, stepsize1, cycleDuration1, cycleStepsize1, &measurementsCount1, measurements1, &measurementsTimestamp1);
  
  // Graph 2
  String name2 = "Temperature";
  String unit2 = "°C";
  int good2 = 30;
  int bad2 = 50;
  int min2 = -10;
  int max2 = 40;
  int stepsize2 = 5;
  int cycleStepsize2 = 600;
  webInterface.addPlot(name2, unit2, cycleDuration2, good2, bad2, min2, max2, stepsize2, cycleDuration2, cycleStepsize2, &measurementsCount2, measurements2, &measurementsTimestamp2);
}



void setup() {
  Serial.begin(115200);

  configWebInterface();
  webInterface.interfaceConfig("Sensor Measurements Title", "Inputfield", "Perform Action", 0);
  webInterface.serverResponseSetup(&server, &interfaceCallback);
  server.begin();

  sensorReading1();
  sensorReading2();

  WiFi.begin("WiFi SSID", "WiFi Password");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
  Serial.print("Use this URL: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}


void loop() {

  if ((millis() - measurementsTimestamp1) > (cycleDuration1 * 1000)) {
    sensorReading1();
  }
  if ((millis() - measurementsTimestamp2) > (cycleDuration2 * 1000)) {
    sensorReading2();
  }
  server.handleClient();

	delay(100);
}
