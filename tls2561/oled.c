
/* Library functions for the SSD1306 display
* Slave address bit (SA0)
* SSD1306 has to recognize the slave address before transmitting or receiving any information by the
* I2C-bus. The device will respond to the slave address following by the slave address bit (“SA0” bit)
* and the read/write select bit (“R/W#” bit) with the following byte format,
* b7 b6 b5 b4 b3 b2 b1 b0
* 0 1 1 1 1 0 SA0 R/W#
* “SA0” bit provides an extension bit for the slave address. Either “0111100” or “0111101”, can be
* selected as the slave address of SSD1306. D/C# pin acts as SA0 for slave address selection.
* “R/W#” bit is used to determine the operati
*/
#include <stdint.h>
#include "font5x7.h"
#include "i2c.h"
uint8_t writeOLEDDataRegister(uint8_t RegNum,uint8_t Value)
{    
    I2CTransaction Transaction;    
    Transaction.Mode = 'w';
    // 0 1 1 1 1 0 1 
    Transaction.SlaveAddress = 0x3d;
    Transaction.TXCount = 2;
    Transaction.RXCount = 0;
    Transaction.TXData[0] = RegNum;
    Transaction.TXData[1] = Value;    
    return I2CDoTransaction(&Transaction);  
}
uint8_t readOLEDDataRegister(uint8_t RegNum)
{    
    I2CTransaction Transaction;    
    Transaction.Mode = 'r';
    Transaction.SlaveAddress = 0x3d;
    Transaction.TXCount = 1;
    Transaction.RXCount = 1;
    Transaction.TXData[0] = RegNum;  
    I2CDoTransaction(&Transaction);
    return Transaction.RXData[0];
}
uint8_t writeOLEDCmdRegister(uint8_t RegNum,uint8_t Value)
{    
    I2CTransaction Transaction;    
    Transaction.Mode = 'w';
    Transaction.SlaveAddress = 0x3c;
    Transaction.TXCount = 2;
    Transaction.RXCount = 0;
    Transaction.TXData[0] = RegNum;
    Transaction.TXData[1] = Value;    
    return I2CDoTransaction(&Transaction);  
}
uint8_t readOLEDCmdRegister(uint8_t RegNum)
{    
    I2CTransaction Transaction;    
    Transaction.Mode = 'r';
    Transaction.SlaveAddress = 0x3c;
    Transaction.TXCount = 1;
    Transaction.RXCount = 1;
    Transaction.TXData[0] = RegNum;  
    I2CDoTransaction(&Transaction);
    return Transaction.RXData[0];
}
uint8_t fillOLEDBytes(uint8_t RegNum, uint8_t Count, uint8_t Value)
{    
    I2CTransaction Transaction;    
    Transaction.Mode = 'w';
    Transaction.SlaveAddress = 0x3c;
    Transaction.TXCount = Count+1;
    Transaction.RXCount = 0;  
    int i;
    Transaction.TXData[0] = RegNum;
    for (i=0;i<Count;i++)
        Transaction.TXData[i+1] = Value;    
    return I2CDoTransaction(&Transaction);
}
uint8_t writeOLEDBytes(uint8_t RegNum, uint8_t Count, uint8_t *Values)
{    
    I2CTransaction Transaction;    
    Transaction.Mode = 'w';
    Transaction.SlaveAddress = 0x3c;
    Transaction.TXCount = Count+1;
    Transaction.RXCount = 0;  
    int i;
    Transaction.TXData[0] = RegNum;
    for (i=0;i<Count;i++)
        Transaction.TXData[i+1] = Values[i];    
    return I2CDoTransaction(&Transaction);
}
void resetOLED()
{
    // Reset sequence got from https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp
    writeOLEDCmdRegister(0x00,0xae);
    writeOLEDCmdRegister(0x00,0xd5);  
    writeOLEDCmdRegister(0x00,0x80);
    writeOLEDCmdRegister(0x00,0xa8);
    writeOLEDCmdRegister(0x00,63);
    writeOLEDCmdRegister(0x00,0xd3);
    writeOLEDCmdRegister(0x00,0);    
    writeOLEDCmdRegister(0x00,0x40);
    writeOLEDCmdRegister(0x00,0x8d);
    writeOLEDCmdRegister(0x00,0x14);
    writeOLEDCmdRegister(0x00,0x20);
    writeOLEDCmdRegister(0x00,0x00);
    writeOLEDCmdRegister(0x00,0xa1);
    writeOLEDCmdRegister(0x00,0xc8);
    
    writeOLEDCmdRegister(0x00,0xda);
    writeOLEDCmdRegister(0x00,0x12);
    writeOLEDCmdRegister(0x00,0x81);
    writeOLEDCmdRegister(0x00,0xcf);
    
    writeOLEDCmdRegister(0x00,0xd9);
    writeOLEDCmdRegister(0x00,0xf1);
    writeOLEDCmdRegister(0x00,0xdb);
    writeOLEDCmdRegister(0x00,0x40);
    writeOLEDCmdRegister(0x00,0xa4);
    writeOLEDCmdRegister(0x00,0xa6);
    writeOLEDCmdRegister(0x00,0x2e);
    writeOLEDCmdRegister(0x00,0xaf);
    
}

void clearOLEDLine(int LineNumber)
{
    writeOLEDCmdRegister(0x00,0x21);
    writeOLEDCmdRegister(0x00,0);
    writeOLEDCmdRegister(0x00,127);    
    writeOLEDCmdRegister(0x00,0x22);
    writeOLEDCmdRegister(0x00,LineNumber); // Page address
    writeOLEDCmdRegister(0x00,7);
    fillOLEDBytes(0x40,128,0x00);                
}
void clearDisplay()
{
    for (int i=0;i<8;i++)
        clearOLEDLine(i);
}
void writeOLEDLine(int LineNumber, uint8_t *Values)
{
    writeOLEDCmdRegister(0x00,0x21);
    writeOLEDCmdRegister(0x00,0);
    writeOLEDCmdRegister(0x00,127);    
    writeOLEDCmdRegister(0x00,0x22);
    writeOLEDCmdRegister(0x00,LineNumber); // Page address
    writeOLEDCmdRegister(0x00,7);
    writeOLEDBytes(0x40,128,Values);                
}
void writeOLEDText(int Col, int Row, const char *Text)
{
    int i;
    uint8_t RowData[128];
    for (i=0;i<128;i++)
        RowData[i]=0;
    while (*Text) {
        for (i=0;i<FONT_WIDTH;i++)
            RowData[Col*FONT_WIDTH+i]=Font5x7[FONT_WIDTH * ((*Text) - 32)+i];
        Col++;
        Text++;
    }
    writeOLEDLine(Row,RowData);
}
void int2Text(char *Text, uint32_t Value)
{
    int index;
    Text[10]=0;
    for (index = 0; index < 10;index++)
    {
        Text[9-index]=(Value % 10) + '0';
        Value = Value / 10;
    }   
}
void int2Hex(char *Hex, uint32_t Value)
{
    int temp;
    int index;
    Hex[8]=0;
    for (index = 0; index < 8;index++)
    {
        temp = Value % 16;
        if (temp < 10)
            temp = temp + '0';
        else
            temp = temp + 'A' - 10;
        Hex[7-index]=temp;
        Value = Value / 16;
    }
}
