#pragma once

// OLED Screen
#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_SDA_PIN 19
#define OLED_SCL_PIN 20
#define OLED_I2C_ADDRESS 0x3C
#define OLED_CHAR_WIDTH 6

// SD Card
#define SD_CMD_PIN 38
#define SD_CLK_PIN 39
#define SD_D0_PIN 40
#define SD_MAX_FILES_OPEN 2
#define SD_FORMAT_IF_MOUNT_FAILED false
#define SD_1_BIT_MODE true
#define SD_MOUNT_PATH "/sdcard"
#define CARDS_PATH "/cards"

// Encoder
#define ENCODER_CLK_PIN 5 // Swap CLK and DT to reverse direction
#define ENCODER_DT_PIN 4
#define ENCODER_SW_PIN 6

// CMC limits
#define MAX_CMC 16