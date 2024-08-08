#include <string.h>

#include "rp2350_playground.h"
#include "hardware/structs/glitch_detector.h"
#include "hardware/flash.h"

#include "display/display.h"

uint32_t joy_io[] = {
	JOY_E, JOY_S, JOY_W, JOY_PUSH, JOY_N
};

void init_uart(void) {
	uart_init(TARGET_UART_INSTANCE, 115200);
	gpio_set_function(TARGET_UART_TX, GPIO_FUNC_UART);
	gpio_set_function(TARGET_UART_RX, GPIO_FUNC_UART);
}

bool glitch_detector_armed(void) {
	if(glitch_detector_hw->arm == 0x5bad) {
		return false;
	} else {
		return true;
	}
}

void glitch_detector_arm(void) {
	glitch_detector_hw->arm = 0;
}

void glitch_detector_set_sensitivity(uint32_t det0, uint32_t det1, uint32_t det2, uint32_t det3) {
	uint32_t detector_value = 0xde000000;
	detector_value |= (det0 & 3);
	detector_value |= ((det0 & 3) ^ 0b11) << 8;
	detector_value |= (det1 & 3) << 2;
	detector_value |= ((det1 & 3) ^ 0b11) << 10;
	detector_value |= (det2 & 3) << 4;
	detector_value |= ((det2 & 3) ^ 0b11) << 12;
	detector_value |= (det3 & 3) << 6;
	detector_value |= ((det3 & 3) ^ 0b11) << 14;
	glitch_detector_hw->sensitivity = detector_value;
}

uint32_t glitch_detector_get_trig_status(void) {
	return glitch_detector_hw->trig_status;
}

void glitch_detector_lock(void) {
	glitch_detector_hw->lock = 0x12345678;
}

#define NVM_DATA_OFFSET 0x8000
#define NVM_DATA_SIZE 0x1000
static const void *nvm_data = (void*)(XIP_BASE + NVM_DATA_OFFSET);

void __not_in_flash_func(nvm_put)(int index, int value) {
	union { int values[NVM_DATA_SIZE/sizeof(int)]; uint8_t raw[NVM_DATA_SIZE]; } buffer;
	memcpy(buffer.raw, nvm_data, NVM_DATA_SIZE);
	flash_range_erase(NVM_DATA_OFFSET, NVM_DATA_SIZE);
	buffer.values[index] = value;
	flash_range_program(NVM_DATA_OFFSET, buffer.raw, sizeof(buffer));
}

int nvm_get(int index) {
	return ((int*)nvm_data)[index];
}


int menu(const char *title, const char *options[], unsigned int num_options) {
	unsigned int selection = 0;
	unsigned int offset = 0;
	int result = -1, finished = 0;
	do {
		watchdog_update();
		cls(false);

		pprintfxy(10, 10, "%s", title);

		for (unsigned option = offset; option < num_options; option++) {
			pprintfxy(10, 26 + 16 * (option - offset), "  %s", options[option]);
		}

		pprintfxy(10, 26 + 16 * (selection - offset), ">");

		uint32_t io = 0;
		int debounce_count = 0;
		do {
			watchdog_update();
			debounce_count++;
			uint32_t new_io = gpio_get_all() & JOY_IO_MASK;
			if (new_io != io || io == JOY_IO_MASK) {
				debounce_count = 0;
			}
			io = new_io;
		} while (debounce_count < 1000);

		if (!(io & (1<<JOY_W))) {
			result = -1;
			finished = 1;
		}
		else if (!(io & (1<<JOY_N))) {
			selection = (selection - 1) % num_options;
		}
		else if (!(io & (1<<JOY_S))) {
			selection = (selection + 1) % num_options;
		}
		else if (!(io & (1<<JOY_E)) || !(io & (1<<JOY_PUSH))) {
			result = (int)selection;
			finished = 1;
		}

		while (selection > offset && selection - offset >= 2) offset++;
		if (offset > selection) offset = selection == 0 ? 0 : selection - 1;

		while ((gpio_get_all() & JOY_IO_MASK) != JOY_IO_MASK) {
			watchdog_update();
		}
	} while (!finished);

	return result;
}
