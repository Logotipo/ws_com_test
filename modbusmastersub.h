#ifndef MODBUSMASTERSUB_H
#define MODBUSMASTERSUB_H

#include <QObject>
#include "modbusmaster.h"

namespace ModBus
{

/**
 * @brief The ModBusMasterSub class provide subscribers functions
 */
class ModBusMasterSub : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief ModBusMasterSub class constructor
     * @param master pointer to master class
     * @param parent parent class (must be 0)
     */
    explicit ModBusMasterSub(ModBus::ModBusMaster *master, QObject *parent = 0);
    /**
     * @brief getLastException get last exception text
     * @return last exception text
     */
    inline QString getLastException() { return lastExceptionText; }

signals:
    /**
     * @brief error emitted on error
     * @param errorType error code (see ModBUs::ModBusError)
     */
    void error(ModBus::ModBusError errorType);
    /**
     * @brief transactionFinished emitted when transaction response has been recieved successfully
     * @param transaction pointer to transaction structure
     */
    void transactionFinished(ModBus::mbTransaction_t *transaction);

protected:
    /**
     * @brief createRequest create transaction to slave device
     * @param fid function id (see ModBus::mbFuncId_t)
     * @param slaveId slave id (1-255)
     * @param valAddr register/coil address
     * @param value value for write
     * @return internal transaction id
     */
    int createRequest(mbFuncId_t fid, uint8_t slaveId, uint16_t valAddr = 0, uint16_t value = 0);
    /**
     * @brief checkError check responce on errors/exception
     * @param transaction pointer to transaction structure
     * @return ModBus::MB_ERROR_NONE if responce has not errors/exception and error code if else
     */
    ModBus::ModBusError checkError(ModBus::mbTransaction_t *transaction);
    /**
     * @brief swapByteOrder swap byte order in responce
     * @param transaction pointer to transaction structure
     */
    void swapByteOrder(ModBus::mbTransaction_t *transaction);

private:
    ModBusMaster *modbusMaster;
    QString lastExceptionText;
};

}

#endif // MODBUSMASTERSUB_H
