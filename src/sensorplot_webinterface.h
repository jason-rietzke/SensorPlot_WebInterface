#ifndef _SENSORPLOT_WEBINTERFACE_H
#define _SENSORPLOT_WEBINTERFACE_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <string.h>

typedef struct Sensor_Plot_S {
    String title;
    String unit;
    String slag;
    int interval;
    int good;
    int bad;

    int min;
    int max;
    int clipping;
    int stepsize;

    int cycle;
    int cycleStepsize;

    int *valuesCount;
    float *values;
    int *valuesMeasurmentMillis;
} Sensor_Plot;


class SensorPlot_WebInterface {
    private:
		String websiteTitle = "";
		String callbackInput = "";
		String callbackButton = "";

        int plotterCount = 0;
        Sensor_Plot *plotter_p[32];
        ESP8266WebServer *server;
        void responseHTML();
        void responseCSS();
        void responseJS();
        void responseConfig();
        void responseGraphData();
        void responseGraphSlag(int index);
        void responseCSV(int index);

    public: 
        SensorPlot_WebInterface();
        void addPlot(String title, String unit, int interval, int good, int bad, int min, int max, int stepsize, int cycle, int cycleStepsize, int *valuesCount, float *values, int *valuesMeasurmentMillis);
		void interfaceConfig(String websiteTitle, String callbackInput, String callbackButton);
        void serverResponseSetup(ESP8266WebServer *server, int (*callback)(String response));
};

#endif