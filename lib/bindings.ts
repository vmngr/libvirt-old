/**
 * Copyright 2019 Leon Rinkel <leon@rinkel.me> and vmngr/libvirt contributers.
 *
 * This file is part of the vmngr/libvirt project and is subject to the MIT
 * license as in the LICENSE file in the project root.
 *
 * @brief Contains actual libvirt bindings and related declarations.
 */

import bindings from "bindings";

// TODO: Check whether it is possible to not use default in this case.
// tslint:disable-next-line:no-default-export
export default bindings("libvirt");

export declare interface HypervisorOptions {
    uri: string;
}

export declare class Hypervisor {

    constructor(options: HypervisorOptions);

    connectOpen(): Promise<void>;
    connectClose(): Promise<void>;
    connectListAllDomains(flags?: ConnectListAllDomainsFlags):
        Promise<Domain[]>;
    connectListDomains(): Promise<number[]>;
    connectListDefinedDomains(): Promise<string[]>;
    connectGetMaxVcpus(type?: string): Promise<number>;
    connectGetHostname(): Promise<string>;

    domainCreateXML(xml: string): Promise<Domain>;
    domainDefineXML(xml: string): Promise<Domain>;
    domainGetInfo(domain: Domain): Promise<DomainInfo>;
    domainGetID(domain: Domain): Promise<number | null>;
    domainGetName(domain: Domain): Promise<string>;
    domainGetUUIDString(domain: Domain): Promise<string>;
    domainLookupByID(id: number): Promise<Domain>;
    domainLookupByName(name: string): Promise<Domain>;
    domainLookupByUUIDString(uuid: string): Promise<Domain>;
    domainSave(domain: Domain, filename: string): Promise<void>;
    domainRestore(filename: string): Promise<void>;
    domainCreate(domain: Domain): Promise<void>;
    domainShutdown(domain: Domain): Promise<void>;
    domainReboot(domain: Domain): Promise<void>;
    domainGetXMLDesc(domain: Domain, flags?: DomainGetXMLDescFlags):
        Promise<string>;
    domainInterfaceStats(domain: Domain, device: string): Promise<DomainInterfaceStatsObject>;
    domainInterfaceTune(domain: Domain, device: string, params: DomainTuneParams): Promise<void>;
    domainInterfaceTuneCurrent(domain:Domain, device: string): Promise<DomainTuneParams>;
    domainMemoryStats(domain: Domain): Promise<any>;


    nodeGetInfo(): Promise<NodeInfo>;
    nodeGetMemoryStats(): Promise<NodeMemoryStatsStruct>;
    nodeCpuStats(percentage?: boolean):Promise<any>;

}

export declare interface NodeMemoryStatsStruct {
    total: number;
    free: number;
    buffers: number;
    cached: number;
}
export declare interface DomainTuneParams {
    inbound?: DomainTuneItem;
    outbound?: DomainTuneItem;
}
export declare interface DomainTuneItem {
    average: number;
    peak: number;
    burst: number;
    floor: number;
}
export declare interface DomainInterfaceStatsObject {
    rx_bytes: number;
    rx_packets: number;
    rx_errs: number;
    rx_drop: number;
    tx_bytes: number;
    tx_packets: number;
    tx_errs: number;
    tx_drop: number;
};

export declare const enum ConnectListAllDomainsFlags {
    ACTIVE         = 1,
    INACTIVE       = 2,
    PERSISTENT     = 4,
    TRANSIENT      = 8,
    RUNNING        = 16,
    PAUSED         = 32,
    SHUTOFF        = 64,
    OTHER          = 128,
    MANAGEDSAVE    = 256,
    NO_MANAGEDSAVE = 512,
    AUTOSTART      = 1024,
    NO_AUTOSTART   = 2048,
    HAS_SNAPSHOT   = 4096,
    NO_SNAPSHOT    = 8192,
    HAS_CHECKPOINT = 16384,
    NO_CHECKPOINT  = 32768,
}

export declare const enum DomainRebootFlagValues {
    VIR_DOMAIN_REBOOT_DEFAULT        = 0,
    VIR_DOMAIN_REBOOT_ACPI_POWER_BTN = 1,
    VIR_DOMAIN_REBOOT_GUEST_AGENT    = 2,
    VIR_DOMAIN_REBOOT_INITCTL	     = 4,
    VIR_DOMAIN_REBOOT_SIGNAL	     = 8,
    VIR_DOMAIN_REBOOT_PARAVIRT	     = 16,
}

export declare const enum DomainGetXMLDescFlags {
    SECURE     = 1,
    INACTIVE   = 2,
    UPDATE_CPU = 4,
    MIGRATABLE = 8,
}

export declare class Domain {}

export declare const enum DomainState {
    NOSTATE     = 0,
    RUNNING     = 1,
    BLOCKED     = 2,
    PAUSED      = 3,
    SHUTDOWN    = 4,
    SHUTOFF     = 5,
    CRASHED     = 6,
    PMSUSPENDED = 7,
}

export declare interface DomainInfo {
    state: DomainState;
    maxMem: number;
    memory: number;
    nrVirtCpu: number;
    cpuTime: number;
}

export declare interface NodeInfo {
    model: string;
    memory: number;
    cpus: number;
    mhz: number;
    nodes: number;
    sockets: number;
    cores: number;
    threads: number;
}
