#pragma once

// /**
//  * Modus für die Steuerung der Aktoren (automatisch, immer an, immer aus)
//  */
// enum ControlMode { MODE_AUTO, MODE_ON, MODE_OFF };

/**
* Definiert alle die zur Laufzeit veränderbaren Einstellungen des Systems.
 */
struct settings {
    // Steuerungsparameter
    float airTempThresholdHigh = 28.0f; // Zielwert für Raumtemperatur in °C (S1)
    float humidityTarget = 70.0f; // Zielwert Luftfeuchtigkeit in % (S1)
    float soilTempTarget = 24.0f; // Zielwert für Bodentemperatur in °C (S2)
    int soilMoistureTarget = 50; // Zielwert für Bodenfeuchte in % (S3)

    // Betriebszeit für Lampen (A1 und A2)
    int lightOnHour = 6; // Stunde, 0-23: Ab wann soll die Lampe angehen?
    int lightOffHour = 20; // Stunde, 0-23: Ab wann soll die Lampe ausgehen?

    // Schwellwerte für Lichtsensor in Lux (S5), dass selbst in der Licht-An-Zeit eine oder beide Lampen aus bleiben dürfen
    float lightLuxThresholdDark = 5.0f; // ist das Tageslicht heller, bleibt eine Lampe aus
    float lightLuxThresholdBright = 15.0f; // ist das Tageslicht heller, bleiben beide Lampen aus

    // Laufzeiten für Aktor A4 und A5
    unsigned long fanCooldownDurationMs = 300000; // Laufzeit des Lüfters in ms (A4, Default: 5 Minuten)
    unsigned long wateringDurationMs = 5000; // Dauer der Bewässerung in ms (A5, Default: 5 Sekunden)

    // // Modus für die Steuerung der Aktoren (automatisch, immer an, immer aus)
    // ControlMode lamp1Mode = MODE_AUTO;  // Modus für Lampe 1 (A1)
    // ControlMode lamp2Mode = MODE_AUTO;  // Modus für Lampe 2 (A2)
    // ControlMode heaterMode = MODE_AUTO; // Modus für Heizer (A3)
    // ControlMode fanMode = MODE_AUTO;    // Modus für Lüfter (A4)
    // ControlMode pumpMode = MODE_AUTO;   // Modus für Wasserpumpe (A5)
    // ControlMode misterMode = MODE_AUTO; // Modus für Vernebler (A6)
};
