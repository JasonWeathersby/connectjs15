var mytotalPrimes = 0;
var pagecnt = 0;

function goSynch() {
    console.time("testsynch");
    for (i = 0; i < 10000000; i++) {
        if (isPrime(i)) {
            mytotalPrimes++;
        }
    }
    console.timeEnd("testsynch");
    console.log("my Total Primes = " + mytotalPrimes);
}


function isPrime(n) {
    if (n < 2) return false;
    var q = Math.floor(Math.sqrt(n));
    for (var i = 2; i <= q; i++) {
        if (n % i == 0) {
            return false;
        }
    }

    return true;
}

function goAsynch() {
    var gif = document.querySelector("#mygif");
    gif.classList.remove("hidden");
    console.time("testasynch");
    var myWorker = new Worker("asynchworker.js");
    myWorker.postMessage([10000000]); //seding message as array to the worker
    console.log('Message posted to worker');
    myWorker.onmessage = function(e) {
        console.log('Message received from worker');
        console.timeEnd("testasynch");
        gif.classList.add('hidden');
    };
}

function startTimer() {
    myTimer();
}

function myTimer() {
    pagecnt++;
    var nd = document.getElementById("mycnt");
    nd.innerHTML = "current count =" + pagecnt;
    setTimeout(myTimer, 10);
}