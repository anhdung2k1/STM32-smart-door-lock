#include <stdio.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "ov7670.h"
#include "ov7670Config.h"
#include "ov7670Reg.h"
#include "tft_spi.h"
#include "applicationSettings.h"

/*** Internal Const Values, Macros ***/
#define OV7670_QVGA_WIDTH  320
#define OV7670_QVGA_HEIGHT 240

/*** Internal Static Variables ***/
static DCMI_HandleTypeDef *sp_hdcmi;
static DMA_HandleTypeDef  *sp_hdma_dcmi;
static I2C_HandleTypeDef  *sp_hi2c;
uint32_t s_destAddressForContiuousMode;
static void (* s_cbHsync)(uint32_t h);
static void (* s_cbVsync)(uint32_t v);
static uint32_t s_currentH;
static uint32_t s_currentV;

/*** Internal Function Declarations ***/
static RET ov7670_write(uint8_t regAddr, uint8_t data);
static RET ov7670_read(uint8_t regAddr, uint8_t *data);

/*** External Function Defines ***/
void ov7670_gpio_init(void){
		GPIO_InitTypeDef GPIO_InitStruct = {0};

	    // Configure GPIO pins for camera
	    __HAL_RCC_GPIOA_CLK_ENABLE();
	    __HAL_RCC_GPIOB_CLK_ENABLE();
	    __HAL_RCC_GPIOC_CLK_ENABLE();

	    GPIO_InitStruct.Pin = HS_Pin | PCLK_Pin | XCLK_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = CAMERA_RESET_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = SDA_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = SCL_Pin;
	   	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	   	GPIO_InitStruct.Pull = GPIO_NOPULL;
	   	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	   	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = D0_Pin | D1_Pin | D2_Pin | D3_Pin | D4_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = D5_Pin | D6_Pin | D7_Pin | VS_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	    // Configure LCD Pins
	    GPIO_InitStruct.Pin = LCD_CS_Pin | LCD_DC_Pin | LCD_RST_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = GPIO_PIN_4;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void ov7670_config_dcmi(void){
		__HAL_RCC_DCMI_CLK_ENABLE();

	    // Configure DCMI peripheral
		sp_hdcmi->Instance = DCMI;
		sp_hdcmi->Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
		sp_hdcmi->Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
		sp_hdcmi->Init.VSPolarity = DCMI_VSPOLARITY_LOW;
		sp_hdcmi->Init.HSPolarity = DCMI_HSPOLARITY_LOW;
		sp_hdcmi->Init.CaptureRate = DCMI_CR_ALL_FRAME;
		sp_hdcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
	    HAL_DCMI_Init(sp_hdcmi);
	    // Start DCMI capture
	    HAL_DCMI_Start_DMA(sp_hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)s_destAddressForContiuousMode, (IMAGE_SIZE_HEIGHT * IMAGE_SIZE_WIDTH * 2));
}
RET ov7670_init(DCMI_HandleTypeDef *p_hdcmi, DMA_HandleTypeDef *p_hdma_dcmi, I2C_HandleTypeDef *p_hi2c)
{
    sp_hdcmi = p_hdcmi;
    sp_hdma_dcmi = p_hdma_dcmi;
    sp_hi2c = p_hi2c;
    s_destAddressForContiuousMode = 0;

    ov7670_gpio_init();
    ov7670_config_dcmi();

    // Camera reset sequence
    HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    // Camera initialization
    ov7670_write(0x12, 0x80);  // RESET
    HAL_Delay(30);

    uint8_t buffer[4];
    ov7670_read(0x0b, buffer);
    printf("[OV7670] dev id = %02X\n", buffer[0]);

    return RET_OK;
}

RET ov7670_config(uint32_t mode)
{
    ov7670_stopCap();
    ov7670_write(0x12, 0x80);  // RESET
    HAL_Delay(30);

    // Configure camera registers
    for (int i = 0; OV7670_reg[i][0] != REG_BATT; i++) {
        ov7670_write(OV7670_reg[i][0], OV7670_reg[i][1]);
        HAL_Delay(1);
    }

    return RET_OK;
}

RET ov7670_startCap(uint32_t capMode, uint32_t destAddress)
{
    ov7670_stopCap();

    if (capMode == OV7670_CAP_CONTINUOUS) {
        /* Note: continuous mode automatically invokes DCMI, but DMA needs to be invoked manually */
        s_destAddressForContiuousMode = destAddress;
        HAL_DCMI_Start_DMA(sp_hdcmi, DCMI_MODE_CONTINUOUS, destAddress, OV7670_QVGA_WIDTH * OV7670_QVGA_HEIGHT / 2);
    } else if (capMode == OV7670_CAP_SINGLE_FRAME) {
        s_destAddressForContiuousMode = 0;
        HAL_DCMI_Start_DMA(sp_hdcmi, DCMI_MODE_SNAPSHOT, destAddress, OV7670_QVGA_WIDTH * OV7670_QVGA_HEIGHT / 2);
    }

    return RET_OK;
}

RET ov7670_stopCap()
{
	s_destAddressForContiuousMode = 0;
    HAL_DCMI_Stop(sp_hdcmi);

    return RET_OK;
}

void ov7670_registerCallback(void (*cbHsync)(uint32_t h), void (*cbVsync)(uint32_t v))
{
    s_cbHsync = cbHsync;
    s_cbVsync = cbVsync;
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    if (s_cbVsync)
        s_cbVsync(s_currentV);

    if (s_destAddressForContiuousMode != 0) {
        HAL_DMA_Start_IT(hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, s_destAddressForContiuousMode,
                         OV7670_QVGA_WIDTH * OV7670_QVGA_HEIGHT / 2);
    }

    s_currentV++;
    s_currentH = 0;

    // Display the capture image on the LCD
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    ILI9341_SetAddress(0, 0, IMAGE_SIZE_WIDTH - 1, IMAGE_SIZE_HEIGHT - 1);

    // Send the image data to the LCD
    for (int j = 0; j < OV7670_QVGA_HEIGHT; j++) {
        for (int i = 0; i < OV7670_QVGA_WIDTH; i++) {
            uint8_t pixelData = 0;
            pixelData |= (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) << 6);
            pixelData |= (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) << 7);
            pixelData |= (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) << 8);
            pixelData |= (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) << 9);
            pixelData |= (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) << 0xB);
            pixelData |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) << 6);
            pixelData |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) << 8);
            pixelData |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) << 9);

            HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
            ILI9341_WriteData(pixelData);
            HAL_SPI_Transmit(&hspi1, (uint8_t *)&pixelData, 1, HAL_MAX_DELAY);
            HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
        }
    }

    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    if (s_cbHsync)
        s_cbHsync(s_currentH);

    s_currentH++;
}

static RET ov7670_write(uint8_t regAddr, uint8_t data)
{
    HAL_StatusTypeDef ret;

    do {
        ret = HAL_I2C_Mem_Write(sp_hi2c, SLAVE_ADDR, regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    } while (ret != HAL_OK && 0);

    return ret;
}

static RET ov7670_read(uint8_t regAddr, uint8_t *data)
{
    HAL_StatusTypeDef ret;

    do {
        // HAL_I2C_Mem_Read doesn't work (because of SCCB protocol(doesn't have ack))? */
        ret = HAL_I2C_Master_Transmit(sp_hi2c, SLAVE_ADDR, &regAddr, 1, 100);
        ret |= HAL_I2C_Master_Receive(sp_hi2c, SLAVE_ADDR, data, 1, 100);
    } while (ret != HAL_OK && 0);

    return ret;
}
