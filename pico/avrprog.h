#pragma once

#include <pico/stdlib.h>

// Initialises the SPI interface bridge to the microcontroller.
void avr_spi_init();

// Causes the microcontroller to perform a reset sequence by pulsing the RESET pin.
void avr_reset();

// Sends a Programming Enable command to enter memory programming mode.
// Returns a boolean indicating the microcontrollers acknowledgement through a satisfactory echo.
// "If the 0x53 did not echo back, give RESET a positive pulse and issue a new Programming Enable command"
bool avr_enter_programming_mode();

// Erases the program memory as well as the EEPROM.
// This operation must be performed before programming of new data.
// This operation is destructive to the underlying flash storage and can only be done a limited amount of times (usually ~10,000 times).
void avr_erase_memory();

// Performs a write to the AVR program memory temporary buffer.
// Addresses are per-word, 0 = first 2 bytes, 1 = 3rd and 4th byte (little endian).
// Combined 16-bit uint version.
void avr_write_temporary_buffer_16(uint16_t word_address, uint16_t data);

// Performs a write to the AVR program memory temporary buffer.
// Addresses are per-word, 0 = first 2 bytes, 1 = 3rd and 4th byte (little endian).
// Separate data byte version.
void avr_write_temporary_buffer(uint16_t word_address, uint8_t low_byte, uint8_t high_byte);

// Flashes the data from the temporary buffer to the program memory at the specified address.
// Addresses are per-word, 0 = first 2 bytes, 1 = 3rd and 4th byte (little endian).
void avr_flash_program_memory(uint16_t word_address);

// Performs a read on the AVR program memory at the specified address.
// Addresses are per-word, 0 = first 2 bytes, 1 = 3rd and 4th byte (little endian).
// Low byte only version.
uint8_t avr_read_program_memory_low_byte(uint16_t word_address);

// Performs a read on the AVR program memory at the specified address.
// Addresses are per-word, 0 = first 2 bytes, 1 = 3rd and 4th byte (little endian).
// High byte only version.
uint8_t avr_read_program_memory_high_byte(uint16_t word_address);

// Performs a read on the AVR program memory at the specified address.
// Addresses are per-word, 0 = first 2 bytes, 1 = 3rd and 4th byte (little endian).
uint16_t avr_read_program_memory(uint16_t word_address);

// Writes a specified amount of words directly from a buffer onto the AVR controllers temporary buffer.
// The maximum supported words depends on the specific AVR controller/page size used, for the ATTiny84A it is 32 words (64 bytes).
// https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny24A-44A-84A-DataSheet-DS40002269A.pdf
void avr_write_temporary_buffer_page(uint16_t* data, size_t data_len);

// Verifies and compares the values on the program memory to the expected_data buffer and returns a boolean indicating result (true = matches).
// This should be done after each page is flashed onto the controller as writes can be often unreliable and fail, so that the operation can be retried.
bool avr_verify_program_memory_page(uint16_t page_address_start, uint16_t* expected_data, size_t data_len);
