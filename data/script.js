const gateway = `ws://${window.location.hostname}/ws`;
let websocket;

// Nach dem Laden der Seite die WebSocket-Verbindung aufbauen
window.addEventListener('load', () => {
    initWebSocket();
});

function initWebSocket() {
    console.log('Versuche, WebSocket zu öffnen...');
    websocket = new WebSocket(gateway);
    websocket.onopen = (event) => console.log('Verbindung hergestellt.');
    websocket.onclose = (event) => {
        console.log('Verbindung getrennt.');
        setTimeout(initWebSocket, 2000); // Erneut versuchen nach 2 Sek.
    };
    websocket.onmessage = onMessage;
}

// Funktion zum Senden einer Nachricht an den ESP32
function sendMessage(message) {
    console.log("Sende: " + message);
    websocket.send(message);
}

// Funktion, die aufgerufen wird, wenn eine Nachricht vom ESP32 empfangen wird
function onMessage(event) {
    console.log("Empfangen: " + event.data);
    const stateElement = document.getElementById('ledState');
    if (event.data === "STATE_ON") {
        stateElement.innerHTML = "EIN";
        stateElement.className = "led-state led-on";
    } else if (event.data === "STATE_OFF") {
        stateElement.innerHTML = "AUS";
        stateElement.className = "led-state led-off";
    }
}