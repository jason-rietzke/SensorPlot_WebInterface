window.onload = createReferences;
window.onpageshow = setup;
window.onresize = ()=>{setMouth();buildFrame();buildGraph();buildLabels();}

const SmileyState = {
	Happy: 'Happy',
	Medium: 'Medium',
	Sad: 'Sad'
};

let state = SmileyState.Happy;
let smiley;
let mouth;

let graphContainer;
let frame;
let graph;

let min;
let max;

let height;
let width;

function createReferences() {
	smiley = document.getElementById('smiley');
	mouth = document.getElementById('mouth');

	graphContainer = document.getElementsByClassName('graphContainer')[0];
	frame = document.getElementsByClassName('frame')[0];
	graph = document.getElementsByClassName('graph')[0];
}

function setup() {
	const c = parseInt(smiley.clientWidth / 2);
	let path = `M${c*0.6},${c*1.3} Q${c},${c*1.3} ${c*1.4},${c*1.3}`;
	mouth.setAttribute('d', path);
	setTimeout(() => {
		setMouth();
	}, 500);

	buildFrame();
	buildGraph();
	buildLabels();
	detailedView();
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


function buildFrame() {
	height = graphContainer.clientHeight;
	width = graphContainer.clientWidth;
	frame.setAttribute('points', `40,0 40,${height} ${width},${height} 40,${height}`);
}

function buildLabels() {
	let labels = document.getElementsByClassName('label');
	for(i=labels.length-1;i>=0;i--) {
		labels[i].remove();
	}
	const stepSize = parseInt(graph.getAttribute('data-stepsize'));
	min = parseInt(min);
	max = parseInt(max);
	for(j=min;j<=max;j+=stepSize) {
		const yPos = height-((j/max)*height);
		const label = document.createElementNS('http://www.w3.org/2000/svg', 'text');
		label.textContent = j;
		graphContainer.appendChild(label);
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

function buildGraph() {
	const values = parseValues();
	min = values[0];
	max = values[0];
	for (i=0; i<values.length;i++) {
		if (values[i] < min) min = values[i];
		if (values[i] > max) max = values[i];
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

	let points = '';
	points += `40,${height} `;
	width = width - 40;
	for(i=0;i<values.length;i++){
		points += `${(width/(values.length-1))*i + 40},${height*(1-((values[i]-min)/max))} `;
	}
	points += `${width + 40},${height} `;
	graph.setAttribute('points', points);
}

function parseValues() {
	const vals = graph.getAttribute('data-values').split(',');
	for(i=0;i<vals.length;i++){
		vals[i] = parseInt(vals[i]);
	}
	return vals;
}

function detailedView() {
	const values = parseValues();
	document.addEventListener('mousemove', function(e){
		const top = graph.getBoundingClientRect().top + document.body.scrollTop;
		const bottom = graph.getBoundingClientRect().bottom + document.body.scrollTop;
		const left = graph.getBoundingClientRect().left + document.body.scrollLeft;
		const right = graph.getBoundingClientRect().right + document.body.scrollLeft;
		if(e.pageX>left && e.pageX<right && e.pageY>top && e.pageY<bottom){
			const perc = (e.offsetX-40)/graph.getBBox().width;
			const i = parseInt(values.length*perc);
			createDetailedPointer(parseInt(graph.getBBox().width*perc)+40);
			createDetailedLabel(parseInt(graph.getBBox().width*perc)+40, values, i);
		}else{
			if(document.getElementById('detailPointer')){document.getElementById('detailPointer').remove();}
			if(document.getElementById('detailLabelRect')){document.getElementById('detailLabelRect').remove();}
			if(document.getElementById('detailLabelText')){document.getElementById('detailLabelText').remove();}
		}
	}, false);
}
function createDetailedPointer(pos) {
	if(document.getElementById('detailPointer')){document.getElementById('detailPointer').remove();}
	const poly = document.createElementNS("http://www.w3.org/2000/svg", "polygon");
	poly.classList.add('detailPointer');
	poly.setAttribute('id', 'detailPointer');
	graphContainer.appendChild(poly);
	const p1 = graphContainer.createSVGPoint();
	p1.x = pos;
	p1.y = height;
	const p2 = graphContainer.createSVGPoint();
	p2.x = pos;
	p2.y = 0;
	poly.points.appendItem(p1);
	poly.points.appendItem(p2);
}
function createDetailedLabel(pos, values, index) {
	if(document.getElementById('detailLabelRect')){document.getElementById('detailLabelRect').remove();}
	const rect = document.createElementNS("http://www.w3.org/2000/svg", "rect");
	rect.classList.add('detailLabel');
	rect.setAttribute('id', 'detailLabelRect');
	graphContainer.appendChild(rect);
	rect.setAttribute('width', 120);
	rect.setAttribute('height', 40);
	rect.setAttribute('rx', 10);
	rect.setAttribute('y', 0);
	rect.setAttribute('x', pos-(rect.getBBox().width/2));

	// offset correction on boundries
	if ((rect.getBBox().x+rect.getBBox().width) > (graph.getBBox().x+graph.getBBox().width)) { rect.setAttribute('x', ((graph.getBBox().x+graph.getBBox().width)-rect.getBBox().width))}
	if (rect.getBBox().x < 40) { rect.setAttribute('x', 40)}

	if(document.getElementById('detailLabelText')){document.getElementById('detailLabelText').remove();}
	const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
	text.classList.add('label');
	text.setAttribute('id', 'detailLabelText');
	graphContainer.appendChild(text);

	const ppt = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');
	ppt.textContent = 'ppt: '+values[index];
	ppt.setAttribute('x', rect.getBBox().x + 5);
	ppt.setAttribute('y', rect.getBBox().y + 15);
	const time = document.createElementNS('http://www.w3.org/2000/svg', 'tspan');
	time.textContent = 'at '+'14:45';
	time.setAttribute('x', rect.getBBox().x + 5);
	time.setAttribute('y', rect.getBBox().y + 32);
	text.appendChild(ppt)
	text.appendChild(time);
}
