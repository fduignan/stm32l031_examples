#include "i2c.h"
uint8_t TLS2561_readReg(uint8_t RegNum)
{   
    I2CTransaction Transaction;            
    Transaction.Mode = 'r';
    Transaction.SlaveAddress = 0x39;
    Transaction.TXCount = 1;
    Transaction.RXCount = 1;
    Transaction.TXData[0] = RegNum;  
    I2CDoTransaction(&Transaction);    
    return Transaction.RXData[0];
}
uint8_t TLS2561_writeReg(uint8_t RegNum, uint8_t Value)
{   
    I2CTransaction Transaction;            
    Transaction.Mode = 'w';
    Transaction.SlaveAddress = 0x39;
    Transaction.TXCount = 2;
    Transaction.RXCount = 0;
    Transaction.TXData[0] = RegNum;
    Transaction.TXData[1] = Value;     
    I2CDoTransaction(&Transaction);    
    return Transaction.RXData[0];
}
uint16_t TLS2561_readData(uint8_t ChanNum)
{
    uint16_t result;
    I2CTransaction Transaction;            
    Transaction.Mode = 'r';
    Transaction.SlaveAddress = 0x39;
    Transaction.TXCount = 1;
    Transaction.RXCount = 2;
    if (ChanNum == 0)
        Transaction.TXData[0] = 0x8c;
    if (ChanNum == 1)
        Transaction.TXData[0] = 0x8e;  
    I2CDoTransaction(&Transaction);  
    result=Transaction.RXData[1];
    result = result << 8;
    result += Transaction.RXData[0];
    return result;
}
