/**
 * Private configuration file for the SSD1306 library.
 */

#ifndef __SSD1306_CONF_H__
#define __SSD1306_CONF_H__

#include "main.h"

// Choose a bus
// not tested yet #define SSD1306_USE_I2C  // I2C clock must be enabled
// #define SSD1306_USE_SPI  // SPI and GPIO clocks must be enabled
#define SSD1306_USE_EMULATED_SPI  // GPIO clocks must be enabled

// I2C Configuration
#if defined(SSD1306_USE_I2C)
#define SSD1306_I2C_PORT I2C1

#define SSD1306_I2C_ADDR (0x3C << 1)
#endif

// SPI Configuration
#if defined(SSD1306_USE_SPI)
#define SSD1306_SPI_PORT SPI1                  // should be SPIx
#define SSD1306_CLK_Port OLED_CLK_GPIO_Port    // derived from SPIx
#define SSD1306_CLK_Pin OLED_CLK_Pin           // derived from SPIx
#define SSD1306_MOSI_Port OLED_MOSI_GPIO_Port  // derived from SPIx
#define SSD1306_MOSI_Pin OLED_MOSI_Pin         // derived from SPIx
#define SSD1306_CS_Port OLED_CS_GPIO_Port      // should be GPIOx
#define SSD1306_CS_Pin OLED_CS_Pin             // should be LL_GPIO_PIN_x
#define SSD1306_DC_Port OLED_DC_GPIO_Port
#define SSD1306_DC_Pin OLED_DC_Pin
#define SSD1306_RES_Port OLED_RES_GPIO_Port
#define SSD1306_RES_Pin OLED_RES_Pin

#define SSD1306_SPI_BAUDRATEPRESCALER LL_SPI_BAUDRATEPRESCALER_DIV4
#endif

// Emulated SPI Configuration
#if defined(SSD1306_USE_EMULATED_SPI)
#define SSD1306_CLK_Port OLED_CLK_GPIO_Port  // should be GPIOx
#define SSD1306_CLK_Pin OLED_CLK_Pin         // should be LL_GPIO_PIN_x
#define SSD1306_MOSI_Port OLED_MOSI_GPIO_Port
#define SSD1306_MOSI_Pin OLED_MOSI_Pin
#define SSD1306_CS_Port OLED_CS_GPIO_Port
#define SSD1306_CS_Pin OLED_CS_Pin
#define SSD1306_DC_Port OLED_DC_GPIO_Port
#define SSD1306_DC_Pin OLED_DC_Pin
#define SSD1306_RES_Port OLED_RES_GPIO_Port
#define SSD1306_RES_Pin OLED_RES_Pin
#endif

// Mirror the screen if needed
// #define SSD1306_MIRROR_VERT
// #define SSD1306_MIRROR_HORIZ

// Set inverse color if needed
// # define SSD1306_INVERSE_COLOR

// Include only needed fonts
#define SSD1306_INCLUDE_FONT_6x8
#define SSD1306_INCLUDE_FONT_7x10
#define SSD1306_INCLUDE_FONT_11x18
#define SSD1306_INCLUDE_FONT_16x26

// The width of the screen can be set using this
// define. The default value is 128.
// #define SSD1306_WIDTH           64

// If your screen horizontal axis does not start
// in column 0 you can use this define to
// adjust the horizontal offset
#define SSD1306_X_OFFSET 2

// The height can be changed as well if necessary.
// It can be 32, 64 or 128. The default value is 64.
// #define SSD1306_HEIGHT          64

// Enable tests that require timing. If enabled, call
// ssd1306_SysTick_Handler() in SysTick_Handler().
// #define SSD1306_INCLUDE_TIMED_TESTS

#endif /* __SSD1306_CONF_H__ */
