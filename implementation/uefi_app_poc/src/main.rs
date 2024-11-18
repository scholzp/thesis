#![no_main]
#![no_std]

use log::info;
use uefi::prelude::*;
use core::arch::asm;
use core::arch::x86_64::__cpuid_count;

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

#[entry]
fn main() -> Status {
    uefi::helpers::init().unwrap();
    info!("Hello world!");
    // CPU ID Name
    let mut cpuid_result = unsafe{ __cpuid_count(0x0, 0x0) };
    info!("CPUID Result: {:#x?}", cpuid_result);
    cpuid_result = unsafe{ __cpuid_count(0x8000_0001, 0x0) };
    info!("CPUID Result: {:#x?}, PerfCtrExtLLC: {:#x?}", cpuid_result, Fn8000_0001_ECX_Feature::PerfCtrExtLLC as u32);
    info!("CPUID LLC extensiosn: {:#x?}", cpuid_result.ecx & (Fn8000_0001_ECX_Feature::PerfCtrExtLLC as u32));
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