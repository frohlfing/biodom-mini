#include "WebUI.h"

WebUI::WebUI(uint16_t port)
    : _server(port), _ws("/ws") {}

bool WebUI::begin(FS* fs, FS* sd) {
    _fs = fs;
    _sd = sd;

    if (!_fs) {
        Serial.println("WebUI FEHLER: Kein Dateisystem für Web-Dateien übergeben.");
        return false;
    }

    // WebSocket-Events an unsere interne Handler-Funktion binden
    _ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, const uint8_t *data, size_t len) {
        this->onWsEvent(server, client, type, arg, data, len);
    });
    _server.addHandler(&_ws);

    // Routen registrieren
    registerRoutes();

    // Webserver starten
    _server.begin();
    Serial.println("Webserver gestartet.");
    return true;
}

void WebUI::registerRoutes() {
    // --- HTTP-Routen für die Web-Dateien ---
    // // Handler für die Hauptseite
    // _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    //     request->send(LittleFS, "/index.html", "text/html");
    // });
    //
    // // Handler für die CSS-Datei
    // _server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    //     request->send(LittleFS, "/style.css", "text/css");
    // });
    //
    // // Handler für die JavaScript-Datei
    // _server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    //     request->send(LittleFS, "/script.js", "application/javascript");
    // });

    // Registriere einen Handler, der ALLE statischen Dateien aus dem LittleFS ausliefert.
    // Dieser eine Befehl kümmert sich um "/", "/style.css", "/script.js" UND alle Anfragen an "/icons/...".
    _server.serveStatic("/", *_fs, "/").setDefaultFile("index.html");

    // Handler zum Auflisten der Bilder auf der SD-Karte
    _server.on("/api/images", HTTP_GET, [this](AsyncWebServerRequest* request) {
        if (onImageListRequest) {
            onImageListRequest(request);
        } else {
            request->send(500, "text/plain", "Image list handler nicht registriert.");
        }
    });

    // Handler zur Anzeige eines einzelnen Bildes von der SD-Karte
    _server.on("/img", HTTP_GET, [this](AsyncWebServerRequest* request) {
        if (request->hasParam("path") && _sd) {
            String path = request->getParam("path")->value();
            if (_sd->exists(path)) {
                request->send(*_sd, path, "image/jpeg");
            } else {
                request->send(404, "text/plain", "Bild nicht gefunden.");
            }
        } else {
            request->send(400, "text/plain", "Fehlender 'path'-Parameter oder keine SD-Karte.");
        }
    });

    // Handler für nicht gefundene Seiten
    _server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Seite nicht gefunden: " + request->url());
    });
}

void WebUI::broadcast(const String& message) {
    _ws.textAll(message);
}

void WebUI::cleanupClients() {
    _ws.cleanupClients();
}

//void WebUI::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, const uint8_t *data, size_t len) const {
void WebUI::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, const uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        // Client hat sich verbunden
        Serial.printf("WebSocket Client #%u verbunden von %s\n", client->id(), client->remoteIP().toString().c_str());
        // Wenn ein Callback für die Settings existiert, rufe ihn auf
        if (onClientConnect) {
            onClientConnect(client);
        }
    } else if (type == WS_EVT_DISCONNECT) {
        // Client hat die Verbindung getrennt
        Serial.printf("WebSocket Client #%u getrennt\n", client->id());
        if (onClientDisconnect) {
            onClientDisconnect(client->id());
        }
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
            onMessage(client, msg);
        }
    }
}
