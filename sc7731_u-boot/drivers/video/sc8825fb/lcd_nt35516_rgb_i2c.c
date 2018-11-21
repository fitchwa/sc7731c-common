/* drivers/video/sc8825/lcd_nt35516_i2c.c
 *
 * Support for nt35516 i2c LCD device
 *
 * Copyright (C) 2010 Spreadtrum
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <asm/arch/tiger_lcd.h>


//#define  LCD_DEBUG
#ifdef LCD_DEBUG
#define LCD_PRINT printk
#else
#define LCD_PRINT(...)
#endif



#define NT35516_I2C_ADDR   0x4C
#define NT35516_I2C_DATA_LENGHT      0x2 
#define NT35516_I2C_ACK              0x01
//#define Set_NT35516_I2C_Send(len, data)  I2C_WriteCmdArr(NT35516_I2C_ADDR, data ,len, NT35516_I2C_ACK)
#define Set_NT35516_I2C_Send(len, data)  i2c_write_burst(NT35516_I2C_ADDR, data ,len)
#define LCDI2C_ID_ADDR 0xDB00

#define NT35516_MAX_PARAM   4

typedef struct LCM_Init_Code_tag {
	unsigned int tag;
	unsigned char data[NT35516_MAX_PARAM];
}LCM_Init_Code;

#define LCM_TAG_SHIFT 24
#define LCM_TAG_MASK  ((1 << 24) -1)
#define LCM_SEND(len) ((1 << LCM_TAG_SHIFT)| len)
#define LCM_SLEEP(ms) ((2 << LCM_TAG_SHIFT)| ms) 
//#define ARRAY_SIZE(array) ( sizeof(array) / sizeof(array[0]))

#define LCM_TAG_SEND  (1<< 0)
#define LCM_TAG_SLEEP (1 << 1)

static LCM_Init_Code init_data[] = {
	{LCM_SEND(4), {0xFF, 0x00, 0x00, 0xAA}},//AA
	{LCM_SEND(4), {0xFF, 0x01, 0x00, 0x55}},//55
	{LCM_SEND(4), {0xFF, 0x02, 0x00, 0x25}},//08
	{LCM_SEND(4), {0xFF, 0x03, 0x00, 0x01}},//01
	{LCM_SEND(4), {0xF3, 0x00, 0x00, 0x02}},
	{LCM_SEND(4), {0xF3, 0x03, 0x00, 0x15}},   
	{LCM_SEND(4), {0xF0, 0x00, 0x00, 0x55}}, //Manufacture Command Set Control   
	{LCM_SEND(4), {0xF0, 0x01, 0x00, 0xAA}},
	{LCM_SEND(4), {0xF0, 0x02, 0x00, 0x52}},
	{LCM_SEND(4), {0xF0, 0x03, 0x00, 0x08}},
	{LCM_SEND(4), {0xF0, 0x04, 0x00, 0x00}},
	{LCM_SEND(4), {0xB8, 0x00, 0x00, 0x01}}, 
	{LCM_SEND(4), {0xB8, 0x01, 0x00, 0x02}},
	{LCM_SEND(4), {0xB8, 0x02, 0x00, 0x02}},
	{LCM_SEND(4), {0xB8, 0x03, 0x00, 0x02}},
	{LCM_SEND(4), {0xBC, 0x00, 0x00, 0x05}}, //Zig-Zag Inversion  
	{LCM_SEND(4), {0xBC, 0x01, 0x00, 0x05}},
	{LCM_SEND(4), {0xBC, 0x02, 0x00, 0x05}},
	{LCM_SEND(4), {0x4C, 0x00, 0x00, 0x11}}, //DB4=1,Enable Vivid Color,DB4=0 Disable Vivid Color 
	{LCM_SEND(4), {0xF0, 0x00, 0x00, 0x55}}, //Manufacture Command Set Control      
	{LCM_SEND(4), {0xF0, 0x01, 0x00, 0xAA}},
	{LCM_SEND(4), {0xF0, 0x02, 0x00, 0x52}},
	{LCM_SEND(4), {0xF0, 0x03, 0x00, 0x08}},
	{LCM_SEND(4), {0xF0, 0x04, 0x00, 0x01}},//Page1
	{LCM_SEND(4), {0xB0, 0x00, 0x00, 0x05}}, // Setting AVDD Voltage 6V
	{LCM_SEND(4), {0xB0, 0x01, 0x00, 0x05}},
	{LCM_SEND(4), {0xB0, 0x02, 0x00, 0x05}},
	{LCM_SEND(4), {0xB6, 0x00, 0x00, 0x44}}, // Setting AVEE boosting time 2.5*vpnl 
	{LCM_SEND(4), {0xB6, 0x01, 0x00, 0x44}},
	{LCM_SEND(4), {0xB6, 0x02, 0x00, 0x44}},
	{LCM_SEND(4), {0xB1, 0x00, 0x00, 0x05}}, // Setting AVEE Voltage -6V
	{LCM_SEND(4), {0xB1, 0x01, 0x00, 0x05}},
	{LCM_SEND(4), {0xB1, 0x02, 0x00, 0x05}},
	{LCM_SEND(4), {0xB7, 0x00, 0x00, 0x34}}, 
	{LCM_SEND(4), {0xB7, 0x01, 0x00, 0x34}},
	{LCM_SEND(4), {0xB7, 0x02, 0x00, 0x34}},
	{LCM_SEND(4), {0xBA, 0x00, 0x00, 0x14}}, //0x24 --> 0x14
	{LCM_SEND(4), {0xBA, 0x01, 0x00, 0x14}},
	{LCM_SEND(4), {0xBA, 0x02, 0x00, 0x14}},
	{LCM_SEND(4), {0xBC, 0x00, 0x00, 0x00}}, 
	{LCM_SEND(4), {0xBC, 0x01, 0x00, 0xA0}},//VGMP 0x88=4.7V  0x78=4.5V   0xA0=5.0V  
	{LCM_SEND(4), {0xBC, 0x02, 0x00, 0x00}},//VGSP 
	{LCM_SEND(4), {0xBD, 0x00, 0x00, 0x00}}, 
	{LCM_SEND(4), {0xBD, 0x01, 0x00, 0xA0}},//VGMN 0x88=-4.7V 0x78=-4.5V   0xA0=-5.0V
	{LCM_SEND(4), {0xBD, 0x02, 0x00, 0x00}},//VGSN  
	{LCM_SEND(4), {0xBE, 0x00, 0x00, 0x57}}, // Setting VCOM Offset Voltage  0x4E��Ϊ0x57  20111019 LIYAN      
	{LCM_SEND(4), {0xD1, 0x00, 0x00, 0x00}},
	{LCM_SEND(4), {0xD1, 0x01, 0x00, 0x32}},
	{LCM_SEND(4), {0xD1, 0x02, 0x00, 0x00}},
	{LCM_SEND(4), {0xD1, 0x03, 0x00, 0x41}},
	{LCM_SEND(4), {0xD1, 0x04, 0x00, 0x00}},
	{LCM_SEND(4), {0xD1, 0x05, 0x00, 0x54}},
	{LCM_SEND(4), {0xD1, 0x06, 0x00, 0x00}},
	{LCM_SEND(4), {0xD1, 0x07, 0x00, 0x67}},
	{LCM_SEND(4), {0xD1, 0x08, 0x00, 0x00}},
	{LCM_SEND(4), {0xD1, 0x09, 0x00, 0x7A}},
	{LCM_SEND(4), {0xD1, 0x0A, 0x00, 0x00}},
	{LCM_SEND(4), {0xD1, 0x0B, 0x00, 0x98}},
	{LCM_SEND(4), {0xD1, 0x0C, 0x00, 0x00}},
	{LCM_SEND(4), {0xD1, 0x0D, 0x00, 0xB0}},
	{LCM_SEND(4), {0xD1, 0x0E, 0x00, 0x00}},
	{LCM_SEND(4), {0xD1, 0x0F, 0x00, 0xDB}},
	{LCM_SEND(4), {0xD2, 0x00, 0x00, 0x01}},
	{LCM_SEND(4), {0xD2, 0x01, 0x00, 0x01}},
	{LCM_SEND(4), {0xD2, 0x02, 0x00, 0x01}},
	{LCM_SEND(4), {0xD2, 0x03, 0x00, 0x3F}},
	{LCM_SEND(4), {0xD2, 0x04, 0x00, 0x01}},
	{LCM_SEND(4), {0xD2, 0x05, 0x00, 0x70}},
	{LCM_SEND(4), {0xD2, 0x06, 0x00, 0x01}},
	{LCM_SEND(4), {0xD2, 0x07, 0x00, 0xB4}},
	{LCM_SEND(4), {0xD2, 0x08, 0x00, 0x01}},
	{LCM_SEND(4), {0xD2, 0x09, 0x00, 0xEC}},
	{LCM_SEND(4), {0xD2, 0x0A, 0x00, 0x01}},
	{LCM_SEND(4), {0xD2, 0x0B, 0x00, 0xED}},
	{LCM_SEND(4), {0xD2, 0x0C, 0x00, 0x02}},
	{LCM_SEND(4), {0xD2, 0x0D, 0x00, 0x1E}},
	{LCM_SEND(4), {0xD2, 0x0E, 0x00, 0x02}},
	{LCM_SEND(4), {0xD2, 0x0F, 0x00, 0x51}},
	{LCM_SEND(4), {0xD3, 0x00, 0x00, 0x02}},
	{LCM_SEND(4), {0xD3, 0x01, 0x00, 0x6C}},
	{LCM_SEND(4), {0xD3, 0x02, 0x00, 0x02}},
	{LCM_SEND(4), {0xD3, 0x03, 0x00, 0x8D}},
	{LCM_SEND(4), {0xD3, 0x04, 0x00, 0x02}},
	{LCM_SEND(4), {0xD3, 0x05, 0x00, 0xA5}},
	{LCM_SEND(4), {0xD3, 0x06, 0x00, 0x02}},
	{LCM_SEND(4), {0xD3, 0x07, 0x00, 0xC9}},
	{LCM_SEND(4), {0xD3, 0x08, 0x00, 0x02}},
	{LCM_SEND(4), {0xD3, 0x09, 0x00, 0xEA}},
	{LCM_SEND(4), {0xD3, 0x0A, 0x00, 0x03}},
	{LCM_SEND(4), {0xD3, 0x0B, 0x00, 0x19}},
	{LCM_SEND(4), {0xD3, 0x0C, 0x00, 0x03}},
	{LCM_SEND(4), {0xD3, 0x0D, 0x00, 0x45}},
	{LCM_SEND(4), {0xD3, 0x0E, 0x00, 0x03}},
	{LCM_SEND(4), {0xD3, 0x0F, 0x00, 0x7A}},
	{LCM_SEND(4), {0xD4, 0x00, 0x00, 0x03}},
	{LCM_SEND(4), {0xD4, 0x01, 0x00, 0xB0}},
	{LCM_SEND(4), {0xD4, 0x02, 0x00, 0x03}},
	{LCM_SEND(4), {0xD4, 0x03, 0x00, 0xF4}},
	{LCM_SEND(4), {0xD5, 0x00, 0x00, 0x00}},
	{LCM_SEND(4), {0xD5, 0x01, 0x00, 0x37}},
	{LCM_SEND(4), {0xD5, 0x02, 0x00, 0x00}},
	{LCM_SEND(4), {0xD5, 0x03, 0x00, 0x41}},
	{LCM_SEND(4), {0xD5, 0x04, 0x00, 0x00}},
	{LCM_SEND(4), {0xD5, 0x05, 0x00, 0x54}},
	{LCM_SEND(4), {0xD5, 0x06, 0x00, 0x00}},
	{LCM_SEND(4), {0xD5, 0x07, 0x00, 0x67}},
	{LCM_SEND(4), {0xD5, 0x08, 0x00, 0x00}},
	{LCM_SEND(4), {0xD5, 0x09, 0x00, 0x7A}},
	{LCM_SEND(4), {0xD5, 0x0A, 0x00, 0x00}},
	{LCM_SEND(4), {0xD5, 0x0B, 0x00, 0x98}},
	{LCM_SEND(4), {0xD5, 0x0C, 0x00, 0x00}},
	{LCM_SEND(4), {0xD5, 0x0D, 0x00, 0xB0}},
	{LCM_SEND(4), {0xD5, 0x0E, 0x00, 0x00}},
	{LCM_SEND(4), {0xD5, 0x0F, 0x00, 0xDB}},
	{LCM_SEND(4), {0xD6, 0x00, 0x00, 0x01}},
	{LCM_SEND(4), {0xD6, 0x01, 0x00, 0x01}},
	{LCM_SEND(4), {0xD6, 0x02, 0x00, 0x01}},
	{LCM_SEND(4), {0xD6, 0x03, 0x00, 0x3F}},
	{LCM_SEND(4), {0xD6, 0x04, 0x00, 0x01}},
	{LCM_SEND(4), {0xD6, 0x05, 0x00, 0x70}},
	{LCM_SEND(4), {0xD6, 0x06, 0x00, 0x01}},
	{LCM_SEND(4), {0xD6, 0x07, 0x00, 0xB4}},
	{LCM_SEND(4), {0xD6, 0x08, 0x00, 0x01}},
	{LCM_SEND(4), {0xD6, 0x09, 0x00, 0xEC}},
	{LCM_SEND(4), {0xD6, 0x0A, 0x00, 0x01}},
	{LCM_SEND(4), {0xD6, 0x0B, 0x00, 0xED}},
	{LCM_SEND(4), {0xD6, 0x0C, 0x00, 0x02}},
	{LCM_SEND(4), {0xD6, 0x0D, 0x00, 0x1E}},
	{LCM_SEND(4), {0xD6, 0x0E, 0x00, 0x02}},
	{LCM_SEND(4), {0xD6, 0x0F, 0x00, 0x51}},
	{LCM_SEND(4), {0xD7, 0x00, 0x00, 0x02}},
	{LCM_SEND(4), {0xD7, 0x01, 0x00, 0x6C}},
	{LCM_SEND(4), {0xD7, 0x02, 0x00, 0x02}},
	{LCM_SEND(4), {0xD7, 0x03, 0x00, 0x8D}},
	{LCM_SEND(4), {0xD7, 0x04, 0x00, 0x02}},
	{LCM_SEND(4), {0xD7, 0x05, 0x00, 0xA5}},
	{LCM_SEND(4), {0xD7, 0x06, 0x00, 0x02}},
	{LCM_SEND(4), {0xD7, 0x07, 0x00, 0xC9}},
	{LCM_SEND(4), {0xD7, 0x08, 0x00, 0x02}},
	{LCM_SEND(4), {0xD7, 0x09, 0x00, 0xEA}},
	{LCM_SEND(4), {0xD7, 0x0A, 0x00, 0x03}},
	{LCM_SEND(4), {0xD7, 0x0B, 0x00, 0x19}},
	{LCM_SEND(4), {0xD7, 0x0C, 0x00, 0x03}},
	{LCM_SEND(4), {0xD7, 0x0D, 0x00, 0x45}},
	{LCM_SEND(4), {0xD7, 0x0E, 0x00, 0x03}},
	{LCM_SEND(4), {0xD7, 0x0F, 0x00, 0x7A}},
	{LCM_SEND(4), {0xD8, 0x00, 0x00, 0x03}},
	{LCM_SEND(4), {0xD8, 0x01, 0x00, 0xA0}},
	{LCM_SEND(4), {0xD8, 0x02, 0x00, 0x03}},
	{LCM_SEND(4), {0xD8, 0x03, 0x00, 0xF4}},
	{LCM_SEND(4), {0xD9, 0x00, 0x00, 0x00}},
	{LCM_SEND(4), {0xD9, 0x01, 0x00, 0x32}},
	{LCM_SEND(4), {0xD9, 0x02, 0x00, 0x00}},
	{LCM_SEND(4), {0xD9, 0x03, 0x00, 0x41}},
	{LCM_SEND(4), {0xD9, 0x04, 0x00, 0x00}},
	{LCM_SEND(4), {0xD9, 0x05, 0x00, 0x54}},
	{LCM_SEND(4), {0xD9, 0x06, 0x00, 0x00}},
	{LCM_SEND(4), {0xD9, 0x07, 0x00, 0x67}},
	{LCM_SEND(4), {0xD9, 0x08, 0x00, 0x00}},
	{LCM_SEND(4), {0xD9, 0x09, 0x00, 0x7A}},
	{LCM_SEND(4), {0xD9, 0x0A, 0x00, 0x00}},
	{LCM_SEND(4), {0xD9, 0x0B, 0x00, 0x98}},
	{LCM_SEND(4), {0xD9, 0x0C, 0x00, 0x00}},
	{LCM_SEND(4), {0xD9, 0x0D, 0x00, 0xB0}},
	{LCM_SEND(4), {0xD9, 0x0E, 0x00, 0x00}},
	{LCM_SEND(4), {0xD9, 0x0F, 0x00, 0xDB}},
	{LCM_SEND(4), {0xDD, 0x00, 0x00, 0x01}},
	{LCM_SEND(4), {0xDD, 0x01, 0x00, 0x01}},
	{LCM_SEND(4), {0xDD, 0x02, 0x00, 0x01}},
	{LCM_SEND(4), {0xDD, 0x03, 0x00, 0x3F}},
	{LCM_SEND(4), {0xDD, 0x04, 0x00, 0x01}},
	{LCM_SEND(4), {0xDD, 0x05, 0x00, 0x70}},
	{LCM_SEND(4), {0xDD, 0x06, 0x00, 0x01}},
	{LCM_SEND(4), {0xDD, 0x07, 0x00, 0xB4}},
	{LCM_SEND(4), {0xDD, 0x08, 0x00, 0x01}},
	{LCM_SEND(4), {0xDD, 0x09, 0x00, 0xEC}},
	{LCM_SEND(4), {0xDD, 0x0A, 0x00, 0x01}},
	{LCM_SEND(4), {0xDD, 0x0B, 0x00, 0xED}},
	{LCM_SEND(4), {0xDD, 0x0C, 0x00, 0x02}},
	{LCM_SEND(4), {0xDD, 0x0D, 0x00, 0x1E}},
	{LCM_SEND(4), {0xDD, 0x0E, 0x00, 0x02}},
	{LCM_SEND(4), {0xDD, 0x0F, 0x00, 0x51}},
	{LCM_SEND(4), {0xDE, 0x00, 0x00, 0x02}},
	{LCM_SEND(4), {0xDE, 0x01, 0x00, 0x6C}},
	{LCM_SEND(4), {0xDE, 0x02, 0x00, 0x02}},
	{LCM_SEND(4), {0xDE, 0x03, 0x00, 0x8D}},
	{LCM_SEND(4), {0xDE, 0x04, 0x00, 0x02}},
	{LCM_SEND(4), {0xDE, 0x05, 0x00, 0xA5}},
	{LCM_SEND(4), {0xDE, 0x06, 0x00, 0x02}},
	{LCM_SEND(4), {0xDE, 0x07, 0x00, 0xC9}},
	{LCM_SEND(4), {0xDE, 0x08, 0x00, 0x02}},
	{LCM_SEND(4), {0xDE, 0x09, 0x00, 0xEA}},
	{LCM_SEND(4), {0xDE, 0x0A, 0x00, 0x03}},
	{LCM_SEND(4), {0xDE, 0x0B, 0x00, 0x19}},
	{LCM_SEND(4), {0xDE, 0x0C, 0x00, 0x03}},
	{LCM_SEND(4), {0xDE, 0x0D, 0x00, 0x45}},
	{LCM_SEND(4), {0xDE, 0x0E, 0x00, 0x03}},
	{LCM_SEND(4), {0xDE, 0x0F, 0x00, 0x7A}},
	{LCM_SEND(4), {0xDF, 0x00, 0x00, 0x03}},
	{LCM_SEND(4), {0xDF, 0x01, 0x00, 0xA0}},
	{LCM_SEND(4), {0xDF, 0x02, 0x00, 0x03}},
	{LCM_SEND(4), {0xDF, 0x03, 0x00, 0xF4}},
	{LCM_SEND(4), {0xE0, 0x00, 0x00, 0x00}},
	{LCM_SEND(4), {0xE0, 0x01, 0x00, 0x32}},
	{LCM_SEND(4), {0xE0, 0x02, 0x00, 0x00}},
	{LCM_SEND(4), {0xE0, 0x03, 0x00, 0x41}},
	{LCM_SEND(4), {0xE0, 0x04, 0x00, 0x00}},
	{LCM_SEND(4), {0xE0, 0x05, 0x00, 0x54}},
	{LCM_SEND(4), {0xE0, 0x06, 0x00, 0x00}},
	{LCM_SEND(4), {0xE0, 0x07, 0x00, 0x67}},
	{LCM_SEND(4), {0xE0, 0x08, 0x00, 0x00}},
	{LCM_SEND(4), {0xE0, 0x09, 0x00, 0x7A}},
	{LCM_SEND(4), {0xE0, 0x0A, 0x00, 0x00}},
	{LCM_SEND(4), {0xE0, 0x0B, 0x00, 0x98}},
	{LCM_SEND(4), {0xE0, 0x0C, 0x00, 0x00}},
	{LCM_SEND(4), {0xE0, 0x0D, 0x00, 0xB0}},
	{LCM_SEND(4), {0xE0, 0x0E, 0x00, 0x00}},
	{LCM_SEND(4), {0xE0, 0x0F, 0x00, 0xDB}},
	{LCM_SEND(4), {0xE1, 0x00, 0x00, 0x01}},
	{LCM_SEND(4), {0xE1, 0x01, 0x00, 0x01}},
	{LCM_SEND(4), {0xE1, 0x02, 0x00, 0x01}},
	{LCM_SEND(4), {0xE1, 0x03, 0x00, 0x3F}},
	{LCM_SEND(4), {0xE1, 0x04, 0x00, 0x01}},
	{LCM_SEND(4), {0xE1, 0x05, 0x00, 0x70}},
	{LCM_SEND(4), {0xE1, 0x06, 0x00, 0x01}},
	{LCM_SEND(4), {0xE1, 0x07, 0x00, 0xB4}},
	{LCM_SEND(4), {0xE1, 0x08, 0x00, 0x01}},
	{LCM_SEND(4), {0xE1, 0x09, 0x00, 0xEC}},
	{LCM_SEND(4), {0xE1, 0x0A, 0x00, 0x01}},
	{LCM_SEND(4), {0xE1, 0x0B, 0x00, 0xED}},
	{LCM_SEND(4), {0xE1, 0x0C, 0x00, 0x02}},
	{LCM_SEND(4), {0xE1, 0x0D, 0x00, 0x1E}},
	{LCM_SEND(4), {0xE1, 0x0E, 0x00, 0x02}},
	{LCM_SEND(4), {0xE1, 0x0F, 0x00, 0x51}},
	{LCM_SEND(4), {0xE2, 0x00, 0x00, 0x02}},
	{LCM_SEND(4), {0xE2, 0x01, 0x00, 0x6C}},
	{LCM_SEND(4), {0xE2, 0x02, 0x00, 0x02}},
	{LCM_SEND(4), {0xE2, 0x03, 0x00, 0x8D}},
	{LCM_SEND(4), {0xE2, 0x04, 0x00, 0x02}},
	{LCM_SEND(4), {0xE2, 0x05, 0x00, 0xA5}},
	{LCM_SEND(4), {0xE2, 0x06, 0x00, 0x02}},
	{LCM_SEND(4), {0xE2, 0x07, 0x00, 0xC9}},
	{LCM_SEND(4), {0xE2, 0x08, 0x00, 0x02}},
	{LCM_SEND(4), {0xE2, 0x09, 0x00, 0xEA}},
	{LCM_SEND(4), {0xE2, 0x0A, 0x00, 0x03}},
	{LCM_SEND(4), {0xE2, 0x0B, 0x00, 0x19}},
	{LCM_SEND(4), {0xE2, 0x0C, 0x00, 0x03}},
	{LCM_SEND(4), {0xE2, 0x0D, 0x00, 0x45}},
	{LCM_SEND(4), {0xE2, 0x0E, 0x00, 0x03}},
	{LCM_SEND(4), {0xE2, 0x0F, 0x00, 0x7A}},
	{LCM_SEND(4), {0xE3, 0x00, 0x00, 0x03}},
	{LCM_SEND(4), {0xE3, 0x01, 0x00, 0xA0}},
	{LCM_SEND(4), {0xE3, 0x02, 0x00, 0x03}},
	{LCM_SEND(4), {0xE3, 0x03, 0x00, 0xF4}},
	{LCM_SEND(4), {0xE4, 0x00, 0x00, 0x00}},
	{LCM_SEND(4), {0xE4, 0x01, 0x00, 0x32}},
	{LCM_SEND(4), {0xE4, 0x02, 0x00, 0x00}},
	{LCM_SEND(4), {0xE4, 0x03, 0x00, 0x41}},
	{LCM_SEND(4), {0xE4, 0x04, 0x00, 0x00}},
	{LCM_SEND(4), {0xE4, 0x05, 0x00, 0x54}},
	{LCM_SEND(4), {0xE4, 0x06, 0x00, 0x00}},
	{LCM_SEND(4), {0xE4, 0x07, 0x00, 0x67}},
	{LCM_SEND(4), {0xE4, 0x08, 0x00, 0x00}},
	{LCM_SEND(4), {0xE4, 0x09, 0x00, 0x7A}},
	{LCM_SEND(4), {0xE4, 0x0A, 0x00, 0x00}},
	{LCM_SEND(4), {0xE4, 0x0B, 0x00, 0x98}},
	{LCM_SEND(4), {0xE4, 0x0C, 0x00, 0x00}},
	{LCM_SEND(4), {0xE4, 0x0D, 0x00, 0xB0}},
	{LCM_SEND(4), {0xE4, 0x0E, 0x00, 0x00}},
	{LCM_SEND(4), {0xE4, 0x0F, 0x00, 0xDB}},
	{LCM_SEND(4), {0xE5, 0x00, 0x00, 0x01}},
	{LCM_SEND(4), {0xE5, 0x01, 0x00, 0x01}},
	{LCM_SEND(4), {0xE5, 0x02, 0x00, 0x01}},
	{LCM_SEND(4), {0xE5, 0x03, 0x00, 0x3F}},
	{LCM_SEND(4), {0xE5, 0x04, 0x00, 0x01}},
	{LCM_SEND(4), {0xE5, 0x05, 0x00, 0x70}},
	{LCM_SEND(4), {0xE5, 0x06, 0x00, 0x01}},
	{LCM_SEND(4), {0xE5, 0x07, 0x00, 0xB4}},
	{LCM_SEND(4), {0xE5, 0x08, 0x00, 0x01}},
	{LCM_SEND(4), {0xE5, 0x09, 0x00, 0xEC}},
	{LCM_SEND(4), {0xE5, 0x0A, 0x00, 0x01}},
	{LCM_SEND(4), {0xE5, 0x0B, 0x00, 0xED}},
	{LCM_SEND(4), {0xE5, 0x0C, 0x00, 0x02}},
	{LCM_SEND(4), {0xE5, 0x0D, 0x00, 0x1E}},
	{LCM_SEND(4), {0xE5, 0x0E, 0x00, 0x02}},
	{LCM_SEND(4), {0xE5, 0x0F, 0x00, 0x51}},
	{LCM_SEND(4), {0xE6, 0x00, 0x00, 0x02}},
	{LCM_SEND(4), {0xE6, 0x01, 0x00, 0x6C}},
	{LCM_SEND(4), {0xE6, 0x02, 0x00, 0x02}},
	{LCM_SEND(4), {0xE6, 0x03, 0x00, 0x8D}},
	{LCM_SEND(4), {0xE6, 0x04, 0x00, 0x02}},
	{LCM_SEND(4), {0xE6, 0x05, 0x00, 0xA5}},
	{LCM_SEND(4), {0xE6, 0x06, 0x00, 0x02}},
	{LCM_SEND(4), {0xE6, 0x07, 0x00, 0xC9}},
	{LCM_SEND(4), {0xE6, 0x08, 0x00, 0x02}},
	{LCM_SEND(4), {0xE6, 0x09, 0x00, 0xEA}},
	{LCM_SEND(4), {0xE6, 0x0A, 0x00, 0x03}},
	{LCM_SEND(4), {0xE6, 0x0B, 0x00, 0x19}},
	{LCM_SEND(4), {0xE6, 0x0C, 0x00, 0x03}},
	{LCM_SEND(4), {0xE6, 0x0D, 0x00, 0x45}},
	{LCM_SEND(4), {0xE6, 0x0E, 0x00, 0x03}},
	{LCM_SEND(4), {0xE6, 0x0F, 0x00, 0x7A}},
	{LCM_SEND(4), {0xE7, 0x00, 0x00, 0x03}},
	{LCM_SEND(4), {0xE7, 0x01, 0x00, 0xA0}},
	{LCM_SEND(4), {0xE7, 0x02, 0x00, 0x03}},
	{LCM_SEND(4), {0xE7, 0x03, 0x00, 0xF4}},
	{LCM_SEND(4), {0xE8, 0x00, 0x00, 0x00}},
	{LCM_SEND(4), {0xE8, 0x01, 0x00, 0x32}},
	{LCM_SEND(4), {0xE8, 0x02, 0x00, 0x00}},
	{LCM_SEND(4), {0xE8, 0x03, 0x00, 0x41}},
	{LCM_SEND(4), {0xE8, 0x04, 0x00, 0x00}},
	{LCM_SEND(4), {0xE8, 0x05, 0x00, 0x54}},
	{LCM_SEND(4), {0xE8, 0x06, 0x00, 0x00}},
	{LCM_SEND(4), {0xE8, 0x07, 0x00, 0x67}},
	{LCM_SEND(4), {0xE8, 0x08, 0x00, 0x00}},
	{LCM_SEND(4), {0xE8, 0x09, 0x00, 0x7A}},
	{LCM_SEND(4), {0xE8, 0x0A, 0x00, 0x00}},
	{LCM_SEND(4), {0xE8, 0x0B, 0x00, 0x98}},
	{LCM_SEND(4), {0xE8, 0x0C, 0x00, 0x00}},
	{LCM_SEND(4), {0xE8, 0x0D, 0x00, 0xB0}},
	{LCM_SEND(4), {0xE8, 0x0E, 0x00, 0x00}},
	{LCM_SEND(4), {0xE8, 0x0F, 0x00, 0xDB}},
	{LCM_SEND(4), {0xE9, 0x00, 0x00, 0x01}},
	{LCM_SEND(4), {0xE9, 0x01, 0x00, 0x01}},
	{LCM_SEND(4), {0xE9, 0x02, 0x00, 0x01}},
	{LCM_SEND(4), {0xE9, 0x03, 0x00, 0x3F}},
	{LCM_SEND(4), {0xE9, 0x04, 0x00, 0x01}},
	{LCM_SEND(4), {0xE9, 0x05, 0x00, 0x70}},
	{LCM_SEND(4), {0xE9, 0x06, 0x00, 0x01}},
	{LCM_SEND(4), {0xE9, 0x07, 0x00, 0xB4}},
	{LCM_SEND(4), {0xE9, 0x08, 0x00, 0x01}},
	{LCM_SEND(4), {0xE9, 0x09, 0x00, 0xEC}},
	{LCM_SEND(4), {0xE9, 0x0A, 0x00, 0x01}},
	{LCM_SEND(4), {0xE9, 0x0B, 0x00, 0xED}},
	{LCM_SEND(4), {0xE9, 0x0C, 0x00, 0x02}},
	{LCM_SEND(4), {0xE9, 0x0D, 0x00, 0x1E}},
	{LCM_SEND(4), {0xE9, 0x0E, 0x00, 0x02}},
	{LCM_SEND(4), {0xE9, 0x0F, 0x00, 0x51}},
	{LCM_SEND(4), {0xEA, 0x00, 0x00, 0x02}},
	{LCM_SEND(4), {0xEA, 0x01, 0x00, 0x6C}},
	{LCM_SEND(4), {0xEA, 0x02, 0x00, 0x02}},
	{LCM_SEND(4), {0xEA, 0x03, 0x00, 0x8D}},
	{LCM_SEND(4), {0xEA, 0x04, 0x00, 0x02}},
	{LCM_SEND(4), {0xEA, 0x05, 0x00, 0xA5}},
	{LCM_SEND(4), {0xEA, 0x06, 0x00, 0x02}},
	{LCM_SEND(4), {0xEA, 0x07, 0x00, 0xC9}},
	{LCM_SEND(4), {0xEA, 0x08, 0x00, 0x02}},
	{LCM_SEND(4), {0xEA, 0x09, 0x00, 0xEA}},
	{LCM_SEND(4), {0xEA, 0x0A, 0x00, 0x03}},
	{LCM_SEND(4), {0xEA, 0x0B, 0x00, 0x19}},
	{LCM_SEND(4), {0xEA, 0x0C, 0x00, 0x03}},
	{LCM_SEND(4), {0xEA, 0x0D, 0x00, 0x45}},
	{LCM_SEND(4), {0xEA, 0x0E, 0x00, 0x03}},
	{LCM_SEND(4), {0xEA, 0x0F, 0x00, 0x7A}},
	{LCM_SEND(4), {0xEB, 0x00, 0x00, 0x03}},
	{LCM_SEND(4), {0xEB, 0x01, 0x00, 0xA0}},
	{LCM_SEND(4), {0xEB, 0x02, 0x00, 0x03}},
	{LCM_SEND(4), {0xEB, 0x03, 0x00, 0xF4}},
	{LCM_SEND(4), {0x3A, 0x00, 0x00, 0x07}},
	{LCM_SEND(4), {0x35, 0x00, 0x00, 0x00}},

	{LCM_SEND(2), {0x11, 0x00}},
};

static LCM_Init_Code disp_on =  {LCM_SEND(2), {0x29, 0x00}};

#if 0
LCD_ERR_E Set_NT35516_I2C_Read(uint16 addr, uint32 addr_len, uint16* data, uint32 data_len)
{
	uint32 rtn=LCD_ERR_NONE;
	uint8 cmd[2]={0x00};
	uint8 value[4]={0x00};
	//uint8 value=0;

	cmd[0]=(uint8)((addr>>0x08)&0xff);
	cmd[1]=(uint8)(addr&0xff);
	
	rtn=I2C_WriteCmdArr(NT35516_I2C_ADDR, cmd, addr_len, NT35516_I2C_ACK);
	rtn=I2C_ReadCmdArr(NT35516_I2C_ADDR, value, data_len,NT35516_I2C_ACK );

	*data=(value[0]<<0x08)|value[1];
        // *data = value;
	return rtn;
}
#endif

static int32_t nt35516_rgb_i2c_init(struct panel_spec *self)
{
	unsigned int i;
	unsigned int tag;
	uint16_t data=0x00;
	uint16_t data1=0x00;
	uint16_t data2=0x00;
	LCM_Init_Code *init = init_data;

	i2c_write_8bits_t i2c_write_8bits = self->info.rgb->bus_info.i2c->ops->i2c_write_8bits; 
	i2c_read_8bits_t i2c_read_8bits = self->info.rgb->bus_info.i2c->ops->i2c_read_8bits; 
	i2c_write_16bits_t i2c_write_16bits = self->info.rgb->bus_info.i2c->ops->i2c_write_16bits; 
	i2c_read_16bits_t i2c_read_16bits = self->info.rgb->bus_info.i2c->ops->i2c_read_16bits; 
	i2c_write_burst_t i2c_write_burst = self->info.rgb->bus_info.i2c->ops->i2c_write_burst; 

	printf("nt35516_rgb_i2c_init\n");


	//    Set_NT35516_I2C_Read(LCDI2C_ID_ADDR, 2, &data, 2);
	i2c_read_16bits(NT35516_I2C_ADDR, 0xda00, FALSE, &data, FALSE);
	i2c_read_16bits(NT35516_I2C_ADDR, 0xdb00, FALSE, &data1, FALSE);
	i2c_read_16bits(NT35516_I2C_ADDR, 0xdc00, FALSE, &data2, FALSE);

	for (i = 0 ; i < ARRAY_SIZE(init_data); i++ ) {
		tag = (init->tag >> 24);
		if (tag & LCM_TAG_SEND) {
			Set_NT35516_I2C_Send( (init->tag & LCM_TAG_MASK) , init->data);
			printf("send one i2c data!\n");
		} else if (tag & LCM_TAG_SLEEP) {
			udelay((init->tag & LCM_TAG_MASK)*1000);
			printf("delay!\n");
		}
		init++;
	} 
	udelay(200);    
	// while(1)
	// 	{
	printf("send disp_on!\n");
	Set_NT35516_I2C_Send( (disp_on.tag & LCM_TAG_MASK) , disp_on.data);
	// 	}

	//I2C_ReadCmd(0xDB00,pCmd,NT35516_I2C_ACK);
	//Set_NT35516_I2C_Read(0xdb00, 2, &data, 2);
	printf("nt35516_rgb_i2c_init finish\n");

	return 0;
}

static uint32_t nt35516_rgb_i2c_readid(struct panel_spec *self)
{
	/*Jessica TODO: need read id*/
	return 0x16;
}

#if 0
void NT35516_RGB_I2C_set_display_window(
    uint16 left,     // start Horizon address
    uint16 right,     // end Horizon address
    uint16 top,         // start Vertical address
    uint16 bottom    // end Vertical address
    )
{
    unsigned char data[4];
	data[0] = 0x2A; 
	data[1] = 0x00; data[2] = (left>>8);
	Set_NT35516_I2C_Send(3, data);
	
	data[1] = 0x01; data[2] = (left&0xff);
	Set_NT35516_I2C_Send(3, data);
	
	data[1] = 0x02; data[2] = (right>>8);
	Set_NT35516_I2C_Send(3, data);

	data[1] = 0x03; data[2] = (right&0xff);
	Set_NT35516_I2C_Send(3, data);

	data[0] = 0x2B; 

	data[1] = 0x00; data[2] = (top>>8);
	Set_NT35516_I2C_Send(3, data);
	
	data[1] = 0x01; data[2] = (top&0xff);
	Set_NT35516_I2C_Send(3, data);
	
	data[1] = 0x02; data[2] = (bottom>>8);
	Set_NT35516_I2C_Send(3, data);

	data[1] = 0x03; data[2] = (bottom&0xff);
	Set_NT35516_I2C_Send(3, data);

}

LCD_ERR_E NT35516_RGB_I2C_EnterSleep(BOOLEAN is_sleep)
{
    unsigned char data[4];
    if(is_sleep==1)
    {
		data[0] = 0x28;data[1] = 0x00; 
		Set_NT35516_I2C_Send(2, data);
        LCD_Delay(200);

		data[0] = 0x10;data[1] = 0x00; 
		Set_NT35516_I2C_Send(2, data);
        LCD_Delay(200);
        //Lcd_EnvidOnOff(0);//RGB TIMENG OFF
        //LCD_Delay(200);
    }
    else
    {
        //Lcd_EnvidOnOff(1);//RGB TIMENG ON 
        //LCD_Delay(200);
        //LCDinit_TFT();
        //LCD_Delay(200);

    }

    return 0;
}

LCD_ERR_E NT35516_RGB_I2C_SetDisplayWindow(
    uint16 left,         //left of the window
    uint16 top,            //top of the window
    uint16 right,        //right of the window
    uint16 bottom        //bottom of the window
    )
{
    unsigned char data[4];
    //NT35516_RGB_I2C_set_display_window(left, right, top, bottom);
	data[0] = 0x2C; data[1] = 0x00; 
	Set_NT35516_I2C_Send(2, data);
    return TRUE;
}
#endif



static struct panel_operations lcd_nt35516_rgb_i2c_operations = {
	.panel_init = nt35516_rgb_i2c_init,
	.panel_readid = nt35516_rgb_i2c_readid,
};

static struct timing_rgb lcd_nt35516_rgb_timing = {
	.hfp = 16,  /* unit: pixel */
	.hbp = 16,
	.hsync = 1,
	.vfp = 16, /*unit: line*/
	.vbp = 16,
	.vsync = 1,
};

static struct i2c_info lcd_nt35516_rgb_i2c_info = {
	.i2c_addr = NT35516_I2C_ADDR,
	.ops = NULL,
};

static struct info_rgb lcd_nt35516_rgb_info = {
	.cmd_bus_mode  = SPRDFB_RGB_BUS_TYPE_I2C,
	.video_bus_width = 24, /*18,16*/
	.h_sync_pol = SPRDFB_POLARITY_NEG, //SPRDFB_POLARITY_POS,
	.v_sync_pol = SPRDFB_POLARITY_NEG, //SPRDFB_POLARITY_POS,
	.de_pol = SPRDFB_POLARITY_POS,
	.timing = &lcd_nt35516_rgb_timing,
	.bus_info = {
		.i2c = &lcd_nt35516_rgb_i2c_info,
	}
};

struct panel_spec lcd_nt35516_rgb_i2c_spec = {
	.width = 540,
	.height = 960,
	.type = LCD_MODE_RGB,
	.direction = LCD_DIRECT_NORMAL,
	.info = {
		.rgb = &lcd_nt35516_rgb_info
	},
	.ops = &lcd_nt35516_rgb_i2c_operations,
};
