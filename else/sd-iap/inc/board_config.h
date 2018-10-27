#pragma once

#include "stm32f10x_gpio.h"

//------------------SD������---------------------------
//CS �����ʱû��
#define SPI_CS_PORT_GROUP   GPIOC
#define SPI_CS_PORT_NUM     GPIO_Pin_4

//CLK
#define SPI_CLK_PORT_GROUP  GPIOA
#define SPI_CLK_PORT_NUM    GPIO_Pin_5

//MISO
#define SPI_MISO_PORT_GROUP GPIOA
#define SPI_MISO_PORT_NUM   GPIO_Pin_6

//MOSI
#define SPI_MOSI_PORT_GROUP GPIOA
#define SPI_MOSI_PORT_NUM   GPIO_Pin_7

//sd card detect
#define SD_DETECT_PORT_GROUP    GPIOA
#define SD_DETECT_PORT_NUM      GPIO_Pin_4

//----------------LED����----------------------------
//LEDʹ�õĶ˿�
#define LED_PORT_GROUP  GPIOB
#define LED_PORT_NUM    GPIO_Pin_10



