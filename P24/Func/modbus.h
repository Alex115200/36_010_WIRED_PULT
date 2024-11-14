#ifndef MODBUS_H
#define MODBUS_H

//===================================================================
// ******     Œœ–≈ƒ≈À≈Õ»ﬂ ‘”Õ ÷»… MODBUS   ******
//===================================================================
#define READ_H_REGS 0x03              // Read Holding Registers
#define READ_I_REGS 0x04              // Read Input Registers
#define WRITE_COIL  0x05              // Write Single Coil
#define WRITE_REG   0x06              // Write Single Register
#define WRITE_REGS  0x10              // Write Multiple Registers
#define READ_DEV_ID 0x2B              // Read Device Identification
#define READ_FILE   0x68              // Read File from Flash
#define WRITE_FILE  0x69              // Write File to Flash
//===================================================================

extern unsigned char DataBuf[256];

//===================================================================
//                      œÓÚÓÚËÔ˚
//===================================================================
u8 ModbusFunc(MsgReqDrvIn_type *mfMsgReqDrvIn, void *Buf );
//===================================================================

#endif  // MODBUS_H
