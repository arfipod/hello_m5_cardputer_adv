#include "hardware/I2CBus.hpp"

#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {

I2CBus::I2CBus() {
    mutex_ = xSemaphoreCreateMutex();
}

I2CBus::~I2CBus() {
    if (initialized_) {
        i2c_driver_delete(config_.port);
    }
    if (mutex_ != nullptr) {
        vSemaphoreDelete(mutex_);
    }
}

esp_err_t I2CBus::init() {
    return init(Config{});
}

esp_err_t I2CBus::init(const Config& config) {
    if (initialized_) {
        return ESP_OK;
    }
    if (mutex_ == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    config_ = config;
    const i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = pins::I2C_SDA,
        .scl_io_num = pins::I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = config_.clock_hz,
        },
        .clk_flags = 0,
    };

    esp_err_t err = i2c_param_config(config_.port, &i2c_config);
    if (err != ESP_OK) {
        return err;
    }

    err = i2c_driver_install(config_.port, I2C_MODE_MASTER, 0, 0, 0);
    if (err == ESP_OK || err == ESP_ERR_INVALID_STATE) {
        initialized_ = true;
        return ESP_OK;
    }
    return err;
}

bool I2CBus::isInitialized() const {
    return initialized_;
}

esp_err_t I2CBus::write(uint8_t address, const uint8_t* data, size_t length) {
    if (!initialized_) {
        return ESP_ERR_INVALID_STATE;
    }
    if (length > 0 && data == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = take();
    if (err != ESP_OK) {
        return err;
    }
    err = i2c_master_write_to_device(config_.port, address, data, length, config_.timeout_ticks);
    give();
    return err;
}

esp_err_t I2CBus::read(uint8_t address, uint8_t* data, size_t length) {
    if (!initialized_) {
        return ESP_ERR_INVALID_STATE;
    }
    if (length == 0 || data == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = take();
    if (err != ESP_OK) {
        return err;
    }
    err = i2c_master_read_from_device(config_.port, address, data, length, config_.timeout_ticks);
    give();
    return err;
}

esp_err_t I2CBus::writeRegister(uint8_t address, uint8_t reg, uint8_t value) {
    return writeRegister(address, reg, &value, 1);
}

esp_err_t I2CBus::writeRegister(uint8_t address, uint8_t reg, const uint8_t* data, size_t length) {
    if (!initialized_) {
        return ESP_ERR_INVALID_STATE;
    }
    if (length > 0 && data == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = take();
    if (err != ESP_OK) {
        return err;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd == nullptr) {
        give();
        return ESP_ERR_NO_MEM;
    }
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, static_cast<uint8_t>((address << 1U) | I2C_MASTER_WRITE), true);
    i2c_master_write_byte(cmd, reg, true);
    if (length > 0) {
        i2c_master_write(cmd, const_cast<uint8_t*>(data), length, true);
    }
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(config_.port, cmd, config_.timeout_ticks);
    i2c_cmd_link_delete(cmd);
    give();
    return err;
}

esp_err_t I2CBus::readRegister(uint8_t address, uint8_t reg, uint8_t* data, size_t length) {
    if (!initialized_) {
        return ESP_ERR_INVALID_STATE;
    }
    if (length == 0 || data == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = take();
    if (err != ESP_OK) {
        return err;
    }
    err = i2c_master_write_read_device(config_.port, address, &reg, 1, data, length, config_.timeout_ticks);
    give();
    return err;
}

esp_err_t I2CBus::scan(std::array<uint8_t, 128>& addresses, size_t& count) {
    if (!initialized_) {
        return ESP_ERR_INVALID_STATE;
    }

    count = 0;
    for (uint8_t address = 1; address < addresses.size(); ++address) {
        esp_err_t err = take();
        if (err != ESP_OK) {
            return err;
        }
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        if (cmd == nullptr) {
            give();
            return ESP_ERR_NO_MEM;
        }
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, static_cast<uint8_t>((address << 1U) | I2C_MASTER_WRITE), true);
        i2c_master_stop(cmd);
        err = i2c_master_cmd_begin(config_.port, cmd, pdMS_TO_TICKS(20));
        i2c_cmd_link_delete(cmd);
        give();

        if (err == ESP_OK) {
            addresses[count++] = address;
        }
    }
    return ESP_OK;
}

esp_err_t I2CBus::take() {
    if (mutex_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    return xSemaphoreTake(mutex_, config_.timeout_ticks) == pdTRUE ? ESP_OK : ESP_ERR_TIMEOUT;
}

void I2CBus::give() {
    xSemaphoreGive(mutex_);
}

}  // namespace cardputer::hardware
