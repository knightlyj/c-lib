#pragma once

#include "stm32f10x_gpio.h"

//------------------SD卡配置---------------------------
//CS 这个暂时没用
#define MCP2515_CS_PORT_GROUP   GPIOA
#define MCP2515_CS_PORT_NUM     GPIO_Pin_4

//CLK
#define MCP2515_CLK_PORT_GROUP  GPIOA
#define MCP2515_CLK_PORT_NUM    GPIO_Pin_5

//MISO
#define MCP2515_MISO_PORT_GROUP GPIOA
#define MCP2515_MISO_PORT_NUM   GPIO_Pin_6

//MOSI
#define MCP2515_MOSI_PORT_GROUP GPIOA
#define MCP2515_MOSI_PORT_NUM   GPIO_Pin_7

//INT
#define MCP2515_INT_PORT_GROUP  GPIOA
#define MCP2515_INT_PORT_NUM    GPIO_Pin_12

#define MCP2515_RST_PORT_GROUP  GPIOB
#define MCP2515_RST_PORT_NUM    GPIO_Pin_10

#define MCP2515_INT_PORT_SOURCE GPIO_PortSourceGPIOA
#define MCP2515_INT_PIN_SOURCE  GPIO_PinSource12
#define MCP2515_INT_LINE        EXTI_Line12


