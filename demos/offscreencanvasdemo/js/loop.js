//Main file for game logic
window.onload = init;
window.onbeforeunload = test;
//Setup function to reset start location

currX= null;
currY = null;
canvas = null;
context = null;
xv = 0;
xy = 0;
ballRadius=12;
anim=null;

function test(){
console.log("leaving");
}

function setup() {

    canvas = document.getElementById('myCanvas');
    canvas.height = window.innerHeight-80;
    canvas.width = window.innerWidth-20;
    context = canvas.getContext('2d');
    currX = canvas.width / 2;
    currY = canvas.height * 0.8;
    xv = 0;
    yv = 0;
    ballRadius = 12;
    drawBall(currX, currY, .8, ballRadius);
}

function moveBall() {

    anim = requestAnimationFrame(moveBall);
    currX += xv;
    currY += yv;
    var hit = false;
    var stopped = false;

    //apply friction
    yv *= .99;
    xv *= .99;
    //Set final speed for a stop condition
    if (Math.sqrt(xv * xv + yv * yv) <= 0.2) {
        stopped = true;
    }

    //hit right wall
    if ((currX + ballRadius) >= canvas.width) {
        currX = canvas.width - ballRadius;
        xv = -1 * xv;
        hit = true;
    }
    //hit left wall
    if ((currX - ballRadius) <= 0) {
        currX = 0 + ballRadius;
        xv = -1 * xv;
        hit = true;
    }
    //top wall
    if ((currY - ballRadius) <= 0) {
        currY = 0 + ballRadius;
        yv = -1 * yv;
        hit = true;
    }
    //bottom Wall
    if ((currY + ballRadius) >= canvas.height) {
        currY = canvas.height - ballRadius;
        yv = -1 * yv;
        hit = true;
    }
    if (hit > 0) {
        navigator.vibrate(100);
        yv = yv * .85;
        xv = xv * .85;
    }
    if (stopped) {
        cancelAnimationFrame(anim);
        setup();
    }

    drawBall(currX, currY, .8, ballRadius);
}



//Initialize game and event handlers

function init() {

    setup();
    window.addEventListener('resize', function (evt) {
        setup();
    }, false);
    setupButton();
    document.addEventListener("visibilitychange", function () {
if (document.hidden) {
  console.log("App is hidden");
} else {
  console.log("App has focus");
} });

}

//Utility function for request animation
requestAnimationFrame = (function () {
    return window.requestAnimationFrame ||
        window.webkitRequestAnimationFrame ||
        window.mozRequestAnimationFrame;
})();

//Utility function to cancel animation
cancelAnimationFrame = (function () {
    return window.cancelAnimationFrame || window.mozCancelAnimationFrame;
})();


function drawBall(x, y, a, ballRadius) {
//do nothing
}

function drawCirc(x, y, radius, r, g, b, a, context) {
    context.beginPath();
    context.arc(x, y, radius, 0, 2 * Math.PI, false);
    context.fillStyle = 'rgba(' + r + ',' + g + ',' + b + ',' + a + ')';
    context.fill();
    context.closePath();
}
function setupButton(){
 var runcode = document.querySelector("#runcode");
    if (runcode) { 
        runcode.onclick = function () {
			xv=25;
			yv=-15;
			moveBall();
		}
	}
}
