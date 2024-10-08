#include "display.h"

#include <stdlib.h>
#include <stdio.h>

#include "font_renderer.h"
#include "ssd1306.h"
#include "bootup2.h"

#define PIN_SDA 20
#define PIN_SCL 21
#define DISPLAY_I2C_INSTANCE i2c0

display *global_display = NULL;

void display_initialize() {
    global_display = malloc(sizeof(display));
    if(global_display == NULL) {
        return;
    }

    display_create(global_display);
}

void display_create(display *display) {

    // i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    // gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    // gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    // gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    // gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    i2c_init(DISPLAY_I2C_INSTANCE, 1000000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);



    ssd1306_context ctx;
    ctx.i2c = DISPLAY_I2C_INSTANCE;

    ssd1306_begin(&ctx, SSD1306_SWITCHCAPVCC);
    ssd1306_clear_display(&ctx);
    ssd1306_display(&ctx);

    display->ctx = ctx;
}

void display_screen(display *d, screen *s) {
    if(d == NULL) {
        return;
    }
    if(s == NULL) {
        return;
    }
    ssd1306_clear_display(&d->ctx);
    void ssd1306_draw_sceen(ssd1306_context *ctx, int16_t x, int16_t y, screen *screen);
    ssd1306_draw_bitmap(&d->ctx, 0, 0, s->data, s->width, s->height, 1);
    ssd1306_display(&d->ctx);
}

void display_printf(display *d, uint8_t x, uint8_t y, const char *format, va_list args) {
    if(d == NULL) {
        return;
    }
    char print_buffer[128];
    vsnprintf(print_buffer, 128, format, args);
    font_render_string(&d->ctx, x, y, &DEFAULT_FONT, print_buffer);
}

// Global helper functions
void cls(bool display) {
    ssd1306_clear_display(&global_display->ctx);
    if(display) {
        ssd1306_display(&global_display->ctx);
    }
}

void ft_cls(bool display) {
    screen *s = &bootup2;
    ssd1306_clear_display(&global_display->ctx);
    ssd1306_draw_bitmap(&global_display->ctx, 0, 0, s->data, s->width, s->height, 1);


    // display_screen(global_display, &bootup2);
    // ssd1306_clear_display(&global_display->ctx);
    if(display) {
        ssd1306_display(&global_display->ctx);
    }
}

void pprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    display_printf(global_display, 0, DEFAULT_FONT_Y, format, args);
    va_end(args);

    ssd1306_display(&global_display->ctx);
}

void pprintfxy(uint8_t x, uint8_t y, const char *format, ...) {
    va_list args;
    va_start(args, format);
    display_printf(global_display, x, y, format, args);
    va_end(args);
    ssd1306_display(&global_display->ctx);
}
