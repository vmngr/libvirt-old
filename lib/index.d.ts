export interface IHypervisorOptions {
    uri: string;
}

export class Hypervisor {

    constructor(options: IHypervisorOptions);

    public connectOpen(): Promise<void>;
    public connectClose(): Promise<void>;
    public connectListAllDomains(
        flags: ConnectListAllDomainsFlags): Promise<Domain[]>;
    public connectListDomains(): Promise<number[]>;
    public connectListDefinedDomains(): Promise<string[]>;

    public domainCreateXML(xml: string): Promise<Domain>;
    public domainDefineXML(xml: string): Promise<Domain>;
    public domainGetInfo(domain: Domain): Promise<DomainInfo>;
    public domainGetID(domain: Domain): Promise<number>;
    public domainGetName(domain: Domain): Promise<string>;
    public domainGetUUIDString(domain: Domain): Promise<string>;
    public domainLookupByID(id: number): Promise<Domain>;
    public domainLookupByName(name: string): Promise<Domain>;
    public domainLookupByUUIDString(uuid: string): Promise<Domain>;
    public domainSave(domain: Domain, filename: string): Promise<void>;
    public domainRestore(filename: string): Promise<void>;

}

export const enum ConnectListAllDomainsFlags {
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

export class Domain {}

export const enum DomainState {
    NOSTATE     = 0,
    RUNNING     = 1,
    BLOCKED     = 2,
    PAUSED      = 3,
    SHUTDOWN    = 4,
    SHUTOFF     = 5,
    CRASHED     = 6,
    PMSUSPENDED = 7,
}

export interface DomainInfo {
    state: DomainState;
    maxMem: number;
    memory: number;
    nrVirtCpu: number;
    cpuTime: number;
}
