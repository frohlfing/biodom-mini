#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <functional> // Notwendig für Callbacks
#include <FS.h> // Notwendig für den FS-Pointer

/**
 * Stellt ein Webinterface zur Steuerung via WebSocket bereit.
 *
 * Kapselt die Logik für den asynchronen Web- und WebSocket-Server.
 * Liefert statische Dateien (HTML, CSS, JS) aus dem LittleFS und
 * meldet empfangene WebSocket-Nachrichten über einen Callback an die Hauptanwendung.
 */
class WebUI {
public:
    /**
     * @brief Konstruktor.
     * @param port Der Port, auf dem der Webserver lauschen soll (Standard 80).
     */
    explicit WebUI(uint16_t port = 80);

    /**
     * @brief Initialisiert den Server und registriert die Routen.
     * @param fs Ein Pointer auf das Dateisystem für die Webseiten-Dateien (z.B. &LittleFS).
     * @param sd Ein Pointer auf das SD-Karten-Dateisystem (optional, für SD-Funktionen).
     * @return true bei Erfolg.
     */
    bool begin(FS* fs, FS* sd = nullptr);

    /**
     * @brief Sendet eine Nachricht an alle verbundenen WebSocket-Clients (Broadcast).
     * @param message Der zu sendende String.
     */
    void broadcast(const String& message);

    /**
     * @brief Diese Funktion entfernt "tote" Clients aus der internen Liste des Servers.
     * Sie sollte regelmäßig in der Haupt-loop() aufgerufen werden, um Speicherlecks zu vermeiden.
     */
    void cleanupClients();

    /**
     * @property onMessage
     * @brief Callback, der aufgerufen wird, wenn eine Nachricht über WebSocket empfangen wird.
     * Die Hauptanwendung (main.cpp) muss diesen Callback definieren, um auf Befehle vom Browser zu reagieren.
     * Format: (Zeiger auf den Client, Nachricht als String)
     */
    std::function<void(AsyncWebSocketClient* client, const String& msg)> onMessage;

    /**
         * @property onClientConnect
         * @brief Callback, der aufgerufen wird, wenn sich ein neuer WebSocket-Client verbindet.
         * Nützlich, um dem neuen Client den initialen Systemstatus zu senden.
         * Format: (Zeiger auf den neuen Client)
         */
    std::function<void(AsyncWebSocketClient* client)> onClientConnect;

    /**
     * @property onClientDisconnect
     * @brief Callback, der aufgerufen wird, wenn ein Client die Verbindung trennt.
     * Format: (ID des getrennten Clients)
     */
    std::function<void(uint32_t clientId)> onClientDisconnect;

    /**
     * @brief Callback, der aufgerufen wird, wenn die Bildliste von der SD-Karte angefordert wird.
     */
    std::function<void(AsyncWebServerRequest* request)> onImageListRequest;

    // Dieser Callback wird von onMessage ausgelöst, wenn ein "captureNow" Befehl kommt
    /**
     * @brief Callback, der aufgerufen wird, um eine sofortige Kameraaufnahme auszulösen.
     */
    //std::function<void()> onCameraCaptureRequest;
private:
    /**
     * @brief Interner Handler, der WebSocket-Events verarbeitet.
     * @param server Pointer auf den WebSocket-Server.
     * @param client Pointer auf den verbundenen Client.
     * @param type Art des Events (Verbindung, Trennung, Daten, etc.).
     * @param arg Event-Argumente.
     * @param data Empfangene Daten (Payload).
     * @param len Länge der empfangenen Daten.
     */
    void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, const uint8_t *data, size_t len);
    //void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, const uint8_t *data, size_t len) const;

    /**
     * @brief Registriert alle Routen
     */
    void registerRoutes();

    AsyncWebServer _server; // Die Instanz des Webservers.
    AsyncWebSocket _ws; // Die Instanz des WebSocket-Servers am Endpunkt "/ws".
    FS* _fs = nullptr; // Pointer auf das Dateisystem (LittleFS) für die Webseiten-Dateien
    FS* _sd = nullptr; // Pointer auf die SD-Karte, wenn vorhanden
    //String _lastStateJson;
};