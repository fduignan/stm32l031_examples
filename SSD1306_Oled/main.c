
// Test program for SSD1306 display controller with a 128x16 OLED display
/* The program drives the display in text mode only (graphics later)
 *   The font is 5 pixels wide by 8 pixels high. In character terms, the screen
 *   can display 25.6 x 8 characters. (i.e. 25x8 characters)
 *   The display is connected to the I2C bus.  The I2C driver has an internal
 *   buffer that can accommodate up to 128 data bytes.  It writes
 *   to the OLED on an interrupt driven basis however, in an effort to avoid
 *   over-runs, the I2C transactions block until complete.
 *   The reset sequence was deduced from the Adafruit_SSD1306 driver.  Unlike
 *   the adafruit driver however, this example does not use a large amount of 
 *   RAM for a frame buffer.  It uses a 128 (actually 130) byte I2C buffer for
 *   transmission only.
 *   In this example, the screen is updated line by line - each line being 8
 *   rows of pixels or one character high.
 */


#include <stdint.h>
#include "stm32l031.h"
#include "i2c.h"
#include "font5x7.h"
void delay(volatile uint32_t dly)
{
    while (dly--);
}
void configPins()
{
    RCC_IOPENR |= BIT2; // enable port C
    GPIOC_MODER &= ~BIT29; // make PC14 an output
    GPIOC_MODER |= BIT28;
}
uint8_t writeOLEDRegister(uint8_t RegNum, uint8_t Value)
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
void resetOLED()
{
    // Reset sequence got from https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp
    
    writeOLEDRegister(0x00, 0xae);
    writeOLEDRegister(0x00, 0xd5);
    writeOLEDRegister(0x00, 0x80);
    writeOLEDRegister(0x00, 0xa8);
    writeOLEDRegister(0x00, 63);
    writeOLEDRegister(0x00, 0xd3);
    writeOLEDRegister(0x00, 0);
    writeOLEDRegister(0x00, 0x40);
    writeOLEDRegister(0x00, 0x8d);
    writeOLEDRegister(0x00, 0x14);
    writeOLEDRegister(0x00, 0x20);
    writeOLEDRegister(0x00, 0x00);
    writeOLEDRegister(0x00, 0xa1);
    writeOLEDRegister(0x00, 0xc8);
    
    writeOLEDRegister(0x00, 0xda);
    writeOLEDRegister(0x00, 0x12);
    writeOLEDRegister(0x00, 0x81);
    writeOLEDRegister(0x00, 0xcf);
    
    writeOLEDRegister(0x00, 0xd9);
    writeOLEDRegister(0x00, 0xf1);
    writeOLEDRegister(0x00, 0xdb);
    writeOLEDRegister(0x00, 0x40);
    writeOLEDRegister(0x00, 0xa4);
    writeOLEDRegister(0x00, 0xa6);
    writeOLEDRegister(0x00, 0x2e);
    writeOLEDRegister(0x00, 0xaf);
}


uint8_t fillOLEDBytes(uint8_t RegNum, uint8_t Count, uint8_t Value)
{
    // Repeatedly writes the given Value to the OLED memory - useful
    // for clearing the display
    I2CTransaction Transaction;
    Transaction.Mode = 'w';
    Transaction.SlaveAddress = 0x3c;
    Transaction.TXCount = Count + 1;
    Transaction.RXCount = 0;
    int i;
    Transaction.TXData[0] = RegNum;
    for (i = 0; i < Count; i++)
    {
        Transaction.TXData[i + 1] = Value;
    }
    return I2CDoTransaction(&Transaction);
}
uint8_t writeOLEDBytes(uint8_t RegNum, uint8_t Count, uint8_t *Values)
{
    // Writes tje array of up to 128 bytes to the OLED display
    if (Count > 128)
        return -1;
    I2CTransaction Transaction;
    Transaction.Mode = 'w';
    Transaction.SlaveAddress = 0x3c;
    Transaction.TXCount = Count + 1;
    Transaction.RXCount = 0;
    int i;
    Transaction.TXData[0] = RegNum;
    for (i = 0; i < Count; i++)
    {
        Transaction.TXData[i + 1] = Values[i];
    }
    return I2CDoTransaction(&Transaction);
}
void clearOLEDLine(int LineNumber)
{
    // Clears the given line (range 0 to 7) on the display
    // A line is 8 pixels high
    writeOLEDRegister(0x00, 0x21);
    writeOLEDRegister(0x00, 0);
    writeOLEDRegister(0x00, 127);
    writeOLEDRegister(0x00, 0x22);
    writeOLEDRegister(0x00, LineNumber); // Page address
    writeOLEDRegister(0x00, 7);
    fillOLEDBytes(0x40, 128, 0x00);
}
void clearDisplay()
{
    for (int i = 0; i < 8; i++)
    {
        clearOLEDLine(i);
    }
}
void writeOLEDLine(int LineNumber, uint8_t *Values)
{
    // Writes the set of values to the given line number
    writeOLEDRegister(0x00, 0x21);
    writeOLEDRegister(0x00, 0);
    writeOLEDRegister(0x00, 127);
    writeOLEDRegister(0x00, 0x22);
    writeOLEDRegister(0x00, LineNumber); // Page address
    writeOLEDRegister(0x00, 7);
    writeOLEDBytes(0x40, 128, Values);
}
void writeOLEDText(int Col, int Row, const char *Text)
{
    // write the supplied text (up to 25 bytes) to the given display line
    int i;
    uint8_t RowData[128];
    for (i = 0; i < 128; i++)
        RowData[i] = 0;
    while (*Text) {
        for (i = 0; i < FONT_WIDTH; i++)
        {
            RowData[Col * FONT_WIDTH + i] = Font5x7[FONT_WIDTH * ((*Text) - 32) + i];
        }
        Col++;
        Text++;
        if (Col > 24)
            break; // Can't print past end of the screen
    }
    writeOLEDLine(Row, RowData);
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
int main()
{
    
    uint32_t i=0;
    char TextString[12];
    configPins();
    initI2C();
    enable_interrupts();
    resetOLED();
    clearDisplay();  
    writeOLEDText(0,0,"Hello World :-)");
    
    while (1)
    {        
        int2Text(TextString,i);
        writeOLEDText(0,2,TextString);
        int2Hex(TextString,i);
        writeOLEDText(0,4,TextString);
        i++;
    }
    
}
