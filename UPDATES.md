# 12 Projects in 12 Months - Project 1 - LED Clock

I chose this as the first of the projects for my 12 projects in 12 months. I've had this knocking around for several years now and wanted to get it into a more finished state.

## Objectives

### Base Goals

- [x] Basic RTC functionality
- [x] Smooth motion for clock ticking
    - [x] Fix millis offset bug - Done
- [x] Ability to re-colour the units
- [x] Ability to re-allocate the led rings at runtime
- [ ] Timer
    - [x] Programmable over serial 
    - [ ] Settable with buttons
    - [x] Start 
    - [x] pause 
    - [x] reset
    - [x] Configurable colour
    - [ ] Smoother animation
- [X] EEPROM to store configuration from last serial config
    - [x] Initial prototype
    - [ ] ~~Abstraction?~~ - I don't think I'm going to bother with this
    - [X] Complete the storage
- [ ] ~~NTP functionality for setting the time~~
- [ ] ~~Web page for configuration~~
- [ ] ~~Self-hosted WiFi for configuration~~
- [x] Dont use delay!
- [ ] ~~Light adjustable intensity~~
- [ ] ~~Basic functionality in Rust~~
    - [ ] Basic RTC functionality
    - [ ] Basic LED functionality
- [x] Fix serial startup bug

### Stretch Goals

- [ ] ~~Perf test the serial interpreter~~

## Project Start

I had already done some of this before, but was basically starting from scratch. I thought I'd start by trying out several different bits of functionality based on the examples available.

I decided to try and organise the project differently, I created separate firmware sources for each experiment, this way if I can focus on the important part and ignore the rest.

### Ring Tests

These were just tests to make sure the rings work and I could address them

See [single ring test](./firmware/arduino/ring_test_single/ring_test_single.ino) and [dual ring test](./firmware/arduino/ring_test_dual/ring_test_dual.ino).

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

See [clock-sim](./firmware/arduino/clock_sim/clock_sim.ino) for an example.

### RTC

This was fairly simple, just using the example to read and write to the RTC.

See [serial_msg_rtc](./firmware/arduino/serial_msg_rtc/serial_msg_rtc.ino) as an example.

### Serial command interpretation

This is always a pain for Arduino projects, the serial libraries are pretty easy to use but the real pain is reading stuff off the serial buffer as it's pretty inconsistent.

I wanted to see if I can develop a general pattern for how to do this in the future.

I came up with the idea of having a `command_buffer`. Each command will have a format of `[x]=[data];` E.g. `T=2025-01-01T10:11:12;` would be the command to set the time. The serial libary reads characters into the command_buffer until the `;` is found.

Once the command is found it is then processed and validated

There are loads of different ways to interpret the data. I would like to try and put a pin in this and see if I can performance measure my approaches.

See [serial_msg_rtc](./firmware/arduino/serial_msg_interpreter/serial_msg_interpreter.ino) as an example.

#### Function pointers

I wanted to keep the code as decoupled as much as possible, so the `serial_interpreter.h` code creates function pointers that can be attached to by the main code which will already have the context and the access to the variables like the RTC or LED strips.

## Update 1 - Timers and working prototype

One use-case for this was to be as a desk clock, and a useful thing would be to have a pomodoro timer (or similar).

Functionally when the timer is active I want it to be on one of the dials exclusively, and when it is finished it should hvae some visual indication (flashing etc). Once it's finished I want everything to go back to normal.

I managed to get the main code assembled into a working example [led-clock](./firmware/arduino/led_clock/led_clock.ino). This currently has most of the working functionality.

### Issues, millisecond offset

The RTC currently only gives me second resolution, so to get the millisecond resolution I need for the smooth movement I need to use the internal arduino `millis()` function to get millisecond accuracy. At startup I create an 'offset' when the second ticks over and then subtract that from the current `millis()` value and then mod the result by 1000. This gives me a value 0 - 999 which should be the milliseconds from the current second. The problem is that over timm the clock in the arduino drifts slightly and it can get a jitter. 

There are ways to get millisecond accuracy from the DS3231 but at the moment it's on the back-burner. I think if I calculate the millis offset each second then it should be ok, it's just annoying to have to keep doing this.

### Relationships with time

I had a conversation with a friend recently after showing them the project. They are more artistic than I am and they came up with the idea that this kind of project allows people to interact with time in a way that they want. I really liked that concept, and it might also make me tempted to create a normal clock with a similar amount of customisability.

## Update 2

At this point I've got the majority of the functionality done and I'm chasing the little improvements and things are getting harder. The code has been pretty easy to modify and improve, especially the serial commands, but adding things like WiFi and NTP is going to add annoying complications that are just frustrating to have to deal with.

* If I add WiFi i need to be able to do the connection, re-connection logic. This will be frustrating with the real-time nature of the clock and I'm not sure how to handle it with a single-threaded single-core micro. I've done it with my temperature sensor and I have to pause the sampling to get it to work
* NTP could possibly spam the EEPROM significantly, so I have to be careful how much I write to it.
* The self hosted WiFi and web page is a frustration and adds a significant amount of complexity to the startup code
* The web page config itself opens questions on how to handle the showing and updating of the config, as well as displaying the page

It's easy to get to this point and just think, "fuck it, i'm happy" but I'm going to try and persevere. We try and teach our children to be resilient but as an adult I think we can also suffer from the malaise that comes with something difficult that doesn't show an immediate return

### Refactors and moving fast

I have been adding code a bit furiously and not really thinking about how maintainable it is, I think I need to actually do a line-by-line review and fix a few things. I noticed a lot of magic numbers cropping up and it's not really a good thing.

I'm a great believer in the "Make it work, make it good, make it fast". And cutting some corners while getting it working is fine, but I think if you cut too many you can end up hurting things in the long run. It's good to take a step back every few commits and see how the overall trajectory of the code is looking and how you can see things moving forward with the objectives you have in mind.

This is especially important in larger projects with multiple contributors. You do have to keep an eye on the future while also keeping things grounded in the present.

### Video blog

I've yet to do a single recording for this. I think there is an element of fear putting this out into the world at large and I'm afraid of the code being judged by someone (especially employers). It's a hard thing to get over, even though I'm pretty certain that I'll probably get 3 views and nobody will care. 

## Update 3

I've got the majority of the functionality working which I'm pretty happy about at this point I've got

* General LED display
* Serial configuration
* RTC functionality
* Timer functionality
    * missing buttons
* NVM functionality

It's the WiFi stuff that is mostly remaining. The question now is do I want to keep going or try to pivot to using Rust? That is the question. I think I'll spend this weekend ( 15 / 16 / 17 August ) working on the WiFi and then after that switch over to trying to get stuff working in Rust.