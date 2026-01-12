#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <iomanip>

#include "ArgParser.h"
#include "SystemUtils.h"
#include "SensorDevice.h"
#include "DataWriter.h"
#include "json.hpp"

using namespace std;


void printHelp() {
    cout << "HSDatalog CLI Example - Refactored\n"
         << "Usage: cli_example [-f config.json] [-u config.ucf] [-t timeout_sec]\n"
         << "  -h : Help\n"
         << "  -g : Get current device config and exit\n";
}

string readFileContent(const string& path) {
    ifstream file(path, ios::in | ios::binary);
    if (!file) return "";
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

int main(int argc, char *argv[]) {
    ArgParser input(argc, argv);
    SystemUtils::setupSignalHandler();

    if (input.cmdOptionExists("-h")) {
        printHelp();
        return 0;
    }

    SensorDevice sensor;
    if (!sensor.connect()) {
        return -1;
    }

    // --- Gestione Export Configurazione Corrente (-g) ---
    if (input.cmdOptionExists("-g")) {
        string config = sensor.getDeviceStatusJSON();
        ofstream out("device_config.json");
        out << config;
        cout << "Configuration saved to device_config.json\n";
        return 0;
    }

    // --- Caricamento Configurazione Dispositivo (-f) ---
    string configFile = input.getCmdOption("-f");
    if (!configFile.empty()) {
        string content = readFileContent(configFile);
        if (content.empty()) {
            cerr << "Error reading config file: " << configFile << endl;
            return -1;
        }
        if (!sensor.setDeviceConfig(content)) {
            cerr << "Error applying device configuration.\n";
            return -1;
        }
        cout << "Device configuration applied.\n";
    } else {
        cout << "Using default device configuration.\n";
    }

    // --- Caricamento Configurazione UCF/MLC (-u) ---
    string ucfFile = input.getCmdOption("-u");
    string ucfContent; // Mantienuta in memoria per salvarla dopo
    if (!ucfFile.empty()) {
        ucfContent = readFileContent(ucfFile);
        if (ucfContent.empty()) {
            cerr << "Error reading UCF file.\n";
            return -1;
        }
        if (!sensor.loadUCF(ucfContent)) {
            cerr << "Error loading UCF to MLC.\n";
        } else {
            cout << "UCF loaded successfully.\n";
        }
    }

    // --- Preparazione Output ---
    string dirName = "./" + SystemUtils::getCurrentTimestampString();
    SystemUtils::createDirectory(dirName);
    cout << "Data Directory: " << dirName << endl;

    // Salvataggio copia dell'UCF 
    if (!ucfContent.empty()) {
        ofstream dst(dirName + "/configuration.ucf", ios::binary);
        dst << ucfContent;
    }

    // Inizializzazione Writer
    DataWriter writer(dirName);
    auto activeSensors = sensor.getActiveSensors();
    writer.initSensorFiles(activeSensors);

    // --- Avvio Logging ---
    cout << "Starting log... (Press 'q' or ESC to stop)\n";
    sensor.startLog();

    // Loop Variabili
    auto startTime = chrono::high_resolution_clock::now();
    unsigned long timeout = 0;
    if (input.cmdOptionExists("-t")) timeout = stoul(input.getCmdOption("-t"));

    vector<uint8_t> dataBuffer; 
    int actualSize = 0;
    long totalBytes = 0;

    // --- Main Loop ---
    while (!g_exit_requested) {
        // Controllo Input Utente
        char key;
        if (SystemUtils::getKeyboardInput(&key)) {
            if (key == 'q' || key == 0x1B) g_exit_requested = true;
        }

        // Controllo Timeout
        auto now = chrono::high_resolution_clock::now();
        auto elapsedSec = chrono::duration_cast<chrono::seconds>(now - startTime).count();
        if (timeout > 0 && static_cast<unsigned long>(elapsedSec) >= timeout) g_exit_requested = true;

        // UI Update 
        cout << "\rElapsed: " << elapsedSec << "s | Total Bytes: " << totalBytes << flush;

        // Lettura Dati Sensori
        for (const auto& sName : activeSensors) {
            if (sensor.getData(sName, dataBuffer, actualSize)) {
                writer.writeData(sName, dataBuffer.data(), actualSize);
                totalBytes += actualSize;
            }
        }
        
        SystemUtils::sleepMs(10); 
    }

    cout << "\nStopping acquisition...\n";
    sensor.stopLog();
    
    // Salvataggio configurazione finale
    ofstream finalConfig(dirName + "/acquisition_info.json");
    finalConfig << sensor.getDeviceStatusJSON();
    
    cout << "Done. Goodbye.\n";
    return 0;
}