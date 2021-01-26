# SensorPlot WebInterface

<img src="/documentation/Graph_with_Cursor.png" alt="Sensor-Graph with Info-Cursor Image" title="Sensor-Graph with Info-Cursor" width="75%">

## Description
The SensorPlot_WebInterface C++ library is made to be used on any Ardunio like microcontroller to make up a webinterface.
This interface is able to display up to 32 Plot-Graphs which represent sensor measurements. Each graph provides a *download csv* link which triggers a download of a CSV-File, containing all tracked Plot-Data of this sensor.

As a summory on the top is a view, containing the latest sensor measurements and a smiley which represents the state of those values. If they are not good or even bad the smiley changes its mood from happy to neutral or sad. These thresholds, as well as Name, Unit, etc. can be configured when adding a Plot.

By hovering over the graph, the Info-Cursor will show up and display detailed information about the measurement at this time. Displayed will be the exact value and time at which the measurement has been made.

Furthermore the webinterface also supports light and dark mode, automatically detected by the system preferences as well as a mobile and desktop mode, which is determined by the window width. 

---

## How to use it
This project depends on the ESP8266WebServer lib to create the server.
This repo contains a SensorPlot_WebInterface_ConfigExample.ino file where you can check out an easy example how to implement this library.
More detailed information about how to use it are here:


#### Step 1 - configuration
After instantiating the ESP8266WebServer and the SensorPlot_WebInterface you have to call the ``interfaceConfig("title", "input field", "perform action")`` method which takes 3 arguments.
1. The Website Title, wich should be displayed in the browser
2. Input Field, which is the placeholder text for the callback input field in the webinterface
3. Perform Action, which is the text of the button, which triggers the callback with the input field's text as param


#### Step 2 - add plots
Next you have to define all the Sensor-Plots you want to display on the interface. You can add up to 32 of them.
They can be added by calling the ``addPlot("Name", "Unit", interval, goodThreshold, badThreshold, min, max, stepsize, cycleDuration, cycleStepsize, &measurementsCount, measurements, &measurementsTimestamp)`` method which takes 13 arguments.

|Argument | Type | Description|
|--|--|---|
|1. The Sensor Name | String | how this measurement is called (e.g. "CO2 Concentration")|
|2. Sensor Unit | String | what this value is measured in (e.g. "ppm - parts per million")|
|3. Reload Interval | Int | in which interval shoud the webinterface reload the sensore data form the server (often same as cycleDuration)|
|4. Good Threshold | Int | up to which value can this measurement be considered as good|
|5. Bad Threshold | Int | at which value can this measurement be considered as bad|
|6. Min Graph-Value | Int | the minimum Y-Axis value that is static on the graph (gets overriden, when reaching this border)|
|7. Max Graph-Value | Int | the maximum Y-Axis value that is static on the graph (gets overriden, when reaching this border)|
|8. Graph Stepsize | Int | at which interval should the Y-Axis labels be displayed (e.g. 0 - 200 - 400 - 600 [stepszie = 200])|
|9. Cycle Duration | Int | in which interval are the values measured in seconds (e.g. cycleDuration = 60 | measurements are 1 minute apart)|
|10. Cycle Stepsize | Int | at which interval should the X-Axis labels be displayed (e.g. cycleStepsize = 300 | the labels are 5 minutes apart)|
|11. Measurements Counter Pointer | *Int | a pointer onto the counter of measurements tracked from this sensor|
|12. Measurements Pointer | *Float | a pointer onto the array containing the measurements from this sensor|
|13. Measurements Timestamp Pointer | *Int | a pointer onto the timestamp of the last measurement done with this sensor (millis since last measurement to calculate the time)|


#### Step 3 - response setup
The final step is to call the ``serverResponseSetup(&server, &callback)`` method which takes just 2 arguments.
1. A pointer onto the ESP8266WebServer
2. A pointer onto a method, returning an int, which gets called when the webinterface triggers the callback.

This callback function receives a String containing the input text from the input field on the website. By checking the input in this method you can trigger or execute any operation you like. Just make sure to return a 0 when the input is invalid and return 1 when it is valid. This will get looped through to the interface and changes the input fields color to green or red.

---

## Why has it been developed
This library was developed as part of an open source project at the Trier University of Applied Sciences Environmental Campus Birkenfeld.
The idea is to provide as many schools, universities as well as private person with a budget friendly way to build their own CO2-Concentration Measurement Stations. This should indicate when it's time to get fresh air, which got even more important throught to the Covid-Pandamic. 
This lib was created to allow a user friendly interaction and to provide an easy way for everyone checking the values of the past couple of hours.

For further information, please check out this website - [CO2 Ampel](http://www.co2ampel.org/)
Here you will find plans and descriptions to build your own device, when you like.

