#include "display.h"

U8G2 u8g2 = U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);

void init_display() {
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}