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
            case 'state': // Live-Daten aktualisieren nur das Dashboard.
                updateDashboard(data.values);
                break;
            case 'settings':  // Einstellungs-Daten aktualisieren nur das Einstellungs-Formular.
                updateSettingsForm(data.values);
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
    if (evt && evt.currentTarget) {
        evt.currentTarget.className += " active";
    }

    if (tabName === 'Camera') {
        loadImages();
    }
}

// Globale Variable, um die Bildpfade zu speichern
let imagePaths = [];

/**
 * TODO: Beschreibung hinzufügen
 */
function loadImages() {
    fetch('/api/images')
        .then(response => response.json())
        .then(images => {
            // Leere das UI
            const select = document.getElementById('image-select');
            select.innerHTML = '';

            // Prüfe, ob überhaupt Bilder vorhanden sind
            if (!images || images.length === 0) {
                document.getElementById('current-image').src = '';
                document.getElementById('image-timestamp').innerText = 'Keine Bilder auf der SD-Karte gefunden.';
                imagePaths = []; // Stelle sicher, dass die globale Liste auch leer ist
                return;
            }

            // 1. Daten verarbeiten: Sortieren und Pfade extrahieren
            // noinspection JSUnresolvedReference
            images.sort((a, b) => b.path.localeCompare(a.path));
            // noinspection JSUnresolvedReference
            imagePaths = images.map(img => img.path);

            // 2. UI füllen: Das Dropdown-Menü erstellen
            imagePaths.forEach(path => {
                const option = document.createElement('option');
                // noinspection JSValidateTypes
                option.value = path;
                option.innerText = path.replace('/', '');
                select.appendChild(option);
            });

            // 3. Initialen Zustand herstellen: Das erste (neueste) Bild anzeigen
            showImage(imagePaths[0]);

            // 4. Event-Listener für zukünftige Interaktionen setzen
            select.onchange = () => showImage(select.value);
        });
}

/**
 * TODO: Beschreibung hinzufügen
 * @param path
 */
function showImage(path) {
    document.getElementById('current-image').src = `/img?path=${path}`;
    document.getElementById('image-timestamp').innerText = path.replace('/', '');
}

/**
 * TODO: Beschreibung hinzufügen
 */
function captureNow() {
    sendCommand("captureNow", "camera");
    // Warte kurz, damit das Bild gespeichert werden kann, dann lade die Liste neu
    setTimeout(loadImages, 2500);
}

let timelapseInterval;
function playTimelapse() {
    if (imagePaths.length === 0) return;

    // Stoppe einen eventuell laufenden Zeitraffer
    clearInterval(timelapseInterval);

    let currentIndex = 0;

    // Starte den neuen Zeitraffer
    timelapseInterval = setInterval(() => {
        // Gehe zum nächsten Bild, beginne von vorn, wenn das Ende erreicht ist
        currentIndex = (currentIndex + 1) % imagePaths.length;
        // Lade und zeige das nächste Bild an
        showImage(imagePaths[currentIndex]);
    }, 500); // Geschwindigkeit: 2 Bilder pro Sekunde
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

    element = document.getElementById('airTemp');
    element.innerText = (values.airTemp !== null) ? values.airTemp.toFixed(1) : '---';

    element = document.getElementById('humidity');
    element.innerText = (values.humidity !== null) ? values.humidity.toFixed(0) : '---';

    element = document.getElementById('fanOn');
    element.innerText = values.fanOn ? 'AN' : 'AUS';
    element.className = values.fanOn ? 'status-on' : 'status-off';

    // --- Kachel für Boden ---

    element = document.getElementById('soilTemp');
    element.innerText = (values.soilTemp !== null) ? values.soilTemp.toFixed(1) : '---';

    element = document.getElementById('soilMoisture');
    element.innerText = (values.soilMoisture !== -1) ? values.soilMoisture : '---';

    element = document.getElementById('heaterOn');
    element.innerText = values.heaterOn ? 'AN' : 'AUS';
    element.className = values.heaterOn ? 'status-on' : 'status-off';

    // --- Kachel für Beleuchtung ---

    element = document.getElementById('lightLux');
    element.innerText = (values.lightLux !== null) ? values.lightLux.toFixed(0) : '---';

    element = document.getElementById('lamp1On');
    element.innerText = values.lamp1On ? 'AN' : 'AUS';
    element.className = values.lamp1On ? 'status-on' : 'status-off';

    element = document.getElementById('lamp2On');
    element.innerText = values.lamp2On ? 'AN' : 'AUS';
    element.className = values.lamp2On ? 'status-on' : 'status-off';

    // --- Kachel für Wasser ---

    element = document.getElementById('waterLevelOk');
    element.innerText = values.waterLevelOk ? '👍' : '👎';
    element.className = values.waterLevelOk ? 'water-level-full' : 'water-level-empty';

    element = document.getElementById('pumpOn');
    element.innerText = values.pumpOn ? 'AN' : 'AUS';
    element.className = values.pumpOn ? 'status-on' : 'status-off';

    element = document.getElementById('misterOn');
    element.innerText = values.misterOn ? 'AN' : 'AUS';
    element.className = values.misterOn ? 'status-on' : 'status-off';
}

/**
 * @typedef {object} Settings
 * @property {number} airTempThresholdHigh - Zielwert für Raumtemperatur in °C (S1)
 * @property {number} humidityTarget - Zielwert Luftfeuchtigkeit in % (S1)
 * @property {number} soilTempTarget - Zielwert für Bodentemperatur in °C (S2)
 * @property {number} soilMoistureTarget - Zielwert für Bodenfeuchte in % (S3)
 * @property {number} light1OnHour - Tageszeit für Lampe 1 (Stunde, 0-23)
 * @property {number} light1OffHour - Nachtzeit für Lampe 1 (Stunde, 0-23)
 * @property {number} light1LuxThresholdDark - in Lux - ist das Tageslicht dunkler, wird die Lampe 1 zur Tageszeit eingeschaltet
 * @property {number} light1LuxThresholdBright - in Lux - ist das Tageslicht heller, wird die Lampe 1 zur Tageszeit ausgeschaltet * @property {number} light2OnHour - Tageszeit für Lampe 2 (Stunde, 0-23)
 * @property {number} light2OffHour - Nachtzeit für Lampe 2 (Stunde, 0-23)
 * @property {number} light2LuxThresholdDark - in Lux - ist das Tageslicht dunkler, wird die Lampe 2 zur Tageszeit eingeschaltet
 * @property {number} light2LuxThresholdBright - in Lux - ist das Tageslicht heller, wird die Lampe 2 zur Tageszeit ausgeschaltet
 * @property {number} fanCooldownDurationMs - Laufzeit des Lüfters in ms (A4, Default: 5 Minuten)
 * @property {number} wateringDurationMs - Dauer der Bewässerung in ms (A5, Default: 5 Sekunden)
 * @property {string} lamp1Mode - Der Steuermodus ('auto', 'on', 'off') für die Lampe 1 (A1).
 * @property {string} lamp2Mode - Der Steuermodus ('auto', 'on', 'off') für die Lampe 2 (A2).
 * @property {string} heaterMode - Der Steuermodus ('auto', 'on', 'off') für die Heizung (A3).
 * @property {string} fanMode - Der Steuermodus ('auto', 'on', 'off') für den Lüfter (A4).
 * @property {string} pumpMode - Der Steuermodus ('auto', 'on', 'off') für die Pumpe (A5).
 * @property {string} misterMode - Der Steuermodus ('auto', 'on', 'off') für den Vernebler (A6).
 */

/**
 * @brief Füllt das Einstellungs-Formular mit den Werten vom ESP32.
 * @param {Settings} settings - Das Objekt mit allen Einstellungs- und Modus-Werten.
 */
function updateSettingsForm(settings) {
    document.getElementById('airTempThresholdHigh').value = settings['airTempThresholdHigh'];
    document.getElementById('humidityTarget').value = settings['humidityTarget'];
    document.getElementById('soilTempTarget').value = settings['soilTempTarget'];
    document.getElementById('soilMoistureTarget').value = settings['soilMoistureTarget'];
    document.getElementById('light1OnHour').value = settings['light1OnHour'];
    document.getElementById('light1OffHour').value = settings['light1OffHour'];
    document.getElementById('light1LuxThresholdDark').value = settings['light1LuxThresholdDark'];
    document.getElementById('light1LuxThresholdBright').value = settings['light1LuxThresholdBright'];
    document.getElementById('light2OnHour').value = settings['light2OnHour'];
    document.getElementById('light2OffHour').value = settings['light2OffHour'];
    document.getElementById('light2LuxThresholdDark').value = settings['light2LuxThresholdDark'];
    document.getElementById('light2LuxThresholdBright').value = settings['light2LuxThresholdBright'];
    document.getElementById('fanCooldownDurationMs').value = settings['fanCooldownDurationMs'] / 1000.0;
    document.getElementById('wateringDurationMs').value = settings['wateringDurationMs'] / 1000.0;
    document.querySelector(`input[name="lamp1"][value="${settings.lamp1Mode}"]`).checked = true;
    document.querySelector(`input[name="lamp2"][value="${settings.lamp2Mode}"]`).checked = true;
    document.querySelector(`input[name="heater"][value="${settings.heaterMode}"]`).checked = true;
    document.querySelector(`input[name="fan"][value="${settings.fanMode}"]`).checked = true;
    document.querySelector(`input[name="pump"][value="${settings.pumpMode}"]`).checked = true;
    document.querySelector(`input[name="mister"][value="${settings.misterMode}"]`).checked = true;
}

/**
 * @brief Liest die Werte aus dem Formular, verpackt sie in ein JSON und sendet sie.
 */
function saveSettings() {
    const statusDiv = document.getElementById('settings-status');
    statusDiv.innerText = 'Speichere...';

    const payload = {};
    const form = document.getElementById('settingsForm');
    // Iteriere über alle Input-Elemente im Formular
    Array.from(form.elements).forEach(input => {
        if (input.id === 'fanCooldownDurationMs' || input.id === 'wateringDurationMs') {
            // Konvertiere den Wert vom Benutzer (in Sekunden) zurück in Millisekunden
            payload[input.id] = parseInt(input.value) * 1000;
        } else if (input.type === 'number') {
            // Konvertiere alle anderen Zahlen-Inputs zu Fließkomma- oder Ganzzahlen
            payload[input.id] = (input.step && input.step.includes('.')) ? parseFloat(input.value) : parseInt(input.value);
        } else {
            // Für andere Typen (z.B. Text) übernehme den Wert direkt
            payload[input.id] = input.value;
        }
    });

    const message = {
        type: "saveSettings",
        payload: payload
    };

    console.log("Sende Einstellungen: ", message);
    websocket.send(JSON.stringify(message));

    setTimeout(() => { statusDiv.innerText = 'Gespeichert!'; }, 1500);
    setTimeout(() => { statusDiv.innerText = ''; }, 4000);
}

// --- Event Listener ---
window.addEventListener('load', () => {
    initWebSocket();

    // Event-Listener für die Tab-Navigation ---
    // 1. Finde alle Elemente mit der Klasse 'tab-link'
    document.querySelectorAll('.tab-link').forEach(button => {
        // 2. Füge jedem einzelnen Button einen 'click'-Event-Listener hinzu
        button.addEventListener('click', (event) => {
            // 3. Wenn ein Button geklickt wird, hole den Tab-Namen aus seinem data-Attribut
            const tabName = button.dataset.tabname;
            // 4. Rufe die bestehende openTab-Funktion mit den richtigen Parametern auf
            openTab(event, tabName);
        });
    });

    // Ereignishändler für Radio-Button auto/on/off
    document.querySelectorAll('.mode-selector').forEach(selector => {
        selector.addEventListener('change', (event) => {
            // 'event.target' ist der Radio-Button, der geklickt wurde.
            const target = event.target.name;
            const mode = event.target.value; // 'on', 'off' oder 'auto'
            sendCommand("setMode", target, { mode: mode });
        });
    });

    // Event-Listener für den "Einstellungen Speichern"-Button ---
    const saveButton = document.getElementById('saveSettingsButton');
    if (saveButton) {
        saveButton.addEventListener('click', () => {
            // Rufe die bestehende saveSettings-Funktion auf, wenn der Button geklickt wird.
            saveSettings();
        });
    }

    // --- NEU: Event-Listener für Kamera-Buttons ---
    const captureBtn = document.getElementById('captureNowButton');
    captureBtn.addEventListener('click', captureNow);

    const timelapseBtn = document.getElementById('playTimelapseButton');
    timelapseBtn.addEventListener('click', playTimelapse);

    const deleteBtn = document.getElementById('deleteImagesButton');
    deleteBtn.addEventListener('click', deleteAllImages);

    // Zeige den ersten Tab explizit an
    openTab(null, 'Dashboard');
    document.querySelector('.tab-link').classList.add('active');
});

/**
 * Löscht alle Bilder
 */
function deleteAllImages() {
    // Stoppe den Zeitraffer, falls er läuft
    clearInterval(timelapseInterval);

    // Sicherheitsabfrage mit Passwort
    const password = prompt("Bitte gib das Admin-Passwort ein, um alle Bilder zu löschen:");
    if (password) {
        // Sende den Befehl mit dem Passwort an den Server
        sendCommand("deleteAllImages", "camera", { password: password });

        // UI sofort leeren und dann neu laden
        document.getElementById('image-select').innerHTML = '';
        document.getElementById('current-image').src = '';

        const timestampEl = document.getElementById('image-timestamp');
        timestampEl.innerText = 'Lösche alle Bilder...';

        // Warte, bis der Löschvorgang wahrscheinlich abgeschlossen ist,
        // und lade dann die (jetzt leere) Liste vom Server,
        // was dann zu "Keine Bilder..." führt.
        setTimeout(loadImages, 1500);
    }
}

/**
 * @brief Sendet einen Befehl als JSON-Objekt an den ESP32.
 * @param {string} command Der Name des Befehls.
 * @param {string} target Das Ziel des Befehls (z.B. der Name des Relais 'lamp1').
 * @param {object} [payload={}] - Ein optionales Objekt mit zusätzlichen Daten.
 */
function sendCommand(command, target, payload = {}) {
    const message = {
        type: "command",
        command: command,
        target: target,
        ...payload,
    };
    console.log("Sende Befehl: ", message);
    websocket.send(JSON.stringify(message));
}
