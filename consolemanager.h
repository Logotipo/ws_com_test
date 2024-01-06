#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include <QObject>
#include "modbus.h"
#include "weatherstation.h"

namespace ModBus
{
    class ModBusMaster;
}
class QSocketNotifier;
class QStringList;

/**
 * @brief The ConsoleManager class provide work of terminal interface of management
 */
class ConsoleManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief ConsoleManager class constructor
     * @param parent pointer to parent class
     */
    explicit ConsoleManager(QObject *parent = 0);
    /**
     * @brief start start work
     */
    void start();

signals:
    /**
     * @brief modbusInit start of modbus initialisation process
     */
    void modbusInit();
    /**
     * @brief wsRequestSlaveId send request for get weather station slave id
     */
    void wsRequestSlaveId();
    /**
     * @brief getBaudRate send request for get baud rate
     */
    void getBaudRate();
    /**
     * @brief getWindSpeed send request for get wind speed
     */
    void getWindSpeed();
    /**
     * @brief getWindStrength send request for get wind strength
     */
    void getWindStrength();
    /**
     * @brief getWindDirection send request for get wind direction
     */
    void getWindDirection();
    /**
     * @brief getWindDirectionGrad send request for get wind direction in grad
     */
    void getWindDirectionGrad();
    /**
     * @brief getHumidity send request for get humidity
     */
    void getHumidity();
    /**
     * @brief getTemperature send request for get temperature
     */
    void getTemperature();
    /**
     * @brief getNoise send request for get noise
     */
    void getNoise();
    /**
     * @brief getPM2_5 send request for get pm 2.5 concentration
     */
    void getPM2_5();
    /**
     * @brief getPM10 send request for get pm 10 concentration
     */
    void getPM10();
    /**
     * @brief getPressure send request for get atmosphere pressure
     */
    void getPressure();
    /**
     * @brief getIlluminanceQ send request for get quality illuminance
     */
    void getIlluminanceQ();
    /**
     * @brief getIlliminance send request for get illuminance
     */
    void getIlliminance();
    /**
     * @brief getRainfall send request for get rainfall
     */
    void getRainfall();
    /**
     * @brief setSlaveId send request for set new slave id
     * @param slaveId new slave id (1-254)
     */
    void setSlaveId(uint8_t slaveId);
    /**
     * @brief setBaudRate send request for set new baud rate
     * @param baudRate baud rate (2400, 4800 or 9600)
     */
    void setBaudRate(uint16_t baudRate);
    /**
     * @brief setWindDirectionOffset  send request for set wind direction offset
     * @param offset offset (see WeatherStation class documentation)
     */
    void setWindDirectionOffset(uint8_t offset);
    /**
     * @brief resetZeroWindSpeed send request for set zero wind speed value
     */
    void resetZeroWindSpeed();
    /**
     * @brief resetRainfall send request for reset level of rainfall
     */
    void resetRainfall();

private slots:
    void readCommand();
    void portConfiguredSlot();
    void wsConfiguredSlot();
    void baudRateSlot(uint16_t baudRate);
    void windSpeedSlot(float speed);
    void windStrengthSlot(uint16_t level);
    void windDirectionSlot(QString direction);
    void windDirectionGradSlot(uint16_t grad);
    void humiditySlot(float humidity);
    void temperatureSlot(float temp);
    void noiseSlot(float noise);
    void pm2_5Slot(uint16_t concentration);
    void pm10Slot(uint16_t concentration);
    void pressureSlot(float pressure);
    void illuminanceSlot(uint32_t illuminance);
    void rainfallSlot(float rainfall);
    void setSlaveIdSlot(uint8_t slaveId);
    void setWindDirectionOffsetSlot(uint8_t offset);
    void resetWindSpeedSlot();
    void resetRainfallSlot();
    void modbusErrorSlot(ModBus::ModBusError errorCode);
    void wsErrorSlot(weatherStationErrors_t errorCode);

private:
    void startWeatherStationCommand();

    enum CommandType
    {
        COMMAND_NONE = 0,
        COMMAND_CHOOSE_DEVICE,
        COMMAND_CHOOSE_BAUDRATE,
        COMMAND_CHOOSE_WS_COMMAND,
        COMMAND_CHOOSE_WS_SLAVEID,
        COMMAND_CHOOSE_WS_BAUDRATE,
        COMMAND_CHOOSE_WS_WINDOFFSET
    };

    WeatherStation *weatherStation;
    QSocketNotifier *socketNotifier;
    ModBus::ModBusMaster *modbus;
    QStringList *deviceNames;
    CommandType currentCommand;
    QString deviceName;
};

#endif // CONSOLEMANAGER_H
