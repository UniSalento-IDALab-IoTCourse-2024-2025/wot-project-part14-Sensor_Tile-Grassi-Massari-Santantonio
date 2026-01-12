#pragma once
#include <string>
#include <vector>
#include "HS_DataLog.h"

/**
 * @brief Wrapper per la gestione del dispositivo ST SensorTile Box Pro.
 * Gestisce l'inizializzazione della libreria, la configurazione dei sensori
 * e il ciclo di vita dell'acquisizione dati.
 */
class SensorDevice {
public:
    SensorDevice();
    ~SensorDevice();

    // Inizializza la libreria e si connette al primo dispositivo disponibile
    bool connect();
    
    // Disconnette e libera le risorse
    void disconnect();

    // Recupera informazioni sul dispositivo
    std::string getDeviceAlias();
    std::string getDeviceStatusJSON();
    
    // Carica configurazione da file JSON (buffer)
    bool setDeviceConfig(const std::string& jsonConfig);
    
    // Carica configurazione MLC (.ucf)
    bool loadUCF(const std::string& ucfContent);

    // Avvia/Ferma il logging su USB
    bool startLog();
    bool stopLog();

    // Ottiene la lista dei nomi dei componenti attivi
    std::vector<std::string> getActiveSensors();

    // Wrapper per ottenere dati. Ritorna true se ci sono dati, riempie buffer
    bool getData(const std::string& sensorName, std::vector<uint8_t>& buffer, int& actualSize);

    int getDeviceId() const { return deviceID; }

private:
    int deviceID;
    bool connected;
};