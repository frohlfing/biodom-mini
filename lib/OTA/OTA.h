#pragma once

#include <Arduino.h>
#include <ArduinoOTA.h>

/**
 * @class OTA
 * @brief Kapselt die Logik für Over-the-Air (OTA) Updates.
 *
 * Diese Klasse vereinfacht die Integration von OTA-Updates in ein Arduino-Projekt.
 * Sie kümmert sich um die Initialisierung und das Handling der Update-Anfragen.
 */
class OTA {
public:
    /**
     * @brief Konstruktor.
     * @param hostname Der Hostname, unter dem das Gerät im Netzwerk erscheint.
     * @param password Das Passwort für den OTA-Update-Vorgang.
     */
    OTA(const char* hostname, const char* password);

    /**
     * @brief Initialisiert den OTA-Dienst.
     * Muss im setup() aufgerufen werden.
     */
    void begin() const;

    /**
     * @brief Muss im loop() aufgerufen werden.
     */
    void handle();

private:
    const char* _hostname;
    const char* _password;
};