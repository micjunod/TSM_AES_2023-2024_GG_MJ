#include "sensor_device.hpp"

namespace bike_computer
{
    SensorDevice::SensorDevice(): _hdc1000(PD_13,PD_12, PC_6)
    {
        
    }

    bool SensorDevice::init()
    {
        return _hdc1000.probe();
    }
    
    float SensorDevice::readHumidity()
    {
        return _hdc1000.getHumidity();
    }

    float SensorDevice::readTemperature()
    {
        return _hdc1000.getTemperature();
    }

    
}