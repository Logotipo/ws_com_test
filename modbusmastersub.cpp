#include "modbusmastersub.h"
#include "modbusmaster.h"
#include <arpa/inet.h>

ModBus::ModBusMasterSub::ModBusMasterSub(ModBus::ModBusMaster *master, QObject *parent)
    : QObject{parent}
{
    this->moveToThread(master->getEventThread());

    modbusMaster = master;
    lastExceptionText = "Undefined exception";
}

int ModBus::ModBusMasterSub::createRequest(mbFuncId_t fid, uint8_t slaveId, uint16_t valAddr, uint16_t value)
{
    return modbusMaster->createRequest(this, fid, slaveId, valAddr, value);
}

ModBus::ModBusError ModBus::ModBusMasterSub::checkError(ModBus::mbTransaction_t *transaction)
{
    ModBus::ModBusError error = ModBus::MB_ERROR_NONE;

    if (0 != transaction->rxFrame->hdr.err)
    {
        switch (transaction->rxFrame->exception.status)
        {
        case 1:
            error = ModBus::MB_ERROR_ILLEGAL_FUNCTION;
            lastExceptionText = "Illegal function";
            break;
        case 2:
            error = ModBus::MB_ERROR_ILLEGAL_DATA_ADDRESS;
            lastExceptionText = "Illegal data address";
            break;
        case 3:
            error = ModBus::MB_ERROR_ILLEGAL_DATA_VALUE;
            lastExceptionText = "Illegal data value";
            break;
        case 4:
            error = ModBus::MB_ERROR_SALVE_FAILURE;
            lastExceptionText = "Slave device failure";
            break;
        case 5:
            error = ModBus::MB_ERROR_ACKNOWLEDGE;
            lastExceptionText = "Acknowledge";
            break;
        case 6:
            error = ModBus::MB_ERROR_SLAVE_BUSY;
            lastExceptionText = "Slave device busy";
            break;
        case 7:
            error = ModBus::MB_ERROR_NEGATIVE_ACKNOWLEDGE;
            lastExceptionText = "Negative acknowledge";
            break;
        case 8:
            error = ModBus::MB_ERROR_MEMORY_PARITY;
            lastExceptionText = "Memory parity error";
            break;
        case 10:
            error = ModBus::MB_ERROR_GATEWAY_PATH;
            lastExceptionText = "Gateway path unavailable";
            break;
        case 11:
            error = ModBus::MB_ERROR_GATEWAY_RESPOND;
            lastExceptionText = "Gateway target device failed to respond";
            break;
        default:
            error = ModBus::MB_ERROR_UNDEFINED_EXCEPTION;
            lastExceptionText = "Undefined exception";
            break;
        }
    }

    return error;
}

void ModBus::ModBusMasterSub::swapByteOrder(ModBus::mbTransaction_t *transaction)
{
    int i = 0;

    if (0 != transaction->rxFrame->hdr.err || ModBus::MB_READ_EXCEPTION_STATUS_FID == transaction->txFrame->hdr.fid)
    {
        transaction->rxFrame->readExceptionResp.status = htons(transaction->rxFrame->readExceptionResp.status);
        transaction->rxFrame->readExceptionResp.crc = htons(transaction->rxFrame->readExceptionResp.crc);
    }
    else if (ModBus::MB_READ_HOLDING_REGISTERS_FID == transaction->txFrame->hdr.fid ||
             ModBus::MB_READ_INPUT_REGISTERS_FID == transaction->txFrame->hdr.fid)
    {
        for (i = 0; i < transaction->rxFrame->readRegsResp.bytesAmount / 2; i++)
        {
            transaction->rxFrame->readRegsResp.regs[i] = htons(transaction->rxFrame->readRegsResp.regs[i]);
        }
    }
    else if (ModBus::MB_FORCE_SINGLE_COIL_FID == transaction->txFrame->hdr.fid ||
             ModBus::MB_FORCE_SINGLE_REGISTER_FID == transaction->txFrame->hdr.fid)
    {
        transaction->rxFrame->writeRegResp.regAddr = htons(transaction->rxFrame->writeRegResp.regAddr);
        transaction->rxFrame->writeRegResp.regVal = htons(transaction->rxFrame->writeRegResp.regVal);
        transaction->rxFrame->writeRegResp.crc = htons(transaction->rxFrame->writeRegResp.crc);
    }
}
