const char body[] = R"html(
  <!DOCTYPE html>
  <html lang="en">
  <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Vroom Vroom</title>
      <style>
          body {
              font-family: Arial, sans-serif;
              text-align: center;
              margin-top: 50px;
              background-color: #000000;
              color: #ffffff;
          }

          h2 {
              font-family: Impact, sans-serif;
              font-size: 2em;
              letter-spacing: 1px;
              color: #7FFFD4;
          }

          .compass {
              display: grid;
              grid-template-columns: repeat(3, 1fr);
              grid-gap: 10px;
              width: 300px;
              margin: 0 auto;
          }

          .button {
              font-family: Impact, sans-serif;
              font-weight: normal;
              background: linear-gradient(145deg, #1a1a1a, #000000);
              border: none;
              color: #7FFFD4;
              padding: 20px;
              text-align: center;
              text-decoration: none;
              display: inline-block;
              font-size: 18px;
              margin: 4px 2px;
              cursor: pointer;
              transition: all 0.3s ease;
              border-radius: 50%;
              box-shadow: 5px 5px 10px #000000, -5px -5px 10px #1a1a1a;
          }

          .button:hover {
              background: linear-gradient(145deg, #000000, #1a1a1a);
          }

          .button:active {
              box-shadow: inset 5px 5px 10px #000000, inset -5px -5px 10px #1a1a1a;
          }

          .button.selected {
              background: linear-gradient(145deg, #7FFFD4, #40E0D0);
              color: #000000;
          }

          #startStopBtn {
          background: linear-gradient(145deg, #00a86b, #00c78a);
          color: white;
          }

          #startStopBtn.started {
            background: linear-gradient(145deg, #d64161, #ff4d6d);
          }

          #startStopBtn:hover {
              background: linear-gradient(145deg, #00c78a, #00a86b);
          }

          #startStopBtn.started:hover {
              background: linear-gradient(145deg, #ff4d6d, #d64161);
          }

          .direction-btn {
              display: none;
          }
      </style>
  </head>

  <body>
      <h2>Vroom Vroom</h2>
      
      <div class="compass">
          <button id="topLeftBtn" class="button direction-btn" onclick="controlMotor('dir', 'topLeft')">↖</button>
          <button id="forwardBtn" class="button direction-btn" onclick="controlMotor('dir', 'forward')">↑</button>
          <button id="topRightBtn" class="button direction-btn" onclick="controlMotor('dir', 'topRight')">↗</button>
          <button id="leftBtn" class="button direction-btn" onclick="controlMotor('dir', 'left')">←</button>
          <button id="startStopBtn" class="button" onclick="toggleStartStop()">Start</button>
          <button id="rightBtn" class="button direction-btn" onclick="controlMotor('dir', 'right')">→</button>
          <button id="bottomLeftBtn" class="button direction-btn" onclick="controlMotor('dir', 'bottomLeft')">↙</button>
          <button id="reverseBtn" class="button direction-btn" onclick="controlMotor('dir', 'reverse')">↓</button>
          <button id="bottomRightBtn" class="button direction-btn" onclick="controlMotor('dir', 'bottomRight')">↘</button>
      </div>
      
      <script>
          let isStarted = false;

          function controlMotor(param, value) {
              var xhr = new XMLHttpRequest();
              xhr.open('GET', '/update?' + param + '=' + value, true);
              xhr.send();
              
              if (param === 'dir') {
                  document.querySelectorAll('.button').forEach(btn => btn.classList.remove('selected'));
                  document.getElementById(value + 'Btn').classList.add('selected');
              }
              console.log(`Motor ${param} set to ${value}`);
          }

          function toggleStartStop() {
              isStarted = !isStarted;
              const startStopBtn = document.getElementById('startStopBtn');
              const directionBtns = document.querySelectorAll('.direction-btn');
              
              if (isStarted) {
                  startStopBtn.textContent = 'Stop';
                  startStopBtn.classList.add('started');
                  directionBtns.forEach(btn => btn.style.display = 'inline-block');
                  controlMotor('dir', 'forward'); // Automatically select forward
              } else {
                  startStopBtn.textContent = 'Start';
                  startStopBtn.classList.remove('started');
                  directionBtns.forEach(btn => {
                      btn.style.display = 'none';
                      btn.classList.remove('selected');
                  });
              }
              console.log(`Bot ${isStarted ? 'started' : 'stopped'}`);
          }

          // Center the Start button initially
          window.onload = function() {
              const compass = document.querySelector('.compass');
              const startStopBtn = document.getElementById('startStopBtn');
              startStopBtn.style.gridColumn = "2";
              startStopBtn.style.gridRow = "2";
              compass.style.gridTemplateAreas = 
                  '"tl t tr" "l c r" "bl b br"';
              startStopBtn.style.gridArea = "c";
          }
      </script>
  </body>
  </html>
)html";
