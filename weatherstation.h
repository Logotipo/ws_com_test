#ifndef WEATHERSTATION_H
#define WEATHERSTATION_H

#include "modbusmastersub.h"
#include "modbusmaster.h"
#include <QMap>

typedef enum _weatherStationErrors_t
{
    WS_ERROR_UNKOWN,                    //! Unknown error
    WS_ERROR_EXCEPTION,                 //! Exception in station respond
    WS_ERROR_SEND_QUEUE,                //! Send message queue overstacked
    WS_ERROR_TRANSMIT,                  //! Transmit message error
    WS_ERROR_RECEIVE,                   //! Receive message error
    WS_ERROR_RECEIVE_TIMEOUT,           //! Receive message timeout
    WS_ERROR_SLAVEID_INCORRECT,         //! Slave id from station incorrect
    WS_ERROR_CRC,                       //! Respond message crc incorrect
    WS_ERROR_BAUDRATE,                  //! Baud rate from station incorrect
    WS_ERROR_WIND_DIRECTION_OFFSET,     //! Wind direction offset from station incorrect
    WS_ERROR_RESET_WIND_SPEED,          //! Fail to set wind speed zero value
    WS_ERROR_RESET_RAINFALL             //! Fail to reset rainfall value
} weatherStationErrors_t;

typedef enum _weatherStationRequestType_t
{
    WS_RT_UNKNOWN = 0,                  //! Unknown request type (no request)
    WS_RT_SLAVEID,                      //! Request current slave id
    WS_RT_BAUDRATE,                     //! Request current baud rate
    WS_RT_WINDSPEED,                    //! Request wind speed
    WS_RT_WINDSTRENGTH,                 //! Request wind strength
    WS_RT_WINDDIRECTION,                //! Request wind direction (cardinal directions)
    WS_RT_WINDDIRECTIONGRAD,            //! Request wind direction in grad
    WS_RT_HUMIDITY,                     //! Request humidity
    WS_RT_TEMPERATURE,                  //! Request temperature
    WS_RT_NOISE,                        //! Request level of noise
    WS_RT_PM2_5,                        //! Request pm 2.5
    WS_RT_PM10,                         //! Request pm 10
    WS_RT_PRESSURE,                     //! Request atmosphere pressure
    WS_RT_ILLUMINANCE_Q,                //! Request quality illuminance
    WS_RT_ILLUMINANCE,                  //! Request illuminance
    WS_RT_RAINFALL,                     //! Request level of rainfall
    WS_RT_SETSLAVEID,                   //! Request set new slave id
    WS_RT_SETBAUDRATE,                  //! Request set baud rate
    WS_RT_SETWINDDIRECTIONOFFSET,       //! Request set wind direction offset
    WS_RT_RESETWINDSPEED,               //! Request reset zero value of wind speed
    WS_RT_RESETRAINFALL                 //! Request reset of rainfall level
} weatherStationRequestType_t;

/**
 * @brief The WeatherStation class provide manage of weather station
 */
class WeatherStation : public ModBus::ModBusMasterSub
{
    Q_OBJECT

public:
    /**
     * @brief WeatherStation constructor of class
     * @param master pointer to mod bus master class
     * @param parent parent class (must be zero)
     */
    explicit WeatherStation(ModBus::ModBusMaster *master, QObject *parent = 0);

signals:
    /**
     * @brief stationError emmited when there is an error
     * @param errorType type of error (see weatherStationErrors_t)
     */
    void stationError(weatherStationErrors_t errorType);
    /**
     * @brief connectionSetuped emitted when there is connection with station has been setuped
     */
    void connectionSetuped();
    /**
     * @brief baudRate emitted when a respond on baud rate request
     * @param baudRate value of baud rate
     */
    void baudRate(uint16_t baudRate);
    /**
     * @brief windSpeed emitted when a respond on wind speed request
     * @param speed wind speed (m/s)
     */
    void windSpeed(float speed);
    /**
     * @brief windStrength emitted when a respond on wind strength request
     * @param level level of wind strength
     */
    void windStrength(uint16_t level);
    /**
     * @brief windDirection emitted when a respond on wind direction request
     * @param direction wind direction (cardinal direction)
     */
    void windDirection(QString direction);
    /**
     * @brief windDirectionGrad emitted when a respond on wind direction in grad request
     * @param grad angle of wind direction (°)
     */
    void windDirectionGrad(uint16_t grad);
    /**
     * @brief humidity emitted when a respond on humidity request
     * @param humidity humidity (% RH)
     */
    void humidity(float humidity);
    /**
     * @brief temperature emitted when a respond on temperature request
     * @param temp temperature (°C)
     */
    void temperature(float temp);
    /**
     * @brief noise emitted when a respond on level of noise request
     * @param noise level of noise (dB)
     */
    void noise(float noise);
    /**
     * @brief pm2_5 emitted when a respond on concentration of pm 2.5 request
     * @param concentration concentration (ug/m3)
     */
    void pm2_5(uint16_t concentration);
    /**
     * @brief pm10 emitted when a respond on concentration of pm 10 request
     * @param concentration concentration (ug/m3)
     */
    void pm10(uint16_t concentration);
    /**
     * @brief pressure emitted when a respond on atmosphere pressure request
     * @param pressure atmposphere pressure (kpa)
     */
    void pressure(float pressure);
    /**
     * @brief illuminance emitted when a respond on illuminance request
     * @param illuminance illuminance (Lux)
     */
    void illuminance(uint32_t illuminance);
    /**
     * @brief rainfall emitted when a respond on level of rainfall request
     * @param rainfall level of rainfall (mm)
     */
    void rainfall(float rainfall);
    /**
     * @brief setSlaveId emitted when a respond on set new slave id request
     * @param slaveId new slave id
     */
    void setSlaveId(uint8_t slaveId);
    /**
     * @brief setWindDirectionOffset emitted when a respond on set wind direction offset request
     * @param offset 0 - means normal direction
     *               1 - means the direction is offset by 180°
     */
    void setWindDirectionOffset(uint8_t offset);
    /**
     * @brief resetWindSpeed emitted when a respond on set zero of wind speed request
     */
    void resetWindSpeed();
    /**
     * @brief resetRainfall emitted when a respond on reset level of rainfall request
     */
    void resetRainfall();

public slots:
    /**
     * @brief requestSlaveIdSlot send request for get station slave id
     */
    void requestSlaveIdSlot();
    /**
     * @brief requestBaudRate send request for get station baud rate
     */
    void requestBaudRate();
    /**
     * @brief requestWindSpeed send request for get wind speed
     */
    void requestWindSpeed();
    /**
     * @brief requestWindStrength send request for get wind strength
     */
    void requestWindStrength();
    /**
     * @brief requestWindDirection send request for get wind direction (cardinal direction)
     */
    void requestWindDirection();
    /**
     * @brief requestWindDirectionGrad send request for get wind direction in grad
     */
    void requestWindDirectionGrad();
    /**
     * @brief requestHumidity send request for get humidity
     */
    void requestHumidity();
    /**
     * @brief requestTemperature send request for get temperature
     */
    void requestTemperature();
    /**
     * @brief requestNoise send request for get level of noise
     */
    void requestNoise();
    /**
     * @brief requestPM2_5 send request for get concentration of pm 2.5
     */
    void requestPM2_5();
    /**
     * @brief requestPM10 send request for get concentration of pm 10
     */
    void requestPM10();
    /**
     * @brief requestPressure send request for get atmosphere pressure
     */
    void requestPressure();
    /**
     * @brief requestIlluminanceQ send request for get quality illuminance
     */
    void requestIlluminanceQ();
    /**
     * @brief requestIlluminance send request for get illuminance
     */
    void requestIlluminance();
    /**
     * @brief requestRainfall send request for get level of rainfall
     */
    void requestRainfall();
    /**
     * @brief requestSetSlaveId send request for set new station slave id
     * @param slaveId new slave id (1-254)
     */
    void requestSetSlaveId(uint8_t slaveId);
    /**
     * @brief requestSetBaudRate send request for set new station baud rate
     * @param baudRate new baud rate (correct values: 2400, 4800, 9600)
     */
    void requestSetBaudRate(uint16_t baudRate);
    /**
     * @brief requestSetWindDirectionOffset send request for set wind direction offset
     * @param offset 0 - means normal direction
     *               1 - means the direction is offset by 180°
     */
    void requestSetWindDirectionOffset(uint8_t offset);
    /**
     * @brief requestResetZeroWindSpeed send request for set zero wind speed value
     */
    void requestResetZeroWindSpeed();
    /**
     * @brief requestResetRainfall send request for reset level of rainfall
     */
    void requestResetRainfall();

private slots:
    void modbusErrorSlot(ModBus::ModBusError mbErrorType);
    void transactionFinishedSlot(ModBus::mbTransaction_t *transaction);

private:
    int16_t unsignedToSigned(uint16_t value);

    QMap<int, weatherStationRequestType_t> requestsMap;
    uint8_t weatherStationSlaveId;
};

#endif // WEATHERSTATION_H
