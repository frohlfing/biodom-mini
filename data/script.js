"use strict";

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

// === Globale Variablen ===

/** @type {WebSocket} Die WebSocket-Verbindung zum ESP32 */
let websocket = null;

/** @type {string[]} Liste der Bildpfade (z.B. ["/img_2025...jpg", ...]) */
let imagePaths = [];

/** @type {number} Der Index des aktuell angezeigten Bildes im imagePaths Array */
let currentImageIndex = -1; // -1 == keine Bilder geladen

/** @type {number|null} ID des Zeitraffer-Timers (für clearTimeout) */
let timelapseTimerId = null;

/** @type {boolean} Status, ob der Zeitraffer gerade läuft */
let isPlaying = false;

/** @type {number|null} Timer-ID für das Warten auf eine Kamera-Antwort. */
let captureTimeoutId = null;

// === Initialisierung ===

/**
 * Wird beim Laden der Seite ausgeführt.
 */
window.addEventListener('load', () => {
    initWebsocket();

    // Event-Listener für Buttons, Inputs und Tabs registrieren

    // --- Tab Navigation ---
    document.querySelectorAll('.tab-link').forEach(button => {
        button.addEventListener('click', handleTabLinkClick);
    });

    // --- Einstellungen ---
    document.getElementById('saveSettingsButton').addEventListener('click', handleSaveSettingsClick);

    // Radio-Buttons (Modus ändern)
    document.querySelectorAll('.mode-selector').forEach(selector => {
        selector.addEventListener('change', handleModeSelectorChange);
    });

    // --- Kamera ---
    document.getElementById('captureNowButton').addEventListener('click', handleCaptureNowButtonClick);
    document.getElementById('deleteImagesButton').addEventListener('click', handleDeleteImagesButtonClick);
    document.getElementById('btnPlayPause').addEventListener('click', handlePlayPauseButtonClick);
    document.getElementById('btnPrev').addEventListener('click', handlePrevButtonClick); // 1 Schritt zurück (älter)
    document.getElementById('btnNext').addEventListener('click', handleNextButtonClick); // 1 Schritt vor (neuer)
    document.getElementById('image-select').addEventListener('change', handleImageSelectChange);
    document.getElementById('timelapseSpeed').addEventListener('input', handleTimelapseSpeedInput);
});

// === Ereignishändler ===

/**
 * Wird aufgerufen, wenn ein Kartenreiter angeklickt wird.
 * @param {Event} event Das Event-Objekt.
 */
function handleTabLinkClick(event) {
    const tabName = event.currentTarget.dataset.tabname; // z.B. "Dashboard"

    // Alle Tab-Inhalte ausblenden
    document.querySelectorAll('.tab-content')
        .forEach(div => div.classList.remove('show'));

    // "active" Klasse von allen Buttons entfernen
    document.querySelectorAll('.tab-link')
        .forEach(button => button.classList.remove('active'));

    // Gewählten Tab anzeigen
    document.getElementById(tabName).classList.add('show');

    // Button aktivieren
    event.currentTarget.className += " active";

    // Bilder anfordern, wenn Kamera-Tab ausgewählt wurde und noch keine Bilder geladen wurden
    if (tabName === 'Camera' && imagePaths.length === 0) {
        console.log("Fordere Bildliste vom Server an...");
        sendMessage("getImageList");
    }
}

// === Hilfsfunktionen ===

/**
 * Wird aufgerufen, wenn auf den Speichern-Button geklickt wurde.
 * @param {Event} _event Das Event-Objekt.
 */
function handleSaveSettingsClick(_event) {
    const statusDiv = document.getElementById('settings-status');
    statusDiv.innerText = 'Speichere...';

    const payload = {};
    const form = document.getElementById('settingsForm');

    // Alle Inputs im Formular durchgehen
    Array.from(form.elements).forEach(input => {
        if (!input.id) return; // Inputs ohne ID ignorieren

        // Konvertierung Sekunde -> Millisekunde
        if (input.id.endsWith('DurationMs')) {
            payload[input.id] = parseInt(input.value, 10) * 1000;
        }
        // Zahlen
        else if (input.type === 'number') {
            // Check auf Float (Step enthält Punkt) oder Int
            payload[input.id] = (input.step && input.step.includes('.')) ? parseFloat(input.value) : parseInt(input.value, 10);
        }
        // Text / Sonstiges
        else {
            payload[input.id] = input.value;
        }
    });

    // Nachricht senden
    sendMessage("saveSettings", payload);

    // Feedback anzeigen
    setTimeout(() => { statusDiv.innerText = 'Gespeichert!'; }, 1500);
    setTimeout(() => { statusDiv.innerText = ''; }, 4000);
}

/**
 * Wird aufgerufen, wenn der Steuermodus eines Aktors (an/aus/auto) geändert wird.
 * @param {Event} event Das Event-Objekt.
 */
function handleModeSelectorChange(event) {
    sendMessage("setMode", {
        target: event.target.name,
        mode: event.target.value,
    });
}

/**
 * Wird aufgerufen, wenn der Button 'Bild jetzt aufnehmen' geklickt wird.
 * @param {Event} _event Das Event-Objekt.
 */
function handleCaptureNowButtonClick(_event) {
    const button = document.getElementById('captureNowButton');
    button.disabled = true;
    button.innerText = 'Aufnahme...'; // todo 1: Wie beim Speichern-button den Text in grüner Schrift daneben schreiben

    // Setze einen Timeout für den Fall, dass der Server nicht antwortet
    captureTimeoutId = setTimeout(() => {
        console.error("Kamera-Aufnahme: Timeout. Keine Antwort vom Server.");
        alert("Fehler: Keine Antwort von der Kamera. Bitte erneut versuchen.");
        resetCaptureButton();
        captureTimeoutId = null;
    }, 15000); // 15 Sekunden warten

    sendMessage("captureNow");
}

/**
 * Wird aufgerufen, wenn der Button 'Alle Bilder löschen' geklickt wird. Fordert ein Passwort an und sendet den Löschbefehl.
 * @param {Event} _event Das Event-Objekt.
 */
function handleDeleteImagesButtonClick(_event) {
    const pwd = prompt("Admin-Passwort zum Löschen ALLER Bilder eingeben:");
    if (!pwd) {
        return;
    }
    stopTimelapse();
    console.log("Sende deleteAllImages");
    sendMessage("deleteAllImages", {password: pwd});
}

/**
 * Startet oder stoppt den Zeitraffer-Player.
 * @param {Event} _event Das Event-Objekt.
 */
function handlePlayPauseButtonClick(_event) {
    // Zeitraffervideo pausieren bzw. fortsetzen
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
 * Zeigt das vorherige (ältere) Bild in der Galerie an.
 * @param {Event} _event Das Event-Objekt.
 */
// 1 Schritt zurück (älter)
function handlePrevButtonClick(_event) {
    stopTimelapse();
    showImageAtIndex(currentImageIndex >= imagePaths.length - 1 ? 0 : currentImageIndex + 1);
}

/**
 * Zeigt das nächste (neuere) Bild in der Galerie an.
 * @param {Event} _event Das Event-Objekt.
 */
// 1 Schritt vor (neuer)
function handleNextButtonClick(_event) {
    stopTimelapse();
    showImageAtIndex(currentImageIndex <= 0 ? imagePaths.length - 1 : currentImageIndex - 1);
}

/**
 * Wird aufgerufen, wenn ein Bild aus der Auswahlliste gewählt wurde.
 * @param {Event} event Das Event-Objekt.
 */
function handleImageSelectChange(event) {
    const path = event.target.value // Pfad des ausgewählten Bildes.
    const idx = imagePaths.indexOf(path);
    if (idx === -1) {
        return;
    }
    stopTimelapse();
    showImageAtIndex(idx);
}

/**
 * Wird aufgerufen, wenn die Geschwindigkeit für den Zeitraffer verändert wurde.
 * @param {Event} event Das Event-Objekt.
 */
function handleTimelapseSpeedInput(event) {
    const ms = parseInt(event.target.value, 10);
    const display = document.getElementById('speedDisplay');
    if (ms >= 1000) {
        display.innerText = (ms / 1000).toFixed(1) + ' s';
    } else {
        display.innerText = ms + ' ms';
    }
    // Wenn der Player läuft, Geschwindigkeit live anpassen (Neustart)
    if (isPlaying) {
        stopTimelapse();
        startTimelapse();
    }
}

// === Websocket Kommunikation ===

/**
 * Baut die WebSocket-Verbindung auf und definiert Handler.
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
                if (data.payload) {
                    handleWSStateMessage(data.payload);
                }
                break;

            case 'settings':
                // Aktualisiert Input-Felder UND Radio-Buttons (Modi)
                if (data.payload) {
                    handleWSSettingsMessage(data.payload);
                }
                break;

            case 'newImage':
                // Ein neues Bild wurde aufgenommen -> Sofort anzeigen
                if (data.payload && data.payload.path) {
                    handleWSNewImageMessage(data.payload.path);
                }
                break;

            case 'captureFailed':
                // Die Aufnahme ist auf dem ESP32 fehlgeschlagen
                handleWSCaptureFailedMessage();
                break;

            case 'imageList':
                // Server sendet die Liste der Bilder
                if (data.payload && data.payload.images) {
                    handleWSImageListMessage(data.payload.images);
                }
                break;

            case 'imageListCleared':
                // Alle Bilder wurden gelöscht
                handleWSImageListClearedMessage();
                break;

            case 'log':
                if (data.payload && data.payload.message) {
                    // Ausgabe in der Browser-Konsole mit türkiser Farbe, damit es auffällt
                    console.log(`%c[ESP32] ${data.payload.message}`, 'color: #00d2d3; font-weight: bold;');
                }
                break;

            default:
                console.log("Unbekannter Nachrichtentyp: ", data.type);
        }
    };
}

/**
 * Wird aufgerufen, wenn der Server neue Live-Werte übermittelt hat.
 * @param {object} payload Aktuelle Werte der Sensoren und Aktoren.
 */
function handleWSStateMessage(payload) {
    const values = /** @type {State} */ payload;
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
 * Wird aufgerufen, wenn Server neue Einstellungen übermittelt hat.
 * @param {object} payload Aktuelle Einstellungen.
 */
function handleWSSettingsMessage(payload) {
    const settings = /** @type {Settings} */ payload;
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
 * Wird aufgerufen, wenn der Server ein neues Bild übermittelt wird.
 * @param {string} path Pfad zum neuen Bild.
 */
function handleWSNewImageMessage(path) {
    console.log("Neues Bild empfangen:", path);

    resetCaptureButton(); // Button zurücksetzen

    // Das Bild im internen Array an die erste Stelle setzen
    imagePaths.unshift(path);

    // Das neue Element oben einfügen
    const select = document.getElementById('image-select');
    if (select) {
        const option = document.createElement('option');
        option.value = path;
        option.innerText = path.replace(/^\//, ''); // Führenden Slash entfernen

        // prepend fügt das Element als erstes Kind ein (Index 0)
        // Das ist viel performanter als innerHTML = '' und alles neu zu bauen.
        select.prepend(option);

        // Das Dropdown visuell auf das neue Element setzen
        select.selectedIndex = 0;
    }

    // Das neue Bild anzeigen
    currentImageIndex = 0;
    showImageAtIndex(0);
}

/**
 * Wird aufgerufen, wenn die Kameraaufnahme fehlgeschlagen ist.
 */
function handleWSCaptureFailedMessage() {
    console.error("Server meldet: Kamera-Aufnahme fehlgeschlagen.");
    alert("Die Kameraaufnahme ist fehlgeschlagen. Bitte überprüfen Sie das System.");
    resetCaptureButton();
}

/**
 * Wird aufgerufen, wenn der Server die Liste der Bilder sendet.
 * @param {ImageFile[]} images Eine Liste von Bild-Objekten.
 */
function handleWSImageListMessage(images) {
    // Neueste zuerst sortieren, falls nicht schon vom Server erledigt
    images.sort((a, b) => b.path.localeCompare(a.path));
    imagePaths = images.map(img => img.path); // Nur die Pfade speichern

    const select = document.getElementById('image-select');
    select.innerHTML = ''; // Alte Einträge löschen

    if (imagePaths.length) {
        imagePaths.forEach(path => {
            const option = document.createElement('option');
            option.value = path;
            option.innerText = path.replace(/^\//, ''); // Führenden Slash entfernen
            select.appendChild(option);
        });
    }
    showImageAtIndex(imagePaths.length > 0 ? 0 : -1); // Erstes Bild anzeigen
}

/**
 * Wird aufgerufen, wenn alle Bilder gelöscht wurden.
 */
function handleWSImageListClearedMessage() {
    imagePaths = [];
    document.getElementById('image-select').innerHTML = '';
    document.getElementById('current-image').src = '';
    document.getElementById('image-timestamp').innerText = '';
}

// === Hilfsfunktionen ===

/**
 * Sendet eine formatierte Nachricht an den WebSocket.
 * @param {string} type Der Nachrichtentyp.
 * @param {object} payload Das Datenobjekt (optional).
 */
function sendMessage(type, payload = {}) {
    if (!websocket || websocket.readyState !== WebSocket.OPEN) {
        console.error("WS nicht verbunden");
        return;
    }
    const message = {
        type: type,
        payload: payload
    };
    console.log("Sende:", message);
    websocket.send(JSON.stringify(message));
}

/**
 * Zeigt das Bild am angegebenen Index an.
 * @param {number} index Index im imagePaths Array.
 * @param {function(Error|null): void} [onDone] Callback, der nach dem Laden aufgerufen wird.
 */
function showImageAtIndex(index, onDone) {
    currentImageIndex = index;
    const currentImage = document.getElementById('current-image');
    const imageTimestamp = document.getElementById('image-timestamp');
    const select = document.getElementById('image-select');

    currentImage.onload = () => {
        if (onDone) {
            onDone(null);
        }
    };
    currentImage.onerror = () => {
        const err = new Error(`Bild konnte nicht geladen werden: ${path}`);
        console.error(err);
        if (onDone) {
            onDone(err);
        }
    };

    if (index < 0 || index >= imagePaths.length) {
        select.value = null;
        currentImage.src = '';
        imageTimestamp.innerText = '';
        if (onDone) onDone(null); // auch hier den Callback ausführen, um die Kette nicht zu unterbrechen
        return;
    }

    const path = imagePaths[index];
    currentImage.src = `/img?path=${path}`;
    //currentImage.src = `/img?path=${path}&t=${new Date().getTime()}`; // Timestamp anhängen, um Browser-Cache zu umgehen.
    imageTimestamp.innerText = path.replace(/^\//, ''); // Dateiname als Zeitstempel anzeigen
    select.value = path;
}

/**
 * Setzt den "Bild aufnehmen"-Button in seinen Ausgangszustand zurück.
 */
function resetCaptureButton() {
    // Timeout abbrechen, falls er noch läuft
    if (captureTimeoutId) {
        clearTimeout(captureTimeoutId);
        captureTimeoutId = null;
    }
    const button = document.getElementById('captureNowButton');
    button.disabled = false;
    button.innerText = 'Bild jetzt aufnehmen';
}

/**
 * Startet das Intervall.
 */
function startTimelapse() {
    isPlaying = true;
    document.getElementById('btnPlayPause').innerText = "⏸"; // Pause Symbol
    playNextFrame(); // Starte die Schleife
}

/**
 * Stoppt das Intervall.
 */
function stopTimelapse() {
    isPlaying = false;
    if (timelapseTimerId) {
        clearTimeout(timelapseTimerId);
        timelapseTimerId = null;
    }
    document.getElementById('btnPlayPause').innerText = "▶"; // Play Symbol
}

/**
 * Spielt das nächste Bild im Zeitraffer ab und plant den nächsten Aufruf.
 */
function playNextFrame() {
    if (!isPlaying) return; // Stopp, wenn der Player pausiert wurde

    // Nächstes Bild anzeigen (Schleife von neu nach alt)
    const nextIndex = currentImageIndex <= 0 ? imagePaths.length - 1 : currentImageIndex - 1;

    // Zeige das nächste Bild an. Der Code im Callback wird ausgeführt, SOBALD das Bild geladen ist.
    showImageAtIndex(nextIndex, (error) => {
        if (error) { // Wenn ein Bild nicht geladen werden kann...
            console.warn("Bild übersprungen:", error.message);
            playNextFrame(); // ...einfach sofort das nächste versuchen.
        }
        // Plane den nächsten Frame, NACHDEM das aktuelle Bild geladen wurde
        const speed = parseInt(document.getElementById('timelapseSpeed').value, 10);
        timelapseTimerId = setTimeout(playNextFrame, speed);
    });
}