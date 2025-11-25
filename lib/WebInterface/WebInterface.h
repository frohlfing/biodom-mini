#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <functional> // Für std::function (Callbacks)
#include <FS.h>

// Vorwärtsdeklaration, um ArduinoJson.h nicht im Header einbinden zu müssen.
class JsonDocument;

/**
 * @class WebInterface
 * @brief Eine generische, wiederverwendbare Klasse für ein Webinterface auf dem ESP32.
 *
 * Stellt einen asynchronen Webserver bereit, der eine Single-Page-Application (SPA)
 * aus einem Dateisystem ausliefert und über WebSockets kommuniziert.
 * Bietet Handler für Datei-Uploads, -Downloads und das Auflisten von Verzeichnissen.
 * Die Anwendungslogik wird über Callbacks an das Hauptprogramm angebunden.
 */
class WebInterface {
public:
    /**
     * @brief Konstruktor.
     * @param port Der Port, auf dem der Webserver lauschen soll (Standard 80).
     */
    WebInterface(uint16_t port = 80);

    /**
     * @brief Initialisiert den Webserver und registriert alle Routen.
     * @param fs Ein Pointer auf das Dateisystem für die Webseiten-Dateien (z.B. &LittleFS).
     * @param sd Ein Pointer auf das SD-Karten-Dateisystem (optional, für SD-Funktionen).
     * @return true bei Erfolg, andernfalls false.
     */
    bool begin(FS* fs, FS* sd = nullptr);

    /**
     * @brief Sendet eine Nachricht an alle verbundenen WebSocket-Clients.
     * @param message Der zu sendende String (sollte typischerweise JSON sein).
     */
    void broadcast(const String& message);

    /**
     * @brief Bereinigt die Liste der WebSocket-Clients. Sollte periodisch aufgerufen werden.
     */
    void cleanupClients();

    // --- Callbacks für die Anwendungslogik in main.cpp ---

    /**
     * @brief Callback, der aufgerufen wird, wenn eine WebSocket-Nachricht empfangen wird.
     * Format: (Client-Pointer, Nachricht als String)
     */
    std::function<void(AsyncWebSocketClient*, const String&)> onWsMessage;

    /**
     * @brief Callback für Anfragen zum Auflisten von Dateien (z.B. /files/sd/).
     * Format: (HTTP-Request-Pointer)
     */
    std::function<void(AsyncWebServerRequest*)> onFileListRequest;

private:
    /**
     * @brief Registriert alle HTTP-Routen des Servers.
     */
    void registerRoutes();

    /**
     * @brief Interner Handler, der WebSocket-Events verarbeitet.
     */
    void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

    /**
     * @brief Interner Handler, der Datei-Uploads verarbeitet.
     */
    void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

    // Member-Variablen
    FS* _fs = nullptr;              // Pointer auf das Haupt-Dateisystem (LittleFS).
    FS* _sd = nullptr;              // Pointer auf das SD-Karten-Dateisystem.
    AsyncWebServer _server;         // Die Instanz des Webservers.
    AsyncWebSocket _ws;             // Die Instanz des WebSocket-Servers am Endpunkt "/ws".
    String _lastStateJson = "{}";   // Cache für den letzten Systemstatus, um neue Clients sofort zu informieren.
};