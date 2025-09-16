# 🥍 Lacrosse Shotclock (Eigenbau)

Dieses Projekt ermöglicht den selbstständigen Bau einer professionellen Shotclock- und Spielzeitanlage für Lacrosse, die über eine intuitive Weboberfläche gesteuert wird. Das System ist modular aufgebaut und kann flexibel für Spiele oder Training eingesetzt werden.

---

### ⚙️ Funktionsweise

Das System besteht aus drei Hauptkomponenten: einem **Master-Controller** und zwei **Anzeigemodulen**.

* **Master-Controller**: Ein ESP32 fungiert über eine integrierte Webseite als zentrale Steuereinheit.
* **Anzeigemodule**: Zwei weitere ESP32 empfangen die Befehle des Master-Controllers per Funk und zeigen die Zeiten auf LED-Panels an.

Die Kommunikation erfolgt drahtlos über **433 MHz HC-12 Funkmodule** für eine zuverlässige und latenzarme Übertragung über weite Distanzen.

---

### 📱 Weboberfläche (UI)

Die Steuerung erfolgt vollständig über ein Smartphone, Tablet oder Laptop, das sich mit dem Wi-Fi-Netzwerk des Master-Controllers verbindet.

**Hauptfunktionen**
* **Shotclock**: Starten, Stoppen, Zurücksetzen und manuelle Korrektur der Zeit. Die Shotclock zählt von 30 Sekunden rückwärts.
* **Spielzeit**: Starten, Stoppen, Zurücksetzen, Korrigieren und Weiterschalten zum nächsten Viertel.
* **Punktestand**: Zähler für Heim- und Gastteam mit der Möglichkeit zur Korrektur.
* **Horn**: Manuelle Aktivierung des Signals.

**Einstellungsmodus (Setup)**
Über einen separaten Einstellungsbereich können folgende Parameter konfiguriert werden:
* Dauer der Shotclock (Standard: 30 Sekunden)
* Dauer der Viertel (Standard: 8 Minuten)
* Dauer von kurzen (Viertelpause) und langen (Halbzeitpause) Pausen
* Teamnamen (Heim und Gast)
* Trainingsmodus: Schaltet die Spielzeituhr aus und ersetzt sie durch die aktuelle Uhrzeit, während die Shotclock weiterhin normal funktioniert.

---

### 🛠️ Benötigtes Material

Für den Aufbau der vollständigen Anlage werden folgende Komponenten benötigt:

* **ESP32 DevKit**: 3 Stück (1x Master, 2x Anzeigemodule)
* **HC-12 Funkmodul**: 3 Stück
* **P10-LED-Panel (32x16px, monochrom, HUB12)**: 4 Stück (je 2 pro Anzeigemodul). **Achtung: Der Empfängercode ist aktuell nur mit diesem spezifischen Panel-Typ kompatibel.**
* **Spannungsversorgung (5V)**: 2 x 8A (für die Anzeigemodule) und 1 x 5A (für den Master-Controller)
* **Horn (5V)**: 1 Stück
* **Verbindungs- und Kabelmaterial**

---

### 💻 Software und Installation

Die Software ist mit der Arduino IDE kompatibel und wurde erfolgreich getestet.

1.  **Arduino IDE einrichten**: Installieren Sie die Arduino IDE und fügen Sie die ESP32-Board-Unterstützung hinzu. **Achtung: Für den Empfängercode ist zwingend die Board-Version 2.0.2 zu verwenden, damit die DMD32-Bibliothek korrekt funktioniert. Der Master-Code wurde mit Version 3.3.0 getestet.**
2.  **Bibliotheken installieren**: Fügen Sie die erforderlichen Bibliotheken (**DMD32**, **WiFi**, **WebServer**, **HardwareSerial**) über den Bibliotheksverwalter hinzu. Die Verkabelung der Panels muss exakt gemäss der Anleitung der [DMD32-Bibliothek](https://github.com/Qudor-Engineer/DMD32) erfolgen.
3.  **Code kompilieren**: Laden Sie den Master-Code auf den Master-ESP32 und den Empfänger-Code auf die beiden anderen ESP32.
4.  **Hardware-Verkabelung**: Verbinden Sie die Komponenten gemäss den Schaltplänen.

---

### 🚀 Geplante Erweiterungen

Dieses Projekt ist als modulare Plattform konzipiert und kann in Zukunft um nützliche Funktionen erweitert werden:

* **Grosses Master-Display für Spieler und Zuschauer**: Anstelle eines kleinen Displays für den Bediener soll ein weiteres P10-Panel verwendet werden, das die Hauptuhr, die Shotclock, den Spielstand und das Viertel anzeigt. Dadurch wird die Weboberfläche nicht mehr für die Anzeige benötigt.
* **Funkhandsender und physische Buttons für die Shotclock**: Die Steuerung über eine Weboberfläche ist bei schnellen Spielzügen weniger reaktionsschnell. Es ist daher eine Erweiterung um einen separaten Funkhandsender mit physischen Tasten für die wichtigsten Funktionen (Start/Stopp, Reset und Horn) geplant.
* **Code für andere Panel-Typen**: Anpassung des Codes, um auch andere LED-Panel-Typen (z. B. RGB oder andere Grössen) zu unterstützen.
* **Code für Einzel-Panel-Anzeige**: Eine optimierte Version des Empfänger-Codes, die nur ein einzelnes P10-Panel pro Anzeigemodul ansteuert.
