#pragma once
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <cstdint>
#include <chrono> 

/**
 * @brief Gestisce la scrittura dei dati su disco.
 * Gestisce automaticamente formati Raw, Float e High-Speed (senza timestamp).
 */
class DataWriter {
public:
    DataWriter(const std::string& outputDir);
    ~DataWriter();

    void initSensorFiles(const std::vector<std::string>& sensorNames);
    void writeData(const std::string& sensorName, const uint8_t* data, int size);
    void closeAll();

private:
    std::string baseDir;
    std::map<std::string, std::ofstream> jsonFiles;
    std::map<std::string, FILE*> binaryFiles;
    std::map<std::string, bool> firstSampleMap;
    
    // Mappa per tracciare l'ultimo timestamp ricevuto per ogni sensore 
    std::map<std::string, double> lastBlockEndTime;

    bool isJsonSensor(const std::string& name);
    
    double getCurrentTimeSec();

    void writeJsonSample(std::ofstream& file, const std::string& name, const uint8_t* sample, bool& isFirst, bool isInt16, bool isTimestampAtEnd = false, double forcedTimestamp = 0.0);
};