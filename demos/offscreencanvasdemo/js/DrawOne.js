//Draw the Ball
function drawBall(x, y, a, ballRadius) {
	context.clearRect(0, 0, canvas.width, canvas.height);


    var radius = ballRadius;
    a = .8;
    
    
    var redval = 28;
    var greenval = 28;
    var blueval = 28;
    drawCirc(x + 1, y + 1, ballRadius + 2, redval, greenval, blueval, 0.3, context);
    
    loopcount = 60;
    for (var i = 0; i < loopcount; i++) {
		var redval = 245;
		var greenval = 245;
		var blueval = 220+i
        drawCirc(x + i /12, y - i /12, radius - i/6, redval, greenval, blueval, a, context);
    }

}
