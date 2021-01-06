#include "sensorplot_webinterface.h"

#include <arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <string.h>

SensorPlot_WebInterface::SensorPlot_WebInterface() {
}

void SensorPlot_WebInterface::addPlot(String title, String unit, int interval, int good, int bad, int min, int max, int stepsize, int cycle, int cycleStepsize, int *valuesCount, int *values, int *valuesMeasurmentMillis) {
    if (this->plotterCount > 31) {
        return;
    }

    Sensor_Plot *plot = (new Sensor_Plot());
	plot->title = title;
	plot->unit = unit;
	plot->slag = String(this->plotterCount);
    plot->interval = interval;
    plot->good = good;
    plot->bad = bad;
    plot->min = min;
    plot->max = max;
    plot->clipping = 0;
    plot->stepsize = stepsize;
    plot->cycle = cycle;
    plot->cycleStepsize = cycleStepsize;
    plot->valuesCount = valuesCount;
    plot->values = values;
    plot->valuesMeasurmentMillis = valuesMeasurmentMillis;

    this->plotter_p[this->plotterCount] = plot;
    this->plotterCount ++;
}

void SensorPlot_WebInterface::serverResponseSetup(ESP8266WebServer *server) {
    this->server = server;
    
    this->server->on("/", [=]() {
        responseHTML();
    });
    this->server->on("/index.html", [=]() {
        responseHTML();
    });
    this->server->on("/style.css", [=]() {
        responseCSS();
    });
    this->server->on("/app.js", [=]() {
        responseJS();
    });
    
    this->server->on("/graphData", [=]() {
        responseGraphData();
    });

    for(int i = 0; i < this->plotterCount; i++) {
        this->server->on("/data/" + String(i), [=]() {
            responseGraphSlag(i);
        });
    }
}

void SensorPlot_WebInterface::responseHTML() {
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/html", this->HTML);
}

void SensorPlot_WebInterface::responseCSS() {
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/css", this->StyleSheet);
}

void SensorPlot_WebInterface::responseJS() {
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/javascript", this->JavaScript);
}

void SensorPlot_WebInterface::responseGraphData() {
    String response;
    response = "";

    for(int i = 0; i < this->plotterCount; i++) {
        String clipping;
        if (this->plotter_p[i]->clipping) {
            clipping = "true";
        } else {
            clipping = "false";
        }
        response += (this->plotter_p[i]->title + ",");
        response += (this->plotter_p[i]->unit + ",");
        response += (this->plotter_p[i]->slag + ",");
        response += (String(this->plotter_p[i]->interval) + ",");
        response += (String(this->plotter_p[i]->good) + ",");
        response += (String(this->plotter_p[i]->bad) + ",");
        response += (String(this->plotter_p[i]->min) + ",");
        response += (String(this->plotter_p[i]->max) + ",");
        response += (clipping + ",");
        response += (String(this->plotter_p[i]->stepsize) + ",");
        response += (String(this->plotter_p[i]->cycle) + ",");
        response += (String(this->plotter_p[i]->cycleStepsize));
        if (i < (this->plotterCount - 1)) {
            response += ";";
        }
    }

    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/plain", response);
}

void SensorPlot_WebInterface::responseGraphSlag(int index) {
    String response;
    response = "";

    response += (String(millis() - (*(this->plotter_p[index]->valuesMeasurmentMillis))) + ";");
    for(int i = 0; i < *(this->plotter_p[index]->valuesCount); i++) {
        response += String(this->plotter_p[index]->values[i]);
        if (i < (*(this->plotter_p[index]->valuesCount) - 1)) {
            response += ",";
        }
    }
    
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/plain", response);
}