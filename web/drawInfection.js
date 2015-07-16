var graph;
var data;
var xPadding = 65;
var yPadding = 70;
var maxYKeys = 7;
var maxXKeys = 17;

function getMaxY() {
	var max = 0;

	for (var i = 0; i < data.values.length; i++) {
		if (data.values[i].y > max) {
			max = data.values[i].y;
		}
	}

	max += 10 - max % 10;
	return max;
}

function getXPixel(val) {
	return ((graph.width() - xPadding) / data.values.length) * val + (xPadding * 1.5) - 15;
}

function getYPixel(val) {
	return (graph.height() - 20) - ((((graph.height() - 20) - yPadding) / getMaxY()) * val) - yPadding + 19;
}

function getMostCommonDegree() {
	var max = 0;
	var maxIndex = 0;

	for (var i = 0; i < data.values.length; i++) {
		if (data.values[i].y > max) {
			max = data.values[i].y;
			maxIndex = i;
		}
	}

	return data.values[maxIndex].x;
}

$(document).ready(function() {
	graph = $('#graph');
	var c = graph[0].getContext('2d');

    // load data
    $.ajax({
        dataType: "json",
        url: "infData.json",
        async: false,
        success: function(response) {data = response;}
    });

	var oldFill = c.fillStyle;

	c.fillStyle = '#fff';
	c.fillRect(0,0,graph.width(),graph.height());

	c.lineWidth = 2;
	c.strokeStyle = '#333';
	c.textAlign = "center";
	c.fillStyle = oldFill;

	// draw axes
	c.beginPath();
	c.moveTo(xPadding + 8, 20);
	c.lineTo(xPadding + 8, graph.height() - yPadding);
	c.lineTo(graph.width(), graph.height() - yPadding);
	c.stroke();

	c.font = 'bold 14pt sans-serif';

	// draw axis lables
	c.fillText('Timestep', graph.width()/2, graph.height() - 20);

	c.save();
	c.rotate(Math.PI/2);
	c.fillText('# of New Infections', graph.height()/2 - 15, -10);
	c.restore();

	// reset font
	c.font = 'italic 8pt sans-serif';

	// Draw x keys
	if (data.values.length > maxXKeys)
		var step = Math.round(data.values.length / maxXKeys);
	else
		step = 1;

	for (var i = 0; i < Math.min(maxXKeys, data.values.length); i++) {
		if (data.values[i*step] != undefined)
			c.fillText(data.values[i*step].x, getXPixel(i*step), graph.height() - yPadding + 20);
	}	

	// Draw y keys
	step = getMaxY() / maxYKeys;
	for (var i = 0; i < maxYKeys; i++) {
		c.fillText(Math.round(i * step), xPadding - 15, getYPixel(i * step));
	}

	// Draw graph
	c.strokeStyle = '#0a0';
	c.fillStyle = 'rgba(0, 100, 0, 0.32)';
	c.beginPath();
	c.moveTo(getXPixel(0), getYPixel(data.values[0].y));

	for (var i = 1; i < data.values.length; i++) {
		if (i > 1) {
			c.beginPath()
			c.moveTo(getXPixel(i-1), getYPixel(data.values[i-1].y))
		}
		c.lineTo(getXPixel(i), getYPixel(data.values[i].y));
		c.stroke();

		c.beginPath();
		c.moveTo(getXPixel(i-1), getYPixel(0));
		c.lineTo(getXPixel(i-1), getYPixel(data.values[i-1].y));
		c.lineTo(getXPixel(i), getYPixel(data.values[i].y));
		c.lineTo(getXPixel(i), getYPixel(0));
		c.closePath();
		c.fill();

	}
	//c.stroke();

	// Draw points
	c.fillStyle = '#383838';

	step = 1;
	if (data.values.length > 100)
		step = Math.floor(data.values.length / 50);

	for (var i = 0; i < data.values.length; i += step) {
		c.beginPath();
		c.arc(getXPixel(i), getYPixel(data.values[i].y), 4, 0, Math.PI * 2, true);
		c.fill();
	}

	drawSIRGraph();

	// Draw info panel
	infoElem = $('#info');
	infoElem.html('<strong>Graph Name:</strong> ' + data.name
			+ '<br><strong>Node Count:</strong> ' + data.nodeCount.toLocaleString()
			+ '<br><strong>Edge Count:</strong> ' + data.edgeCount.toLocaleString()
			+ '<br><strong>Total Infected:</strong> ' + data.infectionCount.toLocaleString() + "/" + data.nodeCount.toLocaleString() 
			+ ' (' + ((data.infectionCount/data.nodeCount)*100).toFixed(2) + '%)'
			+ '<br><strong>Patient Zero:</strong> ' + data.patientZero.toLocaleString()
			+ '<br><strong>Chance of Infection:</strong> ' + (data.infectionChance * 100) + '%'
			+ '<br><strong>Chance of Contact:</strong> ' + Math.floor((data.contactChance * 100)) + '%'
			+ '<br><strong>Simulation Duration:</strong> ' + data.simulDuration.toLocaleString()
			+ '<br><strong>Rounds Needed:</strong> ' + data.roundsNeeded.toLocaleString()
			+ '<br><strong>Infectious Period:</strong> ' + data.infectionPeriod.toLocaleString()
			+ '<br><strong>K Value:</strong> ' + data.kVal.toLocaleString());

	if (data.endStep != undefined) {
		infoElem.html(infoElem.html() + '<br><strong>All Infected By:</strong>'
			+ ' Round ' + data.endStep)
	}

	heading = $('#heading');
	heading.html(heading.html() + ' <span style="font-size: 22pt">(' 
			+ data.name + ')</span>');
});

function sirGetMaxY() {
	var max = 0;

	for (var i = 0; i < data.numSus.length; i++) {
		if (data.numSus[i].y > max) {
			max = data.numSus[i].y;
		}
	}

	max += 10 - max % 10;
	return max;
}

function sirGetXPixel(val) {
	return ((graph.width() - xPadding) / data.numInf.length) * val + (xPadding * 1.5) - 15;
}

function sirGetYPixel(val) {
	return (graph.height() - 20) - ((((graph.height() - 20) - yPadding) / sirGetMaxY()) * val) - yPadding + 19;
}

function drawSIRGraph() {
	graph = $('#sirGraph');
	var c = graph[0].getContext('2d');

	var oldFill = c.fillStyle;

	c.fillStyle = '#fff';
	c.fillRect(0,0,graph.width(),graph.height());

	c.lineWidth = 2;
	c.strokeStyle = '#333';
	c.textAlign = "center";
	c.fillStyle = oldFill;

	// draw axes
	c.beginPath();
	c.moveTo(xPadding + 8, 20);
	c.lineTo(xPadding + 8, graph.height() - yPadding);
	c.lineTo(graph.width(), graph.height() - yPadding);
	c.stroke();

	c.font = 'bold 14pt sans-serif';

	// draw axis lables
	c.fillText('Timestep', graph.width()/2, graph.height() - 20);

	c.save();
	c.rotate(Math.PI/2);
	c.fillText('# of Infections', graph.height()/2 - 15, -10);
	c.restore();

	// reset font
	c.font = 'italic 8pt sans-serif';

	// Draw x keys
	if (data.numInf.length > maxXKeys)
		var step = Math.round(data.numInf.length / maxXKeys);
	else
		step = 1;

	for (var i = 0; i < Math.min(maxXKeys, data.numInf.length); i++) {
		if (data.numInf[i*step] != undefined)
			c.fillText(data.numInf[i*step].x, sirGetXPixel(i*step), graph.height() - yPadding + 20);
	}	

	// Draw y keys
	step = sirGetMaxY() / maxYKeys;
	for (var i = 0; i < maxYKeys; i++) {
		c.fillText(Math.round(i * step), xPadding - 15, sirGetYPixel(i * step));
	}

	// Draw infections graph
	c.strokeStyle = '#a00';
	c.fillStyle = 'rgba(0, 100, 0, 0.32)';
	c.beginPath();
	c.moveTo(sirGetXPixel(0), sirGetYPixel(data.numInf[0].y));
	for (var i = 1; i < data.numInf.length; i++) {
		c.lineTo(sirGetXPixel(i), sirGetYPixel(data.numInf[i].y));

	}
	c.stroke();

	// Draw points
	c.fillStyle = '#383838';

	step = 1;
	if (data.numInf.length > 100)
		step = Math.floor(data.numInf.length / 50);

	for (var i = 0; i < data.numInf.length; i += step) {
		c.beginPath();
		c.arc(sirGetXPixel(i), sirGetYPixel(data.numInf[i].y), 4, 0, Math.PI * 2, true);
		c.fill();
	}

	// Draw recovered graph
	c.strokeStyle = '#00a';
	c.beginPath();
	c.moveTo(sirGetXPixel(0), sirGetYPixel(data.numRec[0].y));
	for (var i = 1; i < data.numRec.length; i++) {
		c.lineTo(sirGetXPixel(i), sirGetYPixel(data.numRec[i].y));
	}
	c.stroke();

	for (var i = 0; i < data.numRec.length; i+= step) {
		c.beginPath();
		c.arc(sirGetXPixel(i), sirGetYPixel(data.numRec[i].y), 4, 0, Math.PI * 2, true);
		c.fill();
	}

	// Draw susceptible graph
	c.strokeStyle = '#0a0';
	c.beginPath();
	c.moveTo(sirGetXPixel(0), sirGetYPixel(data.numSus[0].y));
	for (var i = 1; i < data.numSus.length; i++) {
		c.lineTo(sirGetXPixel(i), sirGetYPixel(data.numSus[i].y));
	}
	c.stroke();

	for (var i = 0; i < data.numSus.length; i+= step) {
		c.beginPath();
		c.arc(sirGetXPixel(i), sirGetYPixel(data.numSus[i].y), 4, 0, Math.PI * 2, true);
		c.fill();
	}
}
