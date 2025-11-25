#include "WebInterface.h"
#include <ArduinoJson.h>

WebInterface::WebInterface(uint16_t port)
    : _server(port), _ws("/ws") {}

bool WebInterface::begin(FS* fs, FS* sd) {
    _fs = fs;
    _sd = sd;

    if (!_fs) {
        Serial.println("WebInterface-Fehler: Kein Dateisystem für Web-Dateien übergeben.");
        return false;
    }

    _ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        this->onWsEvent(server, client, type, arg, data, len);
    });
    _server.addHandler(&_ws);

    registerRoutes();

    _server.begin();
    Serial.println("WebInterface-Dienst bereit.");
    return true;
}

void WebInterface::registerRoutes() {
    // Statische Dateien (index.html, style.css, etc.) ausliefern
    _server.serveStatic("/", *_fs, "/").setDefaultFile("index.html");

    // Handler für Datei-Uploads
    _server.on("/upload", HTTP_POST,
        [](AsyncWebServerRequest *request) {
            request->send(200, "text/plain", "OK");
        },
        [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            this->handleUpload(request, filename, index, data, len, final);
        }
    );

    // Handler zum Auflisten von Dateien (leitet an Callback weiter)
    _server.on("/files", HTTP_GET, [this](AsyncWebServerRequest* request) {
        if (onFileListRequest) {
            onFileListRequest(request);
        } else {
            request->send(500, "text/plain", "File list handler nicht registriert.");
        }
    });

    // Handler zum Herunterladen von Dateien
    _server.on("/download", HTTP_GET, [this](AsyncWebServerRequest* request) {
        if (request->hasParam("file")) {
            String path = request->getParam("file")->value();
            FS* fileSystem = path.startsWith("/sd/") && _sd ? _sd : _fs;
            String cleanPath = path.startsWith("/sd/") ? path.substring(3) : path;

            if (fileSystem->exists(cleanPath)) {
                request->send(*fileSystem, cleanPath, "application/octet-stream");
            } else {
                request->send(404, "text/plain", "Datei nicht gefunden.");
            }
        } else {
            request->send(400, "text/plain", "Fehlender 'file'-Parameter.");
        }
    });

    // Handler zur Anzeige von Bildern (von SD-Karte)
    _server.on("/img", HTTP_GET, [this](AsyncWebServerRequest* request) {
        if (request->hasParam("img") && _sd) {
            String path = request->getParam("img")->value();
            if (_sd->exists(path)) {
                request->send(*_sd, path, "image/jpeg");
            } else {
                request->send(404, "text/plain", "Bild nicht gefunden.");
            }
        } else {
            request->send(400, "text/plain", "Fehlender 'img'-Parameter oder keine SD-Karte.");
        }
    });

    // Fallback-Handler
    _server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Seite nicht gefunden: " + request->url());
    });
}

void WebInterface::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("WebSocket Client #%u verbunden.\n", client->id());
        // Sende dem neuen Client sofort den letzten bekannten Status
        client->text(_lastStateJson);
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("WebSocket Client #%u getrennt.\n", client->id());
    } else if (type == WS_EVT_DATA) {
        // Leite die empfangene Nachricht an den Callback in main.cpp weiter
        if (onWsMessage) {
            String msg = "";
            for (size_t i = 0; i < len; i++) msg += (char)data[i];
            onWsMessage(client, msg);
        }
    }
}

void WebInterface::broadcast(const String& message) {
    _lastStateJson = message; // Cache den aktuellen Status
    _ws.textAll(message);
}

void WebInterface::cleanupClients() {
    _ws.cleanupClients();
}

void WebInterface::handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    String path;
    FS* fileSystem = _fs; // Standardmäßig LittleFS

    // Prüfe, ob in ein Unterverzeichnis der SD-Karte hochgeladen werden soll
    if (filename.startsWith("sd/")) {
        if (!_sd) {
            Serial.println("Upload-Fehler: Kein SD-Karten-Dateisystem verfügbar.");
            return;
        }
        fileSystem = _sd;
        path = filename.substring(3); // "sd/" entfernen
    } else {
        path = "/" + filename;
    }

    if (index == 0) {
        Serial.printf("Upload gestartet: %s nach %s\n", filename.c_str(), fileSystem == _sd ? "SD" : "LittleFS");
        request->_tempFile = fileSystem->open(path, "w");
    }

    if (len) {
        request->_tempFile.write(data, len);
    }

    if (final) {
        Serial.printf("Upload abgeschlossen: %s, %u Bytes\n", filename.c_str(), index + len);
        request->_tempFile.close();
    }
}
