use goblin::elf::program_header::PT_LOAD;
use std::env;
use std::fs::File;
use std::io::{Read, Write};

#[repr(C, packed)]
#[derive(Default, Debug)]
struct HotHeader {
    magic: u32,
    version: u8,
    reserved_pad: [u8; 3],
    entry_point: u64,
    segments_count: u64,
    reserved: u64,
}

#[repr(C, packed)]
#[derive(Default, Debug, Clone, Copy)]
struct HotSegment {
    stype: u64,
    vaddr: u64,
    offset: u64, 
    filesz: u64,
    memsz: u64,
}

const HOT_MAGIC: u32 = 0x21544F48; // "HOT!"

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = env::args().collect();
    if args.len() < 3 {
        println!("Usage: elf2hot <input_elf> <output_hot>");
        return Ok(());
    }

    let input_path = &args[1];
    let output_path = &args[2];

    let mut f = File::open(input_path)?;
    let mut buffer = Vec::new();
    f.read_to_end(&mut buffer)?;

    let elf = goblin::elf::Elf::parse(&buffer)?;
    
    let mut hot_segments = Vec::new();
    let mut segment_data = Vec::new();

    let mut current_offset = std::mem::size_of::<HotHeader>() as u64;

    let load_segments: Vec<_> = elf.program_headers.iter()
        .filter(|ph| ph.p_type == PT_LOAD)
        .collect();
    
    current_offset += (load_segments.len() * std::mem::size_of::<HotSegment>()) as u64;

    for ph in load_segments {
        let is_code = ph.is_executable();
        
        let data = &buffer[ph.p_offset as usize..(ph.p_offset + ph.p_filesz) as usize];
        segment_data.push(data);

        hot_segments.push(HotSegment {
            stype: if is_code { 1 } else { 2 },
            vaddr: ph.p_vaddr,
            offset: current_offset,
            filesz: ph.p_filesz,
            memsz: ph.p_memsz,
        });

        current_offset += ph.p_filesz;
    }

    let header = HotHeader {
        magic: HOT_MAGIC,
        version: 1,
        entry_point: elf.entry,
        segments_count: hot_segments.len() as u64,
        ..Default::default()
    };

    let mut out = File::create(output_path)?;

    let header_bytes: [u8; std::mem::size_of::<HotHeader>()] = unsafe { std::mem::transmute(header) };
    out.write_all(&header_bytes)?;

    for seg in hot_segments {
        let seg_bytes: [u8; std::mem::size_of::<HotSegment>()] = unsafe { std::mem::transmute(seg) };
        out.write_all(&seg_bytes)?;
    }

    for data in segment_data {
        out.write_all(data)?;
    }

    println!("Successfully converted {} to {}!", input_path, output_path);
    println!("Entry point: 0x{:X}", elf.entry);
    Ok(())
}