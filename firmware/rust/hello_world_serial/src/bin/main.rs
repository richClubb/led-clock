#![no_std]
#![no_main]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]

use esp_hal::clock::CpuClock;
use esp_hal::main;
use esp_hal::time::{Duration, Instant};
use esp_hal::timer::timg::TimerGroup;
use esp_println::println;

#[panic_handler]
fn panic(_: &core::panic::PanicInfo) -> ! {
    loop {}
}

extern crate alloc;

// This creates a default app-descriptor required by the esp-idf bootloader.
// For more information see: <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/app_image_format.html#application-description>
esp_bootloader_esp_idf::esp_app_desc!();

#[main]
fn main() -> ! {
    // generator version: 0.5.
    
    // what is this used for?
    esp_println::logger::init_logger_from_env();

    // what is this used for?
    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
    
    // what is this used for
    let peripherals = esp_hal::init(config);

    // this looks scary
    esp_alloc::heap_allocator!(size: 64 * 1024);

    // just an incrementer, I did this.
    let mut count = 0;

    // what is this used for?
    let timg0 = TimerGroup::new(peripherals.TIMG0);
    
    // What is this used for?
    let _init = esp_wifi::init(timg0.timer0, esp_hal::rng::Rng::new(peripherals.RNG)).unwrap();

    // main execution loop
    loop {
        // get current time
        let delay_start = Instant::now();

        // delay loop
        while delay_start.elapsed() < Duration::from_millis(3000) {
        }

        // print current time
        println!("Hello world {}", count);

        // increment count
        count = count + 1;
    }

    // for inspiration have a look at the examples at https://github.com/esp-rs/esp-hal/tree/esp-hal-v1.0.0-rc.0/examples/src/bin
}
