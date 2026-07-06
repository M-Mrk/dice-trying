#include "display.h"
#include "pins.h"

#ifdef DISPLAY_IS_I2C
U8G2 u8g2 = U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE, SCL_PIN, SDA_PIN);
#else
U8G2 u8g2 = U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI(U8G2_R0, /*clock=*/OLED_CLK, /*data=*/OLED_MOSI, /*cs=*/OLED_CS, /*dc=*/OLED_DC, /*reset=*/OLED_RST);
#endif

void init_display() {
    #ifdef DISPLAY_IS_I2C
    u8g2.setI2CAddress(0x3C << 1);
    #else
    pinMode(OLED_CLK, OUTPUT);
    pinMode(OLED_MOSI, OUTPUT);
    pinMode(OLED_CS, OUTPUT);
    pinMode(OLED_DC, OUTPUT);
    pinMode(OLED_RST, OUTPUT);
    u8g2.setBusClock(400000); // 400kHz - conservative, raise later if it works
    u8g2.setContrast(255); // max contrast, rules out "too dim to see"
    #endif
    
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}