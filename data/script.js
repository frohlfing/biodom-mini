"use strict";

// === Globale Variablen ===

/** @type {WebSocket} Die WebSocket-Verbindung zum ESP32 */
let websocket;

/** @type {string[]} Liste der Bildpfade (z.B. ["/img_2025...jpg", ...]) */
let imagePaths = [];

/** @type {number} Der Index des aktuell angezeigten Bildes im imagePaths Array */
let currentImageIndex = 0;

/** @type {number|null} ID des Zeitraffer-Intervalls (für clearInterval) */
let timelapseIntervalId = null;

/** @type {boolean} Status, ob der Zeitraffer gerade läuft */
let isPlaying = false;

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
 * @typedef {object} State
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
 * @typedef {object} ImageFile
 * @property {string} path - Der Pfad zum Bild auf der SD-Karte
 * @property {number} size - Die Größe der Datei in Bytes
 */

/**
 * @typedef {object} WebSocketMessage
 * @property {string} type - Der Typ der Nachricht (z.B. 'state', 'newImage')
 * @property {State} [values] - Nur bei type='state' oder 'settings'
 * @property {string} [path] - Nur bei type='newImage'
 */

// === Initialisierung ===

window.addEventListener('load', () => {
    initWebsocket();

    // Event-Listener für Buttons, Inputs und Tabs registrieren

    // --- Tab Navigation ---
    document.querySelectorAll('.tab-link').forEach(button => {
        button.addEventListener('click', (event) => {
            openTab(event, button.dataset.tabname);
        });
    });

    // Einstellungen
    document.getElementById('saveSettingsButton').addEventListener('click', saveSettings);

    // Radio-Buttons (Modus ändern)
    document.querySelectorAll('.mode-selector').forEach(selector => {
        selector.addEventListener('change', (event) => {
            setMode(event.target.name, event.target.value); // event.target.value == 'auto', 'on', 'off'
        });
    });

    // --- Kamera ---
    document.getElementById('captureNowButton').addEventListener('click', captureNow);
    document.getElementById('deleteImagesButton').addEventListener('click', deleteAllImages);
    document.getElementById('btnPlayPause').addEventListener('click', toggleTimelapse);
    document.getElementById('btnPrev').addEventListener('click', () => stepImage(1)); // 1 Schritt zurück (älter)
    document.getElementById('btnNext').addEventListener('click', () => stepImage(-1)); // 1 Schritt vor (neuer)
    document.getElementById('image-select').addEventListener('change', (e) => {
        handleImageChanged(e.target.value);
    });
    document.getElementById('timelapseSpeed').addEventListener('input', (e) => {
        handleTimelapseSpeedChanged(e.target.value); // todo 1: Konvention überdenken: Parameter spezifisch oder generell immer nur das event an den Handler übergeben?
    });

    // Standardmäßig den Dashboard-Tab öffnen  todo 2: raus damit, sollte im HTML der initiale Zustand sein
    openTab(null, 'Dashboard');
    const firstTab = document.querySelector('.tab-link');
    firstTab.classList.add('active');
});

/**
 * @brief Wechselt den sichtbaren Tab.
 * @param {Event} evt Das Click-Event (kann null sein beim initialen Aufruf).
 * @param {string} tabName Die ID des anzuzeigenden Divs.
 */
function openTab(evt, tabName) {
    // Alle Tab-Inhalte ausblenden
    const tabContents = document.getElementsByClassName("tab-content");
    for (let i = 0; i < tabContents.length; i++) {
        tabContents[i].style.display = "none";
    }

    // "active" Klasse von allen Buttons entfernen
    const tabLinks = document.getElementsByClassName("tab-link");
    for (let i = 0; i < tabLinks.length; i++) {
        tabLinks[i].className = tabLinks[i].className.replace(" active", "");
    }

    // Gewählten Tab anzeigen
    const target = document.getElementById(tabName);
    if (target) target.style.display = "block";

    // Button aktivieren
    if (evt && evt.currentTarget) {
        evt.currentTarget.className += " active";
    }

    // Bilder laden, wenn Kamera-Tab ausgewählt wurde und noch keine Bilder geladen wurden
    if (tabName === 'Camera' && imagePaths.length === 0) {
        loadImages();
    }
}

// === Websocket Kommunikation ===

/**
 * @brief Baut die WebSocket-Verbindung auf und definiert Handler.
 */
function initWebsocket() {
    const gateway = `ws://${window.location.hostname}/ws`;
    const statusIndicator = document.getElementById('ws-status');

    console.log('WS: Versuche Verbindung zu ' + gateway);
    websocket = new WebSocket(gateway);

    websocket.onopen = (_event) => {
        console.log('WS: Verbunden.');
        statusIndicator.className = 'status-indicator connected';
    };

    websocket.onclose = (_event) => {
        console.log('WS: Getrennt.');
        statusIndicator.className = 'status-indicator disconnected';
        // Auto-Reconnect nach 2 Sekunden
        setTimeout(initWebsocket, 2000);
    };

    websocket.onmessage = (event) => {
        // Nachricht parsen
        const data = JSON.parse(event.data);

        // Je nach Typ reagieren
        switch (data.type) {
            case 'state':
                // Aktualisiert Sensorwerte
                handleWSStateMessage(data.values);
                break;

            case 'settings':
                // Aktualisiert Input-Felder UND Radio-Buttons (Modi)
                handleWSSettingsMessage(data.values);
                break;

            case 'newImage':
                // Ein neues Bild wurde aufgenommen -> Sofort anzeigen
                handleWSNewImageMessage(data.path);
                break;

            case 'imageListCleared':
                // Alle Bilder wurden gelöscht
                handleWSImageListClearedMessage();
                break;

            case 'log':
                // Ausgabe in der Browser-Konsole mit türkiser Farbe, damit es auffällt
                console.log(`%c[ESP32] ${data.message}`, 'color: #00d2d3; font-weight: bold;');
                break;

            default:
                console.log("Unbekannter Nachrichtentyp: ", data.type);
        }
    };
}

/**
 * @brief Wird aufgerufen, wenn der Server neue Live-Werte übermittelt hat.
 * @param {State} values Aktuelle Werte der Sensoren und Aktoren.
 */
function handleWSStateMessage(values) {
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
 * @brief Wird aufgerufen, wenn Server neue Einstellungen übermittelt hat.
 * @param {Settings} settings Aktuelle Einstellungen.
 */
function handleWSSettingsMessage(settings) {
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
 * @brief Wird aufgerufen, wenn der Server ein neues Bild übermittelt wird.
 * @param {string} path Pfad zum neuen Bild.
 */
function handleWSNewImageMessage(path) {
    console.log("Neues Bild empfangen:", path);

    // Bild vorne anfügen (da Liste "Neueste zuerst" sortiert ist)
    imagePaths.unshift(path);

    // Dropdown aktualisieren
    updateImageUI();

    // Sofort auf das neue Bild springen
    currentImageIndex = 0;
    showImageAtIndex(0);
}

/**
 * @brief Wird aufgerufen, wenn alle Bilder gelöscht wurden.
 */
function handleWSImageListClearedMessage() {
    imagePaths = [];
    updateImageUI();
    showPlaceholder();
}

// === Bildergalerie Logik ===

/**
 * @brief Baut das <select> Dropdown neu auf.
 */
function updateImageUI() {
    const select = document.getElementById('image-select');
    if (!select) return;

    select.innerHTML = '';

    if (imagePaths.length === 0) {
        return;
    }

    imagePaths.forEach(path => {
        const option = document.createElement('option');
        option.value = path;
        // Entferne führenden Slash für schönere Anzeige
        option.innerText = path.replace(/^\//, '');
        select.appendChild(option);
    });

    // Aktuelles Bild selektieren
    if (imagePaths[currentImageIndex]) {
        select.value = imagePaths[currentImageIndex];
    }
}

/**
 * @brief Zeigt das Bild am angegebenen Index an.
 * @param {number} index Index im imagePaths Array.
 */
function showImageAtIndex(index) {
    if (index < 0 || index >= imagePaths.length) return;

    const path = imagePaths[index];
    const imgEl = document.getElementById('current-image');
    const timeEl = document.getElementById('image-timestamp');
    const select = document.getElementById('image-select');

    if (imgEl) {
        // Timestamp anhängen, um Browser-Cache zu umgehen (falls Dateiname gleich bleibt, was hier nicht der Fall ist, aber sicher ist sicher)
        // imgEl.src = `/img?path=${path}&t=${new Date().getTime()}`;
        imgEl.src = `/img?path=${path}`;
    }

    if (timeEl) {
        timeEl.innerText = path.replace(/^\//, ''); // Dateiname als Zeitstempel anzeigen
    }

    if (select) {
        select.value = path;
    }
}

/**
 * @brief Zeigt einen Platzhalter, wenn keine Bilder da sind.
 */
function showPlaceholder() {
    const imgEl = document.getElementById('current-image');
    const timeEl = document.getElementById('image-timestamp');
    if (imgEl) imgEl.src = ''; // oder Pfad zu einem Platzhalter-Bild
    if (timeEl) timeEl.innerText = 'Keine Bilder auf der SD-Karte.';
}

/**
 * @brief Lädt die Liste der Bilder vom Server.
 */
function loadImages() {
    fetch('/api/images')
        .then(response => response.json())
        .then(images => {
            images.sort((a, b) => b.path.localeCompare(a.path)); // sortieren (neueste zuerst)
            imagePaths = images.map(img => img.path); // Pfade speichern
            updateImageUI();
            if (imagePaths.length > 0) {
                currentImageIndex = 0;
                showImageAtIndex(0); // erstes Bild anzeigen
            } else {
                showPlaceholder();
            }
        })
        .catch(err => {
            console.error("Fehler beim Laden der Bilder:", err);
            document.getElementById('image-timestamp').innerText = "Fehler beim Laden.";
        });
}


// === Zeitraffer Player ===

/**
 * @brief Startet oder pausiert den Zeitraffer.
 */
function toggleTimelapse() {
    if (isPlaying) {
        stopTimelapse();
    } else {
        if (imagePaths.length < 2) {
            alert("Mindestens 2 Bilder für Zeitraffer benötigt.");
            return;
        }
        startTimelapse();
    }
}

/**
 * @brief Startet das Intervall.
 */
function startTimelapse() {
    isPlaying = true;

    // Button Icon ändern
    const btn = document.getElementById('btnPlayPause');
    if (btn) btn.innerText = "⏸"; // Pause Symbol

    // Geschwindigkeit aus Slider lesen
    const speedInput = document.getElementById('timelapseSpeed');
    const speed = speedInput ? parseInt(speedInput.value) : 500;

    timelapseIntervalId = setInterval(() => {
        // Logik: Wir bewegen uns "rückwärts" durch das Array (Index 0 = Neu, Index Ende = Alt).
        // Für einen chronologischen Ablauf (Alt -> Neu) müssen wir den Index verringern.

        currentImageIndex--;

        // Loop: Wenn wir unter 0 fallen, fangen wir beim ältesten Bild (Ende des Arrays) wieder an.
        if (currentImageIndex < 0) {
            currentImageIndex = imagePaths.length - 1;
        }

        showImageAtIndex(currentImageIndex);

    }, speed);
}

/**
 * @brief Stoppt das Intervall.
 */
function stopTimelapse() {
    isPlaying = false;
    if (timelapseIntervalId) {
        clearInterval(timelapseIntervalId);
        timelapseIntervalId = null;
    }

    // Button Icon zurücksetzen
    const btn = document.getElementById('btnPlayPause');
    if (btn) btn.innerText = "▶"; // Play Symbol
}

/**
 * @brief Manuelles Blättern (Prev/Next).
 * @param {number} direction +1 (Richtung älter) oder -1 (Richtung neuer).
 */
function stepImage(direction) {
    stopTimelapse(); // Immer stoppen bei manueller Interaktion

    let newIndex = currentImageIndex + direction;

    // Wrap around Logik
    if (newIndex < 0) {
        newIndex = imagePaths.length - 1;
    } else if (newIndex >= imagePaths.length) {
        newIndex = 0;
    }

    currentImageIndex = newIndex;
    showImageAtIndex(currentImageIndex);
}

/**
 * @brief Wird aufgerufen, wenn ein Bild aus der Auswahlliste gewählt wurde.
 * @param {string} path Pfad des ausgewählten Bildes.
 */
function handleImageChanged(path) {
    const idx = imagePaths.indexOf(path);
    if (idx === -1) {
        return;
    }
    stopTimelapse();
    currentImageIndex = idx;
    showImageAtIndex(currentImageIndex);
}

// todo 3: im Frontend ist "Geschwindigkeit" und Angabe in ms für speedDisplay verwirrend. Gemeint sit ja die Anzeigedauer pro Bild.
// todo 4: Wenn die Anzeigedauer zu klein ist, werden nicht mehr alle Bilder angezeigt, sondern einige einfach übersprungen.
// todo 5: Konvention für den Namen überdenken: ok so oder handleTimelapseSpeedChange oder onTimelapseSpeedChange(d) oder ganz anders?
/**
 * @brief Wird aufgerufen, wenn die Geschwindigkeit für den Zeitraffer verändert wurde.
 * @param {number} ms Neue Geschwindigkeit.
 */
function handleTimelapseSpeedChanged(ms) {
    document.getElementById('speedDisplay').innerText = ms + 'ms';
    // Wenn der Player läuft, Geschwindigkeit live anpassen (Neustart)
    if (isPlaying) {
        stopTimelapse();
        startTimelapse();
    }
}

// todo 6: umwandeln zu sendMessage
/**
 * @brief Hilfsfunktion zum Senden von Befehlen via WebSocket.
 * @param {string} command Der Befehlsname (z.B. "toggle", "setMode").
 * @param {string} target Das Zielgerät (z.B. "lamp1").
 * @param {object} payload Zusätzliche Daten.
 */
function sendCommand(command, target, payload = {}) {
    if (websocket && websocket.readyState === WebSocket.OPEN) {
        const message = {
            type: "command",
            command: command,
            target: target,
            ...payload,
        };
        console.log("Sende:", message);
        websocket.send(JSON.stringify(message));
    } else {
        console.error("WebSocket nicht verbunden.");
        alert("Keine Verbindung zum Controller.");
    }
}

// todo 7: Beschreibung hinzufügen
function setMode(target, mode) {
    sendCommand("setMode", target, { mode: mode });
}

/**
 * @brief Sammelt Formulardaten und sendet sie an den ESP.
 */
function saveSettings() {
    const statusDiv = document.getElementById('settings-status');
    if(statusDiv) statusDiv.innerText = 'Speichere...';

    const payload = {};
    const form = document.getElementById('settingsForm');

    // Alle Inputs im Formular durchgehen
    Array.from(form.elements).forEach(input => {
        if (!input.id) return; // Inputs ohne ID ignorieren

        // Konvertierung Sekunde -> Millisekunde
        if (input.id.endsWith('DurationMs')) {
            payload[input.id] = parseInt(input.value) * 1000;
        }
        // Zahlen
        else if (input.type === 'number') {
            // Check auf Float (Step enthält Punkt) oder Int
            payload[input.id] = (input.step && input.step.includes('.')) ? parseFloat(input.value) : parseInt(input.value);
        }
        // Text / Sonstiges
        else {
            payload[input.id] = input.value;
        }
    });

    // Nachricht senden
    const message = {
        type: "saveSettings",
        payload: payload
    };
    websocket.send(JSON.stringify(message));

    // Feedback anzeigen
    setTimeout(() => { if(statusDiv) statusDiv.innerText = 'Gespeichert!'; }, 1500);
    setTimeout(() => { if(statusDiv) statusDiv.innerText = ''; }, 4000);
}

/**
 * @brief Sendet den Befehl "captureNow" an den ESP.
 */
function captureNow() {
    // todo: 8: Es fehlt ein Feedback für die UI, dass nun ein Bild aufgezeichnet wird. Momentan bleibt der Button unverändert, als wenn man nicht draufgeklickt hat)
    const message = {
        type: "captureNow",
    };
    console.log("captureNow");
    websocket.send(JSON.stringify(message));
}

/**
 * @brief Sendet den Befehl "deleteAllImages" mit Passwortschutz.
 */
function deleteAllImages() {
    // Sicherheitsabfrage
    const pwd = prompt("Admin-Passwort zum Löschen ALLER Bilder eingeben:");
    if (pwd) {
        stopTimelapse();
        // Nachricht als eigenen Typ aufbauen (wie saveSettings)
        const message = {
            type: "deleteAllImages",
            payload: {
                password: pwd
            }
        };

        if (websocket && websocket.readyState === WebSocket.OPEN) {
            console.log("Sende:", message);
            websocket.send(JSON.stringify(message));
        } else {
            console.error("WS nicht verbunden");
        }
    }
}

// todo 9: Funktionen sortieren: Erst init/onload-Funktion, dann die Handler (erst für Websocket, dann UI), dann die Hilfsfunktionen (die von den Händlern aufgerufen werden)