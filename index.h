const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>

    <script>
  function changeColor(button) {
      var buttons = document.querySelectorAll(".button");
      buttons.forEach(function (btn) {
        btn.classList.remove("clicked");
        btn.style.backgroundColor = "";
      });

      if (!button.classList.contains("clicked")) {
        button.classList.add("clicked");
        button.style.backgroundColor = "red";
      } else {
        button.classList.remove("clicked");
        button.style.backgroundColor = "";
      }
    }

    function sendValue(value, button) {
      console.log("Sending request with value:", value);
      button.disabled = true;
      changeColor(button);

      var xhttp = new XMLHttpRequest();
      var buttonRoute;
      switch (value) {
        case 1:
          buttonRoute = "/buttonCCW";
          break;
        case 2:
          buttonRoute = "/buttonCW";
          break;
        case 3:
          buttonRoute = "/buttonSTOP";
          break;
        case 4:
          buttonRoute = "/buttonReset";
          break;
      }
      xhttp.open("GET", buttonRoute + "?value=" + value, true);
      xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
          button.disabled = false;
        }
      };
      xhttp.send();
    }

 function updateIndicators() {
  fetch("/indicator")
    .then(response => response.text())
    .then(text => {
      // Split the text by the HTTP header delimiter
      var parts = text.split("\r\n\r\n");

      // The JSON data should be in the last part
      var jsonData = parts[parts.length - 1];

      try {
        return JSON.parse(jsonData);
      } catch (error) {
        console.error("Invalid JSON:", jsonData)
        throw error;
      }
    })
    .then(data => {
      var cwIndicator = document.getElementById("cwIndicator");
      var ccwIndicator = document.getElementById("ccwIndicator");

      if (data.cwEndStopActivated) {
        cwIndicator.classList.remove("green");
        cwIndicator.classList.add("red");
      } else {
        cwIndicator.classList.remove("red"); 
        cwIndicator.classList.add("green");
      }

      if (data.ccwEndStopActivated) {
        ccwIndicator.classList.remove("green");
        ccwIndicator.classList.add("red");
      } else {
        ccwIndicator.classList.remove("red");
        ccwIndicator.classList.add("green");
      }
    })
    .catch(error => console.error("Error:", error));
}

// Call updateIndicators every second
setInterval(updateIndicators, 1000);
  

  function getData() {
   var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
         var response = JSON.parse(this.responseText);

         // Update the motor direction
         document.getElementById("mDir").textContent = response.mDir;

         // Update the end stops
         document.getElementById("Limit").textContent = response.Limit;
      }
    };

   xhttp.open("GET", "motor", true);
   xhttp.send();
}

  setInterval(function () {
    getData();
  }, 1500);




function checkDateAndDisplayMessage() {
  var currentDate = new Date();
  console.log("Current date and time:", currentDate);
  var currentMonth = currentDate.getMonth();
  var currentDay = currentDate.getDate();

  console.log("Current month:", currentMonth);
  console.log("Current day:", currentDay);

  var events = [
    { month: 1, day: 1, message: "HAPPY NEW YEAR!" },
    { month: 7, day: 1, message: "HAPPY CANADA DAY EH!" },
    { month: 10, day: 31, message: "HAPPY HALLOWEEN!"},
    { month: 11, day: 11, message: "WE WILL REMEMBER."},
    { month: 12, day: 25, message: "MERRY CHRISTMAS!"},
    { month: 12, day: 26, message: "HAPPY BOXING DAY!"}
    
  ];

 var matchingEvent = events.find(function (event) {
  return (currentMonth + 1) === event.month && currentDay === event.day;
});


  console.log("Matching event:", matchingEvent);

  if (matchingEvent) {
    document.getElementById("message").innerHTML = matchingEvent.message;
  } else {
    document.getElementById("message").innerHTML = " ";
  }
}

      window.onload = function () {
      console.log("Page loaded. Checking date and time...");
      checkDateAndDisplayMessage();

      setInterval(function () {
        console.log("Checking date and time...");
        checkDateAndDisplayMessage();
      }, 10000);
    };
    
 

function updatePosition() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            try {
                var response = JSON.parse(this.responseText);

                if (response && typeof response.position !== "undefined") {
                    var position = parseFloat(response.position);

                    if (!isNaN(position) && isFinite(position)) {
                        // Update the position display
                        document.getElementById("position").textContent = position;

                        // Update the meter value
                        var positionMeter = document.getElementById("positionMeter");
                        positionMeter.value = position;

                        // Update the position indicator
                        var positionIndicator = document.getElementById("positionIndicator");
                        positionIndicator.style.left = position + "%";
                    } else {
                        console.error("Invalid position value:", response.position);
                    }
                } else {
                    console.error("Invalid server response:", this.responseText);
                }
            } catch (error) {
                console.error("Error parsing server response:", error);
            }
        }
    };
    xhttp.open("GET", "/position", true);
    xhttp.send();
}

// Update the position every 1.5 seconds (adjust as needed)
setInterval(updatePosition, 1500);

console.log(document.getElementById("position"));
console.log(document.getElementById("positionMeter"));
console.log(document.getElementById("positionIndicator"));

</script>

    <style type="text/css">

      body {
        background-color: #f9e79f;
      }

    .btn-group .button:hover {
      background-color: #3e8e41;
    }

    .btn-group .button.clicked {
      background-color: red;
    }

    #meterContainer {
        display: flex;
        flex-direction: row;
        justify-content: center;
        align-items: center;
        text-align: center;
        margin-top: 0px;
      }

    #cwEndContainer,
    #ccwEndContainer {
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 0 0px; /* Add padding to create space between the indicators and the meter */
  }
    

    #positionMeterContainer {
      display: flex;
      flex-direction: column;
      align-items: center;
      margin: 0; /* Remove margin */
  }

      #positionMeter {
        width: 300px;
        height: 20px;
        transition: value 0.5s;
      }

      #positionIndicator {
        font-weight: bold;
        margin-top: 5px;
      }

      #cwEndContainer,
      #positionMeterContainer {
         display: flex;
         flex-direction: column;
         align-items: center;
         margin: 0 0px; /* Add margin to separate the containers */
      }

      .indicator {
         width: 25px;
         height: 15px;
         margin-top: 5px;
         border-radius: 30%;
         background-color: green;
         transition: background-color 0.5s;
      }

      .red {
         background-color: red;
      }
      
      .green {
         background-color: green;
      }


    </style>
  </head>
  <body>

    <center>
      <h1>Remote Antenna Tuner</h1>
      <span id="position">0</span> 
      
<div id="meterContainer">
    <div id="cwEndContainer">
        <p>CW End</p>
        <div class="indicator" id="cwIndicator"></div>
        <p>Limit</p>
    </div>
    <div id="positionMeterContainer">
        <meter id="positionMeter" min="0" max="5000" value="0">
        <span id="positionIndicator"></span>
        </meter>
    </div>
    <div id="ccwEndContainer">
        <p>CCW End</p>
        <div class="indicator" id="ccwIndicator"></div>
        <p>Limit</p>
    </div>
</div>


         
      <div class="btn-group">
        <button id="buttonCW" class="button" onclick="sendValue(2, this);">CW</button>
        <button id="buttonCCW" class="button" onclick="sendValue(1, this);">CCW</button>
        <button id="buttonSTOP" class="button" onclick="sendValue(3, this);">STOP</button>
        <button id="buttonReset" class="button" onclick="sendValue(4, this);">Soft-Reset</button><br><br><br>
      </div>

      <h2><span id="mDir"></span></h2>
      <h3><span id="Limit" style="color: red;"></span></h2>
      <h3><div id="message" style="color: blue;"></div></h3>

      
</center>
</body>
</html>

)=====";
