#include "modbusmaster.h"
#include "modbusmastersub.h"
#include <QThread>
#include <QTimer>
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <endian.h>

Q_DECLARE_METATYPE(ModBus::ModBusError)

ModBus::ModBusMaster::ModBusMaster(QString device, BaudRate br, QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<ModBus::ModBusError>();

    sendQueue = new QQueue<mbTransaction_t *>();
    sendQueue->clear();

    deviceName = device;
    baudRate = br;
    exchangeState = STATE_IDLE;
    lastTransactionId = 0;

    eventThread = new QThread();
    eventThread->start();
    this->moveToThread(eventThread);

    if (0 != (updateTimer = new QTimer()))
    {
        connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTimeout()));
        updateTimer->start(10);
    }
}

void ModBus::ModBusMaster::startInitSlot()
{
    exchangeState = STATE_INIT;
}

void ModBus::ModBusMaster::updateTimeout()
{
    struct termios portOptions;
    int result = 0;
    mbTransaction_t *transaction = 0;
    static uint8_t rxData[100];
    uint8_t *rxCopy;

    if (STATE_INIT == exchangeState)
    {
        exchangeState = STATE_ERROR;
        if (-1 != (deviceDescriptor = open(deviceName.toUtf8().data(), O_RDWR | O_NOCTTY | O_NONBLOCK)))
        {
            if (0 == tcgetattr(deviceDescriptor, &portOptions))
            {
                std::cout << "[ModBus] " << deviceName.toStdString() << " options:" << std::endl;
                std::cout << "         c_iflag 0x" << std::hex << portOptions.c_iflag << std::endl;
                std::cout << "         c_oflag 0x" << std::hex << portOptions.c_oflag << std::endl;
                std::cout << "         c_cflag 0x" << std::hex << portOptions.c_cflag << std::endl;
                std::cout << "         c_lflag 0x" << std::hex << portOptions.c_lflag << std::endl;
                std::cout << "         c_ispeed " << std::dec << portOptions.c_ispeed << std::endl;
                std::cout << "         c_ospeed " << std::dec << portOptions.c_ospeed << std::endl;


                switch(baudRate)
                {
                case BR_2400:
                    result = cfsetspeed(&portOptions, B2400);
                    break;
                case BR_4800:
                    result = cfsetspeed(&portOptions, B4800);
                    break;
                case BR_9600:
                default:
                    result = cfsetspeed(&portOptions, B9600);
                    break;
                }
                if (0 == result)
                {
                    portOptions.c_cflag &= ~PARENB;
                    portOptions.c_cflag &= ~CSTOPB;
                    portOptions.c_cflag &= ~CSIZE;
                    portOptions.c_cflag |= CS8 | CLOCAL | CREAD;
                    portOptions.c_lflag |= ICANON;

                    if (0 <= tcsetattr(deviceDescriptor, TCSANOW, &portOptions))
                    {
                        std::cout << "[ModBus] Port has been configured success!" << std::endl;
                        exchangeState = STATE_IDLE;
                        emit portConfigured();
                    }
                    else
                    {
                        std::cout << "[Modbus] Can`t set port options!" << std::endl;
                        emit error(MB_ERROR_SETOPTION);
                    }
                }
                else
                {
                    std::cout << "[ModBus] Can`t set speed!" << std::endl;
                    emit error(MB_ERROR_SETSPEED);
                }
            }
            else
            {
                std::cout << "[ModBus] Can`t get port options!" << std::endl;
                emit error(MB_ERROR_PORTOPTION);
            }
        }
        else
        {
            std::cout << "[ModBus] Can`t create device descriptor!" << std::endl;
            emit error(MB_ERROR_DESCRIPTOR);
        }
    }
    else if (STATE_TRANSMIT == exchangeState)
    {
        if (!sendQueue->isEmpty())
        {
            if (0 != (transaction = sendQueue->head()))
            {
                if (transaction->txSize != write(deviceDescriptor, transaction->txFrame->uint8, transaction->txSize))
                {
                    std::cout << "[ModBus] Write len != transactionSize!" << std::endl;
                    sendQueue->dequeue();
                    if (sendQueue->isEmpty())
                        exchangeState = STATE_IDLE;
                    emit transaction->sub->error(MB_ERROR_TRANSMIT);

                    delete transaction->txFrame;
                    delete transaction;
                }
                else
                {
                    readTryCount = 0;
                    memset(rxData, 0, sizeof(rxData));
                    exchangeState = STATE_RECEIVE;
                }
            }
            else
            {
                std::cout << "[ModBus] Transaction data has been corrupted!" << std::endl;
                sendQueue->dequeue();
                if (sendQueue->isEmpty())
                    exchangeState = STATE_IDLE;
            }
        }
        else
        {
            std::cout << "[ModBus] Send queue is empty!" << std::endl;
            exchangeState = STATE_IDLE;
        }
    }
    else if (STATE_RECEIVE == exchangeState)
    {
        if (!sendQueue->isEmpty())
        {
            if (0 != (transaction = sendQueue->head()))
            {
                if (-1 != (result = read(deviceDescriptor, &rxData[transaction->countReadBytes], sizeof(rxData))))
                {
                    transaction->countReadBytes += result;
                    if (transaction->countReadBytes >= transaction->rxSize)
                    {
                        rxCopy = new uint8_t[transaction->rxSize];
                        memcpy(rxCopy, rxData, transaction->rxSize);
                        mbFrame_t *frame = reinterpret_cast<mbFrame_t *>(rxCopy);
                        transaction->rxFrame = frame;
                        transaction->crcCheck = checkCRC(rxData, transaction->rxSize);
                        emit transaction->sub->transactionFinished(transaction);
                        sendQueue->dequeue();

                        if (sendQueue->isEmpty())
                            exchangeState = STATE_IDLE;
                        else
                            exchangeState = STATE_TRANSMIT;
                    }
                    else if (false == transaction->errorChecked && transaction->countReadBytes >= sizeof(ModBus::mbException_t))
                    {
                        transaction->errorChecked = true;
                        if (0 != reinterpret_cast<mbException_t *>(&rxData[0])->err)
                        {
                            rxCopy = reinterpret_cast<uint8_t *>(new ModBus::mbException_t);
                            memcpy(rxCopy, rxData, sizeof(ModBus::mbException_t));
                            mbFrame_t *frame = reinterpret_cast<mbFrame_t *>(rxCopy);
                            transaction->rxFrame = frame;
                            transaction->crcCheck = checkCRC(rxData, sizeof(ModBus::mbException_t));
                            emit transaction->sub->transactionFinished(transaction);
                            sendQueue->dequeue();

                            if (sendQueue->isEmpty())
                                exchangeState = STATE_IDLE;
                            else
                                exchangeState = STATE_TRANSMIT;
                        }
                    }
                    else if (readTryCount++ > 10)
                    {
                        std::cout << "[ModBus] Read timeout!" << std::endl;
                        sendQueue->dequeue();
                        if (sendQueue->isEmpty())
                            exchangeState = STATE_IDLE;
                        else
                            exchangeState = STATE_TRANSMIT;
                        emit transaction->sub->error(MB_ERROR_RECEIVE_TIMEOUT);

                        delete transaction->txFrame;
                        delete transaction;
                    }
                }
                else if (EAGAIN == errno)
                {
                    if (readTryCount++ > 10)
                    {
                        std::cout << "[ModBus] Read timeout!!" << std::endl;
                        sendQueue->dequeue();
                        if (sendQueue->isEmpty())
                            exchangeState = STATE_IDLE;
                        else
                            exchangeState = STATE_TRANSMIT;
                        emit transaction->sub->error(MB_ERROR_RECEIVE_TIMEOUT);

                        delete transaction->txFrame;
                        delete transaction;
                    }
                }
                else
                {
                    std::cout << "[ModBus] Read error!" << std::endl;
                    sendQueue->dequeue();
                    if (sendQueue->isEmpty())
                        exchangeState = STATE_IDLE;
                    else
                        exchangeState = STATE_TRANSMIT;
                    emit transaction->sub->error(MB_ERROR_RECEIVE);

                    delete transaction->txFrame;
                    delete transaction;
                }
            }
            else
            {
                std::cout << "[ModBus] Transaction data has been corrupted!" << std::endl;
                sendQueue->dequeue();
                if (sendQueue->isEmpty())
                    exchangeState = STATE_IDLE;
                else
                    exchangeState = STATE_TRANSMIT;
            }
        }
        else
        {
            std::cout << "[ModBus] Send queue is empty!" << std::endl;
            exchangeState = STATE_IDLE;
        }
    }
}

int ModBus::ModBusMaster::createRequest(ModBusMasterSub *sub, mbFuncId_t fid, uint8_t slaveId, uint16_t valAddr, uint16_t value)
{
    mbTransaction_t *transaction = 0;

    if (sendQueue->size() < 127)
    {
        if (0 != (transaction = new mbTransaction_t))
        {
            if (0 != (transaction->txFrame = reinterpret_cast<mbFrame_t *>(new mbReadRegsReq_t)))
            {
                transaction->sub = sub;
                transaction->errorChecked = false;
                transaction->txFrame->hdr.addr = slaveId;
                transaction->txFrame->hdr.fid = fid;

                switch (fid)
                {
                case MB_READ_HOLDING_REGISTERS_FID:
                case MB_READ_INPUT_REGISTERS_FID:
                    fillReadRegsTransaction(transaction, valAddr, value);
                    break;
                case MB_FORCE_SINGLE_COIL_FID:
                case MB_FORCE_SINGLE_REGISTER_FID:
                    fillWriteSingleValueTransaction(transaction, valAddr, value);
                    break;
                case MB_READ_EXCEPTION_STATUS_FID:
                    fillReadStatus(transaction);
                    break;
                default:
                    delete transaction->txFrame;
                    delete transaction;
                    std::cout << "[ModBus] Unsupported function ID!" << std::endl;
                    return -1;
                }

                transaction->transactionId = lastTransactionId;
                lastTransactionId++;
                sendQueue->enqueue(transaction);

                if (STATE_IDLE == exchangeState)
                    exchangeState = STATE_TRANSMIT;

                return transaction->transactionId;
            }
            else
            {
                delete transaction;
                return -1;
            }
        }
        else return -1;
    }
    else return -1;
}

void ModBus::ModBusMaster::fillReadRegsTransaction(mbTransaction_t *transaction, uint16_t regAddr, uint16_t regsAmount)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    transaction->txFrame->readRegsReq.regAddr = htons(regAddr);
    transaction->txFrame->readRegsReq.regsAmount = htons(regsAmount);
    transaction->txFrame->readRegsReq.crc = htons(crcCalc(transaction->txFrame->uint8, sizeof(mbReadRegsReq_t) - 2));
#elif __BYTE_ORDER == __BIG_ENDIAN
    transaction->txFrame->readRegsReq.regAddr = regAddr;
    transaction->txFrame->readRegsReq.regsAmount = regsAmount;
    transaction->txFrame->readRegsReq.crc = crcCalc(transaction->txFrame->uint8, sizeof(mbReadRegsReq_t) - 2);
#else
    #error("Unknown byte order!")
#endif
    transaction->txSize = sizeof(mbReadRegsReq_t);
    transaction->rxSize = sizeof(mbReadRegsResp_t) + (sizeof(uint16_t) * regsAmount);
}

void ModBus::ModBusMaster::fillWriteSingleValueTransaction(mbTransaction_t *transaction, uint16_t addr, uint16_t value)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    transaction->txFrame->writeRegReq.regAddr = htons(addr);
    transaction->txFrame->writeRegReq.regVal = htons(value);
    transaction->txFrame->writeRegReq.crc = htons(crcCalc(transaction->txFrame->uint8, sizeof(mbWriteRegReq_t) - 2));
#elif __BYTE_ORDER == __BIG_ENDIAN
    transaction->txFrame->writeRegReq.regAddr = addr;
    transaction->txFrame->writeRegReq.regVal = value;
    transaction->txFrame->writeRegReq.crc = crcCalc(transaction->txFrame->uint8, sizeof(mbWriteRegReq_t) - 2);
#else
    #error("Unknown byte order!")
#endif
    transaction->txSize = sizeof(mbWriteRegReq_t);
    transaction->rxSize = sizeof(mbWriteRegResp_t);
}

void ModBus::ModBusMaster::fillReadStatus(mbTransaction_t *transaction)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    transaction->txFrame->readExceptionReq.crc = htons(crcCalc(transaction->txFrame->uint8, sizeof(mbWriteRegReq_t) - 2));
#elif __BYTE_ORDER == __BIG_ENDIAN
    transaction->txFrame->readExceptionReq.crc = crcCalc(transaction->txFrame->uint8, sizeof(mbWriteRegReq_t) - 2);
#else
    #error("Unknown byte order!")
#endif
    transaction->txSize = sizeof(mbReadExceptionReq_t);
    transaction->rxSize = sizeof(mbReadExceptionResp_t);
}

bool ModBus::ModBusMaster::checkCRC(uint8_t *buf, uint16_t len)
{
    uint16_t calcCRC = crcCalc(buf, len - 2);
    uint16_t messageCRC = (buf[len - 2] << 8) | buf[len - 1];
    return (calcCRC == messageCRC);
}

uint16_t ModBus::ModBusMaster::crcCalc(uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint16_t polynom = 0xA001;
    uint16_t i = 0;
    uint16_t j = 0;

    for (i = 0; i < len; i++)
    {
        crc = crc ^ buf[i];
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc = crc >> 1;
                crc = crc ^ polynom;
            }
            else
            {
                crc = crc >> 1;
            }
        }
    }
    return(crc & 0xFFFF);
}
