#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <functional> // Notwendig für Callbacks

/**
 * Stellt ein Webinterface zur Steuerung via WebSocket bereit.
 *
 * Kapselt die Logik für den asynchronen Web- und WebSocket-Server.
 * Liefert statische Dateien (HTML, CSS, JS) aus dem LittleFS und
 * meldet empfangene WebSocket-Nachrichten über einen Callback an die Hauptanwendung.
 */
class WebInterface {
public:
    /**
     * @brief Konstruktor.
     * @param port Der Port, auf dem der Webserver lauschen soll (Standard 80).
     */
    explicit WebInterface(uint16_t port = 80);

    /**
     * @brief Initialisiert den Server und registriert die Routen.
     * @return true bei Erfolg.
     */
    bool begin();

    /**
     * @brief Sendet eine Nachricht an alle verbundenen WebSocket-Clients (Broadcast).
     * @param message Der zu sendende String.
     */
    void broadcast(const String& message);

    /**
     * @property onMessage
     * @brief Callback, der aufgerufen wird, wenn eine Nachricht über WebSocket empfangen wird.
     * Die Hauptanwendung (main.cpp) muss diesen Callback definieren, um auf Befehle vom Browser zu reagieren.
     * Format: (Nachricht als String)
     */
    std::function<void(const String& msg)> onMessage;

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
    void onWsEvent(AsyncWebSocket *server, const AsyncWebSocketClient *client, AwsEventType type, void *arg, const uint8_t *data, size_t len) const;

    AsyncWebServer _server; // Die Instanz des Webservers.
    AsyncWebSocket _ws;     // Die Instanz des WebSocket-Servers am Endpunkt "/ws".
};