#pragma once

/**
 * Modus für die Steuerung der Aktoren (automatisch, immer an, immer aus)
 */
enum ControlMode { MODE_AUTO, MODE_ON, MODE_OFF };

/**
* Definiert alle die zur Laufzeit veränderbaren Einstellungen des Systems.
 */
struct Settings {
    // Zielwerte
    float airTempThresholdHigh = 28.0f; // Zielwert für Raumtemperatur in °C (S1)
    float humidityTarget = 70.0f; // Zielwert Luftfeuchtigkeit in % (S1)
    float soilTempTarget = 24.0f; // Zielwert für Bodentemperatur in °C (S2)
    int soilMoistureTarget = 50; // Zielwert für Bodenfeuchte in % (S3)

    // Lampe 1
    int light1OnHour = 6; // Tageszeit für Lampe 1 (Stunde, 0-23)
    int light1OffHour = 20; // Nachtzeit für Lampe 1 (Stunde, 0-23)
    float light1LuxThresholdDark = 5.0f; // in Lux - ist das Tageslicht dunkler, wird die Lampe 1 zur Tageszeit eingeschaltet
    float light1LuxThresholdBright = 15.0f; // in Lux - ist das Tageslicht heller, wird die Lampe 1 zur Tageszeit ausgeschaltet

    // Lampe 2
    int light2OnHour = 6; // Tageszeit für Lampe 2 (Stunde, 0-23)
    int light2OffHour = 20; // Nachtzeit für Lampe 2 (Stunde, 0-23)
    float light2LuxThresholdDark = 5.0f; // in Lux - ist das Tageslicht dunkler, wird die Lampe 2 zur Tageszeit eingeschaltet
    float light2LuxThresholdBright = 15.0f; // in Lux - ist das Tageslicht heller, wird die Lampe 2 zur Tageszeit ausgeschaltet

    // Laufzeiten für Aktor A4 und A5
    unsigned long fanCooldownDurationMs = 300000; // Laufzeit des Lüfters in ms (A4, Default: 5 Minuten)
    unsigned long wateringDurationMs = 5000; // Dauer der Bewässerung in ms (A5, Default: 5 Sekunden)

    // Modus für die Steuerung der Aktoren (automatisch, immer an, immer aus)
    ControlMode lamp1Mode = MODE_AUTO; // Modus für Lampe 1 (A1)
    ControlMode lamp2Mode = MODE_AUTO; // Modus für Lampe 2 (A2)
    ControlMode heaterMode = MODE_AUTO; // Modus für Heizer (A3)
    ControlMode fanMode = MODE_AUTO;  // Modus für Lüfter (A4)
    ControlMode pumpMode = MODE_AUTO; // Modus für Wasserpumpe (A5)
    ControlMode misterMode = MODE_AUTO; // Modus für Vernebler (A6)
};
