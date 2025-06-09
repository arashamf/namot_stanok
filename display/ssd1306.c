
// Includes ------------------------------------------------------------------//
#include "ssd1306.h"
#include "ssd1306_interface.h"
#include "tim.h"
#include "gpio.h"
#include "typedef.h"
#include <stdio.h>
#include <string.h>

// Declarations and definitions -------------------------------------------------------//
static void void_screen (void);
static void clear_buffer_screen (void);
static void preset1_screen (void);
static void preset2_screen (void);
static void preset3_screen (void);
static void preset4_screen (void);

// Private structure  -----------------------------------------------------------------//
typedef struct 
{
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;

// Private variables -----------------------------------------------------------------//
static SSD1306_t SSD1306;
uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];
uint8_t LCD_buff[LCD_BUFFER_SIZE];

// Functions -------------------------------------------------------------------------//
void ssd1306_Init(void)
{   
	LCD_RESET(OFF);
	delay_us(3000);	
  	LCD_RESET(ON);
	delay_us(3000);	
	
	CS(ON);
	ssd1306_SendCommand(DISPLAYOFF); 		//display off
	ssd1306_SendCommand(SETCONTRAST); //set contrast control register
	ssd1306_SendCommand(0xFF);
	ssd1306_SendCommand(NORMALDISPLAY); //set normal display
	ssd1306_SendCommand(DISPLAYALLON_RESUME); //0xA4-возобновить отображение содержимого RAM;0xA5- полное включение дисплея, вывод игнорирует содержимое RAM		
	
	ssd1306_SendCommand(MEMORYMODE); 	//Set Memory Addressing Mode, 0x20   
	ssd1306_SendCommand(PAGEMODE); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	ssd1306_SendCommand(STARTPAGEADDR); //Set Page Start Address for Page Addressing Mode,0xB0-0xB7 (эта команда применяется только дляPage Addressing Mode) 
	ssd1306_SendCommand(SETLOWCOLUMN); //set low column address, 0x00 (эта команда применяется только дляPage Addressing Mode) 
	ssd1306_SendCommand(SETHIGHCOLUMN); //set high column address, 0x10 (эта команда применяется только дляPage Addressing Mode) 
	
	ssd1306_SendCommand(SETSTARTLINE); //Устанавливает регистр начальной строки в диапазоне 0..63 (0x40)	
	ssd1306_SendCommand(SETSEGREMAP+0x01); //--set segment re-map 0 to 127 	
	ssd1306_SendCommand(SETMULTIPLEX); //Установка коэффициента мультиплексирования (1 to 64)
	ssd1306_SendCommand(0x1F); //32 (128x32)
	ssd1306_SendCommand(COMSCANDEC); //Set COM Output Scan Direction. 0xC8: remapped mode, сканирование от COM[N-1] до COM0
	ssd1306_SendCommand(SETDISPLAYOFFSET); //-set display offset
	ssd1306_SendCommand(0x00); //-not offset
	ssd1306_SendCommand(SETCOMPINS); //set com pins hardware configuration
	ssd1306_SendCommand(0x12); //альтернативная конфигурация выводов COM

	ssd1306_SendCommand(SETDISPLAYCLOCKDIV); //[3:0] определение коэффициента деления (D) для тактов дисплея (DCLK). Коэффициент деления = A[3:0]+1. Значение по умолчанию 0, что соответствует коэффициенту 1.
	//7:4] установка частоты генератора FOSC. Чем больше значение A[7:4], тем выше частота генератора. Значение по умолчанию после сброса 8, допустимый диапазон 0..15.
	ssd1306_SendCommand(SETDIVIDERATIO); //коэффициенту деления = 1, FOSC-максимальная
	//ssd1306_SendCommand(SETPRECHARGE); //установка периода Phase 1 и Phase 2
	//ssd1306_SendCommand(0x22); //	
	ssd1306_SendCommand(SETVCOMDETECT); 	//set Vcomh Deselect Level
	ssd1306_SendCommand(0x20); 		//~ 0.77·VCC

	ssd1306_SendCommand(DISPLAYON); //--turn on SSD1306 panel
	
	delay_us(5000);
	
	SSD1306_Clear_Screen (); 		// Clear screen 
	SSD1306_UpdateScreen(); 	// Update screen 
	
	SSD1306.CurrentX = 0; 	// Set default values 
	SSD1306.CurrentY = 0;
	
	SSD1306.Initialized = 1; 	// Initialized OK 
	CS(OFF);
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_GotoXY(uint16_t x, uint16_t y) 
{
	SSD1306.CurrentX = x; // Set write pointers 
	SSD1306.CurrentY = y;
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) 
{
	if (x >= SSD1306_X_SIZE || y >= SSD1306_Y_SIZE) 
	{	return; } 						// Error 
	
	if (SSD1306.Inverted) 	// Check if pixels are inverted 
	{	color = (SSD1306_COLOR_t)!color;	}
	
	if (color == SSD1306_COLOR_WHITE)  // Set color 
	{	SSD1306_Buffer[x + (y/8) * SSD1306_X_SIZE] |= 1 << (y % 8);	} 
	else 
	{	SSD1306_Buffer[x + (y/8) * SSD1306_X_SIZE] &= ~(1 << (y % 8));	}
}

//-----------------------------------------------------------------------------------------------//
char SSD1306_PutChar(char ch, FontDef_t* Font, SSD1306_COLOR_t color) 
{
	uint32_t i, b, j;
	//if (SSD1306.Inverted) {		color = (SSD1306_COLOR_t)!color;	}
	if (SSD1306_X_SIZE <= (SSD1306.CurrentX + Font->FontWidth) ||
			SSD1306_Y_SIZE <= (SSD1306.CurrentY + Font->FontHeight)) // Check available space in LCD 
	{	return 0;	}     // Error 
	
	for (i = 0; i < Font->FontHeight; i++) // Go through font 
	{
		b = Font->data[(ch-32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) 
		{
			if ((b << j) & 0x8000) 
			{	SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t)color);	} 
			else 
			{	SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t)!color);	}
		}
	}
	
	SSD1306.CurrentX += Font->FontWidth; // Increase pointer 	
	return ch; 	// Return character written 
}

//-----------------------------------------------------------------------------------------------//
char SSD1306_PutString (uint8_t* str, FontDef_t* Font, SSD1306_COLOR_t color) 
{
	while (*str) 
	{
		if (SSD1306_PutChar(*str, Font, color) != *str)
		{	return *str;	}
		str++;
	}
	return *str; // Everything OK, zero should be returned 
}

//-----------------------------------------------------------------------------------------------//
char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color) 
{
	uint32_t i, b, j;

	 // Check available space in LCD 
	//if (SSD1306_X_SIZE <= (SSD1306.CurrentX + Font->FontWidth) ||	SSD1306_Y_SIZE <= (SSD1306.CurrentY + Font->FontHeight)) 
	if (SSD1306_X_SIZE <= (SSD1306.CurrentX) ||	SSD1306_Y_SIZE <= (SSD1306.CurrentY)) 
	{	return 0;	}     // Error

	for (i = 0; i < Font->FontHeight; i++) 	// Go through font 
	{
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) 
		{
			if ((b << j) & 0x8000) 
			{	SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t) color);	} 
			else 
			{	SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t)!color);	}
		}
	}
	
	SSD1306.CurrentX += Font->FontWidth; 	// Increase pointer 
	return ch; 	// Return character written 
}

//---------------------------------заполнение буффера для дисплея---------------------------------//
char SSD1306_Puts(uint8_t* str, FontDef_t* Font, SSD1306_COLOR_t color) 
{
	while (*str) 
	{
		if (SSD1306_Putc(*str, Font, color) != *str) 
		{	return *str;	}
		str++;
	}	
	return *str; // Everything OK, zero should be returned 
}

//-----------------------------------отправка буффера в дисплей-----------------------------------//
void SSD1306_UpdateScreen(void) 
{
	#ifdef __SPI_DMA_MODE	 
	ssd1306_SendDataBuffer (SSD1306_Buffer,  SSD1306_BUFFER_SIZE);
	#else
	uint8_t m = 0;
	for (m = 0; m < 4; m++)  
	{ 
		SSD1306_PageAdress(m);
		ssd1306_SendDataBuffer (&SSD1306_Buffer[(SSD1306_X_SIZE*m) - 4], (SSD1306_BUFFER_SIZE/4)); 	// Write multi data 
	}
	#endif
}

//-----------------------------------посылка адреса страницы-----------------------------------//
void SSD1306_PageAdress(uint8_t i) 
{
		CS(ON);
		SSD1306_WRITECOMMAND(0xB0 + i); //Установка битами X[2:0] адреса начальной страницы RAM (PAGE0~PAGE7) для Page Addressing Mode
		SSD1306_WRITECOMMAND(0x00); //Устанавка младшего ниббла регистра начального адреса столбца для режима адресации страниц (Page Addressing Mode) 
		SSD1306_WRITECOMMAND(0x10); //Устанавка старшего ниббла регистра начального адреса столбца
	//  CS(OFF);
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_Clear_Screen (void)
{
	SSD1306_Fill(SSD1306_COLOR_BLACK); 
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_ToggleInvert(void) 
{
	uint16_t i;
	
	SSD1306.Inverted = !SSD1306.Inverted; // Toggle invert 

	for (i = 0; i < sizeof(SSD1306_Buffer); i++) 	// Do memory toggle 
	{	SSD1306_Buffer[i] = ~SSD1306_Buffer[i];	}
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_Fill(SSD1306_COLOR_t color) 
{
	if (SSD1306.Inverted) 
	{	memset(SSD1306_Buffer, (color == SSD1306_COLOR_WHITE) ? 0x00 : 0xFF, SSD1306_BUFFER_SIZE);	}
	else 
	{	memset(SSD1306_Buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, SSD1306_BUFFER_SIZE);	}
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c) 
{
	int16_t dx, dy, sx, sy, err, e2, i, tmp; 
	
	if (x0 >= SSD1306_X_SIZE) // Check for overflow 
	{	x0 = SSD1306_X_SIZE - 1;	}
	
	if (x1 >= SSD1306_X_SIZE) 
	{	x1 = SSD1306_X_SIZE - 1;	}
	
	if (y0 >= SSD1306_Y_SIZE) 
	{	y0 = SSD1306_Y_SIZE - 1;	}
	
	if (y1 >= SSD1306_Y_SIZE) 
	{	y1 = SSD1306_Y_SIZE - 1;	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0) 
	{
		if (y1 < y0) 
		{
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) 
		{
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		for (i = y0; i <= y1; i++) 
		{	SSD1306_DrawPixel(x0, i, c);	} // Vertical line 
		
		return; 	// Return from function 
	}
	
	if (dy == 0) 
	{
		if (y1 < y0) 
		{
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) 
		{
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
			
		for (i = x0; i <= x1; i++) 
		{	SSD1306_DrawPixel(i, y0, c);	} // Horizontal line 
		return; 	// Return from function 
	}
	
	while (1) 
	{
		SSD1306_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) 
		{	break;	}
		e2 = err; 
		if (e2 > -dx) 
		{
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) 
		{
			err += dx;
			y0 += sy;
		} 
	}
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) 
{
	
	if (x >= SSD1306_X_SIZE ||	y >= SSD1306_Y_SIZE) // Check input parameters 
	{	return; }// Return error 
	
	if ((x + w) >= SSD1306_X_SIZE) // Check width 
	{	w = SSD1306_X_SIZE - x;	}
	
	if ((y + h) >= SSD1306_Y_SIZE)  // Check height 
	{	h = SSD1306_Y_SIZE - y;	}
	
	//--------------Draw 4 lines 
	SSD1306_DrawLine(x, y, x + w, y, c);         // Top line 
	SSD1306_DrawLine(x, y + h, x + w, y + h, c); // Bottom line 
	SSD1306_DrawLine(x, y, x, y + h, c);         // Left line 
	SSD1306_DrawLine(x + w, y, x + w, y + h, c); // Right line 
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) 
{
	uint8_t i;
	
	if (x >= SSD1306_X_SIZE ||	y >= SSD1306_Y_SIZE)  // Check input parameters 
	{	return; }																				// Return error 
	
	if ((x + w) >= SSD1306_X_SIZE) // Check width 
	{	w = SSD1306_X_SIZE - x;	}
	
	if ((y + h) >= SSD1306_Y_SIZE) // Check height 
	{	h = SSD1306_Y_SIZE - y;	}
	
	for (i = 0; i <= h; i++) 
	{	SSD1306_DrawLine(x, y + i, x + w, y + i, c); 	}	// Draw lines 
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, 
uint16_t y3,	SSD1306_COLOR_t color) 
{
	
	SSD1306_DrawLine(x1, y1, x2, y2, color); // Draw lines 
	SSD1306_DrawLine(x2, y2, x3, y3, color);
	SSD1306_DrawLine(x3, y3, x1, y1, color);
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, 
uint16_t y3, SSD1306_COLOR_t color) 
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;
	
	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) 
	{
		xinc1 = 1;
		xinc2 = 1;
	} 
	else 
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) 
	{
		yinc1 = 1;
		yinc2 = 1;
	} 
	else 
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)
	{
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} 
	else
	{
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) 
	{
		SSD1306_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) 
		{
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) 
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

  SSD1306_DrawPixel(x0, y0 + r, c);
  SSD1306_DrawPixel(x0, y0 - r, c);
  SSD1306_DrawPixel(x0 + r, y0, c);
  SSD1306_DrawPixel(x0 - r, y0, c);

  while (x < y) 
	{
		if (f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
    x++;
    ddF_x += 2;
    f += ddF_x;

    SSD1306_DrawPixel(x0 + x, y0 + y, c);
    SSD1306_DrawPixel(x0 - x, y0 + y, c);
    SSD1306_DrawPixel(x0 + x, y0 - y, c);
    SSD1306_DrawPixel(x0 - x, y0 - y, c);

    SSD1306_DrawPixel(x0 + y, y0 + x, c);
    SSD1306_DrawPixel(x0 - y, y0 + x, c);
    SSD1306_DrawPixel(x0 + y, y0 - x, c);
    SSD1306_DrawPixel(x0 - y, y0 - x, c);
  }
}

//-----------------------------------------------------------------------------------------------//
void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) 
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

  SSD1306_DrawPixel(x0, y0 + r, c);
  SSD1306_DrawPixel(x0, y0 - r, c);
  SSD1306_DrawPixel(x0 + r, y0, c);
  SSD1306_DrawPixel(x0 - r, y0, c);
  SSD1306_DrawLine(x0 - r, y0, x0 + r, y0, c);

	while (x < y) 
	{
		if (f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
    }
		x++;
		ddF_x += 2;
		f += ddF_x;

		SSD1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
		SSD1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);
		SSD1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
		SSD1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
  }
}

//-----------------------------------------------------------------------------------------------//
static void void_screen (void)
{
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	SSD1306_Clear_Screen ();
	SSD1306_UpdateScreen();
}

//-----------------------------------------------------------------------------------------------//
static void clear_buffer_screen (void)
{
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	SSD1306_Clear_Screen ();
}

//--------------------------------ф-я отображения основного меню--------------------------------//
void default_screen (uint8_t numb_preset, int16_t r_number, int16_t s_number, uint8_t number_coil)
{
	clear_buffer_screen ();
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "PRESET%u", numb_preset);
	SSD1306_Puts (LCD_buff , &Font_7x10, SSD1306_COLOR_WHITE);
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE+13);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "%uC:%03d/%03d",number_coil, r_number, s_number);
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();
}

//----------------------ф-я отображения установленного передаточного соотношения----------------------//
void setup_ratio_screen (int16_t gear_ratio)
{
	uint8_t int_number = gear_ratio/100;
	uint8_t fraction_part = gear_ratio%100;
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE+5);
	SSD1306_Clear_Screen ();
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "RT:%u.%02u", int_number, fraction_part);
	SSD1306_Puts (LCD_buff , &Font_16x26, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();
}

//-------------------------------ф-я отображения установленной скорости-------------------------------//
void setup_speed_screen (int16_t rotation_speed)
{
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE+5);
	SSD1306_Clear_Screen ();
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "%3u RPM", rotation_speed);
	SSD1306_Puts (LCD_buff , &Font_16x26, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();
}

//------------------------ф-я отображения установленного количества витков------------------------//
void setup_coil_screen (uint8_t count, int32_t number_click)
{
	SSD1306_Clear_Screen ();
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE+5);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "%uC:%03ld", count, number_click);
	SSD1306_Puts (LCD_buff , &Font_16x26, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();
}

//-----------------------------------------------------------------------------------------------//
void main_menu_select_preset_screen (void)
{
	clear_buffer_screen ();
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "SELECT");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE+60, LCD_DEFAULT_Y_SIZE+16);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "MODE");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();	
}

//-----------------------------------------------------------------------------------------------//
void mode_setup_null_screen(void)
{
	clear_buffer_screen ();
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "SET NULL");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);

	SSD1306_UpdateScreen();	
}

//-----------------------------------------------------------------------------------------------//
void mode_setup_manual_screen (void)
{
	clear_buffer_screen ();
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "MANUAL MODE");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);

	SSD1306_UpdateScreen();	
}

//-----------------------------------------------------------------------------------------------//
void mode_setup_manual_dir_screen (void)
{
	clear_buffer_screen ();
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "MANUAL MODE");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE+16);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "<-SET DIR->");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);	

	SSD1306_UpdateScreen();	
}

//-----------------------------------------------------------------------------------------------//
static void preset1_screen (void)
{
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, ">PRESET1");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE+16);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, " PRESET2");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();	
}

//-----------------------------------------------------------------------------------------------//
static void preset2_screen (void)
{
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, " PRESET1");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE+16);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, ">PRESET2");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();	
}

//-----------------------------------------------------------------------------------------------//
static void preset3_screen (void)
{
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, ">PRESET3");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE+16);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, " PRESET4");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();
}

//-----------------------------------------------------------------------------------------------//
static void preset4_screen (void)
{	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, " PRESET3");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE+16);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, ">PRESET4");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();	
}

//-----------------------------------------------------------------------------------------------//
void menu_select_preset_screen (uint8_t screen_mode)
{
	clear_buffer_screen ();
	switch (screen_mode)
	{
		case PRESET1:
			preset1_screen();
			break;
				
		case PRESET2:
			preset2_screen();
			break;
				
		case PRESET3:
			preset3_screen();
			break;
				
		case PRESET4:			
			preset4_screen();
			break;
				
		default:
			preset1_screen();
			break;	
	}	
}

//-----------------------------------------------------------------------------------------------------//
void release_pedal_menu (void)
{
	clear_buffer_screen ();
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "RELEASE");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE+65, LCD_DEFAULT_Y_SIZE+16);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "PEDAL");
	SSD1306_Puts (LCD_buff , &Font_11x17, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();	
}

//-----------------------------------------------------------------------------------------------------//
void start_menu (void)
{
	SSD1306_GotoXY(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
	snprintf ((char *)LCD_buff, LCD_BUFFER_SIZE, "START");
	SSD1306_Puts (LCD_buff , &Font_16x26, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();
}
