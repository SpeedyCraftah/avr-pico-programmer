#include "avrprog.h"

#include <pico/stdlib.h>
#include <hardware/spi.h>

#define mosi_pin 19
#define sck_pin 18
#define miso_pin 16
#define reset_pin 17
#define power_pin 0

uint8_t output_buffer[4] = {0,0,0,0};

void avr_spi_init() {
    // Setup the reset pin.
    gpio_init(reset_pin);
    gpio_set_dir(reset_pin, GPIO_OUT);

    // Setup the SPI pin functions.
    gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(miso_pin, GPIO_FUNC_SPI);

    // Initialise the SPI interface for controller pins.
    spi_init(spi0, 200000);
}

void avr_reset() {
    sleep_ms(50);
    gpio_put(reset_pin, 1);
    sleep_ms(50);
    gpio_put(reset_pin, 0);
    sleep_ms(20);
}

bool avr_enter_programming_mode() {
    // Send the programming enable command.
    uint8_t cmd[4] = {0xAC, 0x53, 0x00, 0x00};
    spi_write_read_blocking(spi0, cmd, output_buffer, 4);

    // If the controller sent an echo of the second byte.
    return output_buffer[2] == 0x53;
}

int erase_c = 0;
void avr_erase_memory() {
    if (erase_c > 200) {
        printf("the erase operation has been performed more than 200 times in this session!\nto protect the flash from a potential infinite erase loop the program has been halted\n");
        while (true) sleep_ms(100);
    }

    // Send the chip erase command.
    uint8_t cmd[4] = {0xAC, 0x80, 0x00, 0x00};
    spi_write_read_blocking(spi0, cmd, output_buffer, 4);
    
    // Wait 9ms as specified by the minimum delay after an erase.
    sleep_ms(9);

    erase_c++;
}

void avr_write_temporary_buffer(uint16_t word_address, uint8_t low_byte, uint8_t high_byte) {
    uint8_t addr_msb = word_address >> 8;
    uint8_t addr_lsb = word_address & 0b11111111;

    // Write the low byte.
    uint8_t cmd[4] = {0x40, addr_msb, addr_lsb, low_byte};
    spi_write_read_blocking(spi0, cmd, output_buffer, 4);

    // Write the high byte.
    uint8_t cmd2[4] = {0x48, addr_msb, addr_lsb, high_byte};
    spi_write_read_blocking(spi0, cmd2, output_buffer, 4);
}

void avr_write_temporary_buffer_16(uint16_t word_address, uint16_t word) {
    avr_write_temporary_buffer(word_address, word & 0b11111111, word >> 8);
}

void avr_flash_program_memory(uint16_t word_address) {
    uint8_t addr_msb = word_address >> 8;
    uint8_t addr_lsb = word_address & 0b11111111;

    // Send the flash command.
    uint8_t cmd[4] = {0x4C, addr_msb, addr_lsb, 0x0};
    spi_write_read_blocking(spi0, cmd, output_buffer, 4);

    // Wait 4.5ms as specified by minimum delay after flash.
    sleep_ms(5);
}

uint8_t avr_read_program_memory_low_byte(uint16_t word_address) {
    uint8_t addr_msb = word_address >> 8;
    uint8_t addr_lsb = word_address & 0b11111111;

    // Send the read command.
    uint8_t cmd[4] = {0x20, addr_msb, addr_lsb, 0x0};
    spi_write_read_blocking(spi0, cmd, output_buffer, 4);

    // Return the read result.
    return output_buffer[3];
}

uint8_t avr_read_program_memory_high_byte(uint16_t word_address) {
    uint8_t addr_msb = word_address >> 8;
    uint8_t addr_lsb = word_address & 0b11111111;

    // Send the read command.
    uint8_t cmd[4] = {0x28, addr_msb, addr_lsb, 0x0};
    spi_write_read_blocking(spi0, cmd, output_buffer, 4);

    // Return the read result.
    return output_buffer[3];
}

uint16_t avr_read_program_memory(uint16_t word_address) {
    uint16_t data;

    // Read the bytes.
    data = avr_read_program_memory_high_byte(word_address);
    data <<= 8;
    data |= avr_read_program_memory_low_byte(word_address);

    // Return the 16 bit data.
    return data;
}

void avr_write_temporary_buffer_page(uint16_t* data, size_t data_len) {
    if (data_len > 32 || data_len == 0) {
        printf("err - data out of bounds (32/0 / data_len)\n");
        return;
    }

    for (size_t i = 0; i < data_len; i++) {
        avr_write_temporary_buffer_16(i, data[i]);
    }
}

bool avr_verify_program_memory_page(uint16_t page_address_start, uint16_t* expected_data, size_t data_len) {
    for (size_t i = 0; i < data_len; i++) {
        uint16_t word = avr_read_program_memory(page_address_start + i);
        if (word != expected_data[i]) return false;
    }

    return true;
}
