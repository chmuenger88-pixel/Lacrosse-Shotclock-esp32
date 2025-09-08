#include <WiFi.h>
#include <WebServer.h>
#include <HardwareSerial.h>

// WLAN-Einstellungen (Access Point)
const char* ssid = "Shotclock_Master";
const char* password = "lacrosse2024";

// Webserver auf Port 80
WebServer server(80);

// HC-12 für Funk zu Displays (ESP32 UART2)
HardwareSerial hc12(2);

// Shotclock Variablen
int shotclockTime = 30;
int resetTime = 30;
bool shotclockRunning = false;
bool hornActive = false;
unsigned long shotclockLastUpdate = 0;
unsigned long hornStartTime = 0;

// Spieluhr Variablen
int quarterLength = 480;
int currentQuarter = 1;
int gameTimeRemaining = 480;
bool gameRunning = false;
unsigned long gameLastUpdate = 0;

// Pausen Variablen
int pauseLengthShort = 120; // 2 Minuten
int pauseLengthLong = 300;  // 5 Minuten
bool isBreak = false;


// Trainingsmodus-Variable
bool trainingModeEnabled = false;

// Punktestand
int scoreHome = 0;
int scoreAway = 0;
String teamHome = "Bern";
String teamAway = "Gast";

// Function prototypes to avoid 'not declared in this scope' errors
void handleRoot();
void handleSetupPage();
void handleStartStop();
void handleReset();
void handleHorn();
void handleAdjustTime();
void handleGameStartStop();
void handleGameReset();
void handleNextQuarter();
void handleGameAdjust();
void handleHomeScore();
void handleAwayScore();
void handleResetScore();
void handleSetShotclock();
void handleSetQuarterLength();
void handleStatus();
void sendToDisplays(String message);
void handleGameSetTime();
void handleSetTrainingMode();
void handleSetBreakLength();
void handleSetHomeName();
void handleSetAwayName();

// --- Setup ---
void setup() {
  Serial.begin(115200);
  hc12.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 für UART2

  // Access Point starten
  WiFi.softAP(ssid, password);
  Serial.println("WLAN Hotspot gestartet");
  Serial.print("IP Adresse: ");
  Serial.println(WiFi.softAPIP());

  // Web-Routes definieren
  server.on("/", handleRoot);
  server.on("/setup", handleSetupPage);

  // Shotclock Routes
  server.on("/startstop", handleStartStop);
  server.on("/reset", handleReset);
  server.on("/horn", handleHorn);
  server.on("/adjust", handleAdjustTime);

  // Spieluhr Routes
  server.on("/gamestartstop", handleGameStartStop);
  server.on("/gamereset", handleGameReset);
  server.on("/nextquarter", handleNextQuarter);
  server.on("/gameadjust", handleGameAdjust);
  server.on("/gamesettime", handleGameSetTime);

  // Punkte Routes
  server.on("/homescore", handleHomeScore);
  server.on("/awayscore", handleAwayScore);
  server.on("/resetscore", handleResetScore);

  // Setup Routes
  server.on("/setshotclock", handleSetShotclock);
  server.on("/setquarter", handleSetQuarterLength);
  server.on("/settmode", handleSetTrainingMode);
  server.on("/setbreak", handleSetBreakLength);
  server.on("/sethomename", handleSetHomeName);
  server.on("/setawayname", handleSetAwayName);

  // Status
  server.on("/status", handleStatus);

  server.begin();
  Serial.println("Web Server gestartet");
}

// --- Loop ---
void loop() {
  server.handleClient();

  // Shotclock Countdown
  // Läuft nur, wenn Spieluhr läuft ODER Trainingsmodus aktiv ist
  if ((shotclockRunning && gameRunning) || (shotclockRunning && trainingModeEnabled)) {
    if (millis() - shotclockLastUpdate >= 1000) {
      shotclockTime--;
      shotclockLastUpdate = millis();
      sendToDisplays("TIME:" + String(shotclockTime));
      Serial.println("Shotclock: " + String(shotclockTime));

      if (shotclockTime <= 0 && shotclockRunning) {
        shotclockRunning = false;
        shotclockTime = 0;
        if (!hornActive) {
          sendToDisplays("HORN:ON");
          hornActive = true;
          hornStartTime = millis();
          Serial.println("SHOTCLOCK ABGELAUFEN - HORN!");
        }
      }
    }
  }

  // Spieluhr Countdown
  if (gameRunning && (millis() - gameLastUpdate >= 1000)) {
    gameTimeRemaining--;
    gameLastUpdate = millis();
    Serial.printf("Spielzeit Q%d: %02d:%02d\n", currentQuarter, gameTimeRemaining / 60, gameTimeRemaining % 60);

    if (gameTimeRemaining <= 0) {
      gameRunning = false;
      shotclockRunning = false;
      hornActive = true;
      hornStartTime = millis();

      if (isBreak) {
        isBreak = false;
        if (currentQuarter < 4) {
          currentQuarter++;
          gameTimeRemaining = quarterLength;
          Serial.println("PAUSE BEENDET! Nächstes Viertel: Q" + String(currentQuarter));
        } else {
          currentQuarter = 1;
          gameTimeRemaining = 0;
          Serial.println("SPIEL BEENDET!");
        }
      } else {
        Serial.println("VIERTEL " + String(currentQuarter) + " BEENDET!");
        if (currentQuarter == 1 || currentQuarter == 3) {
          gameTimeRemaining = pauseLengthShort;
          isBreak = true;
          gameRunning = true;
          Serial.println("STARTE KURZE PAUSE (" + String(pauseLengthShort/60) + " MIN)");
        } else if (currentQuarter == 2) {
          gameTimeRemaining = pauseLengthLong;
          isBreak = true;
          gameRunning = true;
          Serial.println("STARTE LANGE PAUSE (" + String(pauseLengthLong/60) + " MIN)");
        } else {
          currentQuarter = 1;
          gameTimeRemaining = 0;
          Serial.println("SPIEL BEENDET!");
        }
      }
    }
  }

  // Horn nach 2 Sekunden ausschalten
  if (hornActive && (millis() - hornStartTime >= 2000)) {
    hornActive = false;
    sendToDisplays("HORN:OFF");
    Serial.println("HORN AUSGESCHALTET");
  }
}

// --- HTML-Hauptseite ---
void handleRoot() {
  String html = "";
  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "  <title>Lacrosse Master</title>";
  html += "  <meta charset=\"UTF-8\">";
  html += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "  <style>";
  html += "body { font-family: Arial; text-align: center; background: #2c3e50; color: white; }";
  html += ".container { max-width: 400px; margin: 0 auto; padding: 20px; }";
  html += ".section { background: #34495e; padding: 15px; border-radius: 10px; margin-bottom: 20px; }";
  html += "h1 { color: #ecf0f1; }";
  html += "h2 { color: #bdc3c7; margin-top: 0; }";
  html += ".display { font-size: 48px; font-weight: bold; margin: 10px 0; }";
  html += ".button { font-size: 16px; padding: 10px 10px; margin: 3px; border-radius: 8px; cursor: pointer; border: none; color: white; }";
  html += ".btn-startstop { background: #27ae60; }";
  html += ".btn-reset { background: #f39c12; }";
  html += ".btn-horn { background: #9b59b6; }";
  html += ".btn-score { background: #3498db; }";
  html += "#shotclockResetButton { font-size: 32px; padding: 20px 20px; }";
  html += ".score-display { font-size: 48px; font-weight: bold; display: flex; justify-content: space-around; }";
  html += ".team-score { text-align: center; }";
  html += "input[type=\"text\"] { font-size: 16px; padding: 10px; border-radius: 8px; border: none; text-align: center; width: 100px; }";
  html += "#teamNameHome { color: #ffa500; }"; // Orange
  html += "#teamNameAway { color: #ffffff; }"; // Weiß
  html += "  </style>";
  html += "  <script>";
  html += "    function updateStatus() {";
  html += "      fetch('/status')";
  html += "        .then(r => r.json())";
  html += "        .then(data => {";
  html += "          const shotclockDisplay = document.getElementById('shotclockTime');";
  html += "          shotclockDisplay.textContent = data.shotclockTime;";
  html += "          shotclockDisplay.style.color = data.shotclockRunning ? '#f1c40f' : '#e74c3c';";
  html += "          ";
  html += "          const gameClockSection = document.getElementById('gameClockSection');";
  html += "          const liveClockSection = document.getElementById('liveClockSection');";
  html += "          if (data.trainingModeEnabled) {";
  html += "            gameClockSection.style.display = 'none';";
  html += "            liveClockSection.style.display = 'block';";
  html += "            const now = new Date();";
  html += "            const timeString = now.toTimeString().slice(0, 8);";
  html += "            document.getElementById('liveTime').textContent = timeString;";
  html += "          } else {";
  html += "            gameClockSection.style.display = 'block';";
  html += "            liveClockSection.style.display = 'none';";
  html += "            const gameTimeDisplay = document.getElementById('gameTime');";
  html += "            let minutes = Math.floor(data.gameTime / 60);";
  html += "            let seconds = data.gameTime % 60;";
  html += "            let timeString = `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;";
  html += "            gameTimeDisplay.textContent = timeString;";
  html += "            gameTimeDisplay.style.color = data.gameRunning ? '#f1c40f' : '#e74c3c';";
  html += "            const quarterDisplay = document.getElementById('quarterDisplay');";
  html += "            if (data.isBreak) {";
  html += "              quarterDisplay.textContent = 'Pause';";
  html += "            } else {";
  html += "              quarterDisplay.textContent = 'V' + data.currentQuarter;";
  html += "            }";
  html += "          }";
  html += "          document.getElementById('scoreHome').textContent = data.scoreHome;";
  html += "          document.getElementById('scoreAway').textContent = data.scoreAway;";
  html += "          document.getElementById('teamNameHome').textContent = data.teamHome;";
  html += "          document.getElementById('teamNameAway').textContent = data.teamAway;";
  html += "        });";
  html += "    }";
  html += "";
  html += "    function sendCommand(cmd, param = '') { ";
  html += "      let url = '/' + cmd + (param ? '?' + param : '');";
  html += "      fetch(url).then(() => updateStatus());";
  html += "    }";
  html += "";
  html += "    function setGameTime() {";
  html += "      const input = document.getElementById('gameCorrectionInput').value;";
  html += "      if (input.match(/^\\d{1,2}:\\d{2}$/)) {";
  html += "        sendCommand('gamesettime', 'time=' + input);";
  html += "      } else {";
  html += "        alert('Ungültiges Format. Bitte mm:ss eingeben.');";
  html += "      }";
  html += "    }";
  html += "";
  html += "    setInterval(updateStatus, 1000);";
  html += "    updateStatus();";
  html += "  </script>";
  html += "</head>";
  html += "<body>";
  html += "  <div class=\"container\">";
  html += "    <h1>Lacrosse Master</h1>";
  html += "";
  html += "    <div class=\"section\">";
  html += "      <h2>Shotclock</h2>";
  html += "      <div class=\"display\" id=\"shotclockTime\">30</div>";
  html += "      <button class=\"button btn-startstop\" onclick=\"sendCommand('startstop')\">Start/Stop</button>";
  html += "      <button class=\"button btn-reset\" id=\"shotclockResetButton\" onclick=\"sendCommand('reset')\">Reset</button>";
  html += "      <button class=\"button btn-horn\" onclick=\"sendCommand('horn')\">Horn</button>";
  html += "      <p>Korrektur:</p>";
  html += "      <button class=\"button btn-score\" onclick=\"sendCommand('adjust', 'seconds=5')\">+5s</button>";
  html += "      <button class=\"button btn-score\" onclick=\"sendCommand('adjust', 'seconds=1')\">+1s</button>";
  html += "      <button class=\"button btn-score\" onclick=\"sendCommand('adjust', 'seconds=-1')\">-1s</button>";
  html += "      <button class=\"button btn-score\" onclick=\"sendCommand('adjust', 'seconds=-5')\">-5s</button>";
  html += "    </div>";
  html += "";
  html += "    <div class=\"section\" id=\"gameClockSection\">";
  html += "      <h2>Spieluhr</h2>";
  html += "      <div class=\"display\" id=\"gameTime\">08:00</div>";
  html += "      <div class=\"display\" style=\"font-size:32px; color:#bdc3c7;\" id=\"quarterDisplay\">V1</div>";
  html += "      <button class=\"button btn-startstop\" onclick=\"sendCommand('gamestartstop')\">Start/Stop</button>";
  html += "      <button class=\"button btn-reset\" onclick=\"sendCommand('gamereset')\">Reset</button>";
  html += "      <button class=\"button btn-startstop\" onclick=\"sendCommand('nextquarter')\">Nächstes Viertel</button>";
  html += "      <p>Korrektur (mm:ss):</p>";
  html += "      <input type=\"text\" id=\"gameCorrectionInput\" placeholder=\"mm:ss\">";
  html += "      <button class=\"button btn-score\" onclick=\"setGameTime()\">Übernehmen</button>";
  html += "    </div>";
  html += "";
  html += "    <div class=\"section\" id=\"liveClockSection\" style=\"display:none;\">";
  html += "      <h2>Uhrzeit</h2>";
  html += "      <div class=\"display\" id=\"liveTime\">--:--:--</div>";
  html += "    </div>";
  html += "";
  html += "    <div class=\"section\">";
  html += "      <h2>Punktestand</h2>";
  html += "      <div class=\"score-display\">";
  html += "        <div class=\"team-score\">";
  html += "          <h3 id=\"teamNameHome\">Bern</h3>";
  html += "          <span class=\"display\" id=\"scoreHome\">0</span>";
  html += "          <br>";
  html += "          <button class=\"button btn-score\" onclick=\"sendCommand('homescore', 'action=plus')\">+1</button>";
  html += "          <button class=\"button btn-score\" onclick=\"sendCommand('homescore', 'action=minus')\">-1</button>";
  html += "        </div>";
  html += "        <div class=\"team-score\">";
  html += "          <h3 id=\"teamNameAway\">Gast</h3>";
  html += "          <span class=\"display\" id=\"scoreAway\">0</span>";
  html += "          <br>";
  html += "          <button class=\"button btn-score\" onclick=\"sendCommand('awayscore', 'action=plus')\">+1</button>";
  html += "          <button class=\"button btn-score\" onclick=\"sendCommand('awayscore', 'action=minus')\">-1</button>";
  html += "        </div>";
  html += "      </div>";
  html += "      <br>";
  html += "      <button class=\"button btn-reset\" onclick=\"sendCommand('resetscore')\">Punkte-Reset</button>";
  html += "    </div>";
  html += "    <div class=\"section\">";
  html += "      <a href=\"/setup\" class=\"button btn-reset\">Zur Setup-Seite</a>";
  html += "    </div>";
  html += "  </div>";
  html += "</body>";
  html += "</html>";
  server.send(200, "text/html", html);
}

// --- Setup-Seite (HTML und C++ Handler) ---
void handleSetupPage() {
  String html = "";
  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "  <title>Setup</title>";
  html += "  <meta charset=\"UTF-8\">";
  html += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "  <style>";
  html += "body { font-family: Arial; text-align: center; background: #2c3e50; color: white; }";
  html += ".container { max-width: 400px; margin: 0 auto; padding: 20px; }";
  html += ".section { background: #34495e; padding: 15px; border-radius: 10px; margin-bottom: 20px; }";
  html += "h1 { color: #ecf0f1; }";
  html += "h2 { color: #bdc3c7; }";
  html += "input[type=\"number\"], input[type=\"text\"], .button { font-size: 16px; padding: 10px; margin: 5px; border-radius: 8px; border: none; }";
  html += "input[type=\"checkbox\"] { width: 20px; height: 20px; vertical-align: middle; }";
  html += ".button { cursor: pointer; color: white; background: #27ae60; }";
  html += "  </style>";
  html += "</head>";
  html += "<body>";
  html += "  <div class=\"container\">";
  html += "    <h1>Setup</h1>";
  html += "    <p>Hier k&ouml;nnen die Standardzeiten und Teamnamen angepasst werden.</p>";
  html += "    <div class=\"section\">";
  html += "      <h2>Shotclock-Dauer</h2>";
  html += "      <input type=\"number\" id=\"shotclockTime\">";
  html += "      <button class=\"button\" onclick=\"setShotclock()\">Speichern</button>";
  html += "    </div>";
  html += "    <div class=\"section\">";
  html += "      <h2>Viertel-Dauer (Minuten)</h2>";
  html += "      <input type=\"number\" id=\"quarterLength\">";
  html += "      <button class=\"button\" onclick=\"setQuarterLength()\">Speichern</button>";
  html += "    </div>";
  html += "    <div class=\"section\">";
  html += "      <h2>Pausenzeiten (Minuten)</h2>";
  html += "      <p>Kurze Pause (V1 & V3 Ende):</p>";
  html += "      <input type=\"number\" id=\"pauseShort\">";
  html += "      <p>Lange Pause (V2 Ende):</p>";
  html += "      <input type=\"number\" id=\"pauseLong\">";
  html += "      <button class=\"button\" onclick=\"setBreakLength()\">Speichern</button>";
  html += "    </div>";
  html += "    <div class=\"section\">";
  html += "      <h2>Teamnamen</h2>";
  html += "      <p>Heimteam:</p>";
  html += "      <input type=\"text\" id=\"teamNameHomeInput\">";
  html += "      <button class=\"button\" onclick=\"setHomeName()\">Speichern</button>";
  html += "      <p>Gastteam:</p>";
  html += "      <input type=\"text\" id=\"teamNameAwayInput\">";
  html += "      <button class=\"button\" onclick=\"setAwayName()\">Speichern</button>";
  html += "    </div>";
  html += "    <div class=\"section\">";
  html += "      <h2>Trainingsmodus</h2>";
  html += "      <p>Shotclock l&auml;uft unabh&auml;ngig von der Spielzeit.</p>";
  html += "      <input type=\"checkbox\" id=\"trainingModeCheckbox\" onchange=\"setTrainingMode(this.checked)\">";
  html += "      <label for=\"trainingModeCheckbox\">Aktivieren</label>";
  html += "    </div>";
  html += "      <div class=\"section\">";
  html += "        <a href=\"/\" class=\"button btn-reset\">Zurück zur Hauptseite</a>";
  html += "    </div>";
  html += "  </div>";
  html += "  <script>";
  html += "    function setShotclock() {";
  html += "      const time = document.getElementById('shotclockTime').value;";
  html += "      fetch('/setshotclock?time=' + time)";
  html += "        .then(() => alert('Shotclock-Dauer gespeichert!'));";
  html += "    }";
  html += "    function setQuarterLength() {";
  html += "      const minutes = document.getElementById('quarterLength').value;";
  html += "      fetch('/setquarter?minutes=' + minutes)";
  html += "        .then(() => alert('Viertel-Dauer gespeichert!'));";
  html += "    }";
  html += "    function setBreakLength() {";
  html += "      const short = document.getElementById('pauseShort').value;";
  html += "      const long = document.getElementById('pauseLong').value;";
  html += "      fetch('/setbreak?short=' + short + '&long=' + long)";
  html += "        .then(() => alert('Pausenzeiten gespeichert!'));";
  html += "    }";
  html += "    function setHomeName() {";
  html += "      const name = document.getElementById('teamNameHomeInput').value;";
  html += "      fetch('/sethomename?name=' + name)";
  html += "        .then(() => alert('Heimteamname gespeichert!'));";
  html += "    }";
  html += "    function setAwayName() {";
  html += "      const name = document.getElementById('teamNameAwayInput').value;";
  html += "      fetch('/setawayname?name=' + name)";
  html += "        .then(() => alert('Gastteamname gespeichert!'));";
  html += "    }";
  html += "    function setTrainingMode(enabled) {";
  html += "      const state = enabled ? 'on' : 'off';";
  html += "      fetch('/settmode?state=' + state)";
  html += "        .then(() => console.log('Trainingsmodus gesetzt: ' + state));";
  html += "    }";
  html += "    function updateSetupPage() {";
  html += "      fetch('/status')";
  html += "        .then(r => r.json())";
  html += "        .then(data => {";
  html += "          document.getElementById('shotclockTime').value = data.resetTime;";
  html += "          document.getElementById('quarterLength').value = data.quarterLength;";
  html += "          document.getElementById('trainingModeCheckbox').checked = data.trainingModeEnabled;";
  html += "          document.getElementById('pauseShort').value = data.pauseLengthShort;";
  html += "          document.getElementById('pauseLong').value = data.pauseLengthLong;";
  html += "          document.getElementById('teamNameHomeInput').value = data.teamHome;";
  html += "          document.getElementById('teamNameAwayInput').value = data.teamAway;";
  html += "        });";
  html += "    }";
  html += "    updateSetupPage();";
  html += "  </script>";
  html += "</body>";
  html += "</html>";
  server.send(200, "text/html", html);
}

// Handler für die Spieluhr-Korrektur per Eingabe
void handleGameSetTime() {
  if (server.hasArg("time")) {
    String timeStr = server.arg("time");
    int colonIndex = timeStr.indexOf(':');
    if (colonIndex != -1) {
      int minutes = timeStr.substring(0, colonIndex).toInt();
      int seconds = timeStr.substring(colonIndex + 1).toInt();
      if (minutes >= 0 && seconds >= 0 && seconds < 60) {
        gameTimeRemaining = minutes * 60 + seconds;
        Serial.println("GAMETIME SET TO: " + String(minutes) + "m " + String(seconds) + "s");
      }
    }
  }
  server.send(200, "text/plain", "OK");
}

// Handler für den Trainingsmodus
void handleSetTrainingMode() {
  if (server.hasArg("state")) {
    String state = server.arg("state");
    trainingModeEnabled = (state == "on");
    Serial.println("TRAINING MODE SET TO: " + state);
    if (trainingModeEnabled && !shotclockRunning) {
        shotclockRunning = true;
    }
    if (!trainingModeEnabled && !gameRunning && shotclockRunning) {
        shotclockRunning = false;
    }
  }
  server.send(200, "text/plain", "OK");
}

// Handler für die Pausenzeiten
void handleSetBreakLength() {
  if (server.hasArg("short")) {
    int minutesShort = server.arg("short").toInt();
    if (minutesShort >= 0 && minutesShort <= 30) {
      pauseLengthShort = minutesShort * 60;
      Serial.println("SHORT BREAK SET TO: " + String(minutesShort) + " MIN");
    }
  }
  if (server.hasArg("long")) {
    int minutesLong = server.arg("long").toInt();
    if (minutesLong >= 0 && minutesLong <= 30) {
      pauseLengthLong = minutesLong * 60;
      Serial.println("LONG BREAK SET TO: " + String(minutesLong) + " MIN");
    }
  }
  server.send(200, "text/plain", "OK");
}

// Handler zum Setzen des Heimteamnamens
void handleSetHomeName() {
  if (server.hasArg("name")) {
    teamHome = server.arg("name");
    Serial.println("HOME TEAM NAME SET TO: " + teamHome);
  }
  server.send(200, "text/plain", "OK");
}

// Handler zum Setzen des Gastteamnamens
void handleSetAwayName() {
  if (server.hasArg("name")) {
    teamAway = server.arg("name");
    Serial.println("AWAY TEAM NAME SET TO: " + teamAway);
  }
  server.send(200, "text/plain", "OK");
}

void handleSetShotclock() {
  if (server.hasArg("time")) {
    int newTime = server.arg("time").toInt();
    if (newTime >= 1 && newTime <= 99) {
      resetTime = newTime;
      shotclockTime = newTime;
      Serial.println("SHOTCLOCK RESET TIME SET TO: " + String(newTime));
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleSetQuarterLength() {
  if (server.hasArg("minutes")) {
    int minutes = server.arg("minutes").toInt();
    if (minutes >= 1 && minutes <= 30) {
      quarterLength = minutes * 60;
      gameTimeRemaining = quarterLength;
      Serial.println("QUARTER LENGTH SET TO: " + String(minutes) + " MIN");
    }
  }
  server.send(200, "text/plain", "OK");
}

// --- Weitere Handler ---
void handleStartStop() {
  if (shotclockRunning) {
    shotclockRunning = false;
    sendToDisplays("STOP:" + String(shotclockTime));
    Serial.println("SHOTCLOCK GESTOPPT BEI: " + String(shotclockTime));
  } else {
    if (shotclockTime > 0) {
      shotclockRunning = true;
      shotclockLastUpdate = millis();
      sendToDisplays("START:" + String(shotclockTime));
      Serial.println("SHOTCLOCK GESTARTET MIT: " + String(shotclockTime));
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleReset() {
  shotclockTime = resetTime;
  hornActive = false;
  if (gameRunning || trainingModeEnabled) {
    shotclockRunning = true;
    shotclockLastUpdate = millis();
    sendToDisplays("START:" + String(shotclockTime));
    Serial.println("SHOTCLOCK RESET AUF: " + String(shotclockTime) + " UND GESTARTET");
  } else {
    shotclockRunning = false;
    sendToDisplays("TIME:" + String(shotclockTime));
    Serial.println("SHOTCLOCK RESET AUF: " + String(shotclockTime) + " (NICHT GESTARTET)");
  }
  server.send(200, "text/plain", "OK");
}

void handleHorn() {
  sendToDisplays("HORN:ON");
  hornActive = true;
  hornStartTime = millis();
  Serial.println("HORN MANUELL AKTIVIERT");
  server.send(200, "text/plain", "OK");
}

void handleAdjustTime() {
  if (!shotclockRunning && server.hasArg("seconds")) {
    int adjustment = server.arg("seconds").toInt();
    shotclockTime += adjustment;
    if (shotclockTime < 1) shotclockTime = 1;
    if (shotclockTime > 99) shotclockTime = 99;
    sendToDisplays("TIME:" + String(shotclockTime));
    Serial.println("SHOTCLOCK ADJUSTED TO: " + String(shotclockTime));
  }
  server.send(200, "text/plain", "OK");
}

void handleGameStartStop() {
  if (gameRunning) {
    gameRunning = false;
    shotclockRunning = false;
    Serial.println("SPIELUHR UND SHOTCLOCK GESTOPPT");
  } else {
    if (gameTimeRemaining > 0) {
      gameRunning = true;
      if (!isBreak) {
        shotclockRunning = true;
      }
      gameLastUpdate = millis();
      shotclockLastUpdate = millis();
      Serial.println("SPIELUHR GESTARTET");
      if(isBreak) Serial.println("PAUSE LÄUFT");
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleGameReset() {
  gameTimeRemaining = quarterLength;
  gameRunning = false;
  isBreak = false;
  Serial.println("SPIELZEIT RESET AUF: " + String(quarterLength/60) + " MIN");
  server.send(200, "text/plain", "OK");
}

void handleNextQuarter() {
  if (currentQuarter < 4) {
    currentQuarter++;
  } else {
    currentQuarter = 1;
  }
  gameTimeRemaining = quarterLength;
  gameRunning = false;
  isBreak = false;
  Serial.println("NÄCHSTES VIERTEL: Q" + String(currentQuarter));
  server.send(200, "text/plain", "OK");
}

void handleGameAdjust() {
  if (!gameRunning && server.hasArg("seconds")) {
    int adjustment = server.arg("seconds").toInt();
    gameTimeRemaining += adjustment;
    if (gameTimeRemaining < 0) gameTimeRemaining = 0;
    Serial.println("GAMETIME ADJUSTED TO: " + String(gameTimeRemaining));
  }
  server.send(200, "text/plain", "OK");
}

void handleHomeScore() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    if (action == "plus") scoreHome++;
    else if (action == "minus" && scoreHome > 0) scoreHome--;
    Serial.println("HOME PUNKTE: " + String(scoreHome));
  }
  server.send(200, "text/plain", "OK");
}

void handleAwayScore() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    if (action == "plus") scoreAway++;
    else if (action == "minus" && scoreAway > 0) scoreAway--;
    Serial.println("AWAY PUNKTE: " + String(scoreAway));
  }
  server.send(200, "text/plain", "OK");
}

void handleResetScore() {
  scoreHome = 0;
  scoreAway = 0;
  Serial.println("PUNKTE RESET");
  server.send(200, "text/plain", "OK");
}

void handleStatus() {
  String json = "{";
  json += "\"shotclockTime\":" + String(shotclockTime) + ",";
  json += "\"shotclockRunning\":" + String(shotclockRunning ? "true" : "false") + ",";
  json += "\"resetTime\":" + String(resetTime) + ",";
  json += "\"horn\":" + String(hornActive ? "true" : "false") + ",";
  json += "\"gameTime\":" + String(gameTimeRemaining) + ",";
  json += "\"gameRunning\":" + String(gameRunning ? "true" : "false") + ",";
  json += "\"currentQuarter\":" + String(currentQuarter) + ",";
  json += "\"quarterLength\":" + String(quarterLength / 60) + ",";
  json += "\"scoreHome\":" + String(scoreHome) + ",";
  json += "\"scoreAway\":" + String(scoreAway) + ",";
  json += "\"teamHome\":\"" + teamHome + "\",";
  json += "\"teamAway\":\"" + teamAway + "\",";
  json += "\"trainingModeEnabled\":" + String(trainingModeEnabled ? "true" : "false") + ",";
  json += "\"pauseLengthShort\":" + String(pauseLengthShort / 60) + ",";
  json += "\"pauseLengthLong\":" + String(pauseLengthLong / 60) + ",";
  json += "\"isBreak\":" + String(isBreak ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

void sendToDisplays(String message) {
  hc12.println(message);
  Serial.println("Sent: " + message);
}
