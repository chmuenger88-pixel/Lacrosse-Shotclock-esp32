# Lacrosse Shotclock Bern Titans

Dieses Projekt ermöglicht den selbstständigen Bau einer Shotclock- und Spielzeitanlage für Lacrosse, die über eine intuitive Weboberfläche gesteuert wird. Das System kann flexibel für Spiele und Training eingesetzt werden.

## Systemübersicht

Das System besteht aus drei Komponenten:

* **Web-Master (ESP32):** Zentrale Steuereinheit mit integrierter Weboberfläche und eigenem Display für den Zeitnehmer-Tisch
* **Send-Master (ESP32U):** Funk-Brücke. Empfängt Befehle vom Web-Master via UART-Kabel und verteilt sie per ESP-NOW an die Anzeigemodule
* **Anzeigemodule (2× ESP32U):** Empfangen Befehle per ESP-NOW Funk und steuern die LED-Panels an

Die Weboberfläche wird über ein beliebiges Gerät (Smartphone, Tablet, Laptop) im lokalen WLAN des Web-Masters aufgerufen.

## Funktionen

### Shotclock

* Start/Stopp-Kontrolle (Web & physische Taster)
* Automatisches Reset auf konfigurierte Dauer (Standard: 30 oder 80 Sekunden)
* Manuelle Zeit-Korrektur (+5s, +1s, -1s, -5s)
* Konfigurierbar pro Spielmodus (Training, Sixes, Field)

### Spielzeit (Game Clock)

* Viertel-Management (1–4 Viertel konfigurierbar)
* Automatische Pausen zwischen Vierteln (kurz/lang)
* Manuelle Zeit-Korrektur im Format mm:ss
* Weiterschalten zum nächsten Viertel
* Overtime-Modus bei Gleichstand
* Spielende automatisch erkannt

### Timeout-System

* Konfigurierbares Timeout (Standard: 30s für Sixes, 90s für Field)
* Timeout kann nur gestartet werden, wenn Spieluhr gestoppt ist und keine Pause aktiv ist
* Automatisches Horn nach Timeout-Ende
* Manuelles Beenden des Timeouts ohne Horn

### Punktestand & Scoresheet

* Live-Zähler für Heim- und Gastteam
* **Digitales Scoresheet:** Automatische Erfassung von Toren und Strafen mit Zeitstempel
* **Daten-Sicherheit:** Spielstand wird permanent in `sheet.json` gesichert und überlebt einen Neustart

### Hornsignale

* Automatisches Horn bei Shotclock-Ende
* Automatisches Horn bei Viertel-Ende
* Automatisches Horn bei Timeout-Ende
* Manuelles Horn per Button
* Horn kann über das Admin-Panel deaktiviert werden (Passwort: `88956`)

### Trainingsmodus

* Shotclock funktioniert normal
* Spielzeituhr wird durch aktuelle Uhrzeit ersetzt
* Ideal für Training ohne formale Spielzeit

## Spielmodi

Das System unterstützt vordefinierte Spielmodi mit automatischen Konfigurationen:

### Training
* Shotclock: 30s
* Keine Spielzeituhr (aktuelle Uhrzeit wird angezeigt)
* Keine Pausen oder Viertel

### Sixes
* Shotclock: 30s
* Vierteldauer: 8 Minuten
* Kurze Pause: 2 Minuten
* Lange Pause: 5 Minuten
* Anzahl Viertel: 4
* Overtime-Viertel: 4 Minuten
* Overtime-Pause: 2 Minuten
* Timeout-Dauer: 30 Sekunden

### Field
* Shotclock: 80s
* Vierteldauer: 15 Minuten
* Kurze Pause: 2 Minuten
* Lange Pause: 10 Minuten
* Anzahl Viertel: 4
* Overtime-Viertel: 4 Minuten
* Overtime-Pause: 3 Minuten
* Timeout-Dauer: 90 Sekunden

### Benutzerdefiniert
* Alle Parameter frei konfigurierbar
* Alle Einstellungen werden gespeichert

## Hardware-Anforderungen

### Komponenten für Web-Master

* ESP32 DevKit v1 Wroom32
* 2× P10 LED-Panel (32×16 Pixel, monochrom, HUB12-Anschluss) — für den Zeitnehmer-Tisch
* 5V/5A Stromversorgung für die Panels (separat vom ESP32)
* Horn an Relais-Ausgang (**GPIO 14**) 

### Komponenten für Send-Master

* ESP32U DevKit (mit externer Antenne empfohlen)
* UART-Verbindung zum Web-Master (kein Funkmodul nötig)

### Komponenten pro Anzeigemodul

* ESP32U DevKit (mit externer Antenne empfohlen)
* 2× P10 LED-Panel (32×16 Pixel, monochrom, HUB12-Anschluss)
* 5V/5A Stromversorgung für die Panels (separat vom ESP32)
* Verkabelung nach DMD32-Fast Dokumentation

> **Hinweis:** Der aktuelle Empfängercode ist speziell für P10-Panels kalibriert. Andere Panel-Typen erfordern Anpassungen. Die Panels werden **nie** direkt vom ESP32 versorgt.

## Software-Installation

### Voraussetzungen

* Arduino IDE (Version 2.0 oder neuer empfohlen)
* ESP32 Board-Support für Arduino IDE

### Schritt-für-Schritt

1. **Board-Support installieren**
   * Tools → Board → Board-Manager → "esp32" suchen und installieren

2. **Bibliotheken installieren**
   * Tools → Bibliotheken verwalten
   * Folgende Bibliotheken installieren:
     * WiFi (vorinstalliert)
     * WebServer (vorinstalliert)
     * **LittleFS** (für Daten-Speicherung)
   * **DMD32-Fast** manuell von https://github.com/ekapujiw2002/DMD32-Fast herunterladen und installieren

3. **Code hochladen**
   * `web_master_v1_0_14.ino` auf den Web-Master ESP32 hochladen
   * **WICHTIG:** Den Ordner `data` mit dem "ESP32 LittleFS Data Upload" Tool hochladen

4. **MAC-Adressen eintragen (WICHTIG)**
   * Lade zuerst den Empfänger-Code hoch und öffne den Seriellen Monitor (**115200 Baud**)
   * Die MAC-Adresse jedes Empfängers wird beim Start ausgegeben
   * Trage die Adressen im Send-Master-Code ein

5. **Konfiguration**
   * WLAN-SSID: `Shotclock_Master` / Passwort: `lacrosse2026`
   * Delete-Passwort für Feedback & Admin: `88956`

## Verkabelung

### Web-Master ↔ Send-Master (UART)

| Web-Master | Send-Master |
|---|---|
| TX **GPIO 17** | RX **GPIO 16** |
| RX **GPIO 16** | TX **GPIO 17** |
| GND | GND |

> Beide Module müssen zwingend ein gemeinsames GND haben.

## Weboberfläche

### Hauptseite

Nach dem Starten verbinden Sie sich mit dem WLAN `Shotclock_Master` und öffnen: `http://192.168.4.1`

Verfügbare Funktionen:
* Shotclock-Steuerung (Start/Stopp, Reset, Korrektur)
* Horn-Button
* Spielzeit-Steuerung (Start/Stopp, Reset, Korrektur)
* Timeout-Management
* Punktestand & Scoresheet
* Nächstes Viertel

### Setup-Seite

Erreichbar über "Setup-Seite" Button oder direkt unter `http://192.168.4.1/setup`

Konfigurierbar:
* Spielmodus (Training/Sixes/Field/BernOpen/Benutzerdefiniert)
* Benutzerdefinierte Zeiten (nur im Benutzerdefiniert-Modus)
* Anzahl der Viertel
* Teamnamen

Zusätzlich:
* Feedback & Bugs melden
* Gespeichertes Feedback lesen und löschen (Passwortschutz)
* **Spiel-Archiv:** Alte Spiele ansehen, löschen oder laden

### Admin-Panel

Erreichbar unter `http://192.168.4.1/admin` (Passwort: `88956`)

* Horn aktivieren/deaktivieren (z.B. für Hallentraining)
* Verbundene WLAN-Clients anzeigen
* LittleFS Speicherbelegung überwachen

## Betrieb

### Normales Spiel (Sixes oder Field)

1. Wählen Sie den Spielmodus im Setup
2. Konfigurieren Sie die Teamnamen
3. Starten Sie das Spiel mit "Start" Button
4. Timeouts können nur gestartet werden, wenn die Spieluhr gestoppt ist und keine Pause aktiv ist

### Training

1. Wählen Sie "Training" Modus
2. Starten Sie die Shotclock mit Start/Stop (Web oder Taster)
3. Die aktuelle Uhrzeit wird angezeigt (keine formale Spielzeit)

### Feedback-System

Das System speichert Feedback und Bug-Reports lokal:
* Über "Feedback & Bugs melden" Button öffnet sich eine Formular-Seite
* Eingaben werden in `/feedback.txt` auf dem Web-Master gespeichert

## Fehlerbehebung

**Master-Controller verbindet sich nicht**
* WLAN-SSID und Passwort in der Konfiguration prüfen
* Prüfen, ob der `data` Ordner via LittleFS hochgeladen wurde

**Anzeigemodule zeigen nichts**
* ESP-NOW: MAC-Adressen im Send-Master prüfen
* Stromversorgung kontrollieren (5V/5A pro Panel-Paar, separat vom ESP32)

**Shotclock reagiert nicht auf Taster**
* Verkabelung an GPIO 4 und 5 prüfen (Schaltung gegen GND)

**Horn funktioniert nicht**
* GPIO 14 Verkabelung und Relais prüfen
* Horn-Status im Admin-Panel prüfen (könnte deaktiviert sein)

**Serieller Monitor zeigt Zeichensalat**
* Baud-Rate auf **115200** einstellen

---
Version: v1.1.0 · Entwickelt für: Bern Titans Lacrosse · Basis: DMD32-Fast Library (https://github.com/ekapujiw2002/DMD32-Fast)
