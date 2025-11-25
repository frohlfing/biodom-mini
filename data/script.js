"use strict";

// Globale Variable für die WebSocket-Verbindung
let websocket;

/**
 * @brief Initialisiert die WebSocket-Verbindung und die Event-Handler.
 * Wird nach dem Laden der Seite aufgerufen.
 */
function initWebSocket() {
    const gateway = `ws://${window.location.hostname}/ws`;
    const statusIndicator = document.getElementById('ws-status');

    console.log('Versuche, WebSocket zu öffnen...');
    websocket = new WebSocket(gateway);

    websocket.onopen = (_event) => {
        console.log('Verbindung hergestellt.');
        statusIndicator.className = 'status-indicator connected';
    };

    websocket.onclose = (_event) => {
        console.log('Verbindung getrennt.');
        statusIndicator.className = 'status-indicator disconnected';
        // Versuche nach 2 Sekunden, die Verbindung erneut aufzubauen
        setTimeout(initWebSocket, 2000);
    };

    websocket.onmessage = (event) => {
        console.log("Nachricht vom Server empfangen: ", event.data);
        const data = JSON.parse(event.data);

        // Verarbeite die Nachricht basierend auf ihrem Typ
        switch (data.type) {
            case 'state':
                updateDashboard(data.values);
                break;
            case 'config':
                // updateConfigForm(data.values); // Platzhalter für Tab "Einstellungen"
                break;
            case 'files':
                // updateFileList(data.files); // Platzhalter für Tab "Dateimanager"
                break;
            default:
                console.log("Unbekannter Nachrichtentyp: ", data.type);
        }
    };
}

/**
 * @brief Wechselt zwischen den Tabs.
 * @param {Event} evt Das Klick-Event.
 * @param {string} tabName Die ID des zu öffnenden Tab-Inhalts.
 */
function openTab(evt, tabName) {
    // Verstecke alle Tab-Inhalte
    const tabContents = document.getElementsByClassName("tab-content");
    for (let i = 0; i < tabContents.length; i++) {
        tabContents[i].style.display = "none";
    }

    // Entferne die "active"-Klasse von allen Tab-Links
    const tabLinks = document.getElementsByClassName("tab-link");
    for (let i = 0; i < tabLinks.length; i++) {
        tabLinks[i].className = tabLinks[i].className.replace(" active", "");
    }

    // Zeige den aktuellen Tab an und setze den Button auf "active"
    document.getElementById(tabName).style.display = "block";
    evt.currentTarget.className += " active";
}

/**
 * @typedef {object} SystemState
 *
 * @property {number | null} airTemp - Die aktuelle Raumtemperatur in °C (S1).
 * @property {number | null} humidity - Die aktuelle Luftfeuchtigkeit in % (S1).
 * @property {number | null} soilTemp - Die aktuelle Bodentemperatur in °C (S2).
 * @property {number} soilMoisture - Die aktuelle Bodenfeuchte in %, -1 bei Fehler (S3).
 * @property {boolean} waterLevelOk - Zustand des Wasserfüllstands (S4).
 * @property {number | null} lightLux - Die aktuelle Lichtstärke in Lux (S5).
 * @property {boolean} lamp1On - Zustand von Lampe 1 (A1).
 * @property {boolean} lamp2On - Zustand von Lampe 2 (A2).
 * @property {boolean} heaterOn - Zustand der Heizung (A3).
 * @property {boolean} fanOn - Zustand des Lüfters (A4).
 * @property {boolean} pumpOn - Zustand der Pumpe (A5).
 * @property {boolean} misterOn - Zustand des Verneblers (A6).
 */

/**
 * @brief Aktualisiert die Kacheln im Dashboard-Tab mit neuen Werten.
 * @param {SystemState} values Ein Objekt mit den Sensor- und Aktor-Zuständen vom ESP32.
 */
function updateDashboard(values) {
    let element;

    // --- Kachel für Raumklima ---

    const iconAir = document.getElementById('iconAir');
    if (iconAir) {
        iconAir.src = values.fanOn ? '/icons/air_16x16.png' : '/icons/thermometer_16x16.png';
    }

    element = document.getElementById('airTemp');
    if (element) {
        element.innerText = (values.airTemp !== null) ? values.airTemp.toFixed(1) : '---';
    }

    element = document.getElementById('humidity');
    if (element) {
        element.innerText = (values.humidity !== null) ? values.humidity.toFixed(0) : '---';
    }

    element = document.getElementById('fanOn');
    if (element) {
        element.innerText = values.fanOn ? 'AN' : 'AUS';
        element.className = values.fanOn ? 'status-on' : 'status-off';
    }

    // --- Kachel für Boden ---

    const iconSoil = document.getElementById('iconSoil');
    if (iconSoil) {
        iconSoil.src = values.heaterOn ? '/icons/radiator_16x16.png' : '/icons/engine_coolant_16x16.png';
    }

    element = document.getElementById('soilTemp');
    if (element) {
        element.innerText = (values.soilTemp !== null) ? values.soilTemp.toFixed(1) : '---';
    }

    element = document.getElementById('soilMoisture');
    if (element) {
        element.innerText = (values.soilMoisture !== -1) ? values.soilMoisture : '---';
    }

    element = document.getElementById('heaterOn');
    if (element) {
        element.innerText = values.heaterOn ? 'AN' : 'AUS';
        element.className = values.heaterOn ? 'status-on' : 'status-off';
    }

    // --- Kachel für Beleuchtung ---

    const iconLight = document.getElementById('iconLight');
    if (iconLight) {
        if (values.lamp1On && values.lamp2On) {
            iconLight.src = '/icons/sun_16x16.png';
        } else if (values.lamp1On || values.lamp2On) {
            iconLight.src = '/icons/sun_and_moon_16x16.png';
        } else {
            iconLight.src = '/icons/moon_and_stars_16x16.png';
        }
    }

    element = document.getElementById('lightLux');
    if (element) {
        element.innerText = (values.lightLux !== null) ? values.lightLux.toFixed(0) : '---';
    }

    element = document.getElementById('lamp1On');
    if (element) {
        element.innerText = values.lamp1On ? 'AN' : 'AUS';
        element.className = values.lamp1On ? 'status-on' : 'status-off';
    }

    element = document.getElementById('lamp2On');
    if (element) {
        element.innerText = values.lamp2On ? 'AN' : 'AUS';
        element.className = values.lamp2On ? 'status-on' : 'status-off';
    }

    // --- Kachel für Wasser ---

    // Logik: Pumpe hat Vorrang vor Vernebler
    const iconWater = document.getElementById('iconWater');
    if (iconWater) {
        if (values.pumpOn) {
            iconWater.src = '/icons/rainy_weather_16x16.png';
        } else if (values.misterOn) {
            iconWater.src = '/icons/dry_16x16.png';
        } else {
            iconWater.src = '/icons/watering_can_16x16.png';
        }
    }

    element = document.getElementById('waterLevelOk');
    if (element) {
        element.innerText = values.waterLevelOk ? 'OK' : 'NIEDRIG';
        element.className = values.waterLevelOk ? 'status-on' : 'status-off';
    }

    element = document.getElementById('pumpOn');
    if (element) {
        element.innerText = values.pumpOn ? 'AN' : 'AUS';
        element.className = values.pumpOn ? 'status-on' : 'status-off';
    }

    element = document.getElementById('misterOn');
    if (element) {
        element.innerText = values.misterOn ? 'AN' : 'AUS';
        element.className = values.misterOn ? 'status-on' : 'status-off';
    }
}

// --- Event Listener ---
window.addEventListener('load', () => {
    initWebSocket();
    // Standardmäßig den ersten Tab öffnen
    document.querySelector('.tab-link').click();
});

/**
 * @brief Sendet einen Befehl als JSON-Objekt an den ESP32.
 * @param {string} command Der Name des Befehls (z.B. 'toggle').
 * @param {string} target Das Ziel des Befehls (z.B. der Name des Relais 'lamp1').
 */
function sendCommand(command, target) {
    const message = {
        type: "command",
        command: command,
        target: target
    };
    console.log("Sende Befehl: ", message);
    websocket.send(JSON.stringify(message));
}

// --- Platzhalter für zukünftige Funktionen ---
function saveSettings() {
    console.log("Funktion 'saveSettings' noch nicht implementiert.");
}
function uploadFiles() {
    console.log("Funktion 'uploadFiles' noch nicht implementiert.");
}
