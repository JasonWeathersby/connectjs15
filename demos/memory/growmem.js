var x = [];
var myTimer;
var runTimer=1;
function createSomeNodes() {
    var div,
        i = 100,
        frag = document.createDocumentFragment();
    for (;i > 0; i--) {
        div = document.createElement("div");
        var dt = new Date().toTimeString();
        var nn = document.createTextNode(i + " - "+ dt);
        div.appendChild(nn);
        frag.appendChild(div);
    }
    document.getElementById("nodes").appendChild(frag);
}
function go() {
	console.time("testgrow");
    x.push(new Array(1000000).join('x'));
    createSomeNodes();
    if( runTimer === 1 ){
    	myTimer = setTimeout(go,1000);
    }
    console.timeEnd("testgrow");
	console.timeStamp("GoEnded");
}
function del() {
	console.time("testdel");
	runTimer = 0;
	x.length = 0;
	var myNode = document.getElementById("nodes");
	while (myNode.firstChild) {
    	myNode.removeChild(myNode.firstChild);
	}	
	clearTimeout(myTimer);
	myTimer = null;
    console.timeEnd("testdel");
	console.timeStamp("DelEnded");
}
