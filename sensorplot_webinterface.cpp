#include "sensorplot_webinterface.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <string.h>

const char SENSORPLOT_HTML_RAW[] PROGMEM = {"<html>\n\
    <head>\n\
        <title>Sensor-Plots</title>\n\
        <meta charset='utf-16'>\n\
        <link rel='stylesheet' href='style.css'>\n\
        <script defer src='app.js'></script>\n\
    </head>\n\
    <body>\n\
        <div class='container'>\n\
            <svg class='smiley' id='smiley'>\n\
                <circle class='face' cx='50%' cy='50%'/>\n\
                <circle class='eye' cx='35%' cy='35%'/>\n\
                <circle class='eye' cx='65%' cy='35%' r='2.2vh'/>\n\
                <path class='mouth' id='mouth'/>\n\
            </svg>\n\
            <div class='measurementsContainer'>\n\
                <h1 id='widgetTitle'>Latest Measurements</h1>\n\
                <div class='measurements'></div>\n\
                <div>\n\
                    <input type='text' placeholder='input' name='input' id='callbackInput'>\n\
                    <input type='button' value='Perform' id='callbackButton'>\n\
                </div>\n\
            </div>\n\
        </div>\n\
        <div id='graphsContainer'>\n\
        </div>\n\
    </body>\n\
</html>"};

const char SENSORPLOT_STYLESHEET_RAW[] PROGMEM = {"* {\n\
    font-family: -apple-system, BlinkMacSystemFont, sans-serif;\n\
}\n\
@media(prefers-color-scheme: light) {\n\
    * {\n\
        --backgroundPrimaryColor: #f0f0fa;\n\
        --backgroundSecondaryColor: #e0e0ea;\n\
        --foregroundPrimaryColor: #10101a;\n\
        --foregroundSecondaryColor: #30303a;\n\
    }\n\
}\n\
@media(prefers-color-scheme: dark) {\n\
    * {\n\
        --backgroundPrimaryColor: #0a0a1a;\n\
        --backgroundSecondaryColor: #181828;\n\
        --foregroundPrimaryColor: #d0d0da;\n\
        --foregroundSecondaryColor: #a0a0aa;\n\
    }\n\
}\n\
@media (min-width: 750px) {\n\
    * {\n\
        --outterViewWidth: calc(100vw - 120px);\n\
        --outterMargin: 40px;\n\
        --outterPadding: 20px;\n\
        --smileySize: 250px;\n\
        --smileyFloat: left;\n\
        --smileyMarginLeft: 0;\n\
        --graphHeight: 500px;\n\
    }\n\
}\n\
@media (max-width: 750px) {\n\
    * {\n\
        --outterViewWidth: calc(100vw - 60px);\n\
        --outterMargin: 20px;\n\
        --outterPadding: 10px;\n\
        --smileySize: 350px;\n\
        --smileyFloat: none;\n\
        --smileyMarginLeft: calc((var(--outterViewWidth)/2) - (var(--smileySize)/2));\n\
        --graphHeight: 600px;\n\
    }\n\
}\n\
body {\n\
    background-color: var(--backgroundPrimaryColor);\n\
    color: var(--foregroundPrimaryColor);\n\
    padding: 0;\n\
    margin: 0;\n\
    margin-bottom: var(--outterMargin);\n\
    transition: all 0.3s;\n\
    -webkit-transition: all 0.3s;\n\
}\n\
h1 {\n\
    color: var(--foregroundSecondaryColor);\n\
    font-weight: 800;\n\
    font-size: 32px;\n\
}\n\
h2 {\n\
    color: var(--foregroundSecondaryColor);\n\
    font-weight: 600;\n\
    font-size: 24px;\n\
}\n\
a {\n\
    color: var(--foregroundSecondaryColor);\n\
}\n\
p {\n\
    font-size: 20px;\n\
}\n\
input {\n\
    background-color: var(--backgroundSecondaryColor);\n\
    color: var(--foregroundSecondaryColor);\n\
    border-style: solid;\n\
    border-width: 2px;\n\
    border-radius: 5px;\n\
}\n\
.container {\n\
    width: var(--outterViewWidth);\n\
    margin: var(--outterMargin);\n\
    margin-bottom: calc(var(--outterMargin)/2);\n\
    padding: var(--outterPadding);\n\
    background-color: var(--backgroundSecondaryColor);\n\
    border-radius: 25px;\n\
    clear: both;\n\
    overflow: hidden;\n\
}\n\
.measurementsContainer {\n\
    margin: var(--outterMargin);\n\
    color: var(--foregroundPrimaryColor);\n\
    font-weight: 500;\n\
    font-size: 16px;\n\
}\n\
.measurementsContainer .valueLabel {\n\
    color: var(--foregroundSecondaryColor);\n\
    font-weight: 600;\n\
}\n\
.smiley {\n\
    height: var(--smileySize);\n\
    width: var(--smileySize);\n\
    margin: var(--outterMargin);\n\
    margin-left: var(--smileyMarginLeft);\n\
    float: var(--smileyFloat);\n\
}\n\
.face {\n\
    --smileyColor: #ffe600;\n\
    fill: var(--smileyColor);\n\
    r: calc(var(--smileySize)/2);\n\
}\n\
.eye {\n\
    fill: #000000;\n\
    r: calc(var(--smileySize)/15);\n\
}\n\
.mouth {\n\
    fill: none;\n\
    stroke: #000000;\n\
    stroke-width: calc(var(--smileySize)/20);\n\
    stroke-linecap: round;\n\
    --animationTime: 0.5s;\n\
    transition: all var(--animationTime) ease-in-out;\n\
    -webkit-transition: all var(--animationTime) ease-in-out;\n\
}\n\
.graphContainer {\n\
    width: calc(100% - (var(--outterMargin)*2));\n\
    height: var(--graphHeight);\n\
    margin: var(--outterMargin);\n\
}\n\
.frame {\n\
    fill: none;\n\
    stroke: var(--foregroundSecondaryColor);\n\
    stroke-width: 5px;\n\
    stroke-linecap: round;\n\
}\n\
.graph {\n\
    fill: none;\n\
    stroke: var(--foregroundPrimaryColor);\n\
    stroke-width: 5px;\n\
    stroke-linecap: round;\n\
}\n\
circle {\n\
    fill: var(--foregroundPrimaryColor);\n\
    r: 5px;\n\
}\n\
.label {\n\
    fill: var(--foregroundPrimaryColor);\n\
    font-size: 14px;\n\
}\n\
.labelY {\n\
    fill: var(--foregroundPrimaryColor);\n\
    font-size: 14px;\n\
}\n\
.labelX {\n\
    fill: var(--foregroundPrimaryColor);\n\
    font-size: 14px;\n\
}\n\
.detailPointer {\n\
    stroke: var(--foregroundPrimaryColor);\n\
    stroke-width: 3px;\n\
    stroke-linecap: round;\n\
}\n\
.detailLabel {\n\
    fill: var(--backgroundSecondaryColor);\n\
    stroke: var(--foregroundPrimaryColor);\n\
    stroke-width: 3px;\n\
    stroke-linecap: round;\n\
}"};

const char SENSORPLOT_JAVASCRIPT_RAW[] PROGMEM = {"window.onload = createReferences;\n\
window.onresize = ()=>{\n\
    setMouth();\n\
    createGraphs();\n\
}\n\
const SmileyState = {\n\
    Happy: 'Happy',\n\
    Medium: 'Medium',\n\
    Sad: 'Sad'\n\
};\n\
let smiley;\n\
let mouth;\n\
let smileyState = SmileyState.Happy;\n\
function createReferences() {\n\
    smiley = document.getElementById('smiley');\n\
    mouth = document.getElementById('mouth');\n\
    setup();\n\
}\n\
function setup() {\n\
    const c = parseInt(smiley.clientWidth / 2);\n\
    let path = `M${c*0.6},${c*1.3} Q${c},${c*1.3} ${c*1.4},${c*1.3}`;\n\
    mouth.setAttribute('d', path);\n\
    document.getElementById('callbackButton').addEventListener('click', performCallback);\n\
    setTimeout(() => {\n\
        setMouth();\n\
    }, 500);\n\
    loadConfig();\n\
    loadGraphs();\n\
}\n\
// loading callback result from server\n\
function performCallback() {\n\
    let webClient = new XMLHttpRequest();\n\
    webClient.open('POST', '/callback?input=' + document.getElementById('callbackInput').value);\n\
    webClient.addEventListener('load', function(event) {\n\
        let color = 'var(--foregroundSecondaryColor)';\n\
        if (webClient.responseText == '0') {\n\
            color = 'red';\n\
        } else if (webClient.responseText == '1') {\n\
            color = 'green';\n\
        }\n\
        document.getElementById('callbackInput').style.borderColor = color;\n\
        document.getElementById('callbackButton').style.borderColor = color;\n\
    });\n\
    webClient.send();\n\
}\n\
// loading configdata from server\n\
function loadConfig() {\n\
    let webClient = new XMLHttpRequest();\n\
    webClient.open('GET', '/config');\n\
    webClient.addEventListener('load', function(event) {\n\
        const config = webClient.responseText.split(';');\n\
        document.title = config[0];\n\
        document.getElementById('widgetTitle').textContent = config[0];\n\
        document.getElementById('callbackInput').setAttribute('placeholder', config[1]);\n\
        document.getElementById('callbackButton').value = config[2];\n\
    });\n\
    webClient.send();\n\
}\n\
// loading graphdata from server\n\
function loadGraphs() {\n\
    let webClient = new XMLHttpRequest();\n\
    webClient.open('GET', '/graphData');\n\
    webClient.addEventListener('load', function(event) {\n\
        const graphs = webClient.responseText.split(';');\n\
        for(i=0;i<graphs.length;i++){\n\
            const graph = graphs[i];\n\
            const data = graph.split(',');\n\
            if(!(data.length < 12)){\n\
                createGraphModule(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11])\n\
            }\n\
        }\n\
        createGraphs();\n\
        createMeasurements();\n\
        validateSmileyState();\n\
    });\n\
    webClient.send();\n\
}\n\
// loading data from server\n\
function loadData(graph, interval, slag, immediate = 0) {\n\
    setTimeout(() => {\n\
        let webClient = new XMLHttpRequest();\n\
        webClient.open('GET', `/data/${slag}`);\n\
        webClient.addEventListener('load', function(event) {\n\
            const offset = webClient.responseText.split(';')[0];\n\
            graph.setAttribute('data-offset', offset);\n\
            const values = webClient.responseText.split(';')[1];\n\
            graph.setAttribute('data-values', values);\n\
            const date = new Date();\n\
            graph.setAttribute('data-reloaded', date.getTime());\n\
            createMeasurements();\n\
            createGraphs();\n\
            validateSmileyState();\n\
        });\n\
        webClient.send();\n\
        loadData(graph, interval, slag)\n\
    }, immediate ? 0 : (interval * 1000));\n\
}\n\
function validateSmileyState() {\n\
    const graphContainers = document.getElementsByClassName('graphContainer');\n\
    let state = SmileyState.Happy;\n\
    let smileyColor = '#33de18';\n\
    for(var i = 0; i < graphContainers.length; i++) {\n\
        const container = graphContainers[i];\n\
        const graph = container.getElementsByClassName('graph')[0];\n\
        let values = parseValues(graph);\n\
        let currentValue = values[values.length-1];\n\
        let goodThreshold = container.getAttribute('data-good-threshold');\n\
        let badThreshold = container.getAttribute('data-bad-threshold');\n\
        switch (state) {\n\
            case SmileyState.Happy:\n\
                if (currentValue > badThreshold && badThreshold != '') {\n\
                    console.log(currentValue);\n\
                    state = SmileyState.Sad;\n\
                    smileyColor = '#f03030';\n\
                } else if (currentValue > goodThreshold && goodThreshold != '') {\n\
                    console.log(currentValue);\n\
                    state = SmileyState.Medium;\n\
                    smileyColor = '#ffe600';\n\
                }\n\
                break;\n\
            case SmileyState.Medium:\n\
                if (currentValue > badThreshold && badThreshold != '') {\n\
                    console.log(currentValue);\n\
                    state = SmileyState.Sad;\n\
                    smileyColor = '#f03030';\n\
                }\n\
                break;\n\
            default:\n\
                break;\n\
        }\n\
    }\n\
    smileyState = state;\n\
    document.getElementsByClassName('face')[0].style.setProperty('--smileyColor', smileyColor);\n\
    animateMouth();\n\
}\n\
function setMouth() {\n\
    const c = parseInt(smiley.clientWidth / 2);\n\
    let path = `M${c*0.6},${c*1.3} Q${c},${c*1.3} ${c*1.4},${c*1.3}`;\n\
    switch (smileyState) {\n\
        case SmileyState.Happy:\n\
            path = `M${c*0.6},${c*1.3} Q${c},${c*1.8} ${c*1.4},${c*1.3}`; \n\
            break;\n\
        case SmileyState.Medium:\n\
            path = `M${c*0.6},${c*1.4} Q${c},${c*1.3} ${c*1.4},${c*1.2}`; \n\
            break;\n\
        case SmileyState.Sad:\n\
            path = `M${c*0.6},${c*1.4} Q${c},${c} ${c*1.4},${c*1.4}`; \n\
            break;\n\
    }\n\
    mouth.setAttribute('d', path);\n\
    setTimeout(() => {\n\
        mouth.style.setProperty('--animationTime', '0s');\n\
    }, 500);\n\
}\n\
function animateMouth() {\n\
    mouth.style.setProperty('--animationTime', '0.5s');\n\
    setMouth();\n\
}\n\
function createMeasurements() {\n\
    const measurements = document.getElementsByClassName('measurements')[0];\n\
    measurements.innerHTML = '';\n\
    const graphModules = document.getElementsByClassName('graphmodule');\n\
    for(var i = 0; i < graphModules.length; i++) {\n\
        let module = graphModules[i];\n\
        let container = module.getElementsByClassName('graphContainer')[0];\n\
        let graph = container.getElementsByClassName('graph')[0];\n\
        let values = parseValues(graph);\n\
        let p = document.createElement('p');\n\
        let label = document.createElement('span');\n\
        label.classList.add('valueLabel');\n\
        label.textContent = container.getAttribute('data-title') + ': ';\n\
        p.appendChild(label);\n\
        let value = document.createElement('span');\n\
        value.textContent = values[values.length-1] + ' ' + container.getAttribute('data-unit');\n\
        p.appendChild(value);\n\
        measurements.appendChild(p);\n\
    }\n\
}\n\
function createGraphModule(title, unit, slag, interval, good, bad, min, max, clipping, stepsize, cycle, cycleStepsize) {\n\
    const graphModule = document.createElement('div');\n\
    graphModule.classList.add('container', 'graphmodule');\n\
    const headline = document.createElement('h1');\n\
    headline.textContent = title + ' (' + unit + ')';\n\
    graphModule.appendChild(headline);\n\
\n\
    const csvLink = document.createElement('a');\n\
    csvLink.setAttribute('href', '/csv/' + slag);\n\
    csvLink.setAttribute('target', '_blank');\n\
    csvLink.textContent = 'download csv';\n\
    graphModule.appendChild(csvLink);\n\
    \n\
    const graphContainer = document.createElementNS('http://www.w3.org/2000/svg', 'svg');\n\
    graphContainer.classList.add('graphContainer');\n\
    graphContainer.setAttribute('data-title', title);\n\
    graphContainer.setAttribute('data-unit', unit);\n\
    graphContainer.setAttribute('data-slag', slag);\n\
    graphContainer.setAttribute('data-interval', interval);\n\
    graphContainer.setAttribute('data-good-threshold', good);\n\
    graphContainer.setAttribute('data-bad-threshold', bad);\n\
    graphModule.appendChild(graphContainer);\n\
    const graphPolygon = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');\n\
    graphPolygon.classList.add('graph');\n\
    graphPolygon.setAttribute('data-min', min);\n\
    graphPolygon.setAttribute('data-max', max);\n\
    graphPolygon.setAttribute('data-clipping', clipping);\n\
    graphPolygon.setAttribute('data-stepsize', stepsize);\n\
    graphPolygon.setAttribute('data-cycle', cycle);\n\
    graphPolygon.setAttribute('data-cycle-stepsize', cycleStepsize);\n\
    graphPolygon.setAttribute('data-values', '0');\n\
    graphContainer.appendChild(graphPolygon);\n\
    const framePolygon = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');\n\
    framePolygon.classList.add('frame');\n\
    graphContainer.appendChild(framePolygon);\n\
    document.getElementById('graphsContainer').appendChild(graphModule);\n\
    loadData(graphPolygon, interval, slag, 1);\n\
}\n\
function createGraphs() {\n\
    const graphContainers = document.getElementsByClassName('graphContainer');\n\
    for(var i = 0; i < graphContainers.length; i++) {\n\
        let container = graphContainers[i];\n\
        let graph = container.getElementsByClassName('graph')[0];\n\
        let frame = container.getElementsByClassName('frame')[0];\n\
        let height = container.clientHeight;\n\
        let width = container.clientWidth;\n\
        // Min, Max Values\n\
        let values = parseValues(graph);\n\
        let min = values[0];\n\
        let max = values[0];\n\
        for (var j=0; j<values.length; j++) {\n\
            if (values[j] < min) min = values[j];\n\
            if (values[j] > max) max = values[j];\n\
        }\n\
        if (graph.getAttribute('data-clipping')==='false') {\n\
            if (graph.getAttribute('data-min') && min>graph.getAttribute('data-min')) {\n\
                min = graph.getAttribute('data-min');\n\
            }\n\
            if (graph.getAttribute('data-max') && max<graph.getAttribute('data-max')) {\n\
                max = graph.getAttribute('data-max');\n\
            }\n\
        } else {\n\
            if (graph.getAttribute('data-min')) min = graph.getAttribute('data-min');\n\
            if (graph.getAttribute('data-max')) max = graph.getAttribute('data-max');\n\
        }\n\
        buildFrame(frame, height, width);\n\
        buildGraph(graph, values, min, max, height, width);\n\
        buildYLabels(container, graph, min, max, height, width);\n\
        buildXLabels(container, graph, values, height, width);\n\
        detailedView(i, container, graph, frame, values);\n\
    }\n\
}\n\
function buildFrame(frame, height, width) {\n\
    frame.setAttribute('points', `40,0 40,${height-20} ${width},${height-20} 40,${height-20}`);\n\
}\n\
function buildYLabels(container, graph, min, max, height, width) {\n\
    height = height - 20;\n\
    const stepSize = parseInt(graph.getAttribute('data-stepsize'));\n\
    min = parseFloat(min);\n\
    max = parseFloat(max);\n\
    let labels = container.getElementsByClassName('labelY');\n\
    for(i=labels.length-1;i>=0;i--) {\n\
        labels[i].remove();\n\
    }\n\
    for(j=0;j<=parseInt((max-min)/stepSize);j++) {\n\
        const yPos = height-((j/parseInt((max-min)/stepSize))*height);\n\
        if (yPos != NaN) {\n\
            const label = document.createElementNS('http://www.w3.org/2000/svg', 'text');\n\
            label.textContent = (min + (j*stepSize));\n\
            container.appendChild(label);\n\
            label.classList.add('labelY');\n\
            label.setAttribute('x',35-label.getBBox().width);\n\
            label.setAttribute('y',yPos);\n\
        }\n\
    }\n\
    labels = container.getElementsByClassName('labelY');\n\
    const labelTop = labels[labels.length - 1];\n\
    if (labelTop) {\n\
        if (labelTop.getBBox().y < labelTop.getBBox().height) {\n\
            labelTop.setAttribute('y',labelTop.getBBox().height);\n\
        }\n\
    }\n\
    const labelBottom = labels[0];\n\
    if (labelBottom) {\n\
        labelBottom.setAttribute('y',height-(labelTop.getBBox().height/2));\n\
    }\n\
}\n\
function buildXLabels(container, graph, values, height, width) {\n\
    height = height - 20;\n\
    width = width - 40;\n\
    let stepSize = parseInt(graph.getAttribute('data-cycle-stepsize'));\n\
    if (window.innerWidth < 750) {stepSize = stepSize*2}\n\
    const cycle = parseInt(graph.getAttribute('data-cycle'));\n\
    let labels = container.getElementsByClassName('labelX');\n\
    for(i=labels.length-1;i>=0;i--) {\n\
        labels[i].remove();\n\
    }\n\
    for(j=0;j<=values.length;j+=(stepSize/cycle)) {\n\
        const valueoffset = (values.length-(values.length-j))*1000*cycle;\n\
        const offset = graph.getAttribute('data-reloaded') - graph.getAttribute('data-offset') - valueoffset;\n\
        const date = new Date();\n\
        const timestamp = date.getTime();\n\
        const time = new Date(timestamp - (timestamp - offset));\n\
        const timeText = (time.getHours()<10?'0':'')+time.getHours()+':'+(time.getMinutes()<10?'0':'')+time.getMinutes();\n\
\n\
        const xPos = (width-((j/values.length)*width)+35);\n\
        if (xPos != NaN) {\n\
            const label = document.createElementNS('http://www.w3.org/2000/svg', 'text');\n\
            label.textContent = timeText;\n\
            container.appendChild(label);\n\
            label.classList.add('labelX');\n\
            label.setAttribute('x',xPos);\n\
            label.setAttribute('y',height+20);\n\
        }\n\
    }\n\
    labels = container.getElementsByClassName('labelX');\n\
    const labelRight = labels[0];\n\
    if (labelRight) {\n\
        if ((labelRight.getBBox().x+labelRight.getBBox().width) > container.getBBox().width) {\n\
            labelRight.setAttribute('x',container.getBBox().width-(labelRight.getBBox().width*2));\n\
            if (labels[1]) { labels[1].remove(); }\n\
        }\n\
    }\n\
}\n\
function buildGraph(graph, values, min, max, height, width) {\n\
    let dots = graph.parentNode.getElementsByTagName('circle');\n\
    while (dots[0]) {\n\
        dots[0].parentNode.removeChild(dots[0]);\n\
    }\n\
    let points = '';\n\
    width = width - 40;\n\
    height = height - 20;\n\
    points += `40,${height} `;\n\
    for(i=0;i<values.length;i++){\n\
        let offset = 40\n\
        if (i == values.length-1) { offset = 43; }\n\
        points += `${(width/(values.length-1))*i + offset},${height*(1-((values[i]-min)/(max-min)))} `;\n\
        const dot = document.createElementNS('http://www.w3.org/2000/svg', 'circle');\n\
        const dotX = (width/(values.length-1))*i + offset;\n\
        if (dotX != NaN) {\n\
            dot.setAttribute('cx', dotX);\n\
            dot.setAttribute('cy', height*(1-((values[i]-min)/(max-min))));\n\
            graph.parentNode.appendChild(dot);\n\
        }\n\
    }\n\
    points += `${width + 43},${height} `;\n\
    graph.setAttribute('points', points);\n\
}\n\
function parseValues(graph) {\n\
    const vals = graph.getAttribute('data-values').split(',');\n\
    for(i=0;i<vals.length;i++){\n\
        vals[i] = parseFloat(vals[i]);\n\
    }\n\
    return vals;\n\
}\n\
function detailedView(i, container, graph, frame, values) {\n\
    document.removeEventListener('mousemove', function(e){});\n\
    document.addEventListener('mousemove', function(e){\n\
        const top = frame.getBoundingClientRect().top + document.body.scrollTop;\n\
        const bottom = frame.getBoundingClientRect().bottom + document.body.scrollTop;\n\
        const left = frame.getBoundingClientRect().left + document.body.scrollLeft;\n\
        const right = frame.getBoundingClientRect().right + document.body.scrollLeft;\n\
        if(e.pageX>left && e.pageX<right && e.pageY>top && e.pageY<bottom){\n\
            const perc = (e.offsetX-40)/frame.getBBox().width;\n\
            const valueIndex = parseInt(values.length*perc);\n\
            createDetailedPointer(i, container, parseInt(frame.getBBox().width*perc)+40);\n\
            createDetailedLabel(i, container, graph, parseInt(frame.getBBox().width*perc)+40, values, valueIndex);\n\
        }else{\n\
            const dPointer = container.getElementsByClassName('detailPointer');\n\
            const dRect = container.getElementsByClassName('detailLabelRect');\n\
            const dLabels = container.getElementsByClassName('detailLabelText');\n\
            while (dPointer[0]) {\n\
                dPointer[0].parentNode.removeChild(dPointer[0]);\n\
            }\n\
            while (dRect[0]) {\n\
                dRect[0].parentNode.removeChild(dRect[0]);\n\
            }\n\
            while (dLabels[0]) {\n\
                dLabels[0].parentNode.removeChild(dLabels[0]);\n\
            }\n\
        }\n\
    }, false);\n\
}\n\
function createDetailedPointer(i, container, pos) {\n\
    var height = container.clientHeight;\n\
    var width = container.clientWidth;\n\
    const dPointer = container.getElementsByClassName('detailPointer');\n\
    while (dPointer[0]) {\n\
        dPointer[0].parentNode.removeChild(dPointer[0]);\n\
    }\n\
    const poly = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');\n\
    poly.classList.add('detailPointer');\n\
    container.appendChild(poly);\n\
    const dots = container.getElementsByTagName('circle');\n\
    let nearDotPos = (dots[0].getBBox().x + (dots[0].getBBox().width/2));\n\
    let dist = Math.abs(pos - dots[0].getBBox().x);\n\
    for(i=0;i<dots.length;i++){\n\
        const tempDist = Math.abs(pos - dots[i].getBBox().x);\n\
        if(tempDist<dist) { \n\
            dist = tempDist;\n\
            nearDotPos = (dots[i].getBBox().x + (dots[i].getBBox().width/2));\n\
        }\n\
    }\n\
    const p1 = container.createSVGPoint();\n\
    p1.x = nearDotPos\n\
    p1.y = height - 20;\n\
    const p2 = container.createSVGPoint();\n\
    p2.x = nearDotPos;\n\
    p2.y = 0;\n\
    if (nearDotPos != NaN) {\n\
        poly.points.appendItem(p1);\n\
        poly.points.appendItem(p2);\n\
    }\n\
}\n\
function createDetailedLabel(i, container, graph, pos, values, valueIndex) {\n\
    const dRect = container.getElementsByClassName('detailLabelRect');\n\
    while (dRect[0]) {\n\
        dRect[0].parentNode.removeChild(dRect[0]);\n\
    }\n\
    const rect = document.createElementNS('http://www.w3.org/2000/svg', 'rect');\n\
    rect.classList.add('detailLabel', 'detailLabelRect');\n\
    container.appendChild(rect);\n\
    rect.setAttribute('width', 120);\n\
    rect.setAttribute('height', 40);\n\
    rect.setAttribute('rx', 10);\n\
    rect.setAttribute('y', 1);\n\
    const dots = container.getElementsByTagName('circle');\n\
    let nearDotPos = (dots[0].getBBox().x + (dots[0].getBBox().width/2));\n\
    let dist = Math.abs(pos - dots[0].getBBox().x);\n\
    for(i=0;i<dots.length;i++){\n\
        const tempDist = Math.abs(pos - dots[i].getBBox().x);\n\
        if(tempDist<dist) { \n\
            dist = tempDist;\n\
            nearDotPos = (dots[i].getBBox().x + (dots[i].getBBox().width/2));\n\
        }\n\
    }\n\
    rect.setAttribute('x', nearDotPos-(rect.getBBox().width/2));\n\
    // offset correction on boundries\n\
    if ((rect.getBBox().x+rect.getBBox().width)+3 > (graph.getBBox().x+graph.getBBox().width)) { rect.setAttribute('x', ((graph.getBBox().x+graph.getBBox().width)-rect.getBBox().width-3))}\n\
    if (rect.getBBox().x < 40) { rect.setAttribute('x', 40)}\n\
    const dLabels = container.getElementsByClassName('detailLabelText');\n\
    while (dLabels[0]) {\n\
        dLabels[0].parentNode.removeChild(dLabels[0]);\n\
    }\n\
    const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');\n\
    text.classList.add('label', 'detailLabelText');\n\
    container.appendChild(text);\n\
    const valueView = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');\n\
    valueView.textContent = (values[valueIndex] ? values[valueIndex] : values[values.length - 1]) + ' ' + container.getAttribute('data-unit');\n\
    valueView.setAttribute('x', rect.getBBox().x + 5);\n\
    valueView.setAttribute('y', rect.getBBox().y + 15);\n\
    const valueoffset = (values.length-valueIndex)*1000*graph.getAttribute('data-cycle');\n\
    const offset = graph.getAttribute('data-reloaded') - graph.getAttribute('data-offset') - valueoffset;\n\
    const date = new Date();\n\
    const timestamp = date.getTime();\n\
    const time = new Date(timestamp - (timestamp - offset));\n\
    const timeText = (time.getHours()<10?'0':'')+time.getHours()+':'+(time.getMinutes()<10?'0':'')+time.getMinutes();\n\
    const timeView = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');\n\
    timeView.textContent = '🕔 '+timeText;\n\
    timeView.setAttribute('x', rect.getBBox().x + 5);\n\
    timeView.setAttribute('y', rect.getBBox().y + 32);\n\
    text.appendChild(valueView);\n\
    text.appendChild(timeView);\n\
}"};


const char SENSORPLOT_WIDGET_HTML_RAW[] PROGMEM = {"<html>\n\
    <head>\n\
        <title>Sensor-Plots | Widget</title>\n\
        <meta charset='utf-16'>\n\
        <link rel='stylesheet' href='style.css'>\n\
        <script defer src='widget.js'></script>\n\
        <style>\n\
            body {\n\
                background-color: #00000000;\n\
            }\n\
        </style>\n\
    </head>\n\
    <body>\n\
        <div class='container'>\n\
            <svg class='smiley' id='smiley'>\n\
                <circle class='face' cx='50%' cy='50%'/>\n\
                <circle class='eye' cx='35%' cy='35%'/>\n\
                <circle class='eye' cx='65%' cy='35%' r='2.2vh'/>\n\
                <path class='mouth' id='mouth'/>\n\
            </svg>\n\
            <div class='measurementsContainer'>\n\
                <h1 id='widgetTitle'>Latest Measurements</h1>\n\
                <div class='measurements'></div>\n\
            </div>\n\
        </div>\n\
    </body>\n\
</html>"};

const char SENSORPLOT_WIDGET_JAVASCRIPT_RAW[] PROGMEM = {"window.onload = createReferences;\n\
window.onresize = ()=>{\n\
	setMouth();\n\
}\n\
let Graphs = [];\n\
const SmileyState = {\n\
	Happy: 'Happy',\n\
	Medium: 'Medium',\n\
	Sad: 'Sad'\n\
};\n\
let smiley;\n\
let mouth;\n\
let smileyState = SmileyState.Happy;\n\
function createReferences() {\n\
	smiley = document.getElementById('smiley');\n\
	mouth = document.getElementById('mouth');\n\
	setup();\n\
}\n\
function setup() {\n\
	const c = parseInt(smiley.clientWidth / 2);\n\
	let path = `M${c*0.6},${c*1.3} Q${c},${c*1.3} ${c*1.4},${c*1.3}`;\n\
	mouth.setAttribute('d', path);\n\
	setTimeout(() => {\n\
		setMouth();\n\
	}, 500);\n\
	loadConfig();\n\
	loadGraphs();\n\
}\n\
// loading configdata from server\n\
function loadConfig() {\n\
	let webClient = new XMLHttpRequest();\n\
	webClient.open('GET', '/config');\n\
	webClient.addEventListener('load', function(event) {\n\
		const config = webClient.responseText.split(';');\n\
		document.title = config[0] + ' | Widget';\n\
		document.getElementById('widgetTitle').textContent = config[0];\n\
	});\n\
	webClient.send();\n\
}\n\
// loading graphdata from server\n\
function loadGraphs() {\n\
	let webClient = new XMLHttpRequest();\n\
	webClient.open('GET', '/graphData');\n\
	webClient.addEventListener('load', function(event) {\n\
		const graphs = webClient.responseText.split(';');\n\
		Graphs = [];\n\
		for(i=0;i<graphs.length;i++){\n\
			const graph = graphs[i];\n\
			const data = graph.split(',');\n\
			if(!(data.length < 12)){\n\
				Graphs.push({title: data[0], unit: data[1], slag: data[2], interval: data[3], good: data[4], bad: data[5], min: data[6], max: data[7], clipping: data[8], stepsize: data[9], cycle: data[10], cycleStepsize: data[11], value: 0});\n\
				loadData(data[3], data[2], 1);\n\
			}\n\
		}\n\
		createMeasurements();\n\
		validateSmileyState();\n\
	});\n\
	webClient.send();\n\
}\n\
// loading data from server\n\
function loadData(interval, slag, immediate = 0) {\n\
	setTimeout(() => {\n\
		let webClient = new XMLHttpRequest();\n\
		webClient.open('GET', `/data/${slag}`);\n\
		webClient.addEventListener('load', function(event) {\n\
			const values = webClient.responseText.split(';')[1].split(',');\n\
			Graphs[slag].value = values[values.length - 1];\n\
			createMeasurements();\n\
			validateSmileyState();\n\
		});\n\
		webClient.send();\n\
		loadData(interval, slag)\n\
	}, immediate ? 0 : (interval * 1000));\n\
}\n\
function validateSmileyState() {\n\
	let state = SmileyState.Happy;\n\
	let smileyColor = '#33de18';\n\
	for(var i = 0; i < Graphs.length; i++) {\n\
		const Graph = Graphs[i];\n\
		let currentValue = Graph.value;\n\
		let goodThreshold = Graph.good;\n\
		let badThreshold = Graph.bad;\n\
		switch (state) {\n\
			case SmileyState.Happy:\n\
				if (currentValue > badThreshold && badThreshold != '') {\n\
					console.log(currentValue);\n\
					state = SmileyState.Sad;\n\
					smileyColor = '#f03030';\n\
				} else if (currentValue > goodThreshold && goodThreshold != '') {\n\
					console.log(currentValue);\n\
					state = SmileyState.Medium;\n\
					smileyColor = '#ffe600';\n\
				}\n\
				break;\n\
			case SmileyState.Medium:\n\
				if (currentValue > badThreshold && badThreshold != '') {\n\
					console.log(currentValue);\n\
					state = SmileyState.Sad;\n\
					smileyColor = '#f03030';\n\
				}\n\
				break;\n\
			default:\n\
				break;\n\
		}\n\
	}\n\
	smileyState = state;\n\
	document.getElementsByClassName('face')[0].style.setProperty('--smileyColor', smileyColor);\n\
	animateMouth();\n\
}\n\
function setMouth() {\n\
	const c = parseInt(smiley.clientWidth / 2);\n\
	let path = `M${c*0.6},${c*1.3} Q${c},${c*1.3} ${c*1.4},${c*1.3}`;\n\
	switch (smileyState) {\n\
		case SmileyState.Happy:\n\
			path = `M${c*0.6},${c*1.3} Q${c},${c*1.8} ${c*1.4},${c*1.3}`; \n\
			break;\n\
		case SmileyState.Medium:\n\
			path = `M${c*0.6},${c*1.4} Q${c},${c*1.3} ${c*1.4},${c*1.2}`; \n\
			break;\n\
		case SmileyState.Sad:\n\
			path = `M${c*0.6},${c*1.4} Q${c},${c} ${c*1.4},${c*1.4}`; \n\
			break;\n\
	}\n\
	mouth.setAttribute('d', path);\n\
	setTimeout(() => {\n\
		mouth.style.setProperty('--animationTime', '0s');\n\
	}, 500);\n\
}\n\
function animateMouth() {\n\
	mouth.style.setProperty('--animationTime', '0.5s');\n\
	setMouth();\n\
}\n\
function createMeasurements() {\n\
	const measurements = document.getElementsByClassName('measurements')[0];\n\
	measurements.innerHTML = '';\n\
	for(var i = 0; i < Graphs.length; i++) {\n\
		let Graph = Graphs[i];\n\
		let p = document.createElement('p');\n\
		let label = document.createElement('span');\n\
		label.classList.add('valueLabel');\n\
		label.textContent = Graph.title + ': ';\n\
		p.appendChild(label);\n\
		let value = document.createElement('span');\n\
		value.textContent = Graph.value + ' ' + Graph.unit;\n\
		p.appendChild(value);\n\
		measurements.appendChild(p);\n\
	}\n\
}"};

__FlashStringHelper* SENSORPLOT_HTML = (__FlashStringHelper*)SENSORPLOT_HTML_RAW;
__FlashStringHelper* SENSORPLOT_STYLESHEET = (__FlashStringHelper*)SENSORPLOT_STYLESHEET_RAW;
__FlashStringHelper* SENSORPLOT_JAVASCRIPT = (__FlashStringHelper*)SENSORPLOT_JAVASCRIPT_RAW;

__FlashStringHelper* SENSORPLOT_WIDGET_HTML = (__FlashStringHelper*)SENSORPLOT_WIDGET_HTML_RAW;
__FlashStringHelper* SENSORPLOT_WIDGET_JAVASCRIPT = (__FlashStringHelper*)SENSORPLOT_WIDGET_JAVASCRIPT_RAW;


SensorPlot_WebInterface::SensorPlot_WebInterface() {
}

void SensorPlot_WebInterface::addPlot(String title, String unit, int interval, int good, int bad, int min, int max, int stepsize, int cycle, int cycleStepsize, int *valuesCount, float *values, int *valuesMeasurmentMillis) {
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

void SensorPlot_WebInterface::interfaceConfig(String websiteTitle, String callbackInput, String callbackButton, int unixTime = 0) {
    this->websiteTitle = websiteTitle;
    this->callbackInput = callbackInput;
    this->callbackButton = callbackButton;
    this->unixTime = unixTime;
}

void SensorPlot_WebInterface::serverResponseSetup(ESP8266WebServer *server, int (*callback)(String response)) {
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

    this->server->on("/widget.html", [=]() {
        responseWidgetHTML();
    });
    this->server->on("/widget.js", [=]() {
        responseWidgetJS();
    });

    this->server->on("/config", [=]() {
        responseConfig();
    });

    this->server->on("/callback", [=]() {
        if (this->server->hasArg("input")) {
            int res = (*callback)(this->server->arg("input"));
            this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
            this->server->send(200, "text/plain", String(res));
        } else {
            this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
            this->server->send(200, "text/plain", "0");
        }
    });
    
    this->server->on("/graphData", [=]() {
        responseGraphData();
    });

    for(int i = 0; i < this->plotterCount; i++) {
        this->server->on("/data/" + String(i), [=]() {
            responseGraphSlag(i);
        });
        this->server->on("/csv/" + String(i), [=]() {
            responseCSV(i);
        });
    }
}

void SensorPlot_WebInterface::responseHTML() {
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/html", SENSORPLOT_HTML);
}

void SensorPlot_WebInterface::responseCSS() {
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/css", SENSORPLOT_STYLESHEET);
}

void SensorPlot_WebInterface::responseJS() {
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/javascript", SENSORPLOT_JAVASCRIPT);
}

void SensorPlot_WebInterface::responseConfig() {
    String response;
    response = "";

    response += this->websiteTitle;
    response += ";";
    response += this->callbackInput;
    response += ";";
    response += this->callbackButton;

    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/plain", response);
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

void SensorPlot_WebInterface::responseCSV(int index) {
    String response;
    response = "";

    response += (this->plotter_p[index]->title + " (" + this->plotter_p[index]->unit + ") \r\n");
    response += "Zeit: \tMesswert: \r\n";
    for(int i = 0; i < *(this->plotter_p[index]->valuesCount); i++) {
        response += (String(this->plotter_p[index]->interval * i) + " sec");
        response += "\t";
        response += (String(this->plotter_p[index]->values[i]) + " " + this->plotter_p[index]->unit);
        response += "\r\n";
    }
    
    this->server->sendHeader("Cache-Control", "no-cach, no-store, must-revalidate");
    this->server->sendHeader("Progma", "no-cach");
    this->server->sendHeader("Expires", "0");
    this->server->sendHeader("Content-Disposition", "attachment; filename=\"" + this->plotter_p[index]->title + ".csv\"");
    this->server->sendHeader("Content-Type", "charset=utf-8");
    this->server->sendHeader("Connection", "close");
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/csv;charset=utf-8", response);
}


void SensorPlot_WebInterface::responseWidgetHTML() { 
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/html", SENSORPLOT_WIDGET_HTML);
}
void SensorPlot_WebInterface::responseWidgetJS() {
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/javascript", SENSORPLOT_WIDGET_JAVASCRIPT);
}
