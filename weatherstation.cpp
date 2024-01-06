#include "weatherstation.h"
#include <iostream>
#include <endian.h>

Q_DECLARE_METATYPE(weatherStationErrors_t)

WeatherStation::WeatherStation(ModBus::ModBusMaster *master, QObject *parent)
    : ModBus::ModBusMasterSub{master, parent}
{
    qRegisterMetaType<weatherStationErrors_t>();

    requestsMap.clear();
    weatherStationSlaveId = 0xff;

    connect(this, SIGNAL(error(ModBus::ModBusError)), this, SLOT(modbusErrorSlot(ModBus::ModBusError)));
    connect(this, SIGNAL(transactionFinished(ModBus::mbTransaction_t*)), this, SLOT(transactionFinishedSlot(ModBus::mbTransaction_t*)));
}

void WeatherStation::requestSlaveIdSlot()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, 0xFF, 0x07D0, 1)))
    {
        std::cout << "[WeatherStation] Can`t create slaveId request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_SLAVEID);
    }
}

void WeatherStation::requestBaudRate()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x07D1, 1)))
    {
        std::cout << "[WeatherStation] Can`t create baud rate request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_BAUDRATE);
    }
}

void WeatherStation::requestWindSpeed()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01F4, 1)))
    {
        std::cout << "[WeatherStation] Can`t create wind speed request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_WINDSPEED);
    }
}

void WeatherStation::requestWindStrength()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01F5, 1)))
    {
        std::cout << "[WeatherStation] Can`t create wind strength request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_WINDSTRENGTH);
    }
}

void WeatherStation::requestWindDirection()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01F6, 1)))
    {
        std::cout << "[WeatherStation] Can`t create wind direction request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_WINDDIRECTION);
    }
}

void WeatherStation::requestWindDirectionGrad()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01F7, 1)))
    {
        std::cout << "[WeatherStation] Can`t create wind direction (grad) request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_WINDDIRECTIONGRAD);
    }
}

void WeatherStation::requestHumidity()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01F8, 1)))
    {
        std::cout << "[WeatherStation] Can`t create humidity request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_HUMIDITY);
    }
}

void WeatherStation::requestTemperature()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01F9, 1)))
    {
        std::cout << "[WeatherStation] Can`t create temperature request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_TEMPERATURE);
    }
}

void WeatherStation::requestNoise()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01FA, 1)))
    {
        std::cout << "[WeatherStation] Can`t create noise request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_NOISE);
    }
}

void WeatherStation::requestPM2_5()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01FB, 1)))
    {
        std::cout << "[WeatherStation] Can`t create pm 2.5 request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_PM2_5);
    }
}

void WeatherStation::requestPM10()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01FC, 1)))
    {
        std::cout << "[WeatherStation] Can`t create pm 10 request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_PM10);
    }
}

void WeatherStation::requestPressure()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01FD, 1)))
    {
        std::cout << "[WeatherStation] Can`t create atmosphere pressure request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_PRESSURE);
    }
}

void WeatherStation::requestIlluminanceQ()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x01FE, 2)))
    {
        std::cout << "[WeatherStation] Can`t create illuminance (quality) request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_ILLUMINANCE_Q);
    }
}

void WeatherStation::requestIlluminance()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x0200, 1)))
    {
        std::cout << "[WeatherStation] Can`t create illuminance request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_ILLUMINANCE);
    }
}

void WeatherStation::requestRainfall()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_READ_HOLDING_REGISTERS_FID, weatherStationSlaveId, 0x0201, 1)))
    {
        std::cout << "[WeatherStation] Can`t create rainfall request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_RAINFALL);
    }
}

void WeatherStation::requestSetSlaveId(uint8_t slaveId)
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_FORCE_SINGLE_REGISTER_FID, weatherStationSlaveId, 0x07D0, slaveId)))
    {
        std::cout << "[WeatherStation] Can`t create set slave id request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_SETSLAVEID);
    }
}

void WeatherStation::requestSetBaudRate(uint16_t baudRate)
{
    int requestId = 0;
    uint16_t baudRateRegister;

    if (2400 == baudRate)
        baudRateRegister = 0;
    else if (4800 == baudRate)
        baudRateRegister = 1;
    else if (9600 == baudRate)
        baudRateRegister = 2;
    else
    {
        emit stationError(WS_ERROR_BAUDRATE);
        return;
    }
    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_FORCE_SINGLE_REGISTER_FID, weatherStationSlaveId, 0x07D1, baudRateRegister)))
    {
        std::cout << "[WeatherStation] Can`t create set baud rate request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_SETBAUDRATE);
    }
}

void WeatherStation::requestSetWindDirectionOffset(uint8_t offset)
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_FORCE_SINGLE_REGISTER_FID, weatherStationSlaveId, 0x6000, offset)))
    {
        std::cout << "[WeatherStation] Can`t create set wind direction offset request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_SETWINDDIRECTIONOFFSET);
    }
}

void WeatherStation::requestResetZeroWindSpeed()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_FORCE_SINGLE_REGISTER_FID, weatherStationSlaveId, 0x6001, 0x00AA)))
    {
        std::cout << "[WeatherStation] Can`t create reset zero wind speed request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_RESETWINDSPEED);
    }
}

void WeatherStation::requestResetRainfall()
{
    int requestId = 0;

    if (-1 == (requestId = ModBus::ModBusMasterSub::createRequest(ModBus::MB_FORCE_SINGLE_REGISTER_FID, weatherStationSlaveId, 0x6002, 0x005A)))
    {
        std::cout << "[WeatherStation] Can`t create reset rainfall request!" << std::endl;
        emit stationError(WS_ERROR_SEND_QUEUE);
    }
    else
    {
        requestsMap.insert(requestId, WS_RT_RESETRAINFALL);
    }
}

void WeatherStation::transactionFinishedSlot(ModBus::mbTransaction_t *transaction)
{
    weatherStationRequestType_t requestType = requestsMap.value(transaction->transactionId, WS_RT_UNKNOWN);
    uint16_t cacheValue;

    if (false != transaction->crcCheck)
    {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        ModBus::ModBusMasterSub::swapByteOrder(transaction);
#endif
        if (ModBus::MB_ERROR_NONE == ModBusMasterSub::checkError(transaction))
        {
            switch(requestType)
            {
            case WS_RT_SLAVEID:
                if (0xff == (weatherStationSlaveId = static_cast<uint8_t>(transaction->rxFrame->readRegsResp.regs[0] & 0xff)))
                    emit stationError(WS_ERROR_SLAVEID_INCORRECT);
                else
                    emit connectionSetuped();
                break;
            case WS_RT_BAUDRATE:
                switch(transaction->rxFrame->readRegsResp.regs[0])
                {
                case 0:
                    emit baudRate(2400);
                    break;
                case 1:
                    emit baudRate(4800);
                    break;
                case 2:
                    emit baudRate(9600);
                    break;
                default:
                    emit baudRate(0);
                }
                break;
            case WS_RT_WINDSPEED:
                emit windSpeed(static_cast<float>(transaction->rxFrame->readRegsResp.regs[0]) / 100.0f);
                break;
            case WS_RT_WINDSTRENGTH:
                emit windStrength(transaction->rxFrame->readRegsResp.regs[0]);
                break;
            case WS_RT_WINDDIRECTION:
                switch(transaction->rxFrame->readRegsResp.regs[0])
                {
                case 0:
                    emit windDirection("North");
                    break;
                case 1:
                    emit windDirection("Northeast");
                    break;
                case 2:
                    emit windDirection("East");
                    break;
                case 3:
                    emit windDirection("Southeast");
                    break;
                case 4:
                    emit windDirection("South");
                    break;
                case 5:
                    emit windDirection("Southwest");
                    break;
                case 6:
                    emit windDirection("West");
                    break;
                case 7:
                    emit windDirection("Northwest");
                    break;
                default:
                    emit windDirection("Unknown");
                    break;
                }
                break;
            case WS_RT_WINDDIRECTIONGRAD:
                emit windDirectionGrad(transaction->rxFrame->readRegsResp.regs[0]);
                break;
            case WS_RT_HUMIDITY:
                emit humidity(static_cast<float>(transaction->rxFrame->readRegsResp.regs[0]) / 10.0f);
                break;
            case WS_RT_TEMPERATURE:
                emit temperature(static_cast<float>(unsignedToSigned(transaction->rxFrame->readRegsResp.regs[0])) / 10.0f);
                break;
            case WS_RT_NOISE:
                emit noise(static_cast<float>(transaction->rxFrame->readRegsResp.regs[0]) / 10.0f);
                break;
            case WS_RT_PM2_5:
                emit pm2_5(transaction->rxFrame->readRegsResp.regs[0]);
                break;
            case WS_RT_PM10:
                emit pm10(transaction->rxFrame->readRegsResp.regs[0]);
                break;
            case WS_RT_PRESSURE:
                emit pressure(static_cast<float>(transaction->rxFrame->readRegsResp.regs[0]) / 10.0f);
                break;
            case WS_RT_ILLUMINANCE_Q:
                emit illuminance((transaction->rxFrame->readRegsResp.regs[0] << 16) | transaction->rxFrame->readRegsResp.regs[1]);
                break;
            case WS_RT_ILLUMINANCE:
                emit illuminance(transaction->rxFrame->readRegsResp.regs[0] * 100);
                break;
            case WS_RT_RAINFALL:
                emit rainfall(static_cast<float>(transaction->rxFrame->readRegsResp.regs[0]) / 10.0f);
                break;
            case WS_RT_SETSLAVEID:
                if (0xff <= (cacheValue = transaction->rxFrame->writeRegResp.regVal))
                    emit stationError(WS_ERROR_SLAVEID_INCORRECT);
                else
                {
                    weatherStationSlaveId = static_cast<uint8_t>(cacheValue);
                    emit setSlaveId(weatherStationSlaveId);
                }
                break;
            case WS_RT_SETBAUDRATE:
                switch(transaction->rxFrame->writeRegResp.regVal)
                {
                case 0:
                    emit baudRate(2400);
                    break;
                case 1:
                    emit baudRate(4800);
                    break;
                case 2:
                    emit baudRate(9600);
                    break;
                default:
                    emit baudRate(0);
                }
                break;
            case WS_RT_SETWINDDIRECTIONOFFSET:
                if (1 < transaction->rxFrame->writeRegResp.regVal)
                    emit stationError(WS_ERROR_WIND_DIRECTION_OFFSET);
                else
                    emit setWindDirectionOffset(transaction->rxFrame->writeRegResp.regVal);
                break;
            case WS_RT_RESETWINDSPEED:
                if (0x00AA == transaction->rxFrame->writeRegResp.regVal)
                    emit resetWindSpeed();
                else
                    emit stationError(WS_ERROR_RESET_WIND_SPEED);
                break;
            case WS_RT_RESETRAINFALL:
                if (0x005A == transaction->rxFrame->writeRegResp.regVal)
                    emit resetRainfall();
                else
                    emit stationError(WS_ERROR_RESET_RAINFALL);
                break;
            default:
                break;
            }
        }
        else
            emit stationError(WS_ERROR_EXCEPTION);
    }
    else
        emit stationError(WS_ERROR_CRC);

    delete transaction->txFrame;
    delete transaction->rxFrame;
    delete transaction;
}

int16_t WeatherStation::unsignedToSigned(uint16_t value)
{
    if (SHRT_MAX > value)
        return static_cast<int16_t>(value);
    else
        return static_cast<int16_t>(USHRT_MAX - value) * -1;
}

void WeatherStation::modbusErrorSlot(ModBus::ModBusError mbErrorType)
{
    switch (mbErrorType)
    {
    case ModBus::MB_ERROR_TRANSMIT:
        emit stationError(WS_ERROR_TRANSMIT);
        break;
    case ModBus::MB_ERROR_RECEIVE:
        emit stationError(WS_ERROR_RECEIVE);
        break;
    case ModBus::MB_ERROR_RECEIVE_TIMEOUT:
        emit stationError(WS_ERROR_RECEIVE_TIMEOUT);
        break;
    default:
        emit stationError(WS_ERROR_UNKOWN);
        break;
    }
}
