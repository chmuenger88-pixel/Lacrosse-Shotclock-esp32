Achtung die Anlage geht nicht wenn viele Störsignale, bzw. Leute vor Ort sind. Eine Weiterentwicklung ohne 433Mhz ist zurzeit in entwicklung

# Lacrosse Shotclock Bern Titans

Dieses Projekt ermöglicht den selbstständigen Bau einer professionellen Shotclock- und Spielzeitanlage für Lacrosse, die über eine intuitive Weboberfläche gesteuert wird. Das System ist modular aufgebaut und kann flexibel für Spiele und Training eingesetzt werden.

---

## Systemübersicht

Das System besteht aus drei Komponenten:

- **Master-Controller (ESP32)**: Zentrale Steuereinheit mit integrierter Weboberfläche
- **Anzeigemodule (2x ESP32)**: Empfangen Befehle per Funk und steuern LED-Panels an
- **HC-12 Funkmodule (433 MHz)**: Drahtlose Kommunikation zwischen Master und Anzeigemodulen

Die Weboberfläche wird über ein beliebiges Gerät (Smartphone, Tablet, Laptop) im lokalen WLAN des Master-Controllers aufgerufen.

---

## Funktionen

### Shotclock
- Start/Stopp-Kontrolle
- Automatisches Reset auf konfigurierte Dauer (Standard: 30 Sekunden)
- Manuelle Zeit-Korrektur (+5s, +1s, -1s, -5s)
- Konfigurierbar pro Spielmodus (Training, Sixes, Field)

### Spielzeit (Game Clock)
- Viertel-Management (1-4 Viertel konfigurierbar)
- Automatische Pausen zwischen Vierteln (kurz/lang)
- Manuelle Zeit-Korrektur im Format mm:ss
- Weiterschalten zum nächsten Viertel
- Overtime-Modus bei Gleichstand
- Spielende automatisch erkannt

### Timeout-System
- Konfigurierbares Timeout (Standard: 30s für Sixes, 90s für Field)
- Timeout kann nur gestartet werden, wenn Spieluhr gestoppt ist
- Automatisches Horn nach Timeout-Ende
- Manuelles Beenden des Timeouts ohne Horn

### Punktestand
- Live-Zähler für Heim- und Gastteam
- Erhöhen/Erniedrigen der Punkte
- Reset-Funktion

### Hornsignale
- Automatisches Horn bei Shotclock-Ende
- Automatisches Horn bei Viertel-Ende
- Automatisches Horn bei Timeout-Ende
- Manuelles Horn per Button

### Trainingsmodus
- Shotclock funktioniert normal
- Spielzeituhr wird durch aktuelle Uhrzeit ersetzt
- Ideal für Training ohne formale Spielzeit

---

## Spielmodi

Das System unterstützt vordefinierte Spielmodi mit automatischen Konfigurationen:

### Training
- Shotclock: 30s
- Keine Spielzeituhr (aktuelle Uhrzeit wird angezeigt)
- Keine Pausen oder Viertel

### Sixes
- Shotclock: 30s
- Vierteldauer: 8 Minuten
- Kurze Pause: 2 Minuten
- Lange Pause: 5 Minuten
- Anzahl Viertel: 4
- Overtime-Viertel: 4 Minuten
- Overtime-Pause: 2 Minuten
- Timeout-Dauer: 30 Sekunden

### Field
- Shotclock: 80s
- Vierteldauer: 15 Minuten
- Kurze Pause: 2 Minuten
- Lange Pause: 10 Minuten
- Anzahl Viertel: 4
- Overtime-Viertel: 4 Minuten
- Overtime-Pause: 3 Minuten
- Timeout-Dauer: 90 Sekunden

### Benutzerdefiniert
- Alle Parameter frei konfigurierbar
- Alle Einstellungen werden gespeichert

---

## Hardware-Anforderungen

### Komponenten für Master-Controller
- ESP32 DevKit v1 Wroom32
- HC-12 Funkmodul
- 5V/5A Stromversorgung
- Horn (5V, z.B. Summer oder Sirene)
- Optionale physische Buttons für Shotclock (Start/Stopp, Reset)

### Komponenten pro Anzeigemodul
- ESP32 DevKit
- HC-12 Funkmodul
- 2x P10 LED-Panel (32x16 Pixel, monochrom, HUB12-Anschluss)
- 5V/8A Stromversorgung
- Verkabelung nach DMD32-Fast Dokumentation

**Hinweis**: Der aktuelle Empfängercode ist speziell für P10-Panels kalibriert. Andere Panel-Typen erfordern Anpassungen.

---

## Software-Installation

### Voraussetzungen
- Arduino IDE (Version 2.0 oder neuer empfohlen)
- ESP32 Board-Support für Arduino IDE

### Schritt-für-Schritt

1. **Board-Support installieren**
   - Tools → Board → Board-Manager → "esp32" suchen und installieren

2. **Bibliotheken installieren**
   - Tool → Bibliotheken verwalten
   - Folgende Bibliotheken installieren:
     - WiFi (vorinstalliert)
     - WebServer (vorinstalliert)
     - LittleFS (für Feedback-Speicherung)
   - **DMD32-Fast** manuell von https://github.com/ekapujiw2002/DMD32-Fast herunterladen und installieren

3. **Code hochladen**
   - `shotclock_master_v0_8.ino` auf den Master-ESP32 hochladen
   - `shotclock_empfaenger_v0_8.ino` auf beide Anzeigemodule hochladen

4. **Konfiguration**
   - Master-Code: WLAN-SSID und Passwort anpassen (Standard: SSID "Shotclock_Master", Passwort "lacrosse2024")
   - Delete-Passwort für Feedback anpassen (Standard: "88956")

### Verkabelung
- Detaillierte Verkabelungsanleitung für die HC-12 Module und P10-Panels finden Sie in der DMD32-Fast Dokumentation
- Physische Buttons (optional): GPIO5 (Start/Stop) und GPIO4 (Reset)
- Horn: GPIO13

---

## Weboberfläche

### Hauptseite
Nach dem Starten des Master-Controllers verbindete Sie sich mit dem WLAN "Shotclock_Master" und öffnen die Webseite unter `http://192.168.4.1/`

**Verfügbare Funktionen:**
- Shotclock-Steuerung (Start/Stopp, Reset, Korrektur)
- Horn-Button
- Spielzeit-Steuerung (Start/Stopp, Reset, Korrektur)
- Timeout-Management
- Punktestand
- Nächstes Viertel

### Setup-Seite
Erreichbar über "Setup-Seite" Button oder direkt unter `http://192.168.4.1/setup`

**Konfigurierbar:**
- Spielmodus (Training/Sixes/Field/Benutzerdefiniert)
- Benutzerdefinierte Zeiten (nur im Benutzerdefiniert-Modus)
- Anzahl der Viertel
- Teamnamen

**Zusätzlich:**
- Feedback & Bugs melden
- Gespeichertes Feedback lesen und löschen (mit Passwortschutz)

---

## Betrieb

### Normales Spiel (Sixes oder Field)
1. Wählen Sie den Spielmodus im Setup
2. Konfigurieren Sie die Teamnamen
3. Starten Sie das Spiel mit "Start" Button
4. Die Shotclock läuft automatisch weiter, auch wenn die Spielzeit pausiert
5. Timeouts können während Pausen gestartet werden

### Training
1. Wählen Sie "Training" Modus
2. Starten Sie die Shotclock mit Start/Stop
3. Die aktuelle Uhrzeit wird angezeigt (keine formale Spielzeit)

### Timeout
1. Pausieren Sie die Spielzeit (Stop-Button)
2. Drücken Sie "Timeout" Button während die Spieluhr gestoppt ist
3. Das Timeout läuft automatisch ab und signalisiert mit Horn
4. Alternativ: Drücken Sie "Timeout" noch einmal zum manuellen Beenden ohne Horn

---

## Feedback-System

Das System speichert Feedback und Bug-Reports lokal:
- Über "Feedback & Bugs melden" Button öffnet sich eine Formular-Seite
- Eingabein werden in `/feedback.txt` auf dem Master-Controller gespeichert
- Feedback kann unter "Feedback lesen" (mit Passwortschutz zum Löschen) angesehen werden

---

## Fehlerbehebung

### Master-Controller verbindet sich nicht
- Überprüfen Sie WLAN-SSID und Passwort in der Konfiguration
- Serien-Monitor prüfen (Baud-Rate: 115200)
- ESP32 neu starten

### Anzeigemodule zeigen nichts
- HC-12 Verkabelung überprüfen (RX/TX und Antennen)
- Serien-Monitor der Module prüfen auf empfangene Befehle
- Stromversorgung kontrollieren (mindestens 8A für zwei P10-Panels)
- DMD32-Fast Konfiguration überprüfen

### Shotclock zählt nicht runter
- Überprüfen Sie, ob die Spielzeit läuft (gelber Text = läuft)
- Im Training-Modus: Spielzeit ist deaktiviert, aber Shotclock sollte funktionieren
- Serien-Monitor auf Fehler prüfen

### Horn funktioniert nicht
- GPIO13 Verkabelung prüfen
- Horn-Stromversorgung kontrollieren
- Test mit "Horn" Button im Setup durchführen

---

## Geplante Erweiterungen

- **Grosses Master-Display**: Zusätzliches P10-Panel für Spieler und Zuschauer (Haupt-Anzeige)
- **Funkhandsender**: Separate Funk-Fernbedienung mit physischen Buttons für schnellere Reaktion
- **Unterstützung weiterer Panel-Typen**: RGB-Panels und andere Grössen
- **Einzelpanel-Version**: Optimierter Code für nur ein P10-Panel pro Anzeigemodul
- **LED-Panel-Editor**: Web-basiertes Tool zum Zeichnen von Grafiken für die LED-Panels

---

## Version

Aktuelle Version: **v0.8.0**

---

## Lizenz und Attribut

Entwickelt für: **Bern Titans Lacrosse**

Basierend auf: DMD32-Fast Library (https://github.com/ekapujiw2002/DMD32-Fast)
