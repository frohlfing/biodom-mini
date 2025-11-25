#pragma once

/**
* Definiert alle die zur Laufzeit veränderbaren Einstellungen des Systems.
 */
struct settings {
    // Steuerungsparameter
    float airTempThresholdHigh = 28.0f; // Zielwert für Raumtemperatur (S1) in °C
    float humidityTarget = 70.0f; // Zielwert Luftfeuchtigkeit (S1) in %
    float soilTempTarget = 24.0f; // Zielwert für Bodentemperatur (S2) in °C
    int soilMoistureTarget = 50; // Zielwert für Bodenfeuchte (S3) in %

    // Betriebszeit für Lampen (A1 und A2)
    int lightOnHour = 6; // Stunde, 0-23: Ab wann soll die Lampe angehen?
    int lightOffHour = 20; // Stunde, 0-23: Ab wann soll die Lampe ausgehen?

    // Schwellwerte für Lichtsensor (S5), dass selbst in der Licht-An-Zeit eine oder beide Lampen aus bleiben dürfen
    float lightLuxThresholdDark = 5.0f; // Lux-Wert: ist das Tageslicht heller, bleibt eine Lampe aus
    float lightLuxThresholdBright = 15.0f; // Lux-Wert: ist das Tageslicht heller, bleiben beide Lampen aus

    // Laufzeiten für Aktor A4 und A5
    unsigned long fanCooldownDurationMs = 300000; // Laufzeit des Lüfters (A4) in ms (Default: 5 Minuten)
    unsigned long wateringDurationMs = 5000; // Dauer der Bewässerung (A5) in ms (Default: 5 Sekunden)
};
