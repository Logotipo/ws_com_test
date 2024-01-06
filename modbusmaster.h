#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H

#include <QObject>
#include <QQueue>
#include "modbus.h"

class QThread;
class QTimer;
struct termios;

namespace ModBus
{
class ModBusMasterSub;

typedef struct _mbTransaction_t
{
    mbFrame_t *txFrame;                 //!< Pointer to transmit frame
    mbFrame_t *rxFrame;                 //!< Pointer to receive frame
    uint8_t txSize;                     //!< Size of transmit frame
    uint8_t rxSize;                     //!< Size of receive frame
    uint8_t countReadBytes;             //!< Amount received bytes
    uint8_t transactionId;              //!< Embedded transaction id
    ModBusMasterSub *sub;               //!< Pointer to current slave device class
    bool errorChecked;                  //!< Error has been checked: false - transaction is not checked for errors
                                        //!<                         true - transaction is checked for errors
    bool crcCheck;                      //!< CRC is verified: false - incorrect CRC
                                        //!<                  true - correct CRC
} mbTransaction_t;

/**
 * @brief The ModBusMaster class provide master modbus device functions
 */
class ModBusMaster : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief ModBusMaster class constructor
     * @param device path to serial port device (for ex. /dev/ttyUSB0)
     * @param br baud rate (see ModBus::BaudRate)
     * @param parent parent class (must be 0)
     */
    explicit ModBusMaster(QString device, BaudRate br, QObject *parent = 0);
    /**
     * @brief getEventThread get pointer to event thread
     * @return pointer to event thread
     */
    inline QThread *getEventThread() { return eventThread; }
    /**
     * @brief createRequest create transaction to slave device
     * @param sub pointer to class, which provide subscribers functions
     * @param fid function id (see ModBus::mbFuncId_t)
     * @param slaveId slave id (1-255)
     * @param valAddr register/coil address
     * @param value value for write
     * @return internal transaction id
     */
    int createRequest(ModBusMasterSub *sub, mbFuncId_t fid, uint8_t slaveId, uint16_t valAddr = 0, uint16_t value = 0);

signals:
    /**
     * @brief error emitted on general error
     * @param error error code (see ModBus::ModBusError)
     */
    void error(ModBus::ModBusError error);
    /**
     * @brief portConfigured emitted when serial port has been configured
     */
    void portConfigured();

public slots:
    /**
     * @brief startInitSlot start serial port configure
     */
    void startInitSlot();

private slots:
    void updateTimeout();

private:
    uint16_t crcCalc(uint8_t *buf, uint16_t len);
    bool checkCRC(uint8_t *buf, uint16_t len);
    void fillReadRegsTransaction(mbTransaction_t *transaction, uint16_t regAddr, uint16_t regsAmount);
    void fillWriteSingleValueTransaction(mbTransaction_t *transaction, uint16_t addr, uint16_t value);
    void fillReadStatus(mbTransaction_t *transaction);

    enum ExchangeState
    {
        STATE_INIT = 0,
        STATE_TRANSMIT,
        STATE_RECEIVE,
        STATE_IDLE,
        STATE_ERROR
    };

    QThread *eventThread;
    QTimer *updateTimer;
    QQueue<mbTransaction_t *> *sendQueue;

    BaudRate baudRate;
    QString deviceName;
    ExchangeState exchangeState;
    int deviceDescriptor;
    uint8_t readTryCount;
    uint8_t lastTransactionId;
};

}

#endif // MODBUSMASTER_H
