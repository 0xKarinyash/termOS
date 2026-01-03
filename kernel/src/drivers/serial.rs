// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

use core::fmt;
use core::fmt::Write;
use spin::Mutex;
use crate::arch::x86_64::io::{*};

pub struct SerialPort {
    port: u16
}

impl SerialPort {
    pub const fn new(port: u16) -> Self {
        Self { port }
    }

    pub fn init(&self) {
        outb(self.port + 1, 0x00); // Disable interrupts
        outb(self.port + 3, 0x80); // Enable DLAB (set baud rate divisor)
        outb(self.port + 0, 0x03); // Set divisor to 3 (38400 baud)
        outb(self.port + 1, 0x00); // High byte
        outb(self.port + 3, 0x03); // 8 bits, no parity, one stop bit
        outb(self.port + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
        outb(self.port + 4, 0x0B); // IRQs enabled, RTS/DSR set
    }

    fn is_transmit_empty(&self) -> bool {
        inb(self.port + 5) & 0x20 != 0
    }

    pub fn send(&self, data: u8) {
        while !self.is_transmit_empty() {}
        outb(self.port, data);
    }

    pub fn write_string(&self, s: &str) {
        for byte in s.bytes() {
            self.send(byte);
        }
    }
}

impl fmt::Write for SerialPort {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        self.write_string(s);
        Ok(())
    }
}

pub static COM1: Mutex<SerialPort> = Mutex::new(SerialPort::new(0x3F8));
    
#[doc(hidden)]
pub fn _print(args: core::fmt::Arguments) {
    COM1.lock().write_fmt(args).unwrap();
}

