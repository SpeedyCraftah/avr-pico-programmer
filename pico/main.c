#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/spi.h>
#include "avrprog.h"

uint8_t program_code[2000] = {};
char input[10] = {0};

int main() {
    stdio_init_all();
    sleep_ms(1000);

    while (true) {
        int code_c = 0;
        int input_c = 0;

        while (true) {
            char c = getchar();

            // Tells the automatic programmer the programmer is ready.
            if (c == '?') {
                printf("READY");
            }

            if (!(c == 13 || (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || c == ' ')) {
                continue;
            }

            if (c == 13) {
                input[input_c] = 0;
                break;
            }

            input[input_c] = c;

            if (input[input_c] == ' ') {
                input[input_c] = 0;

                // Readback the byte.
                printf(input);

                // Convert from hex to byte.
                program_code[code_c] = strtol(input, NULL, 16);

                input_c = 0;
                code_c++;
            }

            else input_c++;
        }

        printf("Program length: %d / first & last byte: %hhu %hhu\n", code_c, program_code[0], program_code[code_c - 1]);

        if ((code_c % 2) != 0) {
            printf("program bytes are not a multiple of 2!\n");
            return 0;
        }

        avr_spi_init();
        avr_reset();

        if (!avr_enter_programming_mode()) {
            printf("failed to enter programming mode\n");
            return 0;
        }

        printf("Entered programming mode\n");

        avr_erase_memory();

        printf("Erased program memory successfully for programming\n");
        
        // full pages
        for (int i = 0; i < code_c / 64; i++) {
            printf("Flashing page %d..\n", i);

            int page_offset = i * 64;

            for (int j = 0; j < 32; j++) {
                int word_index = (j * 2) + page_offset;
                
                uint8_t high_byte = program_code[word_index + 1];
                uint8_t low_byte = program_code[word_index];

                avr_write_temporary_buffer(j, low_byte, high_byte);
            }

            avr_flash_program_memory(i * 32);

            printf("Verifying flash..\n");

            if (!avr_verify_program_memory_page(i * 32, program_code + page_offset, 32)) {
                printf("Verification failed! Page has not been flashed correctly..\n");
                sleep_ms(100);
                return 0;
            } else printf("Page flash successfully verified!\n");
        }

        int remaining_bytes = code_c % 64;
        int bytes_offset = code_c - remaining_bytes;

        // partial page
        printf("Flashing partial page..\n");

        for (int j = 0; j < remaining_bytes / 2; j++) {
            int word_index = (j * 2) + bytes_offset;
            
            uint8_t high_byte = program_code[word_index + 1];
            uint8_t low_byte = program_code[word_index];

            avr_write_temporary_buffer(j, low_byte, high_byte);
        }

        avr_flash_program_memory(bytes_offset / 2);

        printf("Verifying flash..\n");

        if (!avr_verify_program_memory_page(bytes_offset / 2, program_code + bytes_offset, remaining_bytes / 2)) {
            printf("Verification failed! Page has not been flashed correctly..\n");
            sleep_ms(100);
            return 0;
        } else printf("Page flash successfully verified!\n");

        printf("Verifying overall page flashes..\n");

        if (!avr_verify_program_memory_page(0, program_code, code_c / 2)) {
            printf("Overall page verification failed! Pages have not been flashed correctly..\n");
            sleep_ms(100);
            return 0;
        } else printf("Overall page flash verification successful! Firmware has been flashed correctly\n");

        printf("Going back to standby mode for future flashes..\n");
        printf("FINISH");
    }

    return 0;
}
