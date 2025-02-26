#![no_main]
#![no_std]

mod intel;

extern crate alloc;

use log::info;
use uefi::prelude::*;
use core::arch::asm;
use core::arch::x86_64::__cpuid_count;
use alloc::vec::Vec; 
use alloc::boxed::Box;
use alloc::string::String; 
use uefi::proto::console::text::ScanCode;
use uefi::Char16;
use uefi::system::with_stdin;
use alloc::collections::BTreeMap;
use uefi::{CString16, print, println};

use crate::intel::{query_features_intel, test_offcore_pmc};

use x86::msr;

#[derive(Clone, Copy, Debug)]
#[repr(u32)]
pub enum Fn8000_0001_ECX_Feature {
    Reserved31 = 0x1 << 31,
    AddrMaskEXT = 0x1 << 30,
    MonitorX = 0x1 << 29,
    PerfCtrExtLLC = 0x1 << 28,
    PerfTSC = 0x1 << 27,
    DataBkptExt = 0x1 << 26,
    Reserved25 = 0x1 << 25,
    PerfCtrExtNb = 0x1 << 24,
    PerfCtrExtCore = 0x1 << 23,
    TopologyExtensions = 0x1 << 22,
    Tbm = 0x1 << 21,
    Reserved20 = 0x1 << 20,
    Reserved19 = 0x1 << 19,
    Reserved18 = 0x1 << 18,
    Tce = 0x1 << 17,
    Fma4 = 0x1 << 16,
    Lwp = 0x1 << 15,
    Reserved14 = 0x1 << 14,
    Wdt = 0x1 << 13,
    Skinit = 0x1 << 12,
    Xop = 0x1 << 11,
    Ibs = 0x1 << 10,
    Osvw = 0x1 << 9,
    DNowPrefetch = 0x1 << 8,
    MisalignSse = 0x1 << 7,
    Sse4a = 0x1 << 6,
    Abm = 0x1 << 5,
    AltMovCr8 = 0x1 << 4,
    ExtApicSpace = 0x1 << 3,
    Svm = 0x1 << 2,
    CmpLegacy = 0x1 << 1,
    Lahfsahf = 0x1 << 0,
    Unknown = 0xFF,
}

impl Into<u32> for Fn8000_0001_ECX_Feature {
    fn into(self) -> u32 {
        match self {
            Fn8000_0001_ECX_Feature::Reserved31 => 0x1 << 31,
            Fn8000_0001_ECX_Feature::AddrMaskEXT => 0x1 << 30,
            Fn8000_0001_ECX_Feature::MonitorX => 0x1 << 29,
            Fn8000_0001_ECX_Feature::PerfCtrExtLLC => 0x1 << 28,
            Fn8000_0001_ECX_Feature::PerfTSC => 0x1 << 27,
            Fn8000_0001_ECX_Feature::DataBkptExt => 0x1 << 26,
            Fn8000_0001_ECX_Feature::Reserved25 => 0x1 << 25,
            Fn8000_0001_ECX_Feature::PerfCtrExtNb => 0x1 << 24,
            Fn8000_0001_ECX_Feature::PerfCtrExtCore => 0x1 << 23,
            Fn8000_0001_ECX_Feature::TopologyExtensions => 0x1 << 22,
            Fn8000_0001_ECX_Feature::Tbm => 0x1 << 21,
            Fn8000_0001_ECX_Feature::Reserved20 => 0x1 << 20,
            Fn8000_0001_ECX_Feature::Reserved19 => 0x1 << 19,
            Fn8000_0001_ECX_Feature::Reserved18 => 0x1 << 18,
            Fn8000_0001_ECX_Feature::Tce => 0x1 << 17,
            Fn8000_0001_ECX_Feature::Fma4 => 0x1 << 16,
            Fn8000_0001_ECX_Feature::Lwp => 0x1 << 15,
            Fn8000_0001_ECX_Feature::Reserved14 => 0x1 << 14,
            Fn8000_0001_ECX_Feature::Wdt => 0x1 << 13,
            Fn8000_0001_ECX_Feature::Skinit => 0x1 << 12,
            Fn8000_0001_ECX_Feature::Xop => 0x1 << 11,
            Fn8000_0001_ECX_Feature::Ibs => 0x1 << 10,
            Fn8000_0001_ECX_Feature::Osvw => 0x1 << 9,
            Fn8000_0001_ECX_Feature::DNowPrefetch => 0x1 << 8,
            Fn8000_0001_ECX_Feature::MisalignSse => 0x1 << 7,
            Fn8000_0001_ECX_Feature::Sse4a => 0x1 << 6,
            Fn8000_0001_ECX_Feature::Abm => 0x1 << 5,
            Fn8000_0001_ECX_Feature::AltMovCr8 => 0x1 << 4,
            Fn8000_0001_ECX_Feature::ExtApicSpace => 0x1 << 3,
            Fn8000_0001_ECX_Feature::Svm => 0x1 << 2,
            Fn8000_0001_ECX_Feature::CmpLegacy => 0x1 << 1,
            Fn8000_0001_ECX_Feature::Lahfsahf => 0x1 << 0,
            Fn8000_0001_ECX_Feature::Unknown => 0xFF,
        } 
    }
}

fn get_vendor() {
    let vendor_string = unsafe {
        let mut cpuid_result = __cpuid_count(0x0, 0x0);
        let ebx_bytes = cpuid_result.ebx.to_le_bytes();
        let edx_bytes = cpuid_result.edx.to_le_bytes();
        let ecx_bytes = cpuid_result.ecx.to_le_bytes();
        let bytes : Vec<char> = ebx_bytes.iter()
            .chain(edx_bytes.iter().chain(ecx_bytes.iter()))
            .map(|c| {char::from_u32(u32::from(*c)).expect("Conversion from byte should work")} )
            .collect();
        String::from_iter(bytes.iter())
    };
    info!("Got vendor string {:?}", vendor_string);
    match vendor_string.as_str() {
        "AuthenticAMD" => {
            info!("Identified AMD.");
        },
        "GenuineIntel" => {
            info!("Identified Intel");
            query_features_intel();
        },
        &_ => {
            todo!("Found a not supported vendor string!");  
        },
    }
}
fn query_features_amd() {
    let cpuid_result = unsafe{ __cpuid_count(0x8000_0001, 0x0)};
    info!("CPUID Result: {:#x?}, PerfCtrExtLLC: {:#x?}", cpuid_result, Fn8000_0001_ECX_Feature::PerfCtrExtLLC as u32);
    info!("CPUID LLC extensions: {:#x?}", cpuid_result.ecx & (Fn8000_0001_ECX_Feature::PerfCtrExtLLC as u32));
}


fn print_help() {
    info!("Supported command:");
    info!("help     -- Prints this help.");
    info!("query    -- Queries CPUID for information about MPC.");
    info!("quit     -- Terminate this UEFI app.");
    info!("vendor   -- Queries CPUID for vendor information.");
    info!("pmcTest  -- Test if PMC can be programmed with offcore events.");
}

#[entry]
fn main() -> Status {
    uefi::helpers::init().unwrap();

    // let mut lineBuffer = Vec::new();

    let mut commands: BTreeMap<CString16, Box<dyn Fn()>> = BTreeMap::new();
    commands.insert(CString16::try_from("less").unwrap(), Box::new(||{info!("less: Not implemented")}));
    commands.insert(CString16::try_from("query").unwrap(), Box::new(query_features_intel));
    commands.insert(CString16::try_from("help").unwrap(), Box::new(print_help));
    commands.insert(CString16::try_from("vendor").unwrap(), Box::new(get_vendor));
    commands.insert(CString16::try_from("pmcTest").unwrap(), Box::new(test_offcore_pmc));
    

    // CPU ID Name
    info!("Welcome!");
    info!("Type \"help\" to show valid commands, \"quit\" to terminate!");
    
    let mut line: CString16 = CString16::new();
    while line != CString16::try_from("quit").expect("Should work") {
        line = CString16::new();
        let mut hit_enter = false;
        print!("# ");
        while false == hit_enter {
            let key = with_stdin(| mut input | { input.read_key().expect("Expected input") });
            match key {
                Some(k) =>  {
                    match k {
                        uefi::proto::console::text::Key::Printable(p) => {
                            print!("{}", p);
                            if p == Char16::try_from('\r').expect("Unable to convert the ESC ascii code to Char16") {
                                hit_enter = true;
                                println!("");
                            } else {
                                line.push(p.into());
                            }
                        }
                        uefi::proto::console::text::Key::Special(s) => {
                            info!( "A special key was entered: {:?}\r\n", s);
                            if s == ScanCode::ESCAPE {
                                hit_enter = true;
                            }
                        }
                    };             
                },
                None => { 
                }
            };
        };
        match commands.get(&line) {
            Some(func) => func(),
            None =>{
                info!("Unknown command! {:?}", line);
                info!("Type \"help\" to print commands");  
            },
        };

    }

    info!("Terminating...");
    
    while true {}
    boot::stall(10_000_000);
    Status::SUCCESS
}

// fn cpuid (_function: u32) {
//     let mut eax: u32;
//     let mut ebx: u32;
//     let mut ecx: u32;
//     let mut edx: u32;
//     eax = 0x0;// function;
//     unsafe {
//         asm!(
//             "mov eax, {0}",
//             "cpuid",
//             inout("eax") eax,
//             out("ebx") ebx,
//             out("ecx") ecx,
//             out("edx") edx,
//         );
//     }
//     info!("EAX={}, EBX={}, ECX={}, EDX={}", eax, ebx, ecx, edx);
// }