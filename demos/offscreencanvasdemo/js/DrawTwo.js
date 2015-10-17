m_canvas=null;
function setupBallCanvas() {
    m_canvas = document.createElement('canvas');
    m_canvas.width = ballRadius * 2;
    m_canvas.height = ballRadius * 2;
    m_context = m_canvas.getContext('2d');


    var radius = ballRadius;
    x = radius;
    y = radius;
    a = .8;
    loopcount = 60;
    for (var i = 0; i < loopcount; i++) {
		var redval = 245;
		var greenval = 145;
		var blueval = 120+i
        drawCirc(x + i /12, y - i / 12, radius - i/6, redval, greenval, blueval, a, m_context);
    }

}
//Draw the Ball
function drawBall(x, y, a, ballRadius) {
	context.clearRect(0, 0, canvas.width, canvas.height);
    if (m_canvas == null) {
        setupBallCanvas();
    }
    var redval = 28;
    var greenval = 28;
    var blueval = 28;
    drawCirc(x + 1, y + 1, ballRadius + 2, redval, greenval, blueval, 0.3, context);
    context.drawImage(m_canvas, x - ballRadius, y - ballRadius, ballRadius * 2, ballRadius * 2);
    context.globalAlpha = 1;
}