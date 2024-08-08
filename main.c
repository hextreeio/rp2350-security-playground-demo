#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/watchdog.h"
// #include "hardware/regs/glitch_detector.h"
#include "hardware/regs/powman.h"
#include "hardware/structs/powman.h"
#include "hardware/structs/otp.h"
#include "rp2350_playground.h"

#include "display/display.h"

#include <stdio.h>

#pragma GCC push_options
#pragma GCC optimize("O0")
#define OUTER_LOOP_CNT 100
#define INNER_LOOP_CNT 100

static inline int nvm_get_glitch_detector_sensitivity() {
	int sensitivity = nvm_get(NVM_GLITCH_DETECTOR_SENSITIVITY);
	if (sensitivity == -1) {
		sensitivity = NVM_GLITCH_DETECTOR_SENSITIVITY_OFF;
	}
	return sensitivity;
}

void __not_in_flash_func(glitch_target)(void)
{
	volatile register uint32_t i, j;
	volatile register uint32_t cnt;
	uint32_t blink_status = 1;
	bool glitch_detector_triggered = (powman_hw->chip_reset & 0x04000000) != 0;

	if (glitch_detector_triggered)
	{
		uart_putc_raw(TARGET_UART_INSTANCE, 'G');
	}
	else
	{
		uart_putc_raw(TARGET_UART_INSTANCE, 'R');
	}

#ifdef GLITCH_DETECTOR_ON
	int sensitivity = nvm_get_glitch_detector_sensitivity();

	if(sensitivity != NVM_GLITCH_DETECTOR_SENSITIVITY_OFF) {
		glitch_detector_set_sensitivity(sensitivity, sensitivity, sensitivity, sensitivity);
		glitch_detector_arm();
		glitch_detector_lock();
	}

#endif


	watchdog_update();
	cnt = 0;

	watchdog_update();
	gpio_put(TRIGGER_PIN, 1);
	sleep_ms(1);
	watchdog_update();
	gpio_put(TRIGGER_PIN, 0);

	gpio_put(TRIGGER2_PIN, 1);
	for (i = 0; i < OUTER_LOOP_CNT; i++)
	{
		for (j = 0; j < INNER_LOOP_CNT; j++)
		{
			cnt++;
		}
	}
	gpio_put(TRIGGER2_PIN, 0);

	

	const bool glitch_detected = i != OUTER_LOOP_CNT || j != INNER_LOOP_CNT
		|| cnt != (OUTER_LOOP_CNT * INNER_LOOP_CNT);



	// Check for glitch
	if (glitch_detected)
	{
		// X indicates successful glitch
		uart_putc_raw(uart0, 'X');
	}
	else
	{
		// N indicates regular execution
		uart_putc_raw(uart0, 'N');
	}
	watchdog_update();

	display_initialize();
	if (glitch_detector_triggered) {

		watchdog_update();
		pprintfxy(5, 10, "Glitch detector");
		watchdog_update();

		uint32_t trig_status = glitch_detector_get_trig_status();
		char trig_str[32];
		int str_offset = 0;
		for (int bit = 0; bit < 4; bit++) {
			if (trig_status & (1 << bit)) {
				str_offset += snprintf(&trig_str[str_offset], sizeof(trig_str)-str_offset, "%s%d", str_offset ? ", " : "", bit);
			}
		}

		pprintfxy(5, 26, "triggered (%s)", trig_str);

		while (gpio_get(JOY_PUSH))
		{
			watchdog_update();
		}

		// Wait for the joystick to be released after the press
		while (!gpio_get(JOY_PUSH))
		{
			watchdog_update();
		}
	// } else {
	// 	pprintfxy(5, 25, "not triggered");
	// }
	}

}
#pragma GCC pop_options

static void menu_bootmode(void) {
	switch (menu("Select mode:", (const char*[]){"Glitch Challenge", "OTP Challenge"}, 2)) {
	case -1: break;
	case 0: nvm_put(NVM_BOOT_MODE, NVM_BOOT_MODE_GLITCH_CHALLENGE); break;
	case 1: nvm_put(NVM_BOOT_MODE, NVM_BOOT_MODE_OTP_CHALLENGE); break;
	default: break;
	}
}

static void menu_glitch_detector_sensitivity(void) {
	switch (menu("Glitch Detector", (const char*[]){"Off", "0", "1", "2", "3"}, 5)) {
	case -1: break;
	case 0: nvm_put(NVM_GLITCH_DETECTOR_SENSITIVITY, NVM_GLITCH_DETECTOR_SENSITIVITY_OFF); break;
	case 1: nvm_put(NVM_GLITCH_DETECTOR_SENSITIVITY, 0); break;
	case 2: nvm_put(NVM_GLITCH_DETECTOR_SENSITIVITY, 1); break;
	case 3: nvm_put(NVM_GLITCH_DETECTOR_SENSITIVITY, 2); break;
	case 4: nvm_put(NVM_GLITCH_DETECTOR_SENSITIVITY, 3); break;
	default: break;
	}
}

static void menu_top(void) {
	for (;;) {
		char glitch_detector_text[30];
		int sensitivity = nvm_get_glitch_detector_sensitivity();
		if(sensitivity != NVM_GLITCH_DETECTOR_SENSITIVITY_OFF) {
			snprintf(glitch_detector_text, 30, "Detector (%d)", sensitivity);
		} else {
			snprintf(glitch_detector_text, 30, "Detector (Off)", sensitivity);
		}

		switch (menu("Options:", (const char*[]){
				"Boot mode",
				glitch_detector_text,
				"Restart"
			}, 3)) {
		case -1:
			break;
		case 0:
			menu_bootmode();
			break;
		case 1:
			menu_glitch_detector_sensitivity();
			break;
		case 2:
			watchdog_enable(10, 1);
			while(1) {}
			break;
		default:
			break;
		}
	}
}

void otp_challenge(void) {
	volatile uint32_t *p = &otp_hw->critical;
	volatile uint32_t value = *p;
	uart_putc_raw(uart0, 'N');
	uart_putc_raw(uart0, value & 0xff);
	uart_putc_raw(uart0, (value >> 8) & 0xff);
	uart_putc_raw(uart0, (value >> 16) & 0xff);
	uart_putc_raw(uart0, (value >> 24) & 0xff);
	watchdog_update();
	display_initialize();
	watchdog_update();
	pprintfxy(5, 10, "CRIT1:\n%08X", *p);
	watchdog_update();
	while (gpio_get(JOY_PUSH))
	{
		watchdog_update();
	}

	// Wait for the joystick to be released after the press
	while (!gpio_get(JOY_PUSH))
	{
		watchdog_update();
	}
}

enum BOOTMODE {
	BOOTMODE_GLITCH_CHALLENGE=0,
	BOOTMODE_OTP_CHALLENGE=1
};

int main()
{
	enum BOOTMODE bootmode = BOOTMODE_GLITCH_CHALLENGE;
	switch (nvm_get(NVM_BOOT_MODE)) {
	case NVM_BOOT_MODE_GLITCH_CHALLENGE: case -1:
	bootmode = BOOTMODE_GLITCH_CHALLENGE;
		break;
	case NVM_BOOT_MODE_OTP_CHALLENGE:
	bootmode = BOOTMODE_OTP_CHALLENGE;
		break;
	default:
		bootmode = BOOTMODE_GLITCH_CHALLENGE;
		break;
	}

	init_uart();
	watchdog_enable(100, 1);
	gpio_init(TRIGGER_PIN);
	gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
	gpio_put(TRIGGER_PIN, 0);

	gpio_init(TRIGGER2_PIN);
	gpio_set_dir(TRIGGER2_PIN, GPIO_OUT);
	gpio_put(TRIGGER2_PIN, 0);

	for(int i=0; i < 5; i++) {
		gpio_init(joy_io[i]);
		gpio_set_dir(joy_io[i], GPIO_IN);
		gpio_pull_up(joy_io[i]);
	}

	switch(bootmode) {
		case BOOTMODE_OTP_CHALLENGE:
			otp_challenge();
			break;
		case BOOTMODE_GLITCH_CHALLENGE:
		default:
			glitch_target();
			break;
	}


	menu_top();

	for (;;) { watchdog_update(); }
}
