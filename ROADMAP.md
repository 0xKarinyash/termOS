# termOS Roadmap

## Legend
- ✅ **Done**: Implemented and working.
- 🚧 **WIP**: Currently under active development.
- 📅 **Planned**: Scheduled for future releases.
- 🤔 **Vision**: Long-term goals for v1.0.

---

## Current Status: v0.5.3 (Alpha)
*Where we are now.*
- **Core:** Ring 0 / Ring 3 isolation implemented.
- **Mem:** PMM (bitmap), VMM (PML4, HHDM).
- **Exec:** Custom **HOT!** executable format (static loader).
- **FS:** VFS + Initramfs (CPIO, read-only).
- **User:** Basic jump to userspace (hardcoded stack, no heap yet).

---

## 📅 Phase 1
*Goal: Transform the decorative userspace into a functional mechanism.*

### v0.5.4 (The Shell Update)
*Focus: Interactive Userspace*
- [x] **Userspace Shell (`termosh`)**
    - Porting `ksh` logic into a standalone `.hot` binary.
    - Standard I/O abstraction (stdin/stdout) to pass keyboard input to active process.
- [ ] ~~**Basic Utils** (`ls`, `cat`, `echo`)~~ *(Moved to v0.6.1 — awaiting Apple-style API)*


---

## 📅 Phase 2
*Goal: Total independence from external libraries and architectural hygiene.*

### v0.6.0 (Architecture Overhaul)
- [ ] **System-wide Naming & API Refactoring (Apple-style)**
    - Replace generic types with PascalCase branding (`u64` -> `UInt64`, `i32` -> `Int32`).
    - Standardize system prefixes: `VM` for Virtual Memory, `OS` for Core, `CPU` for HAL.
    - Transition from `snake_case` to `PascalCase` for all kernel functions and structures.
    - Implement `OSReturn` status codes for unified error handling.
- [ ] **Bootloader Independence**
    - Remove `posix-uefi` library.
    - Write custom UEFI entry point (pure PE).
    - Kernel itself becomes a valid `.hot` executable.
- [ ] **HOT! Format Hardening**
    - Fix segment alignment (Page Alignment) in `elf2hot` and kernel loader.
    - Add metadata headers (stack size, permissions).
- [ ] **Concurrency Fixes**
    - Implement spinlocks (`spinlock`).
    - Protect allocator and process lists from interrupt race conditions.

---

## 📅 Phase 3
*Goal: Prepare the environment for self-hosting. We cannot compile on a Read-Only FS.*

### v0.7.0 (Filesystem & Input)
- [ ] **Write Support**
    - Storage driver (NVMe/AHCI or Writable RAMDisk).
    - Writable Filesystem (FAT32 or custom simple FS).
- [ ] **Input Subsystem**
    - Input buffering, Pipe (`|`) support in shell.
    - Basic TTY/PTY abstraction.

### v0.8.0 (The Editor)
- [ ] **Text Editor**
    - Write a simple editor (nano-like) in userspace.
    - Capabilities: Open, Edit, Save files.

---

## 📅 Phase 4
*Goal: Self-Hosting.*

### v0.9.0 (The Toolchain)
- [ ] **Linker (`hld`)**
    - Native linker capable of outputting `.hot` files (running inside termOS).
- [ ] **Compiler Port**
    - Port **TCC (Tiny C Compiler)** to termOS.
    - Adapt libc headers for the OS environment.

### 🤔 v1.0.0 (Self-Hosting)
- [ ] **Bootstrapping**
    - Compile the Dewar kernel *inside* termOS.
    - Boot from the newly compiled kernel.
    - Total victory.

---

## 📜 History

| Version | Date | Key Features |
| :--- | :--- | :--- |
| **v0.5.3** | 2026-01-30 | `sys_mem`, `sys_spawn`, `sys_wait`, `sys_exit` |
| **v0.5.2** | 2026-01-30 | **HOT!** format, Ring 3 isolation, VFS, CPIO parsing. |
| **v0.5.1** | 2026-01-29 | Initial userspace jumps, syscall mechanics. |
| **v0.5.0** | 2026-01-29 | Kernel officially named "Dewar". Versioning standardized. |
| **v0.4.x** | 2026-01-28 | Basic VFS, Shell improvements, basic multitasking. |
| **v0.3.x** | 2025-12-30 | Scheduler, ~~Composer~~, Heap manager. |
| **v0.2.x** | 2025-12-28 | `ksh`, Keyboard driver, HHDM, PMM/VMM. |
| **v0.1.x** | 2025-12-21 | First boot, GDT/IDT, Serial/Console, Panic handler. |
