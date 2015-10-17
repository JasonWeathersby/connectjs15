//requests up-to-date style or layout information, such as getComputedStyle(), getBoundingClientRect(), or offsetTop), we recompute the current style on the main thread.

function three_seconds_of_slow_js()
{
  var input = document.getElementsByTagName("input")[0];
  //var style = window.getComputedStyle(input, null);
  function spin_for_200ms()
  {
    var d = Date.now();
    while (Date.now() - d < 200) {
    }
  }

  setTimeout(spin_for_200ms, 0);
  setTimeout(spin_for_200ms, 700);
  setTimeout(spin_for_200ms, 1400);
  setTimeout(spin_for_200ms, 2100);
  setTimeout(spin_for_200ms, 2800);

  input.classList.add("running");
  setTimeout(function() { input.classList.remove("running") }, 2800);
}
