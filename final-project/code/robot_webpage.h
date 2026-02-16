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
            margin-bottom: 30px;
        }
        .button {
            font-family: Impact, sans-serif;
            font-weight: normal;
            background: linear-gradient(145deg, #1a1a1a, #000000);
            border: none;
            color: #7FFFD4;
            padding: 10px 20px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 18px;
            margin: 4px 2px;
            cursor: pointer;
            transition: all 0.3s ease;
            border-radius: 5px;
            box-shadow: 5px 5px 10px #000000, -5px -5px 10px #1a1a1a;
        }
        .button:hover {
            background: linear-gradient(145deg, #7FFFD4, #40E0D0);
            color: #000000;
        }
        .button.pressed {
          background: linear-gradient(145deg, #7FFFD4, #40E0D0);
          color: #000000;
        }

        #controls {
            display: none;
        }
        .control-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
            max-width: 300px;
            margin: 0 auto;
        }
        #forward { grid-column: 2; }
        #left { grid-column: 1; grid-row: 2; }
        #stop { grid-column: 2; grid-row: 2; }
        #right { grid-column: 3; grid-row: 2; }
        #backward { grid-column: 2; grid-row: 3; }
        #servo { grid-column: 2; grid-row: 4; margin-top: 20px; }
        #back { margin-top: 20px; }
    </style>
</head>
<body>
    <h2>Vroom Vroom</h2>
    
    <div id="mode-buttons">
        <button class="button" onclick="activateTargets()">Targets</button>
        <button class="button" onclick="activateWall()">Wall</button>
        <button class="button" onclick="activateManual()">Manual</button>
    </div>

    <div id="controls">
        <div class="control-grid">
            <button id="forward" class="button" onclick="controlMotor('forward')">Forward</button>
            <button id="left" class="button" onclick="controlMotor('left')">Left</button>
            <button id="stop" class="button" onclick="controlMotor('stop')">Stop</button>
            <button id="right" class="button" onclick="controlMotor('right')">Right</button>
            <button id="backward" class="button" onclick="controlMotor('backward')">Backward</button>
            <button id="servo" class="button" onclick="activateServo()">Servo</button>
        </div>
        <button id="back" class="button" onclick="goBack()">Back</button>
    </div>

<script>
    function activateTargets() {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/targets', true);
        xhr.send();
        console.log("Target attacking activated");
    }

    function activateWall() {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/wall', true);
        xhr.send();
        console.log("Wall activated");
    }

    function activateManual() {
        document.getElementById('mode-buttons').style.display = 'none';
        document.getElementById('controls').style.display = 'block';
        console.log("Manual mode activated");
    }

    function controlMotor(direction) {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/update?dir=' + direction, true);
        xhr.send();
        console.log("Motor set to " + direction);

        unpressAllButtons();
        if (direction !== 'stop') {
            document.getElementById(direction).classList.add('pressed');
        }
    }

    function activateServo() {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/servo', true);
        xhr.send();
        console.log("Servo activated");
        unpressAllButtons();
        document.getElementById('servo').classList.add('pressed');
        setTimeout(() => {
            document.getElementById('servo').classList.remove('pressed');
        }, 200);
    }

    function goBack() {
        document.getElementById('controls').style.display = 'none';
        document.getElementById('mode-buttons').style.display = 'block';
        console.log("Returned to main menu");
    }

    function unpressAllButtons() {
        var buttons = ['forward', 'left', 'right', 'backward', 'stop', 'servo'];
        buttons.forEach(function(id) {
            document.getElementById(id).classList.remove('pressed');
        });
    }

		document.getElementById('stop').onclick = function() { controlMotor('stop'); };;
    </script>
</body>
</html>
)html";