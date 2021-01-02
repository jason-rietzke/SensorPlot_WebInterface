window.onload = createReferences;
window.onpageshow = setup;
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
}

function setup() {
	const c = parseInt(smiley.clientWidth / 2);
	let path = `M${c*0.6},${c*1.3} Q${c},${c*1.3} ${c*1.4},${c*1.3}`;
	mouth.setAttribute('d', path);
	setTimeout(() => {
		setMouth();
	}, 500);

	loadGraphs();
}

// loading graphdata from server
function loadGraphs() {
	var client = new XMLHttpRequest();
	client.open('GET', '/graphData');
	client.onreadystatechange = function() {
		const graphs = client.responseText.split(';');
		for(i=0;i<graphs.length;i++){
			const graph = graphs[i];
			const data = graph.splot(',');
			createGraphModule(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11])
		}
		// initialize loading data from server
		const graphContainers = document.getElementsByClassName('graphContainer');
		for(var i = 0; i < graphContainers.length; i++) {
			let container = graphContainers[i];
			let graph = container.getElementsByClassName('graph')[0];
			loadData(graph, container.getAttribute('data-interval'), container.getAttribute('data-slag'), 1);
		}
		createGraphs();
		createMeasurements();
		validateMouthState()
	}
	client.send();
}

// loading data from server
function loadData(graph, interval, slag, immediate = 0) {
	setTimeout(() => {
		var client = new XMLHttpRequest();
		client.open('GET', '/'+slag);
		client.onreadystatechange = function() {
			const offset = client.responseText.split(';')[0];
			graph.setAttribute('data-offset', offset);
			const values = client.responseText.split(';')[1];
			graph.setAttribute('data-values', values);
			const date = new Date();
			graph.setAttribute('data-reloaded', date.getTime());
			createMeasurements();
			createGraphs();
			validateMouthState();
		}
		client.send();
		loadData(graph, interval, slag)
	}, immediate ? 0 : (interval * 1000));
}


function validateMouthState() {
	const graphContainers = document.getElementsByClassName('graphContainer');
	let state = SmileyState.Happy;
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
				} else if (currentValue > goodThreshold && goodThreshold != '') {
					console.log(currentValue);
					state = SmileyState.Medium;
				}
				break;
			case SmileyState.Medium:
				if (currentValue > badThreshold && badThreshold != '') {
					console.log(currentValue);
					state = SmileyState.Sad;
				}
				break;
			default:
				break;
		}
	}
	smileyState = state;
	animateMouth();
}
function setMouth() {
	const c = parseInt(smiley.clientWidth / 2);
	let path = `M${c*0.6},${c*1.3} Q${c},${c*1.3} ${c*1.4},${c*1.3}`;
	// M${c*0.6},${c*1.3} Q${c},${c*1.8} ${c*1.4},${c*1.3}	| Happy
	// M${c*0.6},${c*1.4} Q${c},${c*1.3} ${c*1.4},${c*1.2}	| Medium
	// M${c*0.6},${c*1.4} Q${c},${c} ${c*1.4},${c*1.4}		| Sad
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
		value.textContent = values[values.length-1] + container.getAttribute('data-unit');
		p.appendChild(value);

		measurements.appendChild(p);
	}
}

function createGraphModule(title, unit, slag, interval, good, bad, min, max, clipping, stepsize, cycle, cycleStepsize) {
	const graphModule = document.createElement('div');
	graphModule.classList.add('container', 'graphmodule');

	const headline = document.createElement('h1');
	headline.textContent = title;
	graphModule.appendChild(headline);
	
	const graphContainer = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
	graphContainer.classList.add('graphContainer');
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
	graphPolygon.setAttribute('data-values', '');
	graphContainer.appendChild(graphPolygon);

	const framePolygon = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');
	framePolygon.classList.add('frame');
	graphContainer.appendChild(framePolygon);

	document.getElementsByClassName('graphsContainer').appendChild(graphModule);
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
			if (min>graph.getAttribute('data-min')) {
				if (graph.getAttribute('data-min')) min = graph.getAttribute('data-min');
			}
			if (max<graph.getAttribute('data-max')) {
				if (graph.getAttribute('data-max')) max = graph.getAttribute('data-max');
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
	for(j=min;j<=max;j+=stepSize) {
		const yPos = height-((j/max)*height);
		const label = document.createElementNS('http://www.w3.org/2000/svg', 'text');
		label.textContent = j;
		container.appendChild(label);
		label.classList.add('labelY');
		label.setAttribute('x',35-label.getBBox().width);
		label.setAttribute('y',yPos);
	}
	labels = container.getElementsByClassName('labelY');
	const labelTop = labels[labels.length - 1];
	if (labelTop.getBBox().y < labelTop.getBBox().height) {
		labelTop.setAttribute('y',labelTop.getBBox().height);
	}
	const labelBottom = labels[0];
	labelBottom.setAttribute('y',height-(labelTop.getBBox().height/2));
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
		const label = document.createElementNS('http://www.w3.org/2000/svg', 'text');
		label.textContent = timeText;
		container.appendChild(label);
		label.classList.add('labelX');
		label.setAttribute('x',xPos);
		label.setAttribute('y',height+20);
	}
	labels = container.getElementsByClassName('labelX');
	const labelRight = labels[0];
	if ((labelRight.getBBox().x+labelRight.getBBox().width) > container.getBBox().width) {
		labelRight.setAttribute('x',container.getBBox().width-(labelRight.getBBox().width*2));
	}
	//const labelBottom = labels[0];
	//labelBottom.setAttribute('y',height-(labelTop.getBBox().height/2));
}
function buildGraph(graph, values, min, max, height, width) {
	let points = '';
	width = width - 40;
	height = height - 20;
	points += `40,${height} `;
	for(i=0;i<values.length;i++){
		if (i == values.length-1) {
			points += `${(width/(values.length-1))*i + 43},${height*(1-((values[i]-min)/max))} `;
		} else {
			points += `${(width/(values.length-1))*i + 40},${height*(1-((values[i]-min)/max))} `;
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
			if(document.getElementById('detailPointer'+i)){document.getElementById('detailPointer'+i).remove();}
			if(document.getElementById('detailLabelRect'+i)){document.getElementById('detailLabelRect'+i).remove();}
			if(document.getElementById('detailLabelText'+i)){document.getElementById('detailLabelText'+i).remove();}
		}
	}, false);
}
function createDetailedPointer(i, container, pos) {
	var height = container.clientHeight;
	var width = container.clientWidth;
	if(document.getElementById('detailPointer'+i)){document.getElementById('detailPointer'+i).remove();}
	const poly = document.createElementNS("http://www.w3.org/2000/svg", "polygon");
	poly.classList.add('detailPointer');
	poly.setAttribute('id', 'detailPointer'+i);
	container.appendChild(poly);
	const p1 = container.createSVGPoint();
	p1.x = pos;
	p1.y = height - 20;
	const p2 = container.createSVGPoint();
	p2.x = pos;
	p2.y = 0;
	poly.points.appendItem(p1);
	poly.points.appendItem(p2);
}
function createDetailedLabel(i, container, graph, pos, values, valueIndex) {
	if(document.getElementById('detailLabelRect'+i)){document.getElementById('detailLabelRect'+i).remove();}
	const rect = document.createElementNS("http://www.w3.org/2000/svg", "rect");
	rect.classList.add('detailLabel');
	rect.setAttribute('id', 'detailLabelRect'+i);
	container.appendChild(rect);
	rect.setAttribute('width', 120);
	rect.setAttribute('height', 40);
	rect.setAttribute('rx', 10);
	rect.setAttribute('y', 1);
	rect.setAttribute('x', pos-(rect.getBBox().width/2));

	// offset correction on boundries
	if ((rect.getBBox().x+rect.getBBox().width)+3 > (graph.getBBox().x+graph.getBBox().width)) { rect.setAttribute('x', ((graph.getBBox().x+graph.getBBox().width)-rect.getBBox().width-3))}
	if (rect.getBBox().x < 40) { rect.setAttribute('x', 40)}

	if(document.getElementById('detailLabelText'+i)){document.getElementById('detailLabelText'+i).remove();}
	const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
	text.classList.add('label');
	text.setAttribute('id', 'detailLabelText'+i);
	container.appendChild(text);

	const valueView = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');
	valueView.textContent = values[valueIndex] + container.getAttribute('data-unit');
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

	text.appendChild(valueView)
	text.appendChild(timeView);
}
