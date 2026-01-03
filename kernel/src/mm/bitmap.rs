// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

pub struct Bitmap<'a> {
    data: &'a mut [u8],
    size_in_bits: usize,
}

impl<'a> Bitmap<'a> {
    pub fn new(start_addr: *mut u8, size_in_bytes: usize) -> Self {
        unsafe {
            let slice = core::slice::from_raw_parts_mut(start_addr, size_in_bytes);
            Self {
                data: slice,
                size_in_bits: size_in_bytes * 8,
            }
        }
    }

    #[inline]
    fn get_index(&self, bit: usize) -> (usize, u8) {
        if bit >= self.size_in_bits {
            panic!("BITMAP Index out of bounds!!!: {bit}");
        }
        ((bit / 8), 1 << (bit % 8)) // bit / 8 is byte index bit % 8 is bit offset
    }

    pub fn set(&mut self, bit: usize) {
        let (idx, mask) = self.get_index(bit);
        self.data[idx] |= mask;
    }

    pub fn clear(&mut self, bit: usize) {
        let (idx, mask) = self.get_index(bit);
        self.data[idx] &= !mask;
    }

    pub fn free_reg(&mut self, phys_addr: u64, size: u64) {
        let start_frame = phys_addr / 4096;
        let num_frames = (size + 4095) / 4096;

        for i in 0..num_frames {
            self.clear((start_frame + i) as usize);
        }
    }

    pub fn alloc_reg(&mut self, phys_addr: u64, size: u64) {
        let start_frame = phys_addr / 4096;
        let num_frames = (size + 4095) / 4096;

        for i in 0..num_frames {
            self.set((start_frame + i) as usize);
        }
    }
}