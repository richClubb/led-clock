# 12 Projects in 12 Months - Project 1 - LED Clock

I chose this as the first of the projects for my 12 projects in 12 months. I've had this knocking around for several years now and wanted to get it into a more finished state.

## Objectives

- [x] Basic RTC functionality
- [x] Smooth motion for clock ticking
- [x] Ability to re-colour the units
- [x] Ability to re-allocate the led rings at runtime
- [ ] Timer
    - [x] Programmable over serial 
    - [ ] Settable with buttons
    - [x] Start 
    - [ ] pause 
    - [ ] reset
    - [x] Configurable colour
- [ ] EEPROM to store configuration from last serial config
- [ ] NTP functionality for setting the time
- [ ] Web page for configuration
- [ ] Self-hosted WiFi for configuration
- [x] Dont use delay!
- [ ] Light adjustable intensity
- [ ] Basic functionality in Rust (stretch goal)
    - [ ] Basic RTC functionality
    - [ ] Basic LED functionality

## Project Start

I had already done some of this before, but was basically starting from scratch. I thought I'd start by trying out several different bits of functionality based on the examples available.

### Project Organisation

I decided for this project that I would try to have separate folders for different examples rather than just trying to build up. This way I can experiment and figure a few things out before comitting to a final code architecture.

### LED ring interpolation

I started off by trying to figure out the LED movement as this would have the most significant visual difference in the finished project

What I wanted was to have a nice smooth movement of the leds walking their way around the ring, and if two units intersect then they should add their colours together on those pixels. Even if I had a 60 ring  The way I achieved this is by thinking about the time in terms of degrees

If we take the 24 ring LED as a starting point, each LED is separated by 15 degrees. In 60 seconds, each second is separated by 6 degrees, this doesn't really intersect nicely, so if you were to just light up the most appropriate LED you'd end up getting a non-regular 'tick'. The initial idea I came up with was to blend between the current LED and the next LED based on the degrees. I have 2 'modifiers' which multiply the initial colour

```
time_degree = time * degree_per_unit;

primary_led = time_degree / degree_per_pixel
secondary_led = primary_led + 1

primary_mod = (deg_per_pixel - (time_degree % deg_per_pixel)) / deg_per_pixel
secondary_mod = (time_degree % deg_per_pixel) / deg_per_pixel

led1_red, led1_green, led1_blue = red_col * primary_mod, green_col * primary_mod, blue_col * primary_mod
led2_red, led2_green, led2_blue = red_col * secondary_mod, green_col * secondary_mod, blue_col * secondary_mod
```

From this, if the LED is 'exactly' on a nice intersecting degree, then `primary_mod = 1` and `secondary_mod = 0`. As the led moves away from the intersecting degree `primary_mod` tends towards 0 and `secondary_mod` tends towards 1.

I could do something more complex with a bell curve, but this is an experiment for a later date.

One note, for the seconds. I had to calculate from millis to get a nice smooth movement, minutes interpolate on seconds, hours interpolate on minutes.

See `led-clock/firmware/arduino/clock_sim` for an example.

### RTC

This was fairly simple, just using the example to read and write to the RTC.

See `led-clock/firmware/arduino/serial_msg_rtc/serial_msg_rtc.ino` as an example.

### Serial command interpretation

This is always a pain for Arduino projects, the serial libraries are pretty easy to use but the real pain is reading stuff off the serial buffer as it's pretty inconsistent.

I wanted to see if I can develop a general pattern for how to do this in the future.

I came up with the idea of having a `command_buffer`. Each command will have a format of `[x]=[data];` E.g. `T=2025-01-01T10:11:12;` would be the command to set the time. The serial libary reads characters into the command_buffer until the `;` is found.

Once the command is found it is then processed and validated

See `led-clock/firmware/arduino/serial_msg_rtc/serial_msg_rtc.ino` as an example.

#### Function pointers

I wanted to keep the code as decoupled as much as possible, so the `serial_interpreter.h` code creates function pointers that can be attached to by the main code which will already have the context and the access to the variables like the RTC or LED strips.

## Timer

One use-case for this was to be as a desk clock, and a useful thing would be to have a pomodoro timer (or similar).

Functionally when the timer is active I want it to be on one of the dials exclusively, and when it is finished it should hvae some visual indication (flashing etc). Once it's finished I want everything to go back to normal.

# Relationships with time

I had a conversation with a friend recently after showing them the project. They are more artistic than I am and they came up with the idea that this kind of project allows people to interact with time in a way that they want. I really liked that concept, and it might also make me tempted to create a normal clock with a similar amount of customisability.

