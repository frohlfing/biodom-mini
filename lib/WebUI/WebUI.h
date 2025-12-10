#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
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
     * @param sd Ein Pointer auf das SD-Karten-Dateisystem (optional, für SD-Funktionen).
     * @return true bei Erfolg.
     */
    bool begin(FS* sd = nullptr);

    /**
     * @brief Sendet eine Nachricht nur mit Typ (ohne Nutzdaten).
     * Beispiel: webInterface.broadcast("imageListCleared");
     */
    void broadcast(const char* type);

    /**
     * @brief Sendet eine standardisierte Nachricht mit Typ und Payload an alle Clients.
     * Wrapper, der das JSON-Erstellen übernimmt.
     * @param type Der Nachrichtentyp (z.B. "state", "settings").
     * @param payload Das Daten-Objekt (z.B. Sensorwerte).
     */
    void broadcast(const char* type, const JsonObject& payload);

    /**
     * @brief Sendet Schlüssel-Wert-Paare als Payload ("payload": {key: value})
     */
    void broadcast(const char* type, const char* key, const String& value);

    /**
     * @brief Sendet eine Nachricht an einen bestimmten Client.
     * @param client Der Ziel-Client.
     * @param type Der Nachrichtentyp.
     */
    void sendTo(const AsyncWebSocketClient* client, const char* type);

    /**
     * @brief Sendet eine Nachricht mit Payload an einen bestimmten Client.
     * @param client Der Ziel-Client.
     * @param type Der Nachrichtentyp.
     * @param payload Das Daten-Objekt.
     */
    void sendTo(const AsyncWebSocketClient* client, const char* type, const JsonObject& payload);

    /**
     * @brief Sendet ein Schlüssel-Wert-Paar an einen bestimmten Client.
     * @param client Der Ziel-Client.
     * @param type Der Nachrichtentyp.
     * @param key Der Schlüssel im Payload.
     * @param value Der Wert im Payload.
     */
    void sendTo(const AsyncWebSocketClient* client, const char* type, const char* key, const String& value);

    /**
     * @brief Sendet eine formatierte Log-Nachricht an die Browser-Konsole eines bestimmten Clients.
     * @param client Der Ziel-Client.
     * @param format Der Format-String (wie bei printf).
     * @param ... Die Argumente für den Format-String.
     */
    void consoleLog(const AsyncWebSocketClient* client, const char *format, ...);

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

    /**
     * @brief Sendet ein JSON-Objekt an alle Clients.
     * Ersetzt die alte broadcast(String) Methode für mehr Typsicherheit.
     */
    void broadcast(const JsonDocument& doc);

    /**
     * @brief Sendet ein JSON-Objekt an einen bestimmten Client.
     * @param client Der Ziel-Client.
     * @param doc Das zu sendende JSON-Dokument.
     */
    void sendTo(const AsyncWebSocketClient* client, const JsonDocument& doc);

    AsyncWebServer _server; // Die Instanz des Webservers.
    AsyncWebSocket _ws; // Die Instanz des WebSocket-Servers am Endpunkt "/ws".
    FS* _sd = nullptr; // Pointer auf die SD-Karte, wenn vorhanden
    //String _lastStateJson;
};