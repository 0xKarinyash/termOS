// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

use std::process::{Command, ExitStatus};
use std::{env, fs, path::PathBuf};

fn main() {
    let args: Vec<String> = env::args().collect();
    let command = args.get(1).map(|s| s.as_str()).unwrap_or("build");

    match command {
        "build" => {
            build();
            create_disk_image();
        }
        "run" => {
            build();
            create_disk_image();
            run();
        }
        _ => println!("Usage: cargo xtask [build|run]"),
    }
}

fn build() {
    println!("🚀 Building Bootloader (UEFI)...");

    let status = Command::new("rustup")
        .args([
            "run", "nightly", "cargo",
            "build",
            "--package", "bootloader",
            "--target", "x86_64-unknown-uefi",
            "-Zbuild-std=std,panic_abort", 
            "--release",
        ])
        .status()
        .expect("Failed to run cargo build for bootloader");

    if !status.success() {
        panic!("Bootloader build failed");
    }

    println!("🦀 Building Kernel (No-std)...");

    let status = Command::new("rustup") 
        .args([
            "run", "nightly", "cargo",
            "build",
            "--package", "kernel",
            "--target", "x86_64-unknown-none",
            "-Zbuild-std=core,alloc", 
            "--release",
        ])
        .status()
        .expect("Failed to run cargo build for kernel");

    if !status.success() {
        panic!("Kernel build failed");
    }
}

fn create_disk_image() {
    println!("💾 Creating Disk Image (termOS.img)...");

    let target_path = PathBuf::from("target");
    let img_path = target_path.join("termOS.img");

    let bootloader_src = target_path
        .join("x86_64-unknown-uefi/release/bootloader.efi");
    let kernel_src = target_path
        .join("x86_64-unknown-none/release/kernel");

    if !bootloader_src.exists() {
        panic!("Bootloader binary not found at {:?}", bootloader_src);
    }
    if !kernel_src.exists() {
        panic!("Kernel binary not found at {:?}", kernel_src);
    }

    if img_path.exists() {
        fs::remove_file(&img_path).unwrap();
    }
    
    let status = Command::new("dd")
        .args(["if=/dev/zero", "of=target/termOS.img", "bs=1M", "count=64"])
        .output()
        .expect("Failed to execute dd");
    check_status(status.status, "dd");

    let status = Command::new("mkfs.fat")
        .args(["-F", "32", "-n", "TERMOS", "target/termOS.img"])
        .output()
        .expect("Failed to execute mkfs.fat");
    check_status(status.status, "mkfs.fat");

    let status = Command::new("mmd")
        .args(["-i", "target/termOS.img", "::/EFI", "::/EFI/BOOT"])
        .output()
        .expect("Failed to execute mmd");
    check_status(status.status, "mmd");

    let status = Command::new("mcopy")
        .args([
            "-i", "target/termOS.img",
            bootloader_src.to_str().unwrap(),
            "::/EFI/BOOT/BOOTX64.EFI"
        ])
        .output()
        .expect("Failed to execute mcopy bootloader");
    check_status(status.status, "mcopy bootloader");

    let status = Command::new("mcopy")
        .args([
            "-i", "target/termOS.img",
            kernel_src.to_str().unwrap(),
            "::/kernel.elf"
        ])
        .output()
        .expect("Failed to execute mcopy kernel");
    check_status(status.status, "mcopy kernel");
    
    println!("✅ Disk Image created successfully!");
}

fn run() {
    println!("📺 Launching QEMU...");

    let ovmf_code = if PathBuf::from("/usr/share/OVMF/OVMF_CODE.fd").exists() {
        "/usr/share/OVMF/OVMF_CODE.fd"
    } else {
        "/usr/share/edk2/ovmf/OVMF_CODE.fd"
    };

    let mut cmd = Command::new("qemu-system-x86_64");
    cmd.args([
        "-drive", &format!("if=pflash,format=raw,readonly=on,file={}", ovmf_code),
        "-drive", "format=raw,file=target/termOS.img",
        "-serial", "stdio",
    ]);

    let mut child = cmd.spawn().expect("Failed to start QEMU");
    child.wait().unwrap();
}

fn check_status(status: ExitStatus, name: &str) {
    if !status.success() {
        panic!("Command {} failed", name);
    }
}