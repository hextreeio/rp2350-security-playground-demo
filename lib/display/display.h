#pragma once

#include <stdarg.h>
#include "ssd1306.h"
#include "FreeSans5pt7b.h"
#include "hardware/i2c.h"

#define DEFAULT_FONT FreeSans7pt7b
#define DEFAULT_FONT_Y 10

typedef struct {
    ssd1306_context ctx;
} display;

typedef struct {
    uint32_t width;
    uint32_t height;
    const uint8_t *data;
} screen;

extern display *global_display;

void display_initialize();
void display_create(display *display);
void display_screen(display *d, screen *s);
void display_printf(display *d, uint8_t x, uint8_t y, const char *format, va_list args);
void cls(bool display);
void pprintf(const char *format, ...);
void pprintfxy(uint8_t x, uint8_t y, const char *format, ...);
