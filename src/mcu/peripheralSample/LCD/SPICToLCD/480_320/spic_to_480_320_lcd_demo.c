/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     lcd_demo.c
* @brief        This file provides demo code of LCD display whose resolution is 480*320.
* @details
* @author   elliot chen
* @date         2018-1-16
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_lcd.h"
#include "rtl876x_gpio.h"
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "flash_driver.h"
#include "string.h"
#include "app_section.h"

/* Defines -------------------------------------------------------------------*/
/* LCD reset pin and BL pin defines -----------------------*/
#define LCD_RST_PIN                     P0_0
#define LCD_BL_PIN                      P0_5
/* LCD color */
#define BLUE                            ((uint32_t)0x001F)
#define GREEN                           ((uint32_t)0x07E0)
/* Picture size */
#define LCD_X_MAX_SIZE                  (320)
#define LCD_Y_MAX_SIZE                  (480)
#define PICTURE_MAX_SIZE                (LCD_X_MAX_SIZE*LCD_Y_MAX_SIZE*2)
#define PICTURE_FRAME_SIZE              ((uint32_t)(61440))
#define PICTURE_FRAME_Y_SIZE            (96)
#define GDMA_TRANS_COUNT                (5)
#define GDMA_SOURCE_DATA_WIDTH          ((uint32_t)4)

/* GDMA defines ---------------------------------------*/
#define GDMA_Channel_TX                 GDMA_Channel0
#define GDMA_Channel_TX_NUM             0
#define GDMA_FRAME_SIZE                 4000
#define GDMA0_Channel_TX_IRQn           GDMA0_Channel0_IRQn
#define GDMA0_Channel_TX_Handler        GDMA0_Channel0_Handler
#define SPIC_DR_ADDR                    (0x40080060UL)

/* Globals -------------------------------------------------------------*/
APP_DATAON_BSS_SECTION GDMA_LLIDef GDMA_LLIStruct[10];

uint32_t flash_address = 0x1960000;
/* GDMA transmission control */
APP_DATAON_BSS_SECTION volatile uint32_t gdma_intr_count = 0;

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
void Board_LCD_Init(void)
{
    Pad_Config(LCD_RST_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_BL_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pinmux_Config(LCD_RST_PIN, DWGPIO);
    Pinmux_Config(LCD_BL_PIN, DWGPIO);

#if 0
    Pad_Config(P0_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P0_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P1_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P1_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P3_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P3_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P3_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P3_5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    Pad_Config(P4_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P4_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);


    Pinmux_Config(P0_2, IDLE_MODE);
    Pinmux_Config(P0_4, IDLE_MODE);
    Pinmux_Config(P1_3, IDLE_MODE);
    Pinmux_Config(P1_4, IDLE_MODE);

    Pinmux_Config(P3_2, IDLE_MODE);
    Pinmux_Config(P3_3, IDLE_MODE);
    Pinmux_Config(P3_4, IDLE_MODE);
    Pinmux_Config(P3_5, IDLE_MODE);

    Pinmux_Config(P4_0, IDLE_MODE);
    Pinmux_Config(P4_1, IDLE_MODE);
    Pinmux_Config(P4_2, IDLE_MODE);
    Pinmux_Config(P4_3, IDLE_MODE);
#else
    //Pad_Config(P1_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    //Pad_Config(P1_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P0_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P0_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P0_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P2_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P3_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P3_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P3_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P3_5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    Pad_Config(P4_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P4_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    /* 8080 interface: D0~D7 */
    Pinmux_Config(P3_5, IDLE_MODE);
    Pinmux_Config(P0_1, IDLE_MODE);
    Pinmux_Config(P0_2, IDLE_MODE);
    Pinmux_Config(P0_4, IDLE_MODE);
    Pinmux_Config(P4_0, IDLE_MODE);
    Pinmux_Config(P4_1, IDLE_MODE);
    Pinmux_Config(P4_2, IDLE_MODE);
    Pinmux_Config(P4_3, IDLE_MODE);

    /* RD */
    Pinmux_Config(P2_0, IDLE_MODE);
    /* WR */
    Pinmux_Config(P3_2, IDLE_MODE);
    /* CS */
    Pinmux_Config(P3_3, IDLE_MODE);
    /* DCX */
    Pinmux_Config(P3_4, IDLE_MODE);
#endif
}

/**
  * @brief  Initialize LCD controller peripheral.
  * @param   No parameter.
  * @return  void
  */
void Driver_LCD_Init(void)
{
    RCC_PeriphClockCmd(APBPeriph_LCD, APBPeriph_LCD_CLOCK, ENABLE);
    LCD_PinGroupConfig(LCD_PinGroup_2);
    LCD_InitTypeDef LCD_InitStruct;
    LCD_StructInit(&LCD_InitStruct);
    LCD_InitStruct.LCD_ClockDiv         = LCD_CLOCK_DIV_2;
    LCD_InitStruct.LCD_Mode             = LCD_MODE_MANUAL;
    LCD_InitStruct.LCD_GuardTimeCmd     = LCD_GUARD_TIME_DISABLE;
    LCD_InitStruct.LCD_GuardTime        = LCD_GUARD_TIME_2T;
    LCD_InitStruct.LCD_8BitSwap         = LCD_8BitSwap_ENABLE;
    LCD_InitStruct.LCD_16BitSwap        = LCD_16BitSwap_DISABLE;
    LCD_InitStruct.LCD_TxThr            = 10;
    LCD_InitStruct.LCD_TxDMACmd         = LCD_TX_DMA_DISABLE;
    LCD_Init(&LCD_InitStruct);
    LCD_Cmd(ENABLE);
}

/**
  * @brief Send command.
  * @param   cmd: command index.
  * @return  void
  */
void WriteComm(uint8_t cmd)
{
    LCD_SetCS();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    LCD_ResetCS();
    LCD_SendCommand(cmd);
}

/**
  * @brief Send data.
  * @param   data: data to be sent.
  * @return  void
  */
void WriteData(uint8_t data)
{
    LCD_SendData(&data, 1);
}

/**
  * @brief Configure parameter of block write.
  * @param  xStart: start position of X axis.
  * @param  xEnd: end position of X axis.
   * @param yStart: start position of Y axis.
  * @param  yEnd: end position of Y axis.
  * @return  void
  */
void WriteBlock(uint16_t xStart, uint16_t xEnd, uint16_t yStart, uint16_t yEnd)
{
    WriteComm(0x2a);
    WriteData(xStart >> 8);
    WriteData(xStart & 0xff);
    WriteData(xEnd >> 8);
    WriteData(xEnd & 0xff);

    WriteComm(0x2b);
    WriteData(yStart >> 8);
    WriteData(yStart & 0xff);
    WriteData(yEnd >> 8);
    WriteData(yEnd & 0xff);
    LCD_SetCS();
}

/**
  * @brief Configure parameter of block write.
  * @param  xStart: start position of X axis.
  * @param  yStart: start position of Y axis.
   * @param xWidth: width of X axis.
  * @param  yWidth: width of Y axis.
  * @param  color: color data.
  * @return  void
  */
void WriteColorBox(uint16_t xStart, uint16_t yStart, uint16_t xWidth, uint16_t yWidth,
                   uint16_t color)
{
    uint32_t temp = 0;

    WriteBlock(xStart, xStart + xWidth - 1, yStart, yStart + yWidth - 1);

    WriteComm(0x2c);

    for (temp = 0; temp < xWidth * yWidth; temp++)
    {
        WriteData(color >> 8);
        WriteData(color);
    }
    LCD_SetCS();
}

/**
  * @brief  Software delay.
  * @param   nCount: delay count.
  * @return  void
  */
volatile static void delay(uint32_t nCount)
{
    volatile int i;
    for (i = 0; i < 7200; i++)
        for (; nCount != 0; nCount--);
}

void ili9488_Init(void)
{
    WriteComm(0xE0);
    WriteData(0x00);
    WriteData(0x07);
    WriteData(0x10);
    WriteData(0x09);
    WriteData(0x17);
    WriteData(0x0B);
    WriteData(0x40);
    WriteData(0x8A);
    WriteData(0x4B);
    WriteData(0x0A);
    WriteData(0x0D);
    WriteData(0x0F);
    WriteData(0x15);
    WriteData(0x16);
    WriteData(0x0F);

    WriteComm(0xE1);
    WriteData(0x00);
    WriteData(0x1A);
    WriteData(0x1B);
    WriteData(0x02);
    WriteData(0x0D);
    WriteData(0x05);
    WriteData(0x30);
    WriteData(0x35);
    WriteData(0x43);
    WriteData(0x02);
    WriteData(0x0A);
    WriteData(0x09);
    WriteData(0x32);
    WriteData(0x36);
    WriteData(0x0F);

    WriteComm(0xB1);
    WriteData(0xA0);

    WriteComm(0xB4);
    WriteData(0x02);

    WriteComm(0xC0);
    WriteData(0x17);
    WriteData(0x15);

    WriteComm(0xC1);
    WriteData(0x41);

    WriteComm(0xC5);
    WriteData(0x00);
    WriteData(0x0A);
    WriteData(0x80);

    WriteComm(0xB6);
    WriteData(0x02);

    WriteComm(0x36);
    WriteData(0x48);

    /* Interface pixel format */
    WriteComm(0x3a);
    WriteData(0x55);

    WriteComm(0xE9);
    WriteData(0x00);

    WriteComm(0XF7);
    WriteData(0xA9);
    WriteData(0x51);
    WriteData(0x2C);
    WriteData(0x82);

    /* Sleep out */
    WriteComm(0x11);
    delay(120);
    /* Display ON */
    WriteComm(0x29);
    /* Pull CS up */
    LCD_SetCS();
}

/**
  * @brief  Initialize external LCD.
  * @param   No parameter.
  * @return  void
  */
void Lcd_Initialize(void)
{
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_ControlMode = GPIO_SOFTWARE_MODE;
    GPIO_InitStruct.GPIO_Pin  = GPIO_GetPin(LCD_RST_PIN) | GPIO_GetPin(LCD_BL_PIN);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd = DISABLE;
    GPIO_Init(&GPIO_InitStruct);

    GPIO_ResetBits(GPIO_GetPin(LCD_RST_PIN));
    delay(10000);
    GPIO_SetBits(GPIO_GetPin(LCD_RST_PIN));
    delay(10000);

    /* Initialize ili9488 */
    ili9488_Init();

    /* Clear screen */
    WriteColorBox(0, 0, 320, 480, GREEN);
    GPIO_SetBits(GPIO_GetPin(LCD_BL_PIN));
}

/**
  * @brief  Initialize GDMA peripheral.
  * @param   No parameter.
  * @return  void
  */
void GDMA_Config_LLIStruct(GDMA_InitTypeDef GDMA_InitStruct)
{
    /* Configure LLI struct */
    uint32_t i = 0;
    uint32_t blk_count = PICTURE_FRAME_SIZE / (GDMA_FRAME_SIZE * GDMA_SOURCE_DATA_WIDTH);
    uint32_t remainer = PICTURE_FRAME_SIZE % (GDMA_FRAME_SIZE * GDMA_SOURCE_DATA_WIDTH) /
                        GDMA_SOURCE_DATA_WIDTH;

    /* No need Multi-block */
    if (blk_count == 0)
    {
        GDMA_SetBufferSize(GDMA_Channel_TX, remainer);

        GDMA_LLIStruct[i].LLP = 0;
        GDMA_LLIStruct[i].SAR = (uint32_t)(SPIC_DR_ADDR);
        GDMA_LLIStruct[i].DAR = (uint32_t)(&(LCD->FIFO));
        /* configure low 32 bit of CTL register */
        GDMA_LLIStruct[i].CTL_LOW = BIT(0)
                                    | (GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                    | (GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                    | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                                    | (GDMA_InitStruct.GDMA_SourceInc << 9)
                                    | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                    | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                                    | (GDMA_InitStruct.GDMA_DIR << 20);
        /* configure high 32 bit of CTL register */
        GDMA_LLIStruct[i].CTL_HIGH = remainer;
    }
    else
    {
        /* Check LLI is overflow or not */
        if (remainer > 0)
        {
            if ((sizeof(GDMA_LLIStruct) / sizeof(GDMA_LLIStruct[0])) <= blk_count)
            {
                return;
            }
        }
        else
        {
            if ((sizeof(GDMA_LLIStruct) / sizeof(GDMA_LLIStruct[0])) < blk_count)
            {
                return;
            }
        }

        /* Configure LLI */
        for (i = 0; i < blk_count; i++)
        {
            GDMA_LLIStruct[i].SAR = (uint32_t)(SPIC_DR_ADDR);
            GDMA_LLIStruct[i].DAR = (uint32_t)(&(LCD->FIFO));
            GDMA_LLIStruct[i].LLP = (uint32_t)&GDMA_LLIStruct[i + 1];
            /* configure low 32 bit of CTL register */
            GDMA_LLIStruct[i].CTL_LOW = BIT(0)
                                        | (GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                        | (GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                        | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                                        | (GDMA_InitStruct.GDMA_SourceInc << 9)
                                        | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                        | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                                        | (GDMA_InitStruct.GDMA_DIR << 20)
                                        | (GDMA_InitStruct.GDMA_Multi_Block_Mode & LLP_SELECTED_BIT);
            /* configure high 32 bit of CTL register */
            GDMA_LLIStruct[i].CTL_HIGH = GDMA_FRAME_SIZE;
        }

        if (remainer > 0)
        {
            GDMA_LLIStruct[i].LLP = 0;
            GDMA_LLIStruct[i].SAR = (uint32_t)(SPIC_DR_ADDR);
            GDMA_LLIStruct[i].DAR = (uint32_t)(&(LCD->FIFO));
            /* configure low 32 bit of CTL register */
            GDMA_LLIStruct[i].CTL_LOW = BIT(0)
                                        | (GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                        | (GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                        | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                                        | (GDMA_InitStruct.GDMA_SourceInc << 9)
                                        | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                        | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                                        | (GDMA_InitStruct.GDMA_DIR << 20);
            /* configure high 32 bit of CTL register */
            GDMA_LLIStruct[i].CTL_HIGH = remainer;
        }
        else
        {
            GDMA_LLIStruct[i - 1].LLP = 0;

            /* configure low 32 bit of CTL register */
            GDMA_LLIStruct[i - 1].CTL_LOW = BIT(0)
                                            | (GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                            | (GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                            | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                                            | (GDMA_InitStruct.GDMA_SourceInc << 9)
                                            | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                            | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                                            | (GDMA_InitStruct.GDMA_DIR << 20);
        }
    }

}

/**
  * @brief  Initialize GDMA peripheral.
  * @param   No parameter.
  * @return  void
  */
void Driver_GDMATx_Init(void)
{
    static bool LLI_Config_Flag = false;

    /* Enable GDMA clock */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = GDMA_Channel_TX_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize          = GDMA_FRAME_SIZE;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_SourceAddr          = SPIC_DR_ADDR;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&(LCD->FIFO));
    GDMA_InitStruct.GDMA_SourceHandshake     = 21;
    GDMA_InitStruct.GDMA_DestHandshake       = 0x16;
    GDMA_InitStruct.GDMA_Multi_Block_Mode    = LLI_TRANSFER;
    GDMA_InitStruct.GDMA_Multi_Block_En      = 1;
    GDMA_InitStruct.GDMA_Multi_Block_Struct  = (uint32_t)GDMA_LLIStruct;

    if (!LLI_Config_Flag)
    {
        LLI_Config_Flag = true;
        GDMA_Config_LLIStruct(GDMA_InitStruct);
        GDMA_Init(GDMA_Channel_TX, &GDMA_InitStruct);
        GDMA_Channel_TX->CFG_HIGH |= 0x02;
        GDMA_INTConfig(GDMA_Channel_TX_NUM, GDMA_INT_Transfer | GDMA_INT_Error, ENABLE);
    }
    else
    {
        GDMA_Init(GDMA_Channel_TX, &GDMA_InitStruct);
        GDMA_Channel_TX->CFG_HIGH |= 0x02;
        GDMA_INTConfig(GDMA_Channel_TX_NUM, GDMA_INT_Transfer | GDMA_INT_Error, ENABLE);
        return ;
    }

    /* Config FIFO mode and Flow control mode */
    //GDMA_Channel_TX->CFG_HIGH &= ~0x03;
    GDMA_Channel_TX->CFG_HIGH |= 0x02;

    /*  Enable GDMA IRQ  */
    NVIC_ClearPendingIRQ((IRQn_Type)GDMA0_Channel_TX_IRQn);
    NVIC_SetPriority((IRQn_Type)GDMA0_Channel_TX_IRQn, 3);
    NVIC_EnableIRQ((IRQn_Type)GDMA0_Channel_TX_IRQn);
}

/**
  * @brief  Write data by auto mode.
  * @param  void.
  * @return  void
  */
void Lcd_AutoWrite(uint8_t cmd, uint32_t flash_addr)
{
    LCD_ClearDataCounter();

    /* Stop LCD parallel interface */
    LCD_Cmd(DISABLE);
    LCD_SetCS();

    /* Enable Auto mode */
    LCD_SwitchMode(LCD_MODE_AUTO);

    /* Configure command */
    LCD_SetCmdSequence(&cmd, 1);

    /* Enable GDMA TX */
    LCD_GDMACmd(ENABLE);

    /* Configure pixel number */
    LCD_SetTxDataLen(PICTURE_FRAME_SIZE);

    /* Configure SPIC mode */
    flash_split_read_by_dma_setup(flash_addr, PICTURE_FRAME_SIZE);
    SPIC->dmardlr = 8;

    /* Start output */
    LCD_Cmd(ENABLE);
    /* Enable GDMA */
    GDMA_Cmd(GDMA_Channel_TX_NUM, ENABLE);

    /* Enable SPIC module */
    SPIC->ssienr = BIT(0);
}

/**
  * @brief  demo code of LCD controller communication.
  * @param   No parameter.
  * @return  void
  */
void LCD_DemoCode(void)
{
    /* Switch SPIC to 4-bit mode */
    extern uint32_t flash_ioctl(uint32_t cmd, uint32_t param_1, uint32_t param_2);
    //flash_set_baudrate(1);
    flash_ioctl(0x3004, 2, 0);
    static uint8_t j = 0;
    j = flash_ioctl(5, FLASH_MODE_4BIT, 0);//flash_ioctl_get_curr_bit_mode

    /* Initialize LCD peripheral */
    Board_LCD_Init();
    Driver_LCD_Init();

    Lcd_Initialize();
    WriteColorBox(0, 0, LCD_X_MAX_SIZE - 1, LCD_Y_MAX_SIZE - 1, BLUE);

    while (1)
    {
        /* Configure data block */
        LCD_Cmd(DISABLE);
        LCD_SwitchMode(LCD_MODE_MANUAL);
        LCD_Cmd(ENABLE);
        WriteBlock(0, LCD_X_MAX_SIZE - 1, 0, PICTURE_FRAME_Y_SIZE);

        /* Auto mode operation */
        Driver_GDMATx_Init();
        flash_address = 0x1960000;
        Lcd_AutoWrite(0x2C, flash_address);
        while (GDMA_GetChannelStatus(GDMA_Channel_TX_NUM) == SET);
        while (gdma_intr_count < GDMA_TRANS_COUNT);
        gdma_intr_count = 0;
        delay(15000000);

        /* Configure data block */
        LCD_Cmd(DISABLE);
        LCD_SwitchMode(LCD_MODE_MANUAL);
        LCD_Cmd(ENABLE);
        WriteBlock(0, LCD_X_MAX_SIZE - 1, 0, PICTURE_FRAME_Y_SIZE);

        /* Auto mode operation */
        Driver_GDMATx_Init();
        flash_address = 0x18B0000;
        Lcd_AutoWrite(0x2C, flash_address);
        while (GDMA_GetChannelStatus(GDMA_Channel_TX_NUM) == SET);
        while (gdma_intr_count < GDMA_TRANS_COUNT);
        gdma_intr_count = 0;
        delay(15000000);
    }
}

/**
* @brief GDMA interrupt handler function.
* @param   No parameter.
* @return  void
*/
void GDMA0_Channel_TX_Handler(void)
{
    GDMA_ClearINTPendingBit(GDMA_Channel_TX_NUM, GDMA_INT_Transfer | GDMA_INT_Error);

    /* Frame transmission control */
    gdma_intr_count++;
    if (gdma_intr_count >= GDMA_TRANS_COUNT)
    {
        return;
    }

    /* Configure new flash data */
    Driver_GDMATx_Init();
    /* Configure data block */
    LCD_Cmd(DISABLE);
    LCD_SwitchMode(LCD_MODE_MANUAL);
    LCD_Cmd(ENABLE);
    WriteBlock(0, LCD_X_MAX_SIZE - 1, PICTURE_FRAME_Y_SIZE * gdma_intr_count,
               PICTURE_FRAME_Y_SIZE * (gdma_intr_count + 1));
    Lcd_AutoWrite(0x2C, flash_address + PICTURE_FRAME_SIZE * gdma_intr_count);
}

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/

