#include "WebInterface.h"
#include <LittleFS.h>

WebInterface::WebInterface(uint16_t port)
    : _server(port), _ws("/ws") {}

bool WebInterface::begin() {
    // WebSocket-Events an unsere interne Handler-Funktion binden
    _ws.onEvent([this](AsyncWebSocket *server, const AsyncWebSocketClient *client, AwsEventType type, void *arg, const uint8_t *data, size_t len) {
        this->onWsEvent(server, client, type, arg, data, len);
    });
    _server.addHandler(&_ws);

    // --- HTTP-Routen für die Web-Dateien ---
    // Handler für die Hauptseite
    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });

    // Handler für die CSS-Datei
    _server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/style.css", "text/css");
    });

    // Handler für die JavaScript-Datei
    _server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/script.js", "application/javascript");
    });

    // Handler für nicht gefundene Seiten
    _server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Seite nicht gefunden: " + request->url());
    });

    // Webserver starten
    _server.begin();
    Serial.println("Webserver gestartet.");
    return true;
}

void WebInterface::broadcast(const String& message) {
    _ws.textAll(message);
}

void WebInterface::onWsEvent(AsyncWebSocket *server, const AsyncWebSocketClient *client, AwsEventType type, void *arg, const uint8_t *data, size_t len) const {
    if (type == WS_EVT_CONNECT) {
        // Client hat sich verbunden
        Serial.printf("WebSocket Client #%u verbunden von %s\n", client->id(), client->remoteIP().toString().c_str());
    } else if (type == WS_EVT_DISCONNECT) {
        // Client hat die Verbindung getrennt
        Serial.printf("WebSocket Client #%u getrennt\n", client->id());
    } else if (type == WS_EVT_DATA) {
        // Daten vom Client empfangen
        // Wir wandeln die empfangenen Bytes in einen String um, um sie zu vergleichen.
        String msg = "";
        for (size_t i = 0; i < len; i++) {
            msg += static_cast<char>(data[i]);
        }
        Serial.printf("Nachricht von Client #%u empfangen: %s\n", client->id(), msg.c_str());

        // Wenn ein Callback registriert ist, rufe ihn mit der Nachricht auf.
        if (onMessage) {
            onMessage(msg);
        }
    }
}
