#include <stdint.h>
#include "timer.h"
// Assumption: Top left of display is 0,0
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135
// drawn off-screen which is a great help to scroller games possibly
// Define a macro to allow easy definition of colours
// Format of colour value: <BGND 1 bit><Red 5 bits><Green 5 bits><Blue 5 bits>
//#define RGBToWord( R,  G,  B)  (  ((G&0xf8) << (11-3)) | ((R&0xfc) << (5-2)) | ((B&0xf8)>>3) )
#define RGBToWord( R,  G,  B)  (  ((R&0xf8) << (11-3)) | ((B&0xfc) << (5-2)) | ((G&0xf8)>>3) )
#define DISP_STR(str) str,sizeof(str)-1
class display {
public:
    display() {};
    int begin(timer *t);
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Colour);
    void drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour);
    void putPixel(uint16_t x, uint16_t y, uint16_t colour);
    void BacklightOn();
    void BacklightOff();
    void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour);
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour);
    void fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour);
    void putImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t * img);
    void print(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);
    void print(uint16_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);
private:    
    uint8_t transferSPI8(uint8_t data);
    uint16_t transferSPI16(uint16_t data);
    void RSLow();
    void RSHigh();
    void RSTLow();
    void RSTHigh();
    void openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);        
    void writeCommand(uint8_t Cmd);
    void writeData8(uint8_t Data);
    void writeData16(uint16_t Data);    
    void drawLineHighSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour);
    void drawLineLowSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour);   
    int iabs(int x) // simple integer version of abs for use by graphics functions        
    
    {
        if (x < 0)
            return -x;
        else
            return x;
    }
};
