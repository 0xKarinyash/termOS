use std::env;
use std::path::PathBuf;

fn main() {
    let linker_script = "linker.ld";
    let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let script_path = manifest_dir.join(linker_script);

    if !script_path.exists() {
        panic!("Linker script not found at: {}", script_path.display());
    }

    println!("cargo:rustc-link-arg=-T{}", script_path.display());
    println!("cargo:rustc-link-arg=-no-pie");
    println!("cargo:rustc-link-arg=-zmax-page-size=0x1000");
    println!("cargo:rerun-if-changed={}", script_path.display());

    let entry_asm_path = "src/arch/entry.asm";
    let interrupts_asm_path = "src/arch/interrupts.asm";
    
    println!("cargo:rerun-if-changed={}", entry_asm_path);
    println!("cargo:rerun-if-changed={}", interrupts_asm_path);

    let entry_build_res = nasm_rs::Build::new()
        .file(entry_asm_path)
        .compile("asm_entry");

    entry_build_res.expect("Failed to build entry.asm!");
    
    let interrupts_build_res = nasm_rs::Build::new()
        .file(interrupts_asm_path)
        .compile("asm_interrupts");

    interrupts_build_res.expect("Failed to build interrupts.asm!");

    println!("cargo:rustc-link-lib=static=asm_entry");
    println!("cargo:rustc-link-lib=static=asm_interrupts");
}