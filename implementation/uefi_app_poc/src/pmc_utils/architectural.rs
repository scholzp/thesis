use x86::msr::{
	rdmsr, wrmsr,
	IA32_PERFEVTSEL0, IA32_A_PMC0,
	IA32_PERFEVTSEL1, IA32_A_PMC1,
	IA32_PERFEVTSEL2, IA32_A_PMC2,
	IA32_PERFEVTSEL3, IA32_A_PMC3,
};

use log::info;

/// USR bit in PERFEVTSEL. When set, counter is incremented when logical core is
/// in privilege level 1,2 or 3.index
const  IA32_PERFEVTSEL_USR: u64 = 0x1 << 16;
/// OS bit of PERFEVTSEL. When set, counter is incremented when logical core is
/// in privilege level 0.
const  IA32_PERFEVTSEL_OS: u64 = 0x1 << 17;
/// E bit in PERFEVTSEL. Enables (when set) edge detection of the selected 
/// microarchitectural condition.
const  IA32_PERFEVTSEL_E: u64 = 0x1 << 18;
/// PC bit in PERFEVTSEL. Not supported since Sandy Bridge (Core 2xxx). When set
/// processor toggles PMi pins and increments the PMC. When clear, processor 
/// toggles PMi pins on counter overflow
const  IA32_PERFEVTSEL_PC: u64 = 0x1 << 19;
/// When set, the logical processor generates an exception through its local
/// APIC on counter overflow
const  IA32_PERFEVTSEL_INT: u64 = 0x1 << 20;
/// When set the corresponding PMC counts the event. When clear, the counting 
/// stops and the corresponding PMC can be written
const  IA32_PERFEVTSEL_EN: u64 = 0x1 << 22;
/// Invert flag. Inverts counter mask when set.
const  IA32_PERFEVTSEL_INV: u64 = 0x1 << 23;

/*
*  IA32_PERFEVTSELx MSRs:
*  | reserved | cmask | flags | UMASK | EventSelect |
*   63      32 31   24 23   16 15    8 7           0
*/
// First operand of or is event selection, second is UMASK
/// PMC Event for Skylake that counts all requests that miss L2
const EVENT_SKYLAKE_L2_REQUEST_MISS: u64 = 0x24_u64 | 0x3f_u64 << 8;
/// Counts th number of Cache lines filling the L2 cache
const EVENT_SKYLAKE_L2_LINES_IN_ALL: u64 = 0xf1_u64 | 0x1f_u64 << 8;
/// Predefined events that counts references to on-die LLC
const EVENT_PREDEFINED_LLC_REFERENCES: u64 = 0x2e_u64 | 0x4f_u64 << 8;


struct ArchitecturalEventCounter {
	pmc_index: u8,
	event_config: u64,
}

impl Default for ArchitecturalEventCounter {
	fn default() -> Self {
		Self {
			pmc_index: 0x0_u8,
			event_config: 0x0_u64,
		}
	} 
}

impl ArchitecturalEventCounter {
	/// Creates new ArchitecturalEventCounter with given id.
	/// 
	/// A processor can implement multiple architectural PMC registers. In this 
	/// case they are denoted IA32_PMCx with corresponding IA32_PERFEVTSELx in
	/// the Intel SDM .
	/// 
	/// * `index`	- Index of the MSR_OFFCORE_RSP to use
	pub fn new(index: u8) -> Self {
		Self {
			pmc_index: index,
			event_config: 0x0_u64,
		}
	}

	/// Updates the configuration stored in this struct.
	/// 
	/// This does not automatically write to the respective IA32_PERFEVTSELx.
	/// 
	/// * `event_config`- Bitvector to use for later operations
	pub fn set_configuration(&mut self, event_config: u64) {
		self.event_config = event_config;
	}

	/// Sets index.
	/// 
	/// * `x`- Index of the IA32_PMCx to use
	pub fn set_index(&mut self, x: u8) {
		self.pmc_index = x;
	}

	/// Initialize and activate the counter facility.
	/// 
	/// Write the configuration to the MSR_OFFCORE_RSP and activate the 
	/// respective GP PMC to count events using this configuration. Reset the 
	/// counter to the given value.
	/// 
	/// 
	/// * `init_v`: Value to reset the counter to
	pub fn activate_counter(&self, init_v: u64) {
		/* To activate a PMC, we need to do the following things:
		*  1) Stop IA32_PMCx.
		*  2) Configure the IA32_PERFEVTSELx with the behavior we wish for
		*  3) Event and UMASK in IA32_PERFEVTSELx are chosen so that the 
		*     PMC uses the configuration from MSR_OFFCORE_RSPx
		*  4) Initialize IA32_PMCx (do we increment, do we decrement...?)
		*  5) Start the counter by setting the bit in IA32_PERFEVTSELx
		*/
		match self.pmc_index {
			0 => {
				// We operate on PMC0 and IA32_PERFEVTSEL0
				Self::init_and_conf_pmc(
					IA32_PERFEVTSEL0, 
					IA32_A_PMC0, 
					init_v,
					self.event_config
				);
			},
			1 => {
				// We operate on PMC1 and IA32_PERFEVTSEL1
				Self::init_and_conf_pmc(
					IA32_PERFEVTSEL1,
					IA32_A_PMC1,
					init_v,
					self.event_config
				);
			},
			2 => {
				// We operate on PMC2 and IA32_PERFEVTSEL2
				Self::init_and_conf_pmc(
					IA32_PERFEVTSEL2, 
					IA32_A_PMC2, 
					init_v,
					self.event_config
				);
				
			},
			3 => {
				// We operate on PMC3 and IA32_PERFEVTSEL3
				Self::init_and_conf_pmc(
					IA32_PERFEVTSEL3, 
					IA32_A_PMC3, 
					init_v,
					self.event_config
				);
				
			},
			_ => {
				info!("No CPU known to implement 4 or more GP PMCs!");
				return;  //TODO: We want, at some point, return an error
			},
		}
	}

	fn init_and_conf_pmc(perfevtsel_register: u32, pmc_register: u32, init_v: u64, perfsel_content: u64) {
		unsafe { 
			// Cancel any running performance measurements
			wrmsr(perfevtsel_register, 0x0_u64);
			// Reset the counter to zero
			wrmsr(pmc_register, init_v);
			// MSR_OFFCOREx was configured before
			// Activate the counter
			wrmsr(perfevtsel_register, perfsel_content | IA32_PERFEVTSEL_EN);
		}
	}

	pub fn read_pcm_val(&self) -> u64 {
		match self.pmc_index {
			0 => unsafe { rdmsr(IA32_A_PMC0) },
			1 => unsafe { rdmsr(IA32_A_PMC1) },
			2 => unsafe { rdmsr(IA32_A_PMC2) },
			3 => unsafe { rdmsr(IA32_A_PMC3) },
			_ => {
				info!("No CPU known to implement 4 or more GP PMCs!");
				//return;  //TODO: We want, at some point, return an error
				0
			},
		}
	}
}

pub fn architectural_test_setup() {
	let mut pmc_1 = ArchitecturalEventCounter::new(1);
	let mut pmc_2 = ArchitecturalEventCounter::new(2);
	let mut pmc_3 = ArchitecturalEventCounter::new(3);

	pmc_1.set_configuration(EVENT_SKYLAKE_L2_REQUEST_MISS | IA32_PERFEVTSEL_OS | IA32_PERFEVTSEL_USR);
	pmc_2.set_configuration(EVENT_SKYLAKE_L2_LINES_IN_ALL | IA32_PERFEVTSEL_OS | IA32_PERFEVTSEL_USR);
	pmc_3.set_configuration(EVENT_PREDEFINED_LLC_REFERENCES | IA32_PERFEVTSEL_OS | IA32_PERFEVTSEL_USR);

	pmc_1.activate_counter(0);
	pmc_2.activate_counter(0);
	pmc_3.activate_counter(0);
}


pub fn read_and_print_pmcs() {
	let pmc_0 = ArchitecturalEventCounter::new(0);
	let pmc_1 = ArchitecturalEventCounter::new(1);
	let pmc_2 = ArchitecturalEventCounter::new(2);
	let pmc_3 = ArchitecturalEventCounter::new(3);

	info!("IA_PMC0 (OFFCORE)  = {:#016x?}", pmc_0.read_pcm_val());
	info!("IA_PMC1 (L2 Req M) = {:#016x?}", pmc_1.read_pcm_val());
	info!("IA_PMC2 (L2 Fills) = {:#016x?}", pmc_2.read_pcm_val());
	info!("IA_PMC3 (LLC Refs) = {:#016x?}", pmc_3.read_pcm_val());
}