// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

use std::env;
use std::path::PathBuf;

fn main() {
    let linker_script = "src/arch/x86_64/linker.ld"; // TODO make abstraction
    let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let script_path = manifest_dir.join(linker_script);

    if !script_path.exists() {
        panic!("Linker script not found at: {}", script_path.display());
    }

    let entry_asm_path      = "src/arch/x86_64/entry.asm";
    let interrupts_asm_path = "src/arch/x86_64/interrupts.asm";
    
    println!("cargo:rerun-if-changed={}", entry_asm_path);
    println!("cargo:rerun-if-changed={}", interrupts_asm_path);

    let entry_build_res = nasm_rs::Build::new()
        .file(entry_asm_path)
        .compile("asm_entry");

    entry_build_res.unwrap();
    
    let interrupts_build_res = nasm_rs::Build::new()
        .file(interrupts_asm_path)
        .compile("asm_interrupts");

    interrupts_build_res.unwrap();

    println!("cargo:rustc-link-lib=static=asm_entry");
    println!("cargo:rustc-link-lib=static=asm_interrupts");
}