# Secure Element: Wissensammlung
Ziel: Ein Secure Element schaffen, das nicht an Hardware gebunden ist. Allem
voran nicht an SGX, SEV, TGX usw. Die Software läuft nur auf lokalem Cache und
ist daher sicher vor Seitenkanalattacken.

Idee: Vom OS wird eine CPU abgekoppelt. Das OS kann dann nicht mehr mit dieser
interagieren, bzw. nur noch über eine definierte Schnittstelle. Auf dieser CPU
läuft dann im lokalen Cache das SE. Durch Performance Counter o.ä. wird 
sichergestellt, bzw. erkannt, wenn Informationen den lokalen Cahce verlassen.
Das verhindert die Übertragung über den Cache-Seitenkanal.

## Existierende Projekte mit ähnlichen Ansatz
Nicht unbedingt SE, aber Systeme, die ihre Ressourcen partitionieren, CPUs einzeln verwalten o.ä.:

- [Popcorn Linux](http://www.popcornlinux.org/index.php/overview)
    * Auf jedem Core läuft ein Linux Kernel
    * Jeder seperat laufende Kernel bietet Services an
    * Ein "primary Kernel" der "secondary kernel" bootet
    * Kernel vernetzt durch ein virtuelles Netzwerk

- [Barrelfish](https://barrelfish.org/documentation.html)
    * [Github](https://github.com/BarrelfishOS/barrelfish) hat seit 4 Jahren kein Update gesehen
    * Ähnlich wie Popcorn Linux, aber mit eigneme Kernel (wird dort CPU driver genannt)
    * Die [Overview-Doku](https://barrelfish.org/publications/TN-000-Overview.pdf) sagt, dass nicht zwischen den Kerneln geshared wird. Dazu zählt auch Cache auf homogenen, cache-koherenten system (vgl. 1.1.).
- Eventuell der Gaming Visor von Basti?
- [CPU Hotplugging](https://docs.kernel.org/core-api/cpu_hotplug.html) in Linux und [Control Groups](https://man7.org/linux/man-pages/man7/cgroups.7.html)
- [fTPM](https://www.usenix.org/system/files/conference/usenixsecurity16/sec16_paper_raj.pdf)
    * Software Implemtierung von TPM 2.0 in der Firmware
- [CaR Projects](https://www.blackhat.com/docs/us-14/materials/us-14-Weis-Protecting-Data-In-Use-From-Firmware-And-Physical-Attacks-WP.pdf)

## In Frage kommende/nützliche(?) Techniken (die man sich anschauen kann)
* Prefetch Instruction in x86
### vCage
* [Stackoverflow, see comments](https://stackoverflow.com/questions/27699197/cache-as-ram-no-fill-mode-executable-code)
### Cache-as-RAM
* Wie stabil und dokumentiert ist es überhaupt? 
* Wie funktioniert es?
* Wie wird der Cache initialisiert/wie macht man der CPU glaubhaft, dass sie sinnvolle Werte im Cache hat ohne initial Cahce Misses?
* Wie verhält sich ein unkonfigurierter DRAM Controller? 
#### Links
* [Stack overflow](https://stackoverflow.com/questions/19591500/how-to-make-sure-a-piece-of-code-never-leaves-the-cpu-cache-l3/24710093#24710093)
* [Coreboot CAR Intel](https://github.com/coreboot/coreboot/tree/main/src/soc/intel/common/block/cpu/car)
* [Coreboot CAR AMD](https://github.com/coreboot/coreboot/tree/main/src/soc/amd/common/block/cpu/car)
* [Intel NEM (no evict mode)](https://community.intel.com/t5/Software-Archive/UnDocumented-MSR-s/td-p/968420)

### ARM MPAM
* Mittels Tags paritionierbare Systemressourcen auf ARM
* Teil von ARM 8.3, optional mit wenigen tatsächlichen Implementierungen
* Ab [ARM Neoverse N2](https://hc33.hotchips.org/assets/program/conference/day1/20210818_Hotchips_NeoverseN2.pdf)
* Micorsoft [Azure Cobalt 100](https://techcommunity.microsoft.com/t5/azure-compute-blog/announcing-the-preview-of-new-azure-vms-based-on-the-azure/ba-p/4146353) nutzt Neoverse Core

#### Links
* [MPAM Docu](https://developer.arm.com/documentation/107768/0100/Overview)
* [Attack on TrustZone](https://eprint.iacr.org/2016/980.pdf)
* [TrueSense; Cache based side channel attack on TrustZone](https://csis.gmu.edu/ksun/publications/trusense-infocom18.pdf)

### TCU
* Ähnlich wie MPAM(?)

#### Links
* [Seit beim Barhausen Institut](https://www.barkhauseninstitut.org/forschung/forschungsbereiche/trustworthy-digital-architectures)

### MSRs in x86
* MSR_EVICT_CTL in Intel
* PQOS Enforcement (PQE) auf AMD kann festlegen, wie der L3 und andere shared resources genutzt werden. CAT (Cache allocation enforcement)(Volume 2, 19.4.2)


### Intel processor trace
* Kann vielleicht genutzt werden
* Kann vielliecht für Malware genutzt werde?