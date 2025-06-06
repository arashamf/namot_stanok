
#ifndef SSD1306_H
#define SSD1306_H

// Includes ---------------------------------------------------------------------//
#include "main.h"
#include "fonts.h"
#include "ssd1306_interface.h"

// Exported types ---------------------------------------------------------------//
typedef enum 
{
	SSD1306_COLOR_BLACK = 0x00, // Black color, no pixel 
	SSD1306_COLOR_WHITE = 0x01  // Pixel is set. Color depends on LCD 
} SSD1306_COLOR_t;

// Defines ---------------------------------------------------------------------//
#define SSD1306_WRITECOMMAND(command)			ssd1306_SendCommand (command) // Write command 
#define SSD1306_WRITEDATA(data)						ssd1306_SendByteData(data) // Write data 
#define ABS(x)  ((x) > 0 ? (x) : -(x)) 		// Absolute value 

#define SSD1306_X_SIZE                                  132
#define SSD1306_Y_SIZE                                  32
#define SSD1306_BUFFER_SIZE                             ((SSD1306_X_SIZE*SSD1306_Y_SIZE) / 8)
#define LCD_BUFFER_SIZE 								20
#define LCD_DEFAULT_X_SIZE  							0
#define LCD_DEFAULT_Y_SIZE 								1

//#define OLED_adress 					0x78
#define DISPLAYOFF 						0xAE
#define DISPLAYON 						0xAF
#define MEMORYMODE 						0x20
#define HORIZONTALMODE 					0x00
#define VERTICALMODE 					0x01
#define PAGEMODE						0x02
#define SETLOWCOLUMN    				0x00
#define SETHIGHCOLUMN 					0x10
#define STARTPAGEADDR   				0xB0
#define COLUMNADDR 						0x21
#define PAGESADDR 						0x22
#define COMSCANDEC 						0xC8
#define COMSCANINC 						0xC0
#define SETSTARTLINE 					0x40
#define SETCONTRAST 					0x81
#define SETSEGREMAP 					0xA0
#define NORMALDISPLAY 					0xA6
#define INVERTDISPLAY 					0xA7
#define SETMULTIPLEX 					0xA8
#define DISPLAYALLON_RESUME 			0xA4 
#define DISPLAYALLON 					0xA5 
#define SETDISPLAYOFFSET 				0xD3
#define SETDISPLAYCLOCKDIV 				0xD5
#define SETDIVIDERATIO					0xF0
#define SETPRECHARGE 					0xD9
#define SETCOMPINS 						0xDA
#define SETVCOMDETECT					0xDB
#define SWITCHCAPVCC 					0x20


// Functions -----------------------------------------------------------------//
void ssd1306_Init(void);
void SSD1306_GotoXY(uint16_t , uint16_t );
void SSD1306_DrawPixel(uint16_t , uint16_t , SSD1306_COLOR_t );
char SSD1306_PutChar(char , FontDef_t* , SSD1306_COLOR_t );
char SSD1306_PutString(uint8_t* , FontDef_t* , SSD1306_COLOR_t );
char SSD1306_Putc(char , FontDef_t* , SSD1306_COLOR_t ) ;
char SSD1306_Puts(uint8_t* , FontDef_t* , SSD1306_COLOR_t ) ;
void SSD1306_UpdateScreen(void);
void SSD1306_PageAdress(uint8_t );
void SSD1306_Clear_Screen (void);
void SSD1306_ToggleInvert(void);
void SSD1306_Fill(SSD1306_COLOR_t );
void SSD1306_DrawLine(uint16_t , uint16_t , uint16_t , uint16_t , SSD1306_COLOR_t );
void SSD1306_DrawRectangle(uint16_t , uint16_t , uint16_t , uint16_t , SSD1306_COLOR_t );
void SSD1306_DrawFilledRectangle(uint16_t , uint16_t , uint16_t , uint16_t , SSD1306_COLOR_t );
void SSD1306_DrawTriangle(uint16_t , uint16_t , uint16_t , uint16_t , uint16_t , uint16_t , SSD1306_COLOR_t );
void SSD1306_DrawFilledTriangle(uint16_t , uint16_t , uint16_t , uint16_t , uint16_t , uint16_t , SSD1306_COLOR_t );
void SSD1306_DrawCircle(int16_t , int16_t , int16_t , SSD1306_COLOR_t );
void SSD1306_DrawFilledCircle(int16_t , int16_t , int16_t , SSD1306_COLOR_t );

void 	default_screen (uint8_t , int16_t , int16_t , uint8_t );
void 	setup_coil_screen (uint8_t , int32_t );
void 	setup_ratio_screen (int16_t);
void 	setup_speed_screen (int16_t );
void 	main_menu_select_preset_screen (void);
void 	mode_setup_null_screen(void);
void	mode_setup_manual_screen (void);
void 	mode_setup_manual_dir_screen (void);
void 	menu_select_preset_screen (uint8_t );
void 	release_pedal_menu (void);
void 	start_menu (void);
// Variables ------------------------------------------------------------------//
extern uint8_t LCD_buff[];
extern uint8_t SSD1306_Buffer[];
#endif // #ifndef SSD1306_H
