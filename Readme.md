Lacrosse Shotclock Bern Titans
Dieses Projekt ermöglicht den selbstständigen Bau einer professionellen Shotclock- und Spielzeitanlage für Lacrosse, die über eine intuitive Weboberfläche gesteuert wird. Das System ist modular aufgebaut und kann flexibel für Spiele und Training eingesetzt werden.
Systemübersicht
Das System besteht aus drei Komponenten:

Web-Master (ESP32): Zentrale Steuereinheit mit integrierter Weboberfläche und eigenem Display für den Zeitnehmer-Tisch
Send-Master (ESP32): Funk-Brücke. Empfängt Befehle vom Web-Master via UART-Kabel und verteilt sie per ESP-NOW an die Anzeigemodule
Anzeigemodule (2× ESP32): Empfangen Befehle per ESP-NOW Funk und steuern die LED-Panels an

Die Weboberfläche wird über ein beliebiges Gerät (Smartphone, Tablet, Laptop) im lokalen WLAN des Web-Masters aufgerufen.
Funktionen
Shotclock

Start/Stopp-Kontrolle
Automatisches Reset auf konfigurierte Dauer (Standard: 30 Sekunden)
Manuelle Zeit-Korrektur (+5s, +1s, -1s, -5s)
Konfigurierbar pro Spielmodus (Training, Sixes, Field)

Spielzeit (Game Clock)

Viertel-Management (1–4 Viertel konfigurierbar)
Automatische Pausen zwischen Vierteln (kurz/lang)
Manuelle Zeit-Korrektur im Format mm:ss
Weiterschalten zum nächsten Viertel
Overtime-Modus bei Gleichstand
Spielende automatisch erkannt

Timeout-System

Konfigurierbares Timeout (Standard: 30s für Sixes, 90s für Field)
Timeout kann nur gestartet werden, wenn Spieluhr gestoppt ist und keine Pause aktiv ist
Automatisches Horn nach Timeout-Ende
Manuelles Beenden des Timeouts ohne Horn

Punktestand

Live-Zähler für Heim- und Gastteam
Erhöhen/Erniedrigen der Punkte
Reset-Funktion

Hornsignale

Automatisches Horn bei Shotclock-Ende
Automatisches Horn bei Viertel-Ende
Automatisches Horn bei Timeout-Ende
Manuelles Horn per Button
Horn kann über das Admin-Panel deaktiviert werden (z.B. für Hallentraining)

Trainingsmodus

Shotclock funktioniert normal
Spielzeituhr wird durch aktuelle Uhrzeit ersetzt
Ideal für Training ohne formale Spielzeit

Spielmodi
Das System unterstützt vordefinierte Spielmodi mit automatischen Konfigurationen:
Training

Shotclock: 30s
Keine Spielzeituhr (aktuelle Uhrzeit wird angezeigt)
Keine Pausen oder Viertel

Sixes

Shotclock: 30s
Vierteldauer: 8 Minuten
Kurze Pause: 2 Minuten
Lange Pause: 5 Minuten
Anzahl Viertel: 4
Overtime-Viertel: 4 Minuten
Overtime-Pause: 2 Minuten
Timeout-Dauer: 30 Sekunden

Field

Shotclock: 80s
Vierteldauer: 15 Minuten
Kurze Pause: 2 Minuten
Lange Pause: 10 Minuten
Anzahl Viertel: 4
Overtime-Viertel: 4 Minuten
Overtime-Pause: 3 Minuten
Timeout-Dauer: 90 Sekunden

BernOpen

Drei Submodi: Round Robin (1×20 min), Placement/SF (2×14 min), 3rd/Final (2×20 min)
Shotclock: 30s
Timeout-Dauer: 60 Sekunden
Submode direkt auf der Hauptseite umschaltbar

Benutzerdefiniert

Alle Parameter frei konfigurierbar
Alle Einstellungen werden gespeichert

Hardware-Anforderungen
Komponenten für Web-Master

ESP32 DevKit v1 Wroom32
2× P10 LED-Panel (32×16 Pixel, monochrom, HUB12-Anschluss) — für den Zeitnehmer-Tisch
5V/3A Stromversorgung für die Panels (separat vom ESP32)
Horn an Relais-Ausgang (GPIO 14) — nur als Relais betreiben, kein PWM
Physische Buttons für Shotclock: GPIO 32 (Start/Stop), GPIO 33 (Reset)

Komponenten für Send-Master

ESP32 DevKit (mit externer Antenne empfohlen)
UART-Verbindung zum Web-Master (kein Funkmodul nötig)

Komponenten pro Anzeigemodul

ESP32 DevKit (mit externer Antenne empfohlen)
2× P10 LED-Panel (32×16 Pixel, monochrom, HUB12-Anschluss)
5V/3A Stromversorgung für die Panels (separat vom ESP32)
Verkabelung nach DMD32-Fast Dokumentation


Hinweis: Der aktuelle Empfängercode ist speziell für P10-Panels kalibriert. Andere Panel-Typen erfordern Anpassungen. Die Panels werden nie direkt vom ESP32 versorgt.

Software-Installation
Voraussetzungen

Arduino IDE (Version 2.0 oder neuer empfohlen)
ESP32 Board-Support für Arduino IDE

Schritt-für-Schritt

Board-Support installieren

Tools → Board → Board-Manager → "esp32" suchen und installieren


Bibliotheken installieren

Tools → Bibliotheken verwalten
Folgende Bibliotheken installieren:

WiFi (vorinstalliert)
WebServer (vorinstalliert)
LittleFS (für Feedback-Speicherung)


DMD32-Fast manuell von https://github.com/ekapujiw2002/DMD32-Fast herunterladen und installieren


Code hochladen

web_master_v0_9_6.ino auf den Web-Master ESP32 hochladen
send_master_v0_9_1.ino auf den Send-Master ESP32 hochladen
shotclock_empfaenger_v0_9_3.ino auf beide Anzeigemodule hochladen


MAC-Adressen eintragen (WICHTIG)

Lade zuerst den Empfänger-Code hoch und öffne den Seriellen Monitor (115200 Baud)
Die MAC-Adresse jedes Empfängers wird beim Start ausgegeben
Trage die Adressen im Send-Master-Code ein:



cpp     uint8_t receiver1MacAddress[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};
     uint8_t receiver2MacAddress[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};

Konfiguration

WLAN-SSID und Passwort prüfen (Standard: Shotclock_Master / lacrosse2024)
Delete-Passwort für Feedback (Standard: 88956)



Verkabelung
Web-Master ↔ Send-Master (UART)
Web-MasterSend-MasterTX GPIO 17RX GPIO 16RX GPIO 16TX GPIO 17GNDGND

Beide Module müssen zwingend ein gemeinsames GND haben.


Weboberfläche
Hauptseite
Nach dem Starten verbinden Sie sich mit dem WLAN Shotclock_Master und öffnen: http://192.168.4.1
Verfügbare Funktionen:

Shotclock-Steuerung (Start/Stopp, Reset, Korrektur)
Horn-Button
Spielzeit-Steuerung (Start/Stopp, Reset, Korrektur)
Timeout-Management
Punktestand
Nächstes Viertel

Setup-Seite
Erreichbar über "Setup-Seite" Button oder direkt unter http://192.168.4.1/setup
Konfigurierbar:

Spielmodus (Training/Sixes/Field/BernOpen/Benutzerdefiniert)
Benutzerdefinierte Zeiten (nur im Benutzerdefiniert-Modus)
Anzahl der Viertel
Teamnamen

Zusätzlich:

Feedback & Bugs melden
Gespeichertes Feedback lesen und löschen (Passwortschutz)

Admin-Panel
Erreichbar unter http://192.168.4.1/admin (Benutzer: Admin, Passwort: 88956)

Horn aktivieren/deaktivieren (z.B. für Hallentraining)
Verbundene WLAN-Clients anzeigen

Betrieb
Normales Spiel (Sixes oder Field)

Wählen Sie den Spielmodus im Setup
Konfigurieren Sie die Teamnamen
Starten Sie das Spiel mit "Start" Button
Timeouts können nur gestartet werden, wenn die Spieluhr gestoppt ist und keine Pause aktiv ist

Training

Wählen Sie "Training" Modus
Starten Sie die Shotclock mit Start/Stop
Die aktuelle Uhrzeit wird angezeigt (keine formale Spielzeit)

Timeout

Pausieren Sie die Spielzeit (Stop-Button)
Drücken Sie "Timeout" Button
Das Timeout läuft automatisch ab und signalisiert mit Horn
Alternativ: Drücken Sie "Timeout" noch einmal zum manuellen Beenden ohne Horn

Feedback-System
Das System speichert Feedback und Bug-Reports lokal:

Über "Feedback & Bugs melden" Button öffnet sich eine Formular-Seite
Eingaben werden in /feedback.txt auf dem Web-Master gespeichert
Feedback kann unter "Feedback lesen" angesehen und mit Passwort gelöscht werden

Fehlerbehebung
Master-Controller verbindet sich nicht

WLAN-SSID und Passwort in der Konfiguration prüfen
Seriellen Monitor prüfen (Baud-Rate: 115200)
ESP32 neu starten

Anzeigemodule zeigen nichts

ESP-NOW: MAC-Adressen im Send-Master prüfen
Seriellen Monitor der Module auf empfangene Befehle prüfen
Stromversorgung kontrollieren (5V/3A pro Panel-Paar, separat vom ESP32)
DMD32-Fast Konfiguration überprüfen

Shotclock zählt nicht runter

Überprüfen Sie, ob die Spielzeit läuft (gelber Text = läuft)
Im Training-Modus: Spielzeit ist deaktiviert, aber Shotclock sollte funktionieren
Seriellen Monitor auf Fehler prüfen

Horn funktioniert nicht

GPIO 14 Verkabelung und Relais prüfen
Sicherstellen, dass der Pin nicht per PWM/LEDC angesteuert wird – nur Digital HIGH/LOW
Horn-Status im Admin-Panel prüfen (könnte deaktiviert sein)
Test mit "Horn" Button auf der Hauptseite

Serieller Monitor zeigt Zeichensalat

Baud-Rate auf 115200 einstellen

Geplante Erweiterungen

Funkhandsender: Separate Funk-Fernbedienung mit physischen Buttons für schnellere Reaktion
Unterstützung weiterer Panel-Typen: RGB-Panels und andere Grössen
Einzelpanel-Version: Optimierter Code für nur ein P10-Panel pro Anzeigemodul
LED-Panel-Editor: Web-basiertes Tool zum Zeichnen von Grafiken für die LED-Panels


Version: v0.9.6 · Entwickelt für: Bern Titans Lacrosse · Basis: DMD32-Fast Library (https://github.com/ekapujiw2002/DMD32-Fast)
