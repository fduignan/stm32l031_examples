#include "display.h"
#include "timer.h"
#include "STM32L0x3.h"
#include "font5x7.h"
/* Pin usage
 * SCL -> Pin 11 (PA5)
 * SDA -> Pin 13 (PA7)
 * RES -> Pin 17 (PA9)
 * DC  -> Pin 12 (PA6)
 * CS  -> Pin 10 (PA4)
 * BLK -> Pin 18 (PA10) Configured as a simple output here however it could be routed to TIM22 and some PWM done
*/
int display::begin(timer *t)
{
    /* Set up the SPI hardware for the display */
    RCC->IOPENR |= BIT0;
    // Need to set up alternate functions for PA4,5 and 7 (SS,SCL,MOSI/SDA)
    // PA6, PA9, PA10 are to be just outputs
    GPIOA->MODER &= ~(BIT8+BIT9+BIT10+BIT11+BIT12+BIT13+BIT14+BIT15+BIT18+BIT19+BIT20+BIT21);
    GPIOA->MODER |= (      BIT9+      BIT11+BIT12+            BIT15+BIT18+       BIT20);
    // SS = AF0, SCL = AF0, MOSI = AF0
    GPIOA->AFRL &= (0x0f00ffff);
    // set port bits up as high speed outputs
	GPIOA->OSPEEDR |= (1 << 15) + (1 << 14) + (1 << 11) + (1 << 10);
    // Turn on SPI 1
    RCC->APB2ENR |= BIT12;
    // Configure SPI interface
    // Now configure the SPI interface
    int drain_count,drain;
	drain = SPI1->SR;				// dummy read of SR to clear MODF	
	// enable SSM, set SSI, enable SPI, PCLK/2, MSB First Master, Clock = 1 when idle
	//SPI1->CR1 = (1 << 8)+(1 << 6)+(1 << 2) +  (1 << 1) + (1 << 5);	 // update : set bit 5 to slow down the clock for debugging + bit 15 for bidimode + bit14 for tx mode
    SPI1->CR1 = (1 << 9) + (1 << 8) +  (1 << 6)+(1 << 2) +  (1 << 1);
	SPI1->CR2 = (1 << 10)+(1 << 9)+(1 << 8) + (1 << 2); 	// configure for 8 bit operation
   
    int dummy = SPI1->CR1;
    dummy = SPI1->CR2;
    dummy = GPIOA->MODER;
    dummy = GPIOA->AFRL;
    for (drain_count = 0; drain_count < 32; drain_count++)
		drain = transferSPI8(0x00);
    BacklightOn();
    RSTHigh(); // Drive reset high
    t->sleep(25); // wait   
    RSTLow(); // Drive reset low
    t->sleep(25); // wait   
    RSTHigh(); // Drive reset high
    t->sleep(25); // wait    
    
  /*  
    writeCommand(0x1);  // software reset
    t->sleep(150); // wait   
    
    writeCommand(0x11);  //exit SLEEP mode
    t->sleep(25); // wait   
    
    writeCommand(0x3A); // Set colour mode        
    writeData8(0x55); // 16bits / pixel @ 64k colors 5-6-5 format 
    t->sleep(25); // wait   
    
    writeCommand(0x36);
    writeData8(0x08);  // RGB Format
    t->sleep(25); // wait   
    
    
    writeCommand(0x51); // maximum brightness
    t->sleep(25); // wait   
    
    writeCommand(0x21);    // display inversion off (datasheet is incorrect on this point)
    writeCommand(0x13);    // partial mode off                 
    writeCommand(0x29);    // display on
    
    t->sleep(25); // wait    
    writeCommand(0x2c);   // put display in to write mode
    */
      RSTHigh(); // Drive reset high
    t->sleep(25); // wait   
    RSTLow(); // Drive reset low
    t->sleep(25); // wait   
    RSTHigh(); // Drive reset high
    t->sleep(25); // wait    
    
    
    writeCommand(0x1);  // software reset
    t->sleep(150); // wait   
    
    writeCommand(0x11);  //exit SLEEP mode
    t->sleep(25); // wait   
    
    writeCommand(0x3A); // Set colour mode        
    writeData8(0x55); // 16bits / pixel @ 64k colors 5-6-5 format 
    t->sleep(25); // wait   
    
    writeCommand(0x36);
    writeData8(0x68);//writeData8(0x08);  // RGB Format, rows are on the long axis.
    t->sleep(25); // wait   
    
    
    writeCommand(0x51); // maximum brightness
    t->sleep(25); // wait   
    
    writeCommand(0x21);    // display inversion off (datasheet is incorrect on this point)
    writeCommand(0x13);    // partial mode off                 
    writeCommand(0x29);    // display on
    t->sleep(25); // wait   
    writeCommand(0x2c);   // put display in to write mode    
    fillRectangle(0,0,SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 , 0);  // black out the screen           
    return 0;
    
    
    
}
uint8_t display::transferSPI8(uint8_t data)
{
    int Timeout = 100;    
    *((uint8_t*)&SPI1->DR) = data;    
    while (((SPI1->SR & (1 << 7))!=0)&&(Timeout--));        
	return *((uint8_t*)&SPI1->DR);//*preg;	        
}

uint16_t display::transferSPI16(uint16_t data)
{
    unsigned Timeout = 100;    
    SPI1->DR = data & 0xff;    
    while (((SPI1->SR & (1 << 7))!=0)&&(Timeout--));
    SPI1->DR = data >> 8;    
    while (((SPI1->SR & (1 << 7))!=0)&&(Timeout--));        
    return SPI1->DR;	
}
void display::RSLow()
{ 
// Using Set/Reset register here as this needs to be as fast as possible   
    GPIOA->BSRR = ((1 << 6) << 16); // drive D/C pin low
    asm(" nop ");    
    asm(" nop ");
}
void display::RSHigh()
{ 
// Using Set/Reset register here as this needs to be as fast as possible     
    GPIOA->BSRR = ((1 << 6)); // drive D/C pin high
    asm(" nop ");    
    asm(" nop ");
}
void display::RSTLow()
{
    GPIOA->ODR &= ~(1 << 9); // Drive reset low
}
void display::RSTHigh()
{
    GPIOA->ODR |= (1 << 9); // Drive reset high
}

void display::openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // open up an area for drawing on the display        
    // This particular display module has an X and Y offset which is 
    // dealt with below
    x1 = x1 + 40;
    x2 = x2 + 40;
    y1 = y1 + 53;
    y2 = y2 + 53;
    RSLow(); 
    transferSPI8(0x2A);
    RSHigh();
    transferSPI8(x1>>8);
    transferSPI8(x1&0xff);        
    transferSPI8(x2>>8);
    transferSPI8(x2&0xff);
    RSLow(); // Set Y limits
    transferSPI8(0x2B);
    RSHigh();    
    transferSPI8(y1>>8);
    transferSPI8(y1&0xff);        
    transferSPI8(y2>>8);
    transferSPI8(y2&0xff);    
        
    RSLow(); // put display in to data write mode
    transferSPI8(0x2c);

} 
void display::fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour)
{
    uint32_t pixelcount = height * width;
    openAperture(x, y, x + width - 1, y + height - 1);
    RSHigh();
    while(pixelcount--)
        transferSPI16(Colour);
}
void display::BacklightOn()
{
    GPIOA->BSRR |= (1 << 10);
}
void display::BacklightOff()
{
    GPIOA->BSRR |= (1 << 10) << 16;    
}
void display::writeCommand(uint8_t Cmd)
{
    RSLow();
    transferSPI8(Cmd);
}
void display::writeData8(uint8_t Data)
{
    RSHigh();
    transferSPI8(Data);
}

void display::writeData16(uint16_t Data)
{
    RSHigh();
    transferSPI16(Data);
}
void display::putPixel(uint16_t x, uint16_t y, uint16_t colour)
{
    openAperture(x, y, x + 1, y + 1);
    writeData16(colour); 
}
void display::drawLineLowSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour)
{
    // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm    
  int dx = x1 - x0;
  int dy = y1 - y0;
  int yi = 1;
  if (dy < 0)
  {
    yi = -1;
    dy = -dy;
  }
  int D = 2*dy - dx;
  
  int y = y0;

  for (int x=x0; x <= x1;x++)
  {
    putPixel(x,y,Colour);    
    if (D > 0)
    {
       y = y + yi;
       D = D - 2*dx;
    }
    D = D + 2*dy;
    
  }
}

void display::drawLineHighSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour)
{
        // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

  int dx = x1 - x0;
  int dy = y1 - y0;
  int xi = 1;
  if (dx < 0)
  {
    xi = -1;
    dx = -dx;
  }  
  int D = 2*dx - dy;
  int x = x0;

  for (int y=y0; y <= y1; y++)
  {
    putPixel(x,y,Colour);
    if (D > 0)
    {
       x = x + xi;
       D = D - 2*dy;
    }
    D = D + 2*dx;
  }
}
void display::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Colour)
{
    // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    if ( iabs(y1 - y0) < iabs(x1 - x0) )
    {
        if (x0 > x1)
        {
            drawLineLowSlope(x1, y1, x0, y0, Colour);
        }
        else
        {
            drawLineLowSlope(x0, y0, x1, y1, Colour);
        }
    }
    else
    {
        if (y0 > y1) 
        {
            drawLineHighSlope(x1, y1, x0, y0, Colour);
        }
        else
        {
            drawLineHighSlope(x0, y0, x1, y1, Colour);
        }
        
    }
}
void display::drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour)
{
    drawLine(x,y,x+w,y,Colour);
    drawLine(x,y,x,y+h,Colour);
    drawLine(x+w,y,x+w,y+h,Colour);
    drawLine(x,y+h,x+w,y+h,Colour);
}

void display::drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour)
{
// Reference : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    if (radius > x0)
        return; // don't draw even parially off-screen circles
    if (radius > y0)
        return; // don't draw even parially off-screen circles
        
    if ((x0+radius) > SCREEN_WIDTH)
        return; // don't draw even parially off-screen circles
    if ((y0+radius) > SCREEN_HEIGHT)
        return; // don't draw even parially off-screen circles    
    while (x >= y)
    {
        putPixel(x0 + x, y0 + y, Colour);
        putPixel(x0 + y, y0 + x, Colour);
        putPixel(x0 - y, y0 + x, Colour);
        putPixel(x0 - x, y0 + y, Colour);
        putPixel(x0 - x, y0 - y, Colour);
        putPixel(x0 - y, y0 - x, Colour);
        putPixel(x0 + y, y0 - x, Colour);
        putPixel(x0 + x, y0 - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
void display::fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour)
{
// Reference : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
// Similar to drawCircle but fills the circle with lines instead
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    if (radius > x0)
        return; // don't draw even parially off-screen circles
    if (radius > y0)
        return; // don't draw even parially off-screen circles
        
    if ((x0+radius) > SCREEN_WIDTH)
        return; // don't draw even parially off-screen circles
    if ((y0+radius) > SCREEN_HEIGHT)
        return; // don't draw even parially off-screen circles        
    while (x >= y)
    {
        drawLine(x0 - x, y0 + y,x0 + x, y0 + y, Colour);        
        drawLine(x0 - y, y0 + x,x0 + y, y0 + x, Colour);        
        drawLine(x0 - x, y0 - y,x0 + x, y0 - y, Colour);        
        drawLine(x0 - y, y0 - x,x0 + y, y0 - x, Colour);        

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
void display::putImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t * img) 
{
    uint16_t Colour;    
    openAperture(x, y, x + w - 1, y + h - 1);
    RSHigh();    
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            Colour = *(img++);
            transferSPI16(Colour);            
        }
    }    
}
void display::print(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
        // This function draws each character individually.  It uses an array called TextBox as a temporary storage
    // location to hold the dots for the character in question.  It constructs the image of the character and then
    // calls on putImage to place it on the screen
    uint8_t Index = 0;
    uint8_t Row, Col;
    const uint8_t *CharacterCode = 0;    
    uint16_t TextBox[FONT_WIDTH * FONT_HEIGHT];
    for (Index = 0; Index < len; Index++)
    {
        CharacterCode = &Font5x7[FONT_WIDTH * (Text[Index] - 32)];
        Col = 0;
        while (Col < FONT_WIDTH)
        {
            Row = 0;
            while (Row < FONT_HEIGHT)
            {
                if (CharacterCode[Col] & (1 << Row))
                {
                    TextBox[(Row * FONT_WIDTH) + Col] = ForeColour;
                }
                else
                {
                    TextBox[(Row * FONT_WIDTH) + Col] = BackColour;
                }
                Row++;
            }
            Col++;
        }
        putImage(x, y, FONT_WIDTH, FONT_HEIGHT, (const uint16_t *)TextBox);
        x = x + FONT_WIDTH + 2;
    }
}
void display::print(uint16_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
     // This function converts the supplied number into a character string and then calls on puText to
    // write it to the display
    char Buffer[5]; // Maximum value = 65535
    Buffer[4] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[3] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[2] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[1] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[0] = Number % 10 + '0';
    print(Buffer, 5, x, y, ForeColour, BackColour);
}
