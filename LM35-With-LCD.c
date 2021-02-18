/*
* LAB Name: LM35 Temp. Sensor + LCD
* Author: Mohamed Hamdi
* please visit my youtube channel:https://www.youtube.com/channel/UCzt_0MFbJiT93KSR74pW6Jw
*/
#include <xc.h>
#include "config.h"
#include <stdint.h>
#include <stdio.h>
//=================================
//--------[ Definitiotns ]---------
#define _XTAL_FREQ 4000000
#define LCD_EN_Delay 500
#define LCD_DATA_PORT_D TRISD
#define LCD_RS_D TRISD2
#define LCD_EN_D TRISD1
#define RS RD2
#define EN RD1
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7
//=================================
//-----[ Globals ]-----
uint16_t AN0RES=0;
float Temperature, Voltage;
char* line1 = " Temperature Is ";
char* TempSTR[16];
//=================================
//-----[ Functions Prototypes ]----
void ADC_Init();
uint16_t ADC_Read(uint8_t);
void LCD_Init();
void LCD_Clear();
void LCD_CMD(char);
void LCD_DATA(char);
void LCD_Set_Cursor(char, char);
void LCD_Write_Char(char);
void LCD_Write_String(char*);
//=================================
//---------[ Main Routine]---------
void main(void) {
  LCD_Init();
  LCD_Clear();
  ADC_Init();
  while(1)
  {
    // Read The ADC
    AN0RES = ADC_Read(0); // Read Analog Channel 0
    // Calculate The Temperature
    Voltage = AN0RES * 0.0048828;
    Temperature = Voltage / 0.01;
    // Convert The Temperature From Float To String
    sprintf(TempSTR, " %.3fc", Temperature);
    // Print Out The Temperature
    LCD_Set_Cursor(1,1);
    LCD_Write_String(line1);
    LCD_Set_Cursor(2,1);
    LCD_Write_String(TempSTR);
    __delay_ms(10);
  }
  return;
}
//=================================
//--------[ ADC Routines ]---------
void ADC_Init()
{
  ADCON0 = 0x41; // Turn ADC ON, Select AN0 Channel, ADC Clock = Fosc/8
  ADCON1 = 0x80; // All 8 Channels Are Analog, Result is "Right-Justified"
  // ADC Clock = Fosc/8
}
uint16_t ADC_Read(uint8_t ANC)
{
  if(ANC<0 || ANC>7) // Check Channel Number Validity
  { return 0;}
  ADCON0 &= 0x11000101; // Clear The Channel Selection Bits
  ADCON0 |= ANC<<3; // Select The Required Channel (ANC)
  // Wait The Aquisition Time
  __delay_us(30); // The Minimum Tacq = 20us, So That should be enough
  GO_DONE = 1; // Start A/D Conversion
  while(ADCON0bits.GO_DONE); // Polling GO_DONE Bit
  // Provides Delay Until Conversion Is Complete
  return ((ADRESH << 8) + ADRESL); // Return The Right-Justified 10-Bit Result
}
//=================================
//--------[ LCD Routines ]---------
void LCD_DATA(char Data)
{
  if(Data& 1)
    D4 = 1;
  else
    D4 = 0;
  if(Data& 2)
    D5 = 1;
  else
    D5 = 0;
  if(Data& 4)
    D6 = 1;
  else
    D6 = 0;
  if(Data& 8)
    D7 = 1;
  else
    D7 = 0;
}
void LCD_CMD(char a)
{
  RS = 0;
  LCD_DATA(a); //Incoming Hex value
  EN = 1;
  __delay_ms(4);
  EN = 0;
}
void LCD_Clear()
{
  LCD_CMD(0); //Clear the LCD
  LCD_CMD(1); //Move the cursor to first position
}
void LCD_Set_Cursor(char a, char b)
{
  char Temp,z,y;
  if(a== 1)
  {
    Temp = 0x80 + b - 1;
    z = Temp>>4; //Lower 8-bits
    y = Temp & 0x0F; //Upper 8-bits
    LCD_CMD(z); //Set Row
    LCD_CMD(y); //Set Column
  }
  else if(a== 2)  
  {
    Temp = 0xC0 + b - 1;
    z = Temp>>4; //Lower 8-bits
    y = Temp & 0x0F; //Upper 8-bits
    LCD_CMD(z); //Set Row
    LCD_CMD(y); //Set Column
  }
}
void LCD_Init()
{
  // IO Pin Configurations
  LCD_DATA_PORT_D = 0x00;
  LCD_RS_D = 0;
  LCD_EN_D = 0;
  // The Procedure As Described in Datasheet
  LCD_DATA(0x00);
  __delay_us(LCD_EN_Delay);
  LCD_CMD(0x03);
  __delay_ms(5);
  LCD_CMD(0x03);
  __delay_ms(11);
  LCD_CMD(0x03);
  LCD_CMD(0x02); //Clears the RAM and initializes the LCD
  LCD_CMD(0x02); //Clears the RAM and initializes the LCD
  LCD_CMD(0x08); //Select Row 1
  LCD_CMD(0x00); //Clear Row 1 Display
  LCD_CMD(0x0C); //Select Row 2
  LCD_CMD(0x00); //Clear Row 2 Display
  LCD_CMD(0x06);
}
void LCD_Write_Char(char data)
{
  char Lower_Nibble,Upper_Nibble;
  Lower_Nibble = data&0x0F;
  Upper_Nibble = data&0xF0;
  RS = 1;
  LCD_DATA(Upper_Nibble>>4);
  EN = 1;
  __delay_us(LCD_EN_Delay);
  EN = 0;
  __delay_us(LCD_EN_Delay);
  LCD_DATA(Lower_Nibble);
  EN = 1;
  __delay_us(LCD_EN_Delay);
  EN = 0;
  __delay_us(LCD_EN_Delay);
}
void LCD_Write_String(char *a)
{
  int i;
  for(i=0;a[i]!='\0';i++)
    LCD_Write_Char(a[i]);
}