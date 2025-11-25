// Globale WebSocket-Variable
let websocket;

// Beim Laden der Seite initialisieren
window.addEventListener('load', () => {
    initWebSocket();
    // Standardmäßig den ersten Tab öffnen
    document.querySelector('.tablinks').click();
});

// WebSocket initialisieren
function initWebSocket() { /* ... (wie in deinem PoC) ... */ }

// WebSocket onMessage Handler
function onMessage(event) {
    const data = JSON.parse(event.data);

    // Je nach Nachrichtentyp unterschiedliche Dinge tun
    if (data.type === 'state') {
        updateDashboard(data.values);
    } else if (data.type === 'config') {
        updateConfigForm(data.values);
    } else if (data.type === 'filelist') {
        updateFileList(data.files);
    }
}

// Tab-Funktionalität

function openTab(evt, tabName) {
    var i, tabcontent, tablinks;
    tabcontent = document.getElementsByClassName("tabcontent");
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }
    tablinks = document.getElementsByClassName("tablinks");
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
    }
    document.getElementById(tabName).style.display = "block";
    evt.currentTarget.className += " active";
}

// Funktionen zum Aktualisieren der UI
function updateDashboard(values) { /* ... */ }
// Füllt das Konfigurations-Formular mit den Werten vom ESP32

function updateConfigForm(config) {
    for (const key in config) {
        const element = document.getElementById(key);
        if (element) {
            element.value = config[key];
        }
    }
}

function updateFileList(files) { /* ... */ }

// Funktionen zum Senden von Befehlen an den ESP32
function toggleRelay(name) {
    websocket.send(JSON.stringify({command: 'toggleRelay', relay: name}));
}

// NEU: Liest die Werte aus dem Formular, verpackt sie in ein JSON und sendet sie
function saveConfig() {
    const statusDiv = document.getElementById('configStatus');
    statusDiv.innerText = 'Speichere...';

    // Ein Objekt, das die Struktur von ConfigData widerspiegelt
    const payload = {
        soilTempTarget: parseFloat(document.getElementById('soilTempTarget').value),
        soilMoistureTarget: parseInt(document.getElementById('soilMoistureTarget').value),
        airTempThresholdHigh: parseFloat(document.getElementById('airTempThresholdHigh').value),
        humidityTarget: parseFloat(document.getElementById('humidityTarget').value),
        lightOnHour: parseInt(document.getElementById('lightOnHour').value),
        lightOffHour: parseInt(document.getElementById('lightOffHour').value),
        lightLuxThresholdDark: parseFloat(document.getElementById('lightLuxThresholdDark').value),
        lightLuxThresholdBright: parseFloat(document.getElementById('lightLuxThresholdBright').value),
        wateringDurationMs: parseInt(document.getElementById('wateringDurationMs').value),
        fanCooldownDurationMs: parseInt(document.getElementById('fanCooldownDurationMs').value)
    };

    // Erstelle das finale Befehls-Objekt
    const command = {
        command: 'saveConfig',
        payload: payload
    };

    websocket.send(JSON.stringify(command));

    // Kurzes Feedback. Eine echte Bestätigung kommt vom Server.
    setTimeout(() => { statusDiv.innerText = 'Gespeichert!'; }, 1000);
    setTimeout(() => { statusDiv.innerText = ''; }, 3000);
}

// ... (weitere Funktionen für Dateimanagement) ...