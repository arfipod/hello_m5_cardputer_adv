#include "hardware/St7789Display.hpp"

#include <cstdlib>
#include <cstring>
#include <vector>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {
namespace {
constexpr uint8_t CMD_SWRESET = 0x01;
constexpr uint8_t CMD_SLPOUT = 0x11;
constexpr uint8_t CMD_INVON = 0x21;
constexpr uint8_t CMD_CASET = 0x2A;
constexpr uint8_t CMD_RASET = 0x2B;
constexpr uint8_t CMD_RAMWR = 0x2C;
constexpr uint8_t CMD_MADCTL = 0x36;
constexpr uint8_t CMD_COLMOD = 0x3A;
constexpr uint8_t CMD_DISPON = 0x29;

constexpr uint8_t MADCTL_MX = 0x40;
constexpr uint8_t MADCTL_MV = 0x20;
constexpr uint16_t kLandscapeOffsetX = 40;
constexpr uint16_t kLandscapeOffsetY = 53;
constexpr uint8_t kGlyphWidth = 5;
constexpr uint8_t kGlyphHeight = 7;
constexpr uint8_t kGlyphSpacing = 1;

const uint8_t* glyphFor(char character) {
    static constexpr uint8_t blank[kGlyphWidth] = {0x00, 0x00, 0x00, 0x00, 0x00};
    static constexpr uint8_t unknown[kGlyphWidth] = {0x7F, 0x41, 0x5D, 0x41, 0x7F};
    static constexpr uint8_t glyph_0[kGlyphWidth] = {0x3E, 0x51, 0x49, 0x45, 0x3E};
    static constexpr uint8_t glyph_1[kGlyphWidth] = {0x00, 0x42, 0x7F, 0x40, 0x00};
    static constexpr uint8_t glyph_2[kGlyphWidth] = {0x62, 0x51, 0x49, 0x49, 0x46};
    static constexpr uint8_t glyph_3[kGlyphWidth] = {0x22, 0x49, 0x49, 0x49, 0x36};
    static constexpr uint8_t glyph_4[kGlyphWidth] = {0x18, 0x14, 0x12, 0x7F, 0x10};
    static constexpr uint8_t glyph_5[kGlyphWidth] = {0x2F, 0x49, 0x49, 0x49, 0x31};
    static constexpr uint8_t glyph_6[kGlyphWidth] = {0x3E, 0x49, 0x49, 0x49, 0x32};
    static constexpr uint8_t glyph_7[kGlyphWidth] = {0x01, 0x71, 0x09, 0x05, 0x03};
    static constexpr uint8_t glyph_8[kGlyphWidth] = {0x36, 0x49, 0x49, 0x49, 0x36};
    static constexpr uint8_t glyph_9[kGlyphWidth] = {0x26, 0x49, 0x49, 0x49, 0x3E};
    static constexpr uint8_t glyph_a[kGlyphWidth] = {0x7E, 0x09, 0x09, 0x09, 0x7E};
    static constexpr uint8_t glyph_b[kGlyphWidth] = {0x7F, 0x49, 0x49, 0x49, 0x36};
    static constexpr uint8_t glyph_c[kGlyphWidth] = {0x3E, 0x41, 0x41, 0x41, 0x22};
    static constexpr uint8_t glyph_d[kGlyphWidth] = {0x7F, 0x41, 0x41, 0x22, 0x1C};
    static constexpr uint8_t glyph_e[kGlyphWidth] = {0x7F, 0x49, 0x49, 0x49, 0x41};
    static constexpr uint8_t glyph_f[kGlyphWidth] = {0x7F, 0x09, 0x09, 0x09, 0x01};
    static constexpr uint8_t glyph_g[kGlyphWidth] = {0x3E, 0x41, 0x49, 0x49, 0x7A};
    static constexpr uint8_t glyph_h[kGlyphWidth] = {0x7F, 0x08, 0x08, 0x08, 0x7F};
    static constexpr uint8_t glyph_i[kGlyphWidth] = {0x00, 0x41, 0x7F, 0x41, 0x00};
    static constexpr uint8_t glyph_j[kGlyphWidth] = {0x20, 0x40, 0x41, 0x3F, 0x01};
    static constexpr uint8_t glyph_k[kGlyphWidth] = {0x7F, 0x08, 0x14, 0x22, 0x41};
    static constexpr uint8_t glyph_l[kGlyphWidth] = {0x7F, 0x40, 0x40, 0x40, 0x40};
    static constexpr uint8_t glyph_m[kGlyphWidth] = {0x7F, 0x02, 0x0C, 0x02, 0x7F};
    static constexpr uint8_t glyph_n[kGlyphWidth] = {0x7F, 0x04, 0x08, 0x10, 0x7F};
    static constexpr uint8_t glyph_o[kGlyphWidth] = {0x3E, 0x41, 0x41, 0x41, 0x3E};
    static constexpr uint8_t glyph_p[kGlyphWidth] = {0x7F, 0x09, 0x09, 0x09, 0x06};
    static constexpr uint8_t glyph_q[kGlyphWidth] = {0x3E, 0x41, 0x51, 0x21, 0x5E};
    static constexpr uint8_t glyph_r[kGlyphWidth] = {0x7F, 0x09, 0x19, 0x29, 0x46};
    static constexpr uint8_t glyph_s[kGlyphWidth] = {0x46, 0x49, 0x49, 0x49, 0x31};
    static constexpr uint8_t glyph_t[kGlyphWidth] = {0x01, 0x01, 0x7F, 0x01, 0x01};
    static constexpr uint8_t glyph_u[kGlyphWidth] = {0x3F, 0x40, 0x40, 0x40, 0x3F};
    static constexpr uint8_t glyph_v[kGlyphWidth] = {0x1F, 0x20, 0x40, 0x20, 0x1F};
    static constexpr uint8_t glyph_w[kGlyphWidth] = {0x7F, 0x20, 0x18, 0x20, 0x7F};
    static constexpr uint8_t glyph_x[kGlyphWidth] = {0x63, 0x14, 0x08, 0x14, 0x63};
    static constexpr uint8_t glyph_y[kGlyphWidth] = {0x07, 0x08, 0x70, 0x08, 0x07};
    static constexpr uint8_t glyph_z[kGlyphWidth] = {0x61, 0x51, 0x49, 0x45, 0x43};
    static constexpr uint8_t glyph_dash[kGlyphWidth] = {0x08, 0x08, 0x08, 0x08, 0x08};
    static constexpr uint8_t glyph_plus[kGlyphWidth] = {0x08, 0x08, 0x3E, 0x08, 0x08};
    static constexpr uint8_t glyph_colon[kGlyphWidth] = {0x00, 0x36, 0x36, 0x00, 0x00};
    static constexpr uint8_t glyph_dot[kGlyphWidth] = {0x00, 0x60, 0x60, 0x00, 0x00};

    if (character >= 'a' && character <= 'z') {
        character = static_cast<char>(character - ('a' - 'A'));
    }

    switch (character) {
        case ' ':
            return blank;
        case '0':
            return glyph_0;
        case '1':
            return glyph_1;
        case '2':
            return glyph_2;
        case '3':
            return glyph_3;
        case '4':
            return glyph_4;
        case '5':
            return glyph_5;
        case '6':
            return glyph_6;
        case '7':
            return glyph_7;
        case '8':
            return glyph_8;
        case '9':
            return glyph_9;
        case 'A':
            return glyph_a;
        case 'B':
            return glyph_b;
        case 'C':
            return glyph_c;
        case 'D':
            return glyph_d;
        case 'E':
            return glyph_e;
        case 'F':
            return glyph_f;
        case 'G':
            return glyph_g;
        case 'H':
            return glyph_h;
        case 'I':
            return glyph_i;
        case 'J':
            return glyph_j;
        case 'K':
            return glyph_k;
        case 'L':
            return glyph_l;
        case 'M':
            return glyph_m;
        case 'N':
            return glyph_n;
        case 'O':
            return glyph_o;
        case 'P':
            return glyph_p;
        case 'Q':
            return glyph_q;
        case 'R':
            return glyph_r;
        case 'S':
            return glyph_s;
        case 'T':
            return glyph_t;
        case 'U':
            return glyph_u;
        case 'V':
            return glyph_v;
        case 'W':
            return glyph_w;
        case 'X':
            return glyph_x;
        case 'Y':
            return glyph_y;
        case 'Z':
            return glyph_z;
        case '-':
            return glyph_dash;
        case '+':
            return glyph_plus;
        case ':':
            return glyph_colon;
        case '.':
            return glyph_dot;
        default:
            return unknown;
    }
}
}  // namespace

St7789Display::~St7789Display() {
    if (device_ != nullptr) {
        spi_bus_remove_device(device_);
    }
    if (bus_initialized_) {
        spi_bus_free(config_.host);
    }
}

esp_err_t St7789Display::init() {
    return init(Config{});
}

esp_err_t St7789Display::init(const Config& config) {
    if (device_ != nullptr) {
        return ESP_OK;
    }
    config_ = config;

    gpio_config_t output_config{};
    output_config.pin_bit_mask = (1ULL << pins::LCD_BL) | (1ULL << pins::LCD_DC) | (1ULL << pins::LCD_RST);
    output_config.mode = GPIO_MODE_OUTPUT;
    output_config.pull_up_en = GPIO_PULLUP_DISABLE;
    output_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    output_config.intr_type = GPIO_INTR_DISABLE;
    esp_err_t err = gpio_config(&output_config);
    if (err != ESP_OK) {
        return err;
    }
    gpio_set_level(pins::LCD_BL, 0);
    gpio_set_level(pins::LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(pins::LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));

    spi_bus_config_t bus_config{};
    bus_config.mosi_io_num = pins::LCD_MOSI;
    bus_config.miso_io_num = -1;
    bus_config.sclk_io_num = pins::LCD_SCK;
    bus_config.quadwp_io_num = -1;
    bus_config.quadhd_io_num = -1;
    bus_config.data4_io_num = -1;
    bus_config.data5_io_num = -1;
    bus_config.data6_io_num = -1;
    bus_config.data7_io_num = -1;
    bus_config.data_io_default_level = false;
    bus_config.max_transfer_sz = kWidth * sizeof(uint16_t);
    bus_config.flags = SPICOMMON_BUSFLAG_MASTER;
    bus_config.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;
    err = spi_bus_initialize(config_.host, &bus_config, SPI_DMA_CH_AUTO);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }
    bus_initialized_ = err == ESP_OK;

    spi_device_interface_config_t device_config{};
    device_config.clock_speed_hz = static_cast<int>(config_.pixel_clock_hz);
    device_config.mode = 0;
    device_config.spics_io_num = pins::LCD_CS;
    device_config.queue_size = 7;
    device_config.flags = SPI_DEVICE_HALFDUPLEX;
    err = spi_bus_add_device(config_.host, &device_config, &device_);
    if (err != ESP_OK) {
        return err;
    }

    err = sendCommand(CMD_SWRESET);
    if (err != ESP_OK) {
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(120));

    err = sendCommand(CMD_SLPOUT);
    if (err != ESP_OK) {
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(120));

    const uint8_t color_mode = 0x55;
    err = sendCommand(CMD_COLMOD);
    if (err != ESP_OK) {
        return err;
    }
    err = sendData(&color_mode, sizeof(color_mode));
    if (err != ESP_OK) {
        return err;
    }

    const uint8_t madctl = MADCTL_MX | MADCTL_MV;
    err = sendCommand(CMD_MADCTL);
    if (err != ESP_OK) {
        return err;
    }
    err = sendData(&madctl, sizeof(madctl));
    if (err != ESP_OK) {
        return err;
    }

    err = sendCommand(CMD_INVON);
    if (err != ESP_OK) {
        return err;
    }
    err = sendCommand(CMD_DISPON);
    if (err != ESP_OK) {
        return err;
    }
    return setBacklight(true);
}

esp_err_t St7789Display::clear(uint16_t rgb565) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    return drawFilledRect(0, 0, kWidth, kHeight, rgb565);
}

esp_err_t St7789Display::drawPixels(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t* pixels) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (pixels == nullptr || width == 0 || height == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (x >= kWidth || y >= kHeight || width > static_cast<uint16_t>(kWidth - x) || height > static_cast<uint16_t>(kHeight - y)) {
        return ESP_ERR_INVALID_ARG;
    }
    for (uint16_t row = 0; row < height; ++row) {
        const uint16_t current_y = static_cast<uint16_t>(y + row);
        const uint16_t* row_pixels = &pixels[row * width];
        esp_err_t err = setAddressWindow(x, current_y, width, 1);
        if (err != ESP_OK) {
            return err;
        }
        err = sendCommand(CMD_RAMWR);
        if (err != ESP_OK) {
            return err;
        }
        err = writePixelsToCurrentWindow(row_pixels, width);
        if (err != ESP_OK) {
            return err;
        }
    }
    return ESP_OK;
}

esp_err_t St7789Display::drawFilledRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t rgb565) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (width == 0 || height == 0 || x >= kWidth || y >= kHeight) {
        return ESP_ERR_INVALID_ARG;
    }
    if (width > static_cast<uint16_t>(kWidth - x) || height > static_cast<uint16_t>(kHeight - y)) {
        return ESP_ERR_INVALID_ARG;
    }

    constexpr uint16_t kChunkRows = 16;
    std::vector<uint16_t> chunk(width * (height < kChunkRows ? height : kChunkRows), rgb565);
    uint16_t rows_remaining = height;
    uint16_t current_y = y;
    while (rows_remaining > 0) {
        const uint16_t rows = rows_remaining < kChunkRows ? rows_remaining : kChunkRows;
        esp_err_t err = drawPixels(x, current_y, width, rows, chunk.data());
        if (err != ESP_OK) {
            return err;
        }
        current_y = static_cast<uint16_t>(current_y + rows);
        rows_remaining = static_cast<uint16_t>(rows_remaining - rows);
    }
    return ESP_OK;
}

esp_err_t St7789Display::drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t rgb565) {
    if (width == 0 || height == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = drawLine(x, y, static_cast<uint16_t>(x + width - 1U), y, rgb565);
    if (err != ESP_OK) {
        return err;
    }
    err = drawLine(x, static_cast<uint16_t>(y + height - 1U), static_cast<uint16_t>(x + width - 1U),
                   static_cast<uint16_t>(y + height - 1U), rgb565);
    if (err != ESP_OK) {
        return err;
    }
    err = drawLine(x, y, x, static_cast<uint16_t>(y + height - 1U), rgb565);
    if (err != ESP_OK) {
        return err;
    }
    return drawLine(static_cast<uint16_t>(x + width - 1U), y, static_cast<uint16_t>(x + width - 1U),
                    static_cast<uint16_t>(y + height - 1U), rgb565);
}

esp_err_t St7789Display::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t rgb565) {
    if (x0 >= kWidth || y0 >= kHeight || x1 >= kWidth || y1 >= kHeight) {
        return ESP_ERR_INVALID_ARG;
    }

    int current_x = x0;
    int current_y = y0;
    const int target_x = x1;
    const int target_y = y1;
    const int delta_x = std::abs(target_x - current_x);
    const int step_x = current_x < target_x ? 1 : -1;
    const int delta_y = -std::abs(target_y - current_y);
    const int step_y = current_y < target_y ? 1 : -1;
    int error = delta_x + delta_y;

    while (true) {
        esp_err_t err = drawPixel(static_cast<uint16_t>(current_x), static_cast<uint16_t>(current_y), rgb565);
        if (err != ESP_OK) {
            return err;
        }
        if (current_x == target_x && current_y == target_y) {
            break;
        }
        const int twice_error = 2 * error;
        if (twice_error >= delta_y) {
            error += delta_y;
            current_x += step_x;
        }
        if (twice_error <= delta_x) {
            error += delta_x;
            current_y += step_y;
        }
    }
    return ESP_OK;
}

esp_err_t St7789Display::drawText(uint16_t x, uint16_t y, const char* text, uint16_t rgb565) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (text == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    if (x >= kWidth || y >= kHeight) {
        return ESP_ERR_INVALID_ARG;
    }

    uint16_t cursor_x = x;
    for (size_t index = 0; index < std::strlen(text); ++index) {
        if (cursor_x >= kWidth) {
            return ESP_OK;
        }
        esp_err_t err = drawGlyph(cursor_x, y, text[index], rgb565);
        if (err != ESP_OK) {
            return err;
        }
        cursor_x = static_cast<uint16_t>(cursor_x + kGlyphWidth + kGlyphSpacing);
    }
    return ESP_OK;
}

esp_err_t St7789Display::drawText(uint16_t x, uint16_t y, const char* text, uint16_t foreground_rgb565, uint16_t background_rgb565) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (text == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    if (x >= kWidth || y >= kHeight || kGlyphHeight > static_cast<uint16_t>(kHeight - y)) {
        return ESP_ERR_INVALID_ARG;
    }

    const size_t text_length = std::strlen(text);
    if (text_length == 0U) {
        return ESP_OK;
    }
    const uint16_t text_width = static_cast<uint16_t>(text_length * (kGlyphWidth + kGlyphSpacing));
    const uint16_t clipped_width = x + text_width > kWidth ? static_cast<uint16_t>(kWidth - x) : text_width;
    esp_err_t err = drawFilledRect(x, y, clipped_width, kGlyphHeight, background_rgb565);
    if (err != ESP_OK) {
        return err;
    }
    return drawText(x, y, text, foreground_rgb565);
}

esp_err_t St7789Display::drawTextPlaceholder(uint16_t x, uint16_t y, const char* text, uint16_t rgb565) {
    return drawText(x, y, text, rgb565);
}

esp_err_t St7789Display::setBacklight(bool enabled) {
    return gpio_set_level(pins::LCD_BL, enabled ? 1 : 0);
}

esp_err_t St7789Display::sendCommand(uint8_t command) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    spi_transaction_t transaction{};
    transaction.length = 8;
    transaction.flags = SPI_TRANS_USE_TXDATA;
    transaction.tx_data[0] = command;
    gpio_set_level(pins::LCD_DC, 0);
    return spi_device_transmit(device_, &transaction);
}

esp_err_t St7789Display::sendData(const void* data, size_t length) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (data == nullptr || length == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    spi_transaction_t transaction{};
    transaction.length = length * 8;
    transaction.tx_buffer = data;
    gpio_set_level(pins::LCD_DC, 1);
    return spi_device_transmit(device_, &transaction);
}

esp_err_t St7789Display::setAddressWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    const uint16_t x_start = static_cast<uint16_t>(x + kLandscapeOffsetX);
    const uint16_t y_start = static_cast<uint16_t>(y + kLandscapeOffsetY);
    const uint16_t x_end = static_cast<uint16_t>(x_start + width - 1U);
    const uint16_t y_end = static_cast<uint16_t>(y_start + height - 1U);
    const uint8_t column_data[] = {
        static_cast<uint8_t>(x_start >> 8U), static_cast<uint8_t>(x_start & 0xFFU),
        static_cast<uint8_t>(x_end >> 8U), static_cast<uint8_t>(x_end & 0xFFU),
    };
    const uint8_t row_data[] = {
        static_cast<uint8_t>(y_start >> 8U), static_cast<uint8_t>(y_start & 0xFFU),
        static_cast<uint8_t>(y_end >> 8U), static_cast<uint8_t>(y_end & 0xFFU),
    };

    esp_err_t err = sendCommand(CMD_CASET);
    if (err != ESP_OK) {
        return err;
    }
    err = sendData(column_data, sizeof(column_data));
    if (err != ESP_OK) {
        return err;
    }
    err = sendCommand(CMD_RASET);
    if (err != ESP_OK) {
        return err;
    }
    return sendData(row_data, sizeof(row_data));
}

esp_err_t St7789Display::writePixelsToCurrentWindow(const uint16_t* pixels, size_t count) {
    if (pixels == nullptr || count == 0U) {
        return ESP_ERR_INVALID_ARG;
    }
    return sendData(pixels, count * sizeof(uint16_t));
}

esp_err_t St7789Display::drawPixel(uint16_t x, uint16_t y, uint16_t rgb565) {
    return drawPixels(x, y, 1, 1, &rgb565);
}

esp_err_t St7789Display::drawGlyph(uint16_t x, uint16_t y, char character, uint16_t rgb565) {
    if (y >= kHeight) {
        return ESP_ERR_INVALID_ARG;
    }

    const uint8_t* glyph = glyphFor(character);
    for (uint8_t column = 0; column < kGlyphWidth; ++column) {
        const uint16_t pixel_x = static_cast<uint16_t>(x + column);
        if (pixel_x >= kWidth) {
            break;
        }
        for (uint8_t row = 0; row < kGlyphHeight; ++row) {
            const uint16_t pixel_y = static_cast<uint16_t>(y + row);
            if (pixel_y >= kHeight) {
                break;
            }
            if ((glyph[column] & (1U << row)) != 0U) {
                esp_err_t err = drawPixel(pixel_x, pixel_y, rgb565);
                if (err != ESP_OK) {
                    return err;
                }
            }
        }
    }
    return ESP_OK;
}

}  // namespace cardputer::hardware
