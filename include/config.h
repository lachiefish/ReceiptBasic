#pragma once

// OLED Screen
#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_SDA_PIN 19
#define OLED_SCL_PIN 20
#define OLED_I2C_ADDRESS 0x3C
#define OLED_CHAR_WIDTH 6
#define OLED_CHAR_HEIGHT 8

// SD Card
#define SD_CMD_PIN 38
#define SD_CLK_PIN 39
#define SD_D0_PIN 40
#define SD_MAX_FILES_OPEN 2
#define SD_FORMAT_IF_MOUNT_FAILED false
#define SD_1_BIT_MODE true
#define SD_MOUNT_PATH "/sdcard"
#define CARDS_PATH "/cards"
#define TOKENS_PATH "/tokens"

// Encoder
#define ENCODER_CLK_PIN 5 // Swap CLK and DT to reverse direction
#define ENCODER_DT_PIN 4
#define ENCODER_SW_PIN 6

// Power sensing (4S battery pack)
#define BATTERY_SENSE_PIN 7
#define BATTERY_DIVIDER_R_TOP_OHMS 1000000.0f
#define BATTERY_DIVIDER_R_BOTTOM_OHMS 220000.0f
#define BATTERY_CALIBRATION_FACTOR 1.010f
#define POWER_SAMPLE_INTERVAL_MS 500
#define POWER_SAMPLE_COUNT 8
#define POWER_FILTER_ALPHA 0.2f
#define BATTERY_CELL_COUNT 4

// Printer
#define PRINTER_RX_PIN 18
#define PRINTER_TX_PIN 17
#define PRINTER_BAUD 9600

// FTP server
#define FTP_USERNAME "receipt"
#define FTP_PASSWORD "basic"

// CMC limits
#define MAX_CMC 16