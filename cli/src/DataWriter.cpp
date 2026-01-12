#include "DataWriter.h"
#include <iostream>
#include <cstring> 
#include <cmath>
#include <chrono> 

DataWriter::DataWriter(const std::string& outputDir) : baseDir(outputDir) {}

DataWriter::~DataWriter() {
    closeAll();
}

double DataWriter::getCurrentTimeSec() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
}

bool DataWriter::isJsonSensor(const std::string& name) {
    return (name.find("temp") != std::string::npos || 
            name.find("gyro") != std::string::npos || 
            name.find("acc") != std::string::npos || 
            name.find("press") != std::string::npos || 
            name.find("mag") != std::string::npos);
}

void DataWriter::initSensorFiles(const std::vector<std::string>& sensorNames) {
    for (const auto& name : sensorNames) {
        std::string path = baseDir + "/" + name;
        
        if (isJsonSensor(name)) {
            jsonFiles[name].open(path + ".json");
            jsonFiles[name] << "[\n";
            firstSampleMap[name] = true;
            lastBlockEndTime[name] = getCurrentTimeSec();
        } else {
            FILE* f = fopen((path + ".dat").c_str(), "wb+");
            if (f) binaryFiles[name] = f;
        }
    }
}

void DataWriter::writeData(const std::string& name, const uint8_t* data, int size) {
    if (isJsonSensor(name)) {
        
        
        if (name.find("temp") != std::string::npos || name.find("press") != std::string::npos) {
            
            int sampleSize = 0;
            bool isInt16 = false;       
            bool isTimestampAtEnd = false;

            if (size % 16 == 0) {
                sampleSize = 16;
                isInt16 = false;        // Temp/Press formato 16 byte = FLOAT
                isTimestampAtEnd = true;
            } else if (size % 12 == 0) {
                sampleSize = 12; 
                isInt16 = false;
            } else {
                sampleSize = 10; 
                isInt16 = true;
            }

            if (sampleSize == 0) return;

            int nSamples = size / sampleSize;
            for (int i = 0; i < nSamples; i++) {
                writeJsonSample(jsonFiles[name], name, data + (i * sampleSize), firstSampleMap[name], isInt16, isTimestampAtEnd, 0.0);
            }
            return; 
        } 

        
        if ((size >= 10) && ((size - 4) % 6 == 0)) {
            const int headerSize = 4;
            const int sampleSize = 6;
            int nSamples = (size - headerSize) / sampleSize;

            double now = getCurrentTimeSec();
            double prev = lastBlockEndTime[name];
            if (prev == 0.0) prev = now - 0.05; 

            double totalDuration = now - prev;
            double timeStep = (nSamples > 0) ? totalDuration / nSamples : 0.0;

            lastBlockEndTime[name] = now;

            const uint8_t* payload = data + headerSize; 
            for (int i = 0; i < nSamples; i++) {
                double ts = prev + (i * timeStep);
                writeJsonSample(jsonFiles[name], name, payload + (i * sampleSize), firstSampleMap[name], true, false, ts);
            }
            return; 
        }
        
        
        int sampleSize = 0;
        bool isInt16 = false;
        
        if (size % 14 == 0) { sampleSize = 14; isInt16 = true; }
        else if (size % 20 == 0) { sampleSize = 20; isInt16 = false; }
        
        if (sampleSize > 0) {
            int nSamples = size / sampleSize;
            for (int i = 0; i < nSamples; i++) {
                writeJsonSample(jsonFiles[name], name, data + (i * sampleSize), firstSampleMap[name], isInt16, false, 0.0);
            }
        }

    } else {
        if (binaryFiles.count(name)) {
            fwrite(data, 1, size, binaryFiles[name]);
        }
    }
}

void DataWriter::writeJsonSample(std::ofstream& file, const std::string& name, const uint8_t* sample, bool& isFirst, bool isInt16, bool isTimestampAtEnd, double forcedTimestamp) {
    double timestamp;

    if (forcedTimestamp > 0.0) {
        timestamp = forcedTimestamp;
    } else {
        if (isTimestampAtEnd) {
            std::memcpy(&timestamp, sample + 8, sizeof(double));
        } else {
            std::memcpy(&timestamp, sample, sizeof(double));
        }
    }
    
    if (forcedTimestamp == 0.0 && (std::isnan(timestamp) || timestamp < 0 || timestamp > 4e9)) return; 
    
    if (!isFirst) file << ",\n";
    else isFirst = false;

    file.setf(std::ios::fixed, std::ios::floatfield);
    file.precision(6);
    file << "{ \"timestamp\": " << timestamp;
    file.unsetf(std::ios::floatfield);
    file.precision(6);
    
    if (name.find("temp") != std::string::npos || name.find("press") != std::string::npos) {
        float value = 0.0f;
        
        if (isTimestampAtEnd) {
            std::memcpy(&value, sample + 4, sizeof(float)); 
        } 
        else if (isInt16) {
            int16_t raw;
            std::memcpy(&raw, sample + 8, sizeof(int16_t));
            value = static_cast<float>(raw);
        } else {
            std::memcpy(&value, sample + 8, sizeof(float));
        }
        file << ", \"value\": " << value << " }";
    } 
    else {
        // Acc / Gyro / Mag
        if (isInt16) {
            int16_t x, y, z;
            int offset = (forcedTimestamp > 0.0) ? 0 : 8; 
            
            std::memcpy(&x, sample + offset, sizeof(int16_t));
            std::memcpy(&y, sample + offset + 2, sizeof(int16_t));
            std::memcpy(&z, sample + offset + 4, sizeof(int16_t));
            
            file << ", \"x\": " << x << ", \"y\": " << y << ", \"z\": " << z << " }";
        } else {
            float x, y, z;
            std::memcpy(&x, sample + 8, sizeof(float));
            std::memcpy(&y, sample + 12, sizeof(float));
            std::memcpy(&z, sample + 16, sizeof(float));
            file << ", \"x\": " << x << ", \"y\": " << y << ", \"z\": " << z << " }";
        }
    }
}

void DataWriter::closeAll() {
    for (auto& pair : jsonFiles) {
        if (pair.second.is_open()) {
            pair.second << "\n]";
            pair.second.close();
        }
    }
    jsonFiles.clear();

    for (auto& pair : binaryFiles) {
        if (pair.second) {
            fclose(pair.second);
        }
    }
    binaryFiles.clear();
}