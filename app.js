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

let state = SmileyState.Happy;
let smiley;
let mouth;


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

	createGraphs();

	// initialize loading data from server
	const graphContainers = document.getElementsByClassName('graphContainer');
	for(var i = 0; i < graphContainers.length; i++) {
		let container = graphContainers[i];
		let graph = container.getElementsByClassName('graph')[0];
		loadData(graph, container.getAttribute('data-interval'), container.getAttribute('data-slag'), 1);
	}
}

// loading data from server
function loadData(graph, interval, slag, immediate = 0) {
	setTimeout(() => {
		var client = new XMLHttpRequest();
		client.open('GET', '/'+slag);
		client.onreadystatechange = function() {
			graph.setAttribute('data-values', client.responseText);
			createGraphs();
		}
		client.send();
		loadData(graph, interval, slag)
	}, immediate ? 0 : (interval * 1000));
}


function setMouth() {
	const c = parseInt(smiley.clientWidth / 2);
	let path = `M${c*0.6},${c*1.3} Q${c},${c*1.3} ${c*1.4},${c*1.3}`;
	// M${c*0.6},${c*1.3} Q${c},${c*1.8} ${c*1.4},${c*1.3}	| Happy
	// M${c*0.6},${c*1.4} Q${c},${c*1.3} ${c*1.4},${c*1.2}	| Medium
	// M${c*0.6},${c*1.4} Q${c},${c} ${c*1.4},${c*1.4}		| Sad
	switch (state) {
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
function setMouthState() {
	mouth.style.setProperty('--animationTime', '0.5s');
	setMouth();
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

		buildFrame(container, frame, height, width);
		buildGraph(graph, values, min, max, height, width);
		buildLabels(container, graph, min, max, height, width);
		detailedView(i, container, graph, frame, values);
	}
}


function buildFrame(container, frame, height, width) {
	frame.setAttribute('points', `40,0 40,${height} ${width},${height} 40,${height}`);
}
function buildLabels(container, graph, min, max, height, width) {
	let labels = container.getElementsByClassName('label');
	for(i=labels.length-1;i>=0;i--) {
		labels[i].remove();
	}
	const stepSize = parseInt(graph.getAttribute('data-stepsize'));
	min = parseFloat(min);
	max = parseFloat(max);
	for(j=min;j<=max;j+=stepSize) {
		const yPos = height-((j/max)*height);
		const label = document.createElementNS('http://www.w3.org/2000/svg', 'text');
		label.textContent = j;
		container.appendChild(label);
		label.classList.add('label');
		label.setAttribute('x',35-label.getBBox().width);
		label.setAttribute('y',yPos);
	}
	labels = document.getElementsByClassName('label')
	const labelTop = labels[labels.length - 1];
	if (labelTop.getBBox().y < labelTop.getBBox().height) {
		labelTop.setAttribute('y',labelTop.getBBox().height);
	}
	const labelBottom = labels[0];
	labelBottom.setAttribute('y',height-(labelTop.getBBox().height/2));
}
function buildGraph(graph, values, min, max, height, width) {
	let points = '';
	points += `40,${height} `;
	width = width - 40;
	for(i=0;i<values.length;i++){
		points += `${(width/(values.length-1))*i + 40},${height*(1-((values[i]-min)/max))} `;
	}
	points += `${width + 40},${height} `;
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
	p1.y = height;
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
	rect.setAttribute('y', 0);
	rect.setAttribute('x', pos-(rect.getBBox().width/2));

	// offset correction on boundries
	if ((rect.getBBox().x+rect.getBBox().width) > (graph.getBBox().x+graph.getBBox().width)) { rect.setAttribute('x', ((graph.getBBox().x+graph.getBBox().width)-rect.getBBox().width))}
	if (rect.getBBox().x < 40) { rect.setAttribute('x', 40)}

	if(document.getElementById('detailLabelText'+i)){document.getElementById('detailLabelText'+i).remove();}
	const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
	text.classList.add('label');
	text.setAttribute('id', 'detailLabelText'+i);
	container.appendChild(text);

	const ppt = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');
	ppt.textContent = values[valueIndex];
	ppt.setAttribute('x', rect.getBBox().x + 5);
	ppt.setAttribute('y', rect.getBBox().y + 15);
	const time = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');
	time.textContent = 'at '+'14:45';
	time.setAttribute('x', rect.getBBox().x + 5);
	time.setAttribute('y', rect.getBBox().y + 32);
	text.appendChild(ppt)
	text.appendChild(time);
}
