window.onload = createReferences;
window.onresize = ()=>{
	setMouth();
	createGraphs();
}
const SmileyState = {
	Happy: 'Happy',
	Medium: 'Medium',
	Sad: 'Sad'
};
let smiley;
let mouth;
let smileyState = SmileyState.Happy;
function createReferences() {
	smiley = document.getElementById('smiley');
	mouth = document.getElementById('mouth');
	setup();
}
function setup() {
	const c = parseInt(smiley.clientWidth / 2);
	let path = `M${c*0.6},${c*1.3} Q${c},${c*1.3} ${c*1.4},${c*1.3}`;
	mouth.setAttribute('d', path);
	document.getElementById('callbackButton').addEventListener('click', performCallback);
	setTimeout(() => {
		setMouth();
	}, 500);
	loadGraphs();
}
// loading callback result from server
function performCallback() {
	let webClient = new XMLHttpRequest();
	webClient.open('POST', '/callback?password=' + document.getElementById('callbackPassword').value);
	webClient.addEventListener('load', function(event) {
		let color = 'var(--foregroundSecondaryColor)';
		if (webClient.responseText == '0') {
			color = 'red';
		} else if (webClient.responseText == '1') {
			color = 'green';
		}
		document.getElementById('callbackPassword').style.borderColor = color;
		document.getElementById('callbackButton').style.borderColor = color;
	});
	webClient.send();
}
// loading graphdata from server
function loadGraphs() {
	let webClient = new XMLHttpRequest();
	webClient.open('GET', '/graphData');
	webClient.addEventListener('load', function(event) {
		const resp = webClient.responseText.split('/');
		document.title = resp[0];
		const graphs = resp[1].split(';');
		for(i=0;i<graphs.length;i++){
			const graph = graphs[i];
			const data = graph.split(',');
			if(!(data.length < 12)){
				createGraphModule(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11])
			}
		}
		createGraphs();
		createMeasurements();
		validateSmileyState();
	});
	webClient.send();
}
// loading data from server
function loadData(graph, interval, slag, immediate = 0) {
	setTimeout(() => {
		let webClient = new XMLHttpRequest();
		webClient.open('GET', `/data/${slag}`);
		webClient.addEventListener('load', function(event) {
			const offset = webClient.responseText.split(';')[0];
			graph.setAttribute('data-offset', offset);
			const values = webClient.responseText.split(';')[1];
			graph.setAttribute('data-values', values);
			const date = new Date();
			graph.setAttribute('data-reloaded', date.getTime());
			createMeasurements();
			createGraphs();
			validateSmileyState();
		});
		webClient.send();
		loadData(graph, interval, slag)
	}, immediate ? 0 : (interval * 1000));
}
function validateSmileyState() {
	const graphContainers = document.getElementsByClassName('graphContainer');
	let state = SmileyState.Happy;
	let smileyColor = '#33de18';
	for(var i = 0; i < graphContainers.length; i++) {
		const container = graphContainers[i];
		const graph = container.getElementsByClassName('graph')[0];
		let values = parseValues(graph);
		let currentValue = values[values.length-1];
		let goodThreshold = container.getAttribute('data-good-threshold');
		let badThreshold = container.getAttribute('data-bad-threshold');
		switch (state) {
			case SmileyState.Happy:
				if (currentValue > badThreshold && badThreshold != '') {
					console.log(currentValue);
					state = SmileyState.Sad;
					smileyColor = '#f03030';
				} else if (currentValue > goodThreshold && goodThreshold != '') {
					console.log(currentValue);
					state = SmileyState.Medium;
					smileyColor = '#ffe600';
				}
				break;
			case SmileyState.Medium:
				if (currentValue > badThreshold && badThreshold != '') {
					console.log(currentValue);
					state = SmileyState.Sad;
					smileyColor = '#f03030';
				}
				break;
			default:
				break;
		}
	}
	smileyState = state;
	document.getElementsByClassName('face')[0].style.setProperty('--smileyColor', smileyColor);
	animateMouth();
}
function setMouth() {
	const c = parseInt(smiley.clientWidth / 2);
	let path = `M${c*0.6},${c*1.3} Q${c},${c*1.3} ${c*1.4},${c*1.3}`;
	switch (smileyState) {
		case SmileyState.Happy:
			path = `M${c*0.6},${c*1.3} Q${c},${c*1.8} ${c*1.4},${c*1.3}`; 
			break;
		case SmileyState.Medium:
			path = `M${c*0.6},${c*1.4} Q${c},${c*1.3} ${c*1.4},${c*1.2}`; 
			break;
		case SmileyState.Sad:
			path = `M${c*0.6},${c*1.4} Q${c},${c} ${c*1.4},${c*1.4}`; 
			break;
	}
	mouth.setAttribute('d', path);
	setTimeout(() => {
		mouth.style.setProperty('--animationTime', '0s');
	}, 500);
}
function animateMouth() {
	mouth.style.setProperty('--animationTime', '0.5s');
	setMouth();
}
function createMeasurements() {
	const measurements = document.getElementsByClassName('measurements')[0];
	measurements.innerHTML = '';
	const graphModules = document.getElementsByClassName('graphmodule');
	for(var i = 0; i < graphModules.length; i++) {
		let module = graphModules[i];
		let container = module.getElementsByClassName('graphContainer')[0];
		let graph = container.getElementsByClassName('graph')[0];
		let values = parseValues(graph);
		let p = document.createElement('p');
		let label = document.createElement('span');
		label.classList.add('valueLabel');
		label.textContent = container.getAttribute('data-title') + ': ';
		p.appendChild(label);
		let value = document.createElement('span');
		value.textContent = values[values.length-1] + ' ' + container.getAttribute('data-unit');
		p.appendChild(value);
		measurements.appendChild(p);
	}
}
function createGraphModule(title, unit, slag, interval, good, bad, min, max, clipping, stepsize, cycle, cycleStepsize) {
	const graphModule = document.createElement('div');
	graphModule.classList.add('container', 'graphmodule');
	const headline = document.createElement('h1');
	headline.textContent = title + ' (' + unit + ')';
	graphModule.appendChild(headline);

	const csvLink = document.createElement('a');
	csvLink.setAttribute('href', '/csv/' + slag);
	csvLink.setAttribute('target', '_blank');
	csvLink.textContent = 'download csv';
	graphModule.appendChild(csvLink);
	
	const graphContainer = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
	graphContainer.classList.add('graphContainer');
	graphContainer.setAttribute('data-title', title);
	graphContainer.setAttribute('data-unit', unit);
	graphContainer.setAttribute('data-slag', slag);
	graphContainer.setAttribute('data-interval', interval);
	graphContainer.setAttribute('data-good-threshold', good);
	graphContainer.setAttribute('data-bad-threshold', bad);
	graphModule.appendChild(graphContainer);
	const graphPolygon = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');
	graphPolygon.classList.add('graph');
	graphPolygon.setAttribute('data-min', min);
	graphPolygon.setAttribute('data-max', max);
	graphPolygon.setAttribute('data-clipping', clipping);
	graphPolygon.setAttribute('data-stepsize', stepsize);
	graphPolygon.setAttribute('data-cycle', cycle);
	graphPolygon.setAttribute('data-cycle-stepsize', cycleStepsize);
	graphPolygon.setAttribute('data-values', '0');
	graphContainer.appendChild(graphPolygon);
	const framePolygon = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');
	framePolygon.classList.add('frame');
	graphContainer.appendChild(framePolygon);
	document.getElementById('graphsContainer').appendChild(graphModule);
	loadData(graphPolygon, interval, slag, 1);
}
function createGraphs() {
	const graphContainers = document.getElementsByClassName('graphContainer');
	for(var i = 0; i < graphContainers.length; i++) {
		let container = graphContainers[i];
		let graph = container.getElementsByClassName('graph')[0];
		let frame = container.getElementsByClassName('frame')[0];
		let height = container.clientHeight;
		let width = container.clientWidth;
		// Min, Max Values
		let values = parseValues(graph);
		let min = values[0];
		let max = values[0];
		for (var j=0; j<values.length; j++) {
			if (values[j] < min) min = values[j];
			if (values[j] > max) max = values[j];
		}
		if (graph.getAttribute('data-clipping')==='false') {
			if (graph.getAttribute('data-min') && min>graph.getAttribute('data-min')) {
				min = graph.getAttribute('data-min');
			}
			if (graph.getAttribute('data-max') && max<graph.getAttribute('data-max')) {
				max = graph.getAttribute('data-max');
			}
		} else {
			if (graph.getAttribute('data-min')) min = graph.getAttribute('data-min');
			if (graph.getAttribute('data-max')) max = graph.getAttribute('data-max');
		}
		buildFrame(frame, height, width);
		buildGraph(graph, values, min, max, height, width);
		buildYLabels(container, graph, min, max, height, width);
		buildXLabels(container, graph, values, height, width);
		detailedView(i, container, graph, frame, values);
	}
}
function buildFrame(frame, height, width) {
	frame.setAttribute('points', `40,0 40,${height-20} ${width},${height-20} 40,${height-20}`);
}
function buildYLabels(container, graph, min, max, height, width) {
	height = height - 20;
	const stepSize = parseInt(graph.getAttribute('data-stepsize'));
	min = parseFloat(min);
	max = parseFloat(max);
	let labels = container.getElementsByClassName('labelY');
	for(i=labels.length-1;i>=0;i--) {
		labels[i].remove();
	}
	for(j=0;j<=parseInt((max-min)/stepSize);j++) {
		const yPos = height-((j/parseInt((max-min)/stepSize))*height);
		if (yPos != NaN) {
			const label = document.createElementNS('http://www.w3.org/2000/svg', 'text');
			label.textContent = (min + (j*stepSize));
			container.appendChild(label);
			label.classList.add('labelY');
			label.setAttribute('x',35-label.getBBox().width);
			label.setAttribute('y',yPos);
		}
	}
	labels = container.getElementsByClassName('labelY');
	const labelTop = labels[labels.length - 1];
	if (labelTop) {
		if (labelTop.getBBox().y < labelTop.getBBox().height) {
			labelTop.setAttribute('y',labelTop.getBBox().height);
		}
	}
	const labelBottom = labels[0];
	if (labelBottom) {
		labelBottom.setAttribute('y',height-(labelTop.getBBox().height/2));
	}
}
function buildXLabels(container, graph, values, height, width) {
	height = height - 20;
	width = width - 40;
	let stepSize = parseInt(graph.getAttribute('data-cycle-stepsize'));
	if (window.innerWidth < 750) {stepSize = stepSize*2}
	const cycle = parseInt(graph.getAttribute('data-cycle'));
	let labels = container.getElementsByClassName('labelX');
	for(i=labels.length-1;i>=0;i--) {
		labels[i].remove();
	}
	for(j=0;j<=values.length;j+=(stepSize/cycle)) {
		const valueoffset = (values.length-(values.length-j))*1000*cycle;
		const offset = graph.getAttribute('data-reloaded') - graph.getAttribute('data-offset') - valueoffset;
		const date = new Date();
		const timestamp = date.getTime();
		const time = new Date(timestamp - (timestamp - offset));
		const timeText = (time.getHours()<10?'0':'')+time.getHours()+':'+(time.getMinutes()<10?'0':'')+time.getMinutes();

		const xPos = (width-((j/values.length)*width)+35);
		if (xPos != NaN) {
			const label = document.createElementNS('http://www.w3.org/2000/svg', 'text');
			label.textContent = timeText;
			container.appendChild(label);
			label.classList.add('labelX');
			label.setAttribute('x',xPos);
			label.setAttribute('y',height+20);
		}
	}
	labels = container.getElementsByClassName('labelX');
	const labelRight = labels[0];
	if (labelRight) {
		if ((labelRight.getBBox().x+labelRight.getBBox().width) > container.getBBox().width) {
			labelRight.setAttribute('x',container.getBBox().width-(labelRight.getBBox().width*2));
			if (labels[1]) { labels[1].remove(); }
		}
	}
}
function buildGraph(graph, values, min, max, height, width) {
	let dots = graph.parentNode.getElementsByTagName('circle');
	while (dots[0]) {
		dots[0].parentNode.removeChild(dots[0]);
	}
	let points = '';
	width = width - 40;
	height = height - 20;
	points += `40,${height} `;
	for(i=0;i<values.length;i++){
		let offset = 40
		if (i == values.length-1) { offset = 43; }
		points += `${(width/(values.length-1))*i + offset},${height*(1-((values[i]-min)/(max-min)))} `;
		const dot = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
		const dotX = (width/(values.length-1))*i + offset;
		if (dotX != NaN) {
			dot.setAttribute('cx', dotX);
			dot.setAttribute('cy', height*(1-((values[i]-min)/(max-min))));
			graph.parentNode.appendChild(dot);
		}
	}
	points += `${width + 43},${height} `;
	graph.setAttribute('points', points);
}
function parseValues(graph) {
	const vals = graph.getAttribute('data-values').split(',');
	for(i=0;i<vals.length;i++){
		vals[i] = parseFloat(vals[i]);
	}
	return vals;
}
function detailedView(i, container, graph, frame, values) {
	document.removeEventListener('mousemove', function(e){});
	document.addEventListener('mousemove', function(e){
		const top = frame.getBoundingClientRect().top + document.body.scrollTop;
		const bottom = frame.getBoundingClientRect().bottom + document.body.scrollTop;
		const left = frame.getBoundingClientRect().left + document.body.scrollLeft;
		const right = frame.getBoundingClientRect().right + document.body.scrollLeft;
		if(e.pageX>left && e.pageX<right && e.pageY>top && e.pageY<bottom){
			const perc = (e.offsetX-40)/frame.getBBox().width;
			const valueIndex = parseInt(values.length*perc);
			createDetailedPointer(i, container, parseInt(frame.getBBox().width*perc)+40);
			createDetailedLabel(i, container, graph, parseInt(frame.getBBox().width*perc)+40, values, valueIndex);
		}else{
			const dPointer = container.getElementsByClassName('detailPointer');
			const dRect = container.getElementsByClassName('detailLabelRect');
			const dLabels = container.getElementsByClassName('detailLabelText');
			while (dPointer[0]) {
				dPointer[0].parentNode.removeChild(dPointer[0]);
			}
			while (dRect[0]) {
				dRect[0].parentNode.removeChild(dRect[0]);
			}
			while (dLabels[0]) {
				dLabels[0].parentNode.removeChild(dLabels[0]);
			}
		}
	}, false);
}
function createDetailedPointer(i, container, pos) {
	var height = container.clientHeight;
	var width = container.clientWidth;
	const dPointer = container.getElementsByClassName('detailPointer');
	while (dPointer[0]) {
		dPointer[0].parentNode.removeChild(dPointer[0]);
	}
	const poly = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');
	poly.classList.add('detailPointer');
	container.appendChild(poly);
	const dots = container.getElementsByTagName('circle');
	let nearDotPos = (dots[0].getBBox().x + (dots[0].getBBox().width/2));
	let dist = Math.abs(pos - dots[0].getBBox().x);
	for(i=0;i<dots.length;i++){
		const tempDist = Math.abs(pos - dots[i].getBBox().x);
		if(tempDist<dist) { 
			dist = tempDist;
			nearDotPos = (dots[i].getBBox().x + (dots[i].getBBox().width/2));
		}
	}
	const p1 = container.createSVGPoint();
	p1.x = nearDotPos
	p1.y = height - 20;
	const p2 = container.createSVGPoint();
	p2.x = nearDotPos;
	p2.y = 0;
	if (nearDotPos != NaN) {
		poly.points.appendItem(p1);
		poly.points.appendItem(p2);
	}
}
function createDetailedLabel(i, container, graph, pos, values, valueIndex) {
	const dRect = container.getElementsByClassName('detailLabelRect');
	while (dRect[0]) {
		dRect[0].parentNode.removeChild(dRect[0]);
	}
	const rect = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
	rect.classList.add('detailLabel', 'detailLabelRect');
	container.appendChild(rect);
	rect.setAttribute('width', 120);
	rect.setAttribute('height', 40);
	rect.setAttribute('rx', 10);
	rect.setAttribute('y', 1);
	const dots = container.getElementsByTagName('circle');
	let nearDotPos = (dots[0].getBBox().x + (dots[0].getBBox().width/2));
	let dist = Math.abs(pos - dots[0].getBBox().x);
	for(i=0;i<dots.length;i++){
		const tempDist = Math.abs(pos - dots[i].getBBox().x);
		if(tempDist<dist) { 
			dist = tempDist;
			nearDotPos = (dots[i].getBBox().x + (dots[i].getBBox().width/2));
		}
	}
	rect.setAttribute('x', nearDotPos-(rect.getBBox().width/2));
	// offset correction on boundries
	if ((rect.getBBox().x+rect.getBBox().width)+3 > (graph.getBBox().x+graph.getBBox().width)) { rect.setAttribute('x', ((graph.getBBox().x+graph.getBBox().width)-rect.getBBox().width-3))}
	if (rect.getBBox().x < 40) { rect.setAttribute('x', 40)}
	const dLabels = container.getElementsByClassName('detailLabelText');
	while (dLabels[0]) {
		dLabels[0].parentNode.removeChild(dLabels[0]);
	}
	const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
	text.classList.add('label', 'detailLabelText');
	container.appendChild(text);
	const valueView = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');
	valueView.textContent = values[valueIndex] + ' ' + container.getAttribute('data-unit');
	valueView.setAttribute('x', rect.getBBox().x + 5);
	valueView.setAttribute('y', rect.getBBox().y + 15);
	const valueoffset = (values.length-valueIndex)*1000*graph.getAttribute('data-cycle');
	const offset = graph.getAttribute('data-reloaded') - graph.getAttribute('data-offset') - valueoffset;
	const date = new Date();
	const timestamp = date.getTime();
	const time = new Date(timestamp - (timestamp - offset));
	const timeText = (time.getHours()<10?'0':'')+time.getHours()+':'+(time.getMinutes()<10?'0':'')+time.getMinutes();
	const timeView = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');
	timeView.textContent = '🕔 '+timeText;
	timeView.setAttribute('x', rect.getBBox().x + 5);
	timeView.setAttribute('y', rect.getBBox().y + 32);
	text.appendChild(valueView);
	text.appendChild(timeView);
}