# 1_simple_loop

This example exposes a very simple nested loop as a glitch-target. On reset, the target will send "R" to the serial console (with TX on GPIO 12), on regular execution of the loop it will send "N", and on a successful glitch it will send "X".

Two binaries will be built by this project:
- 1_simple_loop.uf2 - Just the simple loop glitch target
- 1_simple_loop_glitch_detector.uf2 - The same code but with the glitch-detector enabled and set to high sensitivity

The firmware also activates the watchdog to ensure that an automatic restart occurs if the chip hangs.

```c
for(i = 0; i < OUTER_LOOP_CNT; i++) {
    for(j=0; j < INNER_LOOP_CNT; j++){
        cnt++;
    }
}
if (i != OUTER_LOOP_CNT || j != INNER_LOOP_CNT || cnt != (OUTER_LOOP_CNT * INNER_LOOP_CNT)  ) {
    watchdog_update();
    // X indicates successful glitch
    uart_putc_raw(uart0, 'X');
} else {
    // N indicate4s regular execution
    uart_putc_raw(uart0, 'N');
}
```

For triggering:
- A trigger signal is generated on IO 14
- While the loop is running IO 15 is pulled high to allow for positioning the glitch better on an oscilloscope
