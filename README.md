# termOS

<p align='left'>
    <img src='assets/logo.png'>
</p>

> "Just a hobby, won't be big and professional like Linux."  
> — *Linus Torvalds (and now me)*

**termOS** is a 64-bit, UNIX-hating, bespoke operating system written from scratch in C.

Current Kernel: **Dewar** (v0.5.x)

## Philosophy

- **Zero Bloat:** We don't port libraries; we write them.
- **Custom Everything:** Why use ELF when you can invent **HOT!**? Why use GRUB when you can write your own bootloader (soon)?

## Features (v0.5.2)

- **Architecture:** x86_64 / UEFI.
- **Memory Management:** PMM (Bitmap), VMM (PML4 + Higher Half Direct Map), Kernel Heap.
- **Multitasking:** Preemptive scheduler with Round Robin.
- **Isolation:** Ring 0 (Kernel) / Ring 3 (Userspace) protection.
- **Binaries:** Custom **HOT!** executable format (parsed via custom `elf2hot` toolchain).
- **Filesystem:** VFS abstraction with CPIO Initramfs support.
- **Graphics:** `ShitGUI` (yes, really) linear framebuffer driver.
- **Shell:** `ksh` (Kernel Shell) -> transitioning to userspace `ush`

## 🔥 The HOT! Format

termOS uses its own executable format called **HOT!**.
Because parsing ELF headers is too mainstream. HOT! is a flat, segment-based format designed specifically for the Dewar kernel loader.
Made only because ELF is *Not-invented-here*

## Roadmap

See [ROADMAP.md](ROADMAP.md) for the detailed plan of world domination (or at least self-hosting).

## License

**GPL-3.0-or-later**.