var totalPrimes = 0;
onmessage = function(e) {
    console.log('Message received from main script');
    for (i = 0; i < e.data[0]; i++) {
        if (isPrime(i)) {
            totalPrimes++;
        }
    }
    var workerResult = totalPrimes;
    console.log('Posting message back to main script');
    self.postMessage(totalPrimes);
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