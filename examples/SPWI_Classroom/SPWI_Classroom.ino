#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <sensorplot_webinterface.h>


ESP8266WebServer server(80);
SensorPlot_WebInterface webInterface = SensorPlot_WebInterface();

int interfaceCallback(String input) {
  if (input == "42") {
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
int co2measurementsCount = 0;
float co2measurements[128] = {};
int co2measurementsTimestamp = millis();
int co2cycleDuration = 60; // duration in seconds
void co2sensorReading() {
  float sensorInput = (float) readCO2Sensor();    // <- sensor reading for first input
  sensorReading(co2measurements, &co2measurementsCount, 128, &co2measurementsTimestamp, sensorInput);
}

// Graph 2
int tempMeasurementsCount = 0;
float tempMeasurements[128] = {};
int tempMeasurementsTimestamp = millis();
int tempCycleDuration = 60; // duration in seconds
void tempSensorReading() {
  float sensorInput = (float) readTemperature();    // <- sensor reading for second input
  sensorReading(tempMeasurements, &tempMeasurementsCount, 128, &tempMeasurementsTimestamp, sensorInput);
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
  webInterface.addPlot(name1, unit1, co2cycleDuration, good1, bad1, min1, max1, stepsize1, co2cycleDuration, cycleStepsize1, &co2measurementsCount, co2measurements, &co2measurementsTimestamp);
  
  // Graph 2
  String name2 = "Temperature";
  String unit2 = "°C";
  int good2 = 30;
  int bad2 = 50;
  int min2 = -10;
  int max2 = 40;
  int stepsize2 = 5;
  int cycleStepsize2 = 600;
  webInterface.addPlot(name2, unit2, tempCycleDuration, good2, bad2, min2, max2, stepsize2, tempCycleDuration, cycleStepsize2, &tempMeasurementsCount, tempMeasurements, &tempMeasurementsTimestamp);
}



void setup() {
  Serial.begin(115200);

  // setup webinterface
  configWebInterface();
  webInterface.interfaceConfig("Classroom 8a", "Input", "Perform Action");
  webInterface.serverResponseSetup(&server, &interfaceCallback);
  server.begin();

  co2sensorReading();
  tempSensorReading();

  // setup WiFi
  WiFi.begin("myWiFi", "myWiFiPassword");
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

  if ((millis() - co2measurementsTimestamp) > (co2cycleDuration * 1000)) {
    co2sensorReading();
  }
  if ((millis() - tempMeasurementsTimestamp) > (tempCycleDuration * 1000)) {
    tempSensorReading();
  }
  server.handleClient();

	delay(100);
}




// this creates simulated input values from a hypothetical CO2-Sensor
float readCO2Sensor() {
  float output = 0;
  if (co2measurementsCount > 0) {
    output = co2measurements[(co2measurementsCount - 1)] + ((float) random(-100, 200));
  } else {
    output = (float) random(700, 900);
  }
  return output;
}


// this creates simulates input values from a hypothetical Temperature-Sensor
float readTemperature() {
  float output = 0;
  if (tempMeasurementsCount > 0) {
    output = tempMeasurements[(tempMeasurementsCount - 1)] + ((float) random(-1, 1));
  } else {
    output = (float) random(19, 26);
  }
  return output;
}
