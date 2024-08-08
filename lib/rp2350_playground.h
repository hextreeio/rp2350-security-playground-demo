#pragma once

#include "hardware/pio.h"
#include "hardware/watchdog.h"
#include "hardware/uart.h"

#define TARGET_UART_INSTANCE uart0
#define TARGET_UART_TX 12
#define TARGET_UART_RX 13

#define TRIGGER_PIN 14
#define TRIGGER2_PIN 15


void init_uart(void);
bool glitch_detector_armed(void);
void glitch_detector_arm(void);
void glitch_detector_set_sensitivity(uint32_t det0, uint32_t det1, uint32_t det2, uint32_t det3);
void glitch_detector_lock(void);
uint32_t glitch_detector_get_trig_status(void);


#define LED_BASE 26
#define JOY_E 22
#define JOY_S 19
#define JOY_W 23
#define JOY_PUSH 24
#define JOY_N 25
#define JOY_IO_MASK ((1<<JOY_PUSH)|(1<<JOY_N)|(1<<JOY_S)|(1<<JOY_W)|(1<<JOY_E))

extern uint32_t joy_io[5];

#define NVM_BOOT_MODE 0
#define NVM_GLITCH_DETECTOR_SENSITIVITY 1
#define NVM_GLITCH_DETECTOR_SENSITIVITY_OFF 5

#define NVM_BOOT_MODE_GLITCH_CHALLENGE 0
#define NVM_BOOT_MODE_OTP_CHALLENGE 1

void nvm_put(int index, int value);
int nvm_get(int index);

int menu(const char *title, const char *options[], unsigned int num_options);
