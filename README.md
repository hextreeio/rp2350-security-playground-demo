# RP2350 Security Playground Demo


This firmware demonstrates some of the security testing features of the RP2350 security playground, mainly focusing on bypassing the glitch-detector and glitching the OTP.

## Building

```
mkdir build
cmake -DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2 ..
make
```

## Glitch Detector Mode

This example exposes a very simple nested loop as a glitch-target. On reset, the target will send "R" to the serial console (with TX on GPIO 12), on regular execution of the loop it will send "N", and on a successful glitch it will send "X".

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

## OTP Glitching

To attempt to glitch OTP values in CRIT1 this mode will read out CRIT1 and send it out via UART.

Note that there's a strange behavior where the value will appear corrupted, but will not actually be corrupted when checked in if-conditions etcpp.