#include "consolemanager.h"
#include "weatherstation.h"
#include "modbusmaster.h"
#include <QSocketNotifier>
#include <QDir>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace ModBus;

ConsoleManager::ConsoleManager(QObject *parent) :
    QObject(parent)
{
    currentCommand = COMMAND_NONE;
    deviceNames = new QStringList();
    deviceNames->clear();
}

void ConsoleManager::start()
{
    int index = 0;

    if (0 != (socketNotifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this)))
    {
        connect(socketNotifier, SIGNAL(activated(int)), this, SLOT(readCommand()));

        std::cout << "Choose serial port device: " << std::endl;
        QStringList files = QDir("/dev").entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::System);
        for (int i = 0; i < files.length(); i++)
        {
            if (files[i].contains("ttyS") || files[i].contains("ttyUSB"))
            {
                index++;
                std::cout << "\t" << index << ". " << files[i].toStdString() << std::endl;
                deviceNames->append(files[i]);
            }
        }
        currentCommand = COMMAND_CHOOSE_DEVICE;
        std::cout << "Enter device number: " << std::flush;
    }
    else
        std::cout << "[ConsoleManager] Can`t create socket notifier!" << std::endl;
}

void ConsoleManager::portConfiguredSlot()
{
    std::cout << "[ConsoleManager] Port configured!" << std::endl;
    emit wsRequestSlaveId();
}

void ConsoleManager::wsConfiguredSlot()
{
    std::cout << "[ConsoleManager] Weather station configured!" << std::endl;

    startWeatherStationCommand();
}

void ConsoleManager::baudRateSlot(uint16_t baudRate)
{
    if (0 != baudRate)
        std::cout << "Baud rate value: " << baudRate << std::endl;
    else
        std::cout << "Undefined baud rate value!" << std::endl;

    startWeatherStationCommand();
}

void ConsoleManager::windSpeedSlot(float speed)
{
    std::cout << "Wind speed: " << speed << " m/s" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::windStrengthSlot(uint16_t level)
{
    std::cout << "Level of wind strength: " << level << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::windDirectionSlot(QString direction)
{
    std::cout << "Wind direction:" << direction.toStdString() << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::windDirectionGradSlot(uint16_t grad)
{
    std::cout << "Wind direction: " << grad << "°" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::humiditySlot(float humidity)
{
    std::cout << "Humidity: " << humidity << "% RH" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::temperatureSlot(float temp)
{
    std::cout << "Temperature: " << temp << "°C" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::noiseSlot(float noise)
{
    std::cout << "Noise: " << noise << " dB" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::pm2_5Slot(uint16_t concentration)
{
    std::cout << "PM2.5 concentration: " << concentration << " ug/m3" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::pm10Slot(uint16_t concentration)
{
    std::cout << "PM10 concentration: " << concentration << " ug/m3" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::pressureSlot(float pressure)
{
    std::cout << "Atmosphere pressure: " << pressure << " kpa" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::illuminanceSlot(uint32_t illuminance)
{
    std::cout << "Illuminance: " << illuminance << " Lux" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::rainfallSlot(float rainfall)
{
    std::cout << "Rainfall: " << rainfall << " mm" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::setSlaveIdSlot(uint8_t slaveId)
{
    std::cout << "New slave id has been installed successfully: " << slaveId << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::setWindDirectionOffsetSlot(uint8_t offset)
{
    std::cout << "Wind speed direction offset has been installed successfully: " << offset << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::resetWindSpeedSlot()
{
    std::cout << "Wind speed has been reseted successfully!" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::resetRainfallSlot()
{
    std::cout << "Rainfall has been reseted successfully!" << std::endl;
    startWeatherStationCommand();
}

void ConsoleManager::modbusErrorSlot(ModBus::ModBusError errorCode)
{
    std::string errorText = "unknown error";
    switch(errorCode)
    {
    case ModBus::MB_ERROR_DESCRIPTOR:
        errorText = "invalid descriptor";
        break;
    case ModBus::MB_ERROR_PORTOPTION:
        errorText = "get port option error";
        break;
    case ModBus::MB_ERROR_SETSPEED:
        errorText = "set speed error";
        break;
    case ModBus::MB_ERROR_SETOPTION:
        errorText = "set port option error";
        break;
    default:
        errorText = "unknown error";
        break;
    }
    std::cout << "Init modbus port has been fail!" << std::endl << "Error: " << errorText << std::endl;
}

void ConsoleManager::wsErrorSlot(weatherStationErrors_t errorCode)
{
    switch(errorCode)
    {
    case WS_ERROR_EXCEPTION:
        std::cout << "Slave device respond with exception: " << weatherStation->getLastException().toStdString() << std::endl;
        break;
    case WS_ERROR_SEND_QUEUE:
        std::cout << "Add message to queue has been fail (queue overstacked)!" << std::endl;
        break;
    case WS_ERROR_TRANSMIT:
        std::cout << "Transmit message error!" << std::endl;
        break;
    case WS_ERROR_RECEIVE:
        std::cout << "Receive message error!" << std::endl;
        break;
    case WS_ERROR_RECEIVE_TIMEOUT:
        std::cout << "Receive message timeout!" << std::endl;
        break;
    case WS_ERROR_SLAVEID_INCORRECT:
        std::cout << "Device slave id incorrect!" << std::endl;
        return;
    case WS_ERROR_BAUDRATE:
        std::cout << "Device baud rate incorrect!" << std::endl;
        break;
    case WS_ERROR_CRC:
        std::cout << "Responce message crc incorrect!" << std::endl;
        break;
    case WS_ERROR_WIND_DIRECTION_OFFSET:
        std::cout << "Failed to set wind direction offset!" << std::endl;
        break;
    case WS_ERROR_RESET_WIND_SPEED:
        std::cout << "Failed to reset wind speed!" << std::endl;
        break;
    case WS_ERROR_RESET_RAINFALL:
        std::cout << "Failed to reset rainfall!" << std::endl;
        break;
    default:
        std::cout << "Unknown error!" << std::endl;
        break;
    }
    startWeatherStationCommand();
}

void ConsoleManager::readCommand()
{
    BaudRate baudRate;
    int numCommand;
    std::string line;
    std::getline(std::cin, line);
    numCommand = atoi(line.c_str());

    switch(currentCommand)
    {
    case COMMAND_CHOOSE_DEVICE:
        if (numCommand <= 0 || numCommand > deviceNames->length())
        {
            std::cout << "Invalid number of device!" << std::endl;
            std::cout << "Enter device number: " << std::flush;
        }
        else
        {
            std::cout << "Choosed device - " << deviceNames->at(numCommand - 1).toStdString() << std::endl << std::endl;
            deviceName = "/dev/" + deviceNames->at(numCommand - 1);
            deviceNames->clear();
            delete deviceNames;

            currentCommand = COMMAND_CHOOSE_BAUDRATE;
            std::cout << "Choose baud rate: " << std::endl;
            std::cout << "\t1. 2400" << std::endl << "\t2. 4800" << std::endl << "\t3. 9600" << std::endl;
            std::cout << "Enter baud rate number: " << std::flush;
        }
        break;

    case COMMAND_CHOOSE_BAUDRATE:
        if (numCommand <= 0 || numCommand > 3)
        {
            std::cout << "Invalid number of baud rate!" << std::endl;
            std::cout << "Enter baud rate number: " << std::flush;
        }
        else
        {
            switch(numCommand)
            {
            case 1:
                std::cout << "Choosed baud rate - 2400" << std::endl << std::endl;
                baudRate = BR_2400;
                break;
            case 2:
                std::cout << "Choosed baud rate - 4800" << std::endl << std::endl;
                baudRate = BR_4800;
                break;
            case 3:
            default:
                std::cout << "Choosed baud rate - 9600" << std::endl << std::endl;
                baudRate = BR_9600;
                break;
            }
            if (0 != (modbus = new ModBus::ModBusMaster(deviceName, baudRate)))
            {
                if (0 != (weatherStation = new WeatherStation(modbus)))
                {
                    connect(this, SIGNAL(modbusInit()), modbus, SLOT(startInitSlot()));
                    connect(modbus, SIGNAL(portConfigured()), this, SLOT(portConfiguredSlot()));
                    connect(modbus, SIGNAL(error(ModBus::ModBusError)), this, SLOT(modbusErrorSlot(ModBus::ModBusError)));

                    connect(this, SIGNAL(wsRequestSlaveId()), weatherStation, SLOT(requestSlaveIdSlot()));
                    connect(this, SIGNAL(getBaudRate()), weatherStation, SLOT(requestBaudRate()));
                    connect(this, SIGNAL(getWindSpeed()), weatherStation, SLOT(requestWindSpeed()));
                    connect(this, SIGNAL(getWindStrength()), weatherStation, SLOT(requestWindStrength()));
                    connect(this, SIGNAL(getWindDirection()), weatherStation, SLOT(requestWindDirection()));
                    connect(this, SIGNAL(getWindDirectionGrad()), weatherStation, SLOT(requestWindDirectionGrad()));
                    connect(this, SIGNAL(getWindDirectionGrad()), weatherStation, SLOT(requestHumidity()));
                    connect(this, SIGNAL(getTemperature()), weatherStation, SLOT(requestTemperature()));
                    connect(this, SIGNAL(getNoise()), weatherStation, SLOT(requestNoise()));
                    connect(this, SIGNAL(getPM2_5()), weatherStation, SLOT(requestPM2_5()));
                    connect(this, SIGNAL(getPM10()), weatherStation, SLOT(requestPM10()));
                    connect(this, SIGNAL(getPressure()), weatherStation, SLOT(requestPressure()));
                    connect(this, SIGNAL(getIlluminanceQ()), weatherStation, SLOT(requestIlluminanceQ()));
                    connect(this, SIGNAL(getIlliminance()), weatherStation, SLOT(requestIlluminance()));
                    connect(this, SIGNAL(getRainfall()), weatherStation, SLOT(requestRainfall()));
                    connect(this, SIGNAL(setSlaveId(uint8_t)), weatherStation, SLOT(requestSetSlaveId(uint8_t)));
                    connect(this, SIGNAL(setWindDirectionOffset(uint8_t)), weatherStation, SLOT(requestSetWindDirectionOffset(uint8_t)));
                    connect(this, SIGNAL(resetZeroWindSpeed()), weatherStation, SLOT(requestResetZeroWindSpeed()));
                    connect(this, SIGNAL(resetRainfall()), weatherStation, SLOT(requestResetRainfall()));

                    connect(weatherStation, SIGNAL(connectionSetuped()), this, SLOT(wsConfiguredSlot()));
                    connect(weatherStation, SIGNAL(baudRate(uint16_t)), this, SLOT(baudRateSlot(uint16_t)));
                    connect(weatherStation, SIGNAL(windSpeed(float)), this, SLOT(windSpeedSlot(float)));
                    connect(weatherStation, SIGNAL(windStrength(uint16_t)), this, SLOT(windStrengthSlot(uint16_t)));
                    connect(weatherStation, SIGNAL(windDirection(QString)), this, SLOT(windDirectionSlot(QString)));
                    connect(weatherStation, SIGNAL(windDirectionGrad(uint16_t)), this, SLOT(windDirectionGradSlot(uint16_t)));
                    connect(weatherStation, SIGNAL(humidity(float)), this, SLOT(humiditySlot(float)));
                    connect(weatherStation, SIGNAL(temperature(float)), this, SLOT(temperatureSlot(float)));
                    connect(weatherStation, SIGNAL(noise(float)), this, SLOT(noiseSlot(float)));
                    connect(weatherStation, SIGNAL(pm2_5(uint16_t)), this, SLOT(pm2_5Slot(uint16_t)));
                    connect(weatherStation, SIGNAL(pm10(uint16_t)), this, SLOT(pm10Slot(uint16_t)));
                    connect(weatherStation, SIGNAL(pressure(float)), this, SLOT(pressureSlot(float)));
                    connect(weatherStation, SIGNAL(illuminance(uint32_t)), this, SLOT(illuminanceSlot(uint32_t)));
                    connect(weatherStation, SIGNAL(rainfall(float)), this, SLOT(rainfallSlot(float)));
                    connect(weatherStation, SIGNAL(setSlaveId(uint8_t)), this, SLOT(setSlaveIdSlot(uint8_t)));
                    connect(weatherStation, SIGNAL(setWindDirectionOffset(uint8_t)), this, SLOT(setWindDirectionOffsetSlot(uint8_t)));
                    connect(weatherStation, SIGNAL(resetWindSpeed()), this, SLOT(resetWindSpeedSlot()));
                    connect(weatherStation, SIGNAL(resetRainfall()), this, SLOT(resetRainfallSlot()));
                    connect(weatherStation, SIGNAL(stationError(weatherStationErrors_t)), this, SLOT(wsErrorSlot(weatherStationErrors_t)));

                    emit modbusInit();
                }
                else
                {
                    delete modbus;
                    std::cout << "[ConsoleManager] Can`t create weather station!" << std::endl;
                }
            }
            else
                std::cout << "[ConsoleManager] Can`t create modbus!" << std::endl;

            currentCommand = COMMAND_NONE;
        }
        break;
    case COMMAND_CHOOSE_WS_COMMAND:
        if (numCommand <= 0 || numCommand > 14)
        {
            std::cout << "Invalid number of command!" << std::endl;
            std::cout << "Enter command number: " << std::flush;
        }
        else
        {
            switch(numCommand)
            {
            case 1:
                emit getBaudRate();
                break;
            case 2:
                emit getWindSpeed();
                break;
            case 3:
                emit getWindStrength();
                break;
            case 4:
                emit getWindDirection();
                break;
            case 5:
                emit getWindDirectionGrad();
                break;
            case 6:
                emit getHumidity();
                break;
            case 7:
                emit getTemperature();
                break;
            case 8:
                emit getNoise();
                break;
            case 9:
                emit getPM2_5();
                break;
            case 10:
                emit getPM10();
                break;
            case 11:
                emit getPressure();
                break;
            case 12:
                emit getIlluminanceQ();
                break;
            case 13:
                emit getIlliminance();
                break;
            case 14:
                emit getRainfall();
                break;
            case 15:
                currentCommand = COMMAND_CHOOSE_WS_SLAVEID;
                std::cout << "Enter new weather station slave ID (1-254): " << std::flush;
                break;
            case 16:
                currentCommand = COMMAND_CHOOSE_WS_BAUDRATE;
                std::cout << "Choose baud rate: " << std::endl;
                std::cout << "\t1. 2400" << std::endl << "\t2. 4800" << std::endl << "\t3. 9600" << std::endl;
                std::cout << "Enter baud rate number: " << std::flush;
                break;
            case 17:
                currentCommand = COMMAND_CHOOSE_WS_WINDOFFSET;
                std::cout << "Choose wind offset:" << std::endl;
                std::cout << "\t1. Without offset" << std::endl << "\t2. Direction offset 180°" << std::endl;
                std::cout << "Enter number of offset: " << std::flush;
                break;
            case 18:
                emit resetZeroWindSpeed();
                break;
            case 19:
                emit resetRainfall();
                break;
            }
            currentCommand = COMMAND_NONE;
        }
        break;
    case COMMAND_CHOOSE_WS_SLAVEID:
        if (numCommand <= 0 || numCommand > 254)
        {
            std::cout << "Invalid slave ID!" << std::endl;
            std::cout << "Enter new weather station slave ID (1-254): " << std::flush;
        }
        else
        {
            emit setSlaveId(numCommand);
            currentCommand = COMMAND_NONE;
        }
        break;
    case COMMAND_CHOOSE_WS_BAUDRATE:
        if (numCommand <= 0 || numCommand > 3)
        {
            std::cout << "Invalid number of baud rate!" << std::endl;
            std::cout << "Enter baud rate number: " << std::flush;
        }
        else
        {
            switch(numCommand)
            {
            case 1:
                std::cout << "Choosed baud rate - 2400" << std::endl << std::endl;
                emit setBaudRate(2400);
                break;
            case 2:
                std::cout << "Choosed baud rate - 4800" << std::endl << std::endl;
                emit setBaudRate(4800);
                break;
            case 3:
            default:
                std::cout << "Choosed baud rate - 9600" << std::endl << std::endl;
                emit setBaudRate(9600);
                break;
            }
            currentCommand = COMMAND_NONE;
        }
        break;
    case COMMAND_CHOOSE_WS_WINDOFFSET:
        if (numCommand <= 0 || numCommand > 2)
        {
            std::cout << "Invalid number of wind direction offset!" << std::endl;
            std::cout << "Enter number of offset: " << std::flush;
        }
        else
        {
            emit setWindDirectionOffset(numCommand - 1);
            currentCommand = COMMAND_NONE;
        }
        break;
    default:
        break;
    }
}

void ConsoleManager::startWeatherStationCommand()
{
    std::cout << "Choose command:" << std::endl;
    std::cout << "1.  Get baud rate" << std::endl;
    std::cout << "2.  Get wind speed" << std::endl;
    std::cout << "3.  Get wind strength" << std::endl;
    std::cout << "4.  Get wind direction" << std::endl;
    std::cout << "5.  Get wind direction (in grad)" << std::endl;
    std::cout << "6.  Get humidity" << std::endl;
    std::cout << "7.  Get temperature" << std::endl;
    std::cout << "8.  Get noise" << std::endl;
    std::cout << "9.  Get PM 2.5" << std::endl;
    std::cout << "10. Get PM 10" << std::endl;
    std::cout << "11. Get atmosphere pressure" << std::endl;
    std::cout << "12. Get illuminance (quality)" << std::endl;
    std::cout << "13. Get illiminance" << std::endl;
    std::cout << "14. Get rainfall" << std::endl;
    std::cout << "15. Set slave ID" << std::endl;
    std::cout << "16. Set baud rate" << std::endl;
    std::cout << "17. Set wind direction offset" << std::endl;
    std::cout << "18. Reset zero wind speed" << std::endl;
    std::cout << "19. Reset zero rainfall" << std::endl;
    std::cout << "Enter command number: " << std::flush;
    currentCommand = COMMAND_CHOOSE_WS_COMMAND;
}
