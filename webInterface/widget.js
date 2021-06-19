window.onload = createReferences;
window.onresize = ()=>{
	setMouth();
}
let Graphs = [];
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
	setTimeout(() => {
		setMouth();
	}, 500);
	loadConfig();
	loadGraphs();
}
// loading configdata from server
function loadConfig() {
	let webClient = new XMLHttpRequest();
	webClient.open('GET', '/config');
	webClient.addEventListener('load', function(event) {
		const config = webClient.responseText.split(';');
		document.title = config[0] + ' | Widget';
		document.getElementById('widgetTitle').textContent = config[0];
	});
	webClient.send();
}
// loading graphdata from server
function loadGraphs() {
	let webClient = new XMLHttpRequest();
	webClient.open('GET', '/graphData');
	webClient.addEventListener('load', function(event) {
		const graphs = webClient.responseText.split(';');
		Graphs = [];
		for(i=0;i<graphs.length;i++){
			const graph = graphs[i];
			const data = graph.split(',');
			if(!(data.length < 12)){
				Graphs.push({title: data[0], unit: data[1], slag: data[2], interval: data[3], good: parseInt(data[4]), bad: parseInt(data[5]), min: parseInt(data[6]), max: parseInt(data[7]), clipping: data[8], stepsize: parseInt(data[9]), cycle: parseInt(data[10]), cycleStepsize: parseInt(data[11]), value: 0});
				loadData(data[3], data[2], 1);
			}
		}
		createMeasurements();
		validateSmileyState();
	});
	webClient.send();
}
// loading data from server
function loadData(interval, slag, immediate = 0) {
	setTimeout(() => {
		let webClient = new XMLHttpRequest();
		webClient.open('GET', `/data/${slag}`);
		webClient.addEventListener('load', function(event) {
			const values = webClient.responseText.split(';')[1].split(',');
			Graphs[slag].value = parseInt(values[values.length - 1]);
			createMeasurements();
			validateSmileyState();
		});
		webClient.send();
		loadData(interval, slag)
	}, immediate ? 0 : (interval * 1000));
}
function validateSmileyState() {
	let state = SmileyState.Happy;
	let smileyColor = '#33de18';
	for(var i = 0; i < Graphs.length; i++) {
		const Graph = Graphs[i];
		let currentValue = Graph.value;
		let goodThreshold = Graph.good;
		let badThreshold = Graph.bad;
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
	for(var i = 0; i < Graphs.length; i++) {
		let Graph = Graphs[i];
		let p = document.createElement('p');
		let label = document.createElement('span');
		label.classList.add('valueLabel');
		label.textContent = Graph.title + ': ';
		p.appendChild(label);
		let value = document.createElement('span');
		value.textContent = Graph.value + ' ' + Graph.unit;
		p.appendChild(value);
		measurements.appendChild(p);
	}
}