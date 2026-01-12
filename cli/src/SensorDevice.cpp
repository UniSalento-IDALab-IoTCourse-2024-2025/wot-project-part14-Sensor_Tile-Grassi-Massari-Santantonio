#include "SensorDevice.h"
#include <iostream>
#include <cstring>
#include "json.hpp"

SensorDevice::SensorDevice() : deviceID(0), connected(false) {}

SensorDevice::~SensorDevice() {
    disconnect();
}

bool SensorDevice::connect() {
    if (hs_datalog_open() != ST_HS_DATALOG_OK) {
        std::cerr << "[Error] Failed to initialize datalog library.\n";
        return false;
    }

    int nDevices = 0;
    hs_datalog_get_device_number(&nDevices);
    if (nDevices == 0) {
        std::cerr << "[Error] No devices found.\n";
        return false;
    }
    
    //  dispositivo con ID 0 
    this->deviceID = 0;
    this->connected = true;
    return true;
}

void SensorDevice::disconnect() {
    if (connected) {
        hs_datalog_close();
        connected = false;
    }
}

std::string SensorDevice::getDeviceAlias() {
    char* fwInfo = nullptr;
    if (hs_datalog_get_component_status(deviceID, &fwInfo, (char*)"firmware_info") != ST_HS_DATALOG_OK) {
        return "Unknown Device";
    }
    auto json = nlohmann::json::parse(fwInfo);
    std::string alias = json["firmware_info"]["alias"];
    hs_datalog_free(fwInfo);
    return alias;
}

std::string SensorDevice::getDeviceStatusJSON() {
    char* status = nullptr;
    if (hs_datalog_get_device_status(deviceID, &status) != ST_HS_DATALOG_OK) return "{}";
    std::string res(status);
    hs_datalog_free(status);
    return res;
}

bool SensorDevice::setDeviceConfig(const std::string& jsonConfig) {
    char* configStr = new char[jsonConfig.length() + 1];
    strcpy(configStr, jsonConfig.c_str());
    
    int res = hs_datalog_set_device_status(deviceID, configStr);
    delete[] configStr;
    
    return res == ST_HS_DATALOG_OK;
}

bool SensorDevice::loadUCF(const std::string& ucfContent) {
    char* pnpl_response = nullptr;
    // Caricamento specifico per ism330dhcx 
    int res = hs_datalog_load_ucf_to_mlc(deviceID, (char*)"ism330dhcx", 
                                         (uint8_t*)ucfContent.data(), 
                                         ucfContent.size(), 
                                         &pnpl_response);
    if (pnpl_response) hs_datalog_free(pnpl_response);
    
    // MLC
    char* resp2 = nullptr;
    hs_datalog_set_boolean_property(deviceID, true, (char*)"ism330dhcx_mlc", (char*)"enable", nullptr, &resp2);
    if (resp2) hs_datalog_free(resp2);

    // Aggiorna mappa componenti interna
    char* devStatus;
    hs_datalog_get_device_status(deviceID, &devStatus);
    hs_datalog_update_components_map(deviceID, devStatus);
    hs_datalog_free(devStatus);

    return res == ST_HS_DATALOG_OK;
}

bool SensorDevice::startLog() {
    char* response = nullptr;
    hs_datalog_set_rtc_time(deviceID, &response); // Sincronizzazione ora
    if(response) hs_datalog_free(response);
    
    hs_datalog_start_log(deviceID, 1, &response); // 1 = USB
    if(response) hs_datalog_free(response);
    return true;
}

bool SensorDevice::stopLog() {
    char* response = nullptr;
    hs_datalog_stop_log(deviceID, &response);
    if(response) hs_datalog_free(response);
    return true;
}

std::vector<std::string> SensorDevice::getActiveSensors() {
    int nSensors = 0;
    hs_datalog_get_sensor_components_number(deviceID, &nSensors, true);
    std::vector<char*> names(nSensors);
    hs_datalog_get_sensor_components_names(deviceID, names.data(), true);
    
    std::vector<std::string> strNames;
    for(auto p : names) strNames.emplace_back(p);
    return strNames;
}

bool SensorDevice::getData(const std::string& sensorName, std::vector<uint8_t>& buffer, int& actualSize) {
    int size = 0;
   
    hs_datalog_get_available_data_size(deviceID, const_cast<char*>(sensorName.c_str()), &size);
    
    if (size <= 0) return false;

    if (buffer.size() < (size_t)size) buffer.resize(size);
    
    hs_datalog_get_data(deviceID, const_cast<char*>(sensorName.c_str()), buffer.data(), size, &actualSize);
    return true;
}