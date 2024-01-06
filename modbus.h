#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>

namespace ModBus
{
enum BaudRate
{
    BR_2400 = 0,                                        //!< Baud rate is 2400 baud
    BR_4800,                                            //!< Baud rate is 4800 baud
    BR_9600                                             //!< Baud rate is 9600 baud
};

enum ModBusError
{
    MB_ERROR_NONE = 0,                                  //!< Without error
    MB_ERROR_DESCRIPTOR,                                //!< Failed to create port descriptor
    MB_ERROR_PORTOPTION,                                //!< Failed to get port option
    MB_ERROR_SETSPEED,                                  //!< Failed to set port speed
    MB_ERROR_SETOPTION,                                 //!< Failed to set port option
    MB_ERROR_TRANSMIT,                                  //!< Failed to transmit message
    MB_ERROR_RECEIVE,                                   //!< Failed to receive message
    MB_ERROR_RECEIVE_TIMEOUT,                           //!< Receive message timeout
    MB_ERROR_ILLEGAL_FUNCTION,                          //!< The function code received in the request is not an authorized
                                                        //!< action for the slave. The slave may be in the wrong state to process
                                                        //!< a specific request.
    MB_ERROR_ILLEGAL_DATA_ADDRESS,                      //!< The data address received by the slave is not an authorized address
                                                        //!< for the slave.
    MB_ERROR_ILLEGAL_DATA_VALUE,                        //!< The value in the request data field is not an authorized value for the slave.
    MB_ERROR_SALVE_FAILURE,                             //!< The slave fails to perform a requested action because of an unrecoverable error.
    MB_ERROR_ACKNOWLEDGE,                               //!< The slave accepts the request but needs a long time to process it.
    MB_ERROR_SLAVE_BUSY,                                //!< The slave is busy processing another command. The master must send
                                                        //!< the request once the slave is available.
    MB_ERROR_NEGATIVE_ACKNOWLEDGE,                      //!< The slave cannot perform the programming request sent by the master.
    MB_ERROR_MEMORY_PARITY,                             //!< The slave detects a parity error in the memory when attempting to
                                                        //!< read extended memory.
    MB_ERROR_GATEWAY_PATH,                              //!< The gateway is overloaded or not correctly configured.
    MB_ERROR_GATEWAY_RESPOND,                           //!< The slave is not present on the network.
    MB_ERROR_UNDEFINED_EXCEPTION                        //!< Undefined exception reason code.
};

typedef enum _mbFuncId_t
{
    MB_READ_COIL_STATUS_FID                     = 0x01, //!< Get current state (ON/OFF) group of coils
    MB_READ_INPUT_STATUS_FID                    = 0x02, //!< Get current state (ON/OFF) group of discrete inputs
    MB_READ_HOLDING_REGISTERS_FID               = 0x03, //!< Get current values of holing registers
    MB_READ_INPUT_REGISTERS_FID                 = 0x04, //!< Get current values of input registers
    MB_FORCE_SINGLE_COIL_FID                    = 0x05, //!< Change state of coil (ON/OFF)
    MB_FORCE_SINGLE_REGISTER_FID                = 0x06, //!< Write new value in holding register
    MB_READ_EXCEPTION_STATUS_FID                = 0x07, //!< Get state (ON/OFF) of 8 embedded logic cells, which
                                                        //!< purpose depends by type of controller
    MB_LOOPBACK_DIAGNOSTIC_TEST_FID             = 0x08, //!< Test message, which sent to slave for get infornation about connection
    MB_FETCH_EVENT_COUNTER_COMMUNICATIONS_FID   = 0x0B, //!< Get communication event counter
    MB_FETCH_COMMUNICATIONS_EVENT_LOG_FID       = 0x0C, //!< Get communication log with all transaction for this slave device
    MB_PROGRAM_FID                              = 0x0D, //!< Request to programming slave device
    MB_POLL_PROGRAM_COMPLETE_FID                = 0x0E, //!< Request about end of programming operation
    MB_FORCE_MULTIPLE_COILS_FID                 = 0x0F, //!< Change state (ON/OFF) multiple serial logic cells
    MB_FORCE_MULTIPLE_REGISTERS_FID             = 0x10, //!< Write new value multiple serial registers
    MB_REPORT_SLAVE_ID_FID                      = 0x11, //!< Get state and working staus of slave device
    MB_RESET_COMMUNICATIONS_LINK_FID            = 0x12, //!< Reset slave device after critical error
} mbFuncId_t;

#pragma pack(1)

//! Structure of read registers request (function ids 0x03 and 0x04)
typedef struct _mbReadRegsReq_t
{
    uint8_t addr;                                       //!< Slave address
    uint8_t fid;                                        //!< Function Id
    uint16_t regAddr;                                   //!< Start register address (big endian)
    uint16_t regsAmount;                                //!< Registers amount (big endian)
    uint16_t crc;                                       //!< Message CRC (big endian)
} mbReadRegsReq_t;

//! Structure of read registers responce (function ids 0x03 and 0x04)
typedef struct _mbReadRegsResp_t
{
    uint8_t addr;                                       //!< Slave address
    uint8_t fid     : 7;                                //!< Function Id
    uint8_t err     : 1;                                //!< Error flag
    uint8_t bytesAmount;                                //!< Amount bytes of responce
    uint16_t regs[1];                                   //!< Registers values (big endian)
} mbReadRegsResp_t;

//! Structure of write signle coil or register request (function ids 0x05 and 0x06)
typedef struct _mbWriteRegReq_t
{
    uint8_t addr;                                       //!< Slave address
    uint8_t fid;                                        //!< Function Id
    uint16_t regAddr;                                   //!< Register/coil address (big endian)
    uint16_t regVal;                                    //!< Register/coil value (big endian)
    uint16_t crc;                                       //!< Message CRC (big endian)
} mbWriteRegReq_t;

//! Structure of write signle coil or register responce (function ids 0x05 and 0x06)
typedef struct _mbWriteRegResp_t
{
    uint8_t addr;                                       //!< Slave address
    uint8_t fid     : 7;                                //!< Function Id
    uint8_t err     : 1;                                //!< Error flag
    uint16_t regAddr;                                   //!< Register/coil address (big endian)
    uint16_t regVal;                                    //!< Register/cois value (big endian)
    uint16_t crc;                                       //!< Message CRC (big endian)
} mbWriteRegResp_t;

//! Structure of read status register request (function id 0x07)
typedef struct _mbReadExceptionReq_t
{
    uint8_t addr;                                       //!< Slave address
    uint8_t fid;                                        //!< Function Id
    uint16_t crc;                                       //!< Message CRC (big endian)
} mbReadExceptionReq_t;

//! Structure of read status register responce (function id 0x07)
typedef struct _mbReadExceptionResp_t
{
    uint8_t addr;                                       //!< Slave address
    uint8_t fid;                                        //!< Function Id
    uint8_t status;                                     //!< Value of status register
    uint16_t crc;                                       //!< Message CRC (big endian)
} mbReadExceptionResp_t;

//! Structure of message with exception
typedef struct _mbException_t
{
    uint8_t addr;                                       //!< Slave address
    uint8_t fid     : 7;                                //!< Function Id
    uint8_t err     : 1;                                //!< Error flag
    uint8_t status;                                     //!< Code of exception
    uint16_t crc;                                       //!< Message CRC (big endian)
} mbException_t;

typedef union
{
    struct
    {
        uint8_t addr;                                   //!< Slave address
        uint8_t fid     : 7;                            //!< Function Id
        uint8_t err     : 1;                            //!< Error flag
    } hdr;                                              //!< Header of modbus message
    mbReadRegsReq_t readRegsReq;                        //!< Structure of read registers request (function ids 0x03 and 0x04)
    mbReadRegsResp_t readRegsResp;                      //!< Structure of read registers responce (function ids 0x03 and 0x04)
    mbWriteRegReq_t writeRegReq;                        //!< Structure of write signle coil or register request (function ids 0x05 and 0x06)
    mbWriteRegReq_t writeRegResp;                       //!< Structure of write signle coil or register responce (function ids 0x05 and 0x06)
    mbException_t exception;                            //!< Structure of message with exception
    mbReadExceptionReq_t readExceptionReq;              //!< Structure of read status register request (function id 0x07)
    mbReadExceptionResp_t readExceptionResp;            //!< Structure of read status register responce (function id 0x07)
    uint8_t uint8[100];
} mbFrame_t;

#pragma pack()
}

#endif // MODBUS_H
