/**
 * Copyright 2019 Leon Rinkel <leon@rinkel.me> and vmngr/libvirt contributers.
 *
 * This file is part of the vmngr/libvirt project and is subject to the MIT
 * license as in the LICENSE file in the project root.
 *
 * @brief Contains helper functions to deserialize / serialize domain
 *  descriptions from / to XML.
 */

import xml2js from "xml2js";

import {
    DomainDesc,
    DomainOsDesc,
    DomainDiskDesc,
    DomainInterfaceDesc,
    DomainGraphicsDesc,
} from "./domain-desc";

export const domainOsXml = {

    serialize(osDesc: DomainOsDesc): { } {
        // tslint:disable-next-line:no-any
        const os: any = { $: { } };

        if (osDesc.type) {
            os.type = { $: { } };
            if (osDesc.type.arch) os.type.$.arch = osDesc.type.arch;
            if (osDesc.type.machine) os.type.$.machine = osDesc.type.machine;
            if (osDesc.type.value) os.type._ = osDesc.type.value;
        }

        if (osDesc.boot) {
            os.boot = { $: { } };
            if (osDesc.boot.dev) os.boot.$.dev = osDesc.boot.dev;
        }

        return os;
    },

    // tslint:disable-next-line:no-any
    deserialize(os: any): DomainOsDesc {
        const osDesc: DomainOsDesc = { };

        if (os.type[0]) {
            osDesc.type = { };
            if (os.type[0].$.arch) {
                osDesc.type.arch = os.type[0].$.arch;
            }
            if (os.type[0].$.machine) {
                osDesc.type.machine = os.type[0].$.machine;
            }
            if (os.type[0]._) osDesc.type.value = os.type[0]._;
        }

        if (os.boot[0]) {
            osDesc.boot = { };
            if (os.boot[0].$.dev) osDesc.boot.dev = os.boot[0].$.dev;
        }

        return osDesc;
    },

};

export const domainDiskXml = {

    serialize(diskDesc: DomainDiskDesc): { } {
        // tslint:disable-next-line:no-any
        const disk: any = { $: { } };

        if (diskDesc.type) disk.$.type = diskDesc.type;
        if (diskDesc.device) disk.$.device = diskDesc.device;

        if (diskDesc.driver) {
            disk.driver = { $: { } };
            if (diskDesc.driver.name) disk.driver.$.name = diskDesc.driver.name;
            if (diskDesc.driver.type) disk.driver.$.type = diskDesc.driver.type;
        }

        if (diskDesc.source) {
            disk.source = { $: { } };
            if (diskDesc.source.file) disk.source.$.file = diskDesc.source.file;
        }

        if (diskDesc.target) {
            disk.target = { $: { } };
            if (diskDesc.target.dev) disk.target.$.dev = diskDesc.target.dev;
            if (diskDesc.target.bus) disk.target.$.bus = diskDesc.target.bus;
        }

        return disk;
    },

    // tslint:disable-next-line:no-any
    deserialize(disk: any): DomainDiskDesc {
        const diskDesc: DomainDiskDesc = { };

        if (disk.$.type) diskDesc.type = disk.$.type;
        if (disk.$.device) diskDesc.device = disk.$.device;

        if (disk.driver[0]) {
            diskDesc.driver = { };
            if (disk.driver[0].$.name) {
                diskDesc.driver.name = disk.driver[0].$.name;
            }
            if (disk.driver[0].$.type) {
                diskDesc.driver.type = disk.driver[0].$.type;
            }
        }

        if (disk.source[0]) {
            diskDesc.source = { };
            if (disk.source[0].$.file) {
                diskDesc.source.file = disk.source[0].$.file;
            }
        }

        if (disk.target[0]) {
            diskDesc.target = { };
            if (disk.target[0].$.dev) {
                diskDesc.target.dev = disk.target[0].$.dev;
            }
            if (disk.target[0].$.bus) {
                diskDesc.target.bus = disk.target[0].$.bus;
            }
        }

        return diskDesc;
    },

};

export const domainInterfaceXml = {

    serialize(interfaceDesc: DomainInterfaceDesc): { } {
        // tslint:disable-next-line:no-any
        const iface: any = { $: { } };

        if (interfaceDesc.type) iface.$.type = interfaceDesc.type;

        if (interfaceDesc.source) {
            iface.source = { $: { } };
            if (interfaceDesc.source.network) {
                iface.source.$.network = interfaceDesc.source.network;
            }
        }

        if (interfaceDesc.mac) {
            iface.mac = { $: { } };
            if (interfaceDesc.mac.address) {
                iface.mac.$.address = interfaceDesc.mac.address;
            }
        }

        if (interfaceDesc.model) {
            iface.model = { $: { } };
            if (interfaceDesc.model.type) {
                iface.model.$.type = interfaceDesc.model.type;
            }
        }

        return iface;
    },

    // tslint:disable-next-line:no-any
    deserialize(iface: any): DomainInterfaceDesc {
        const interfaceDesc: DomainInterfaceDesc = { };

        if (iface.$.type) interfaceDesc.type = iface.$.type;

        if (iface.source[0]) {
            interfaceDesc.source = { };
            if (iface.source[0].$.network) {
                interfaceDesc.source.network = iface.source[0].$.network;
            }
        }

        if (iface.mac[0]) {
            interfaceDesc.mac = { };
            if (iface.mac[0].$.address) {
                interfaceDesc.mac.address = iface.mac[0].$.address;
            }
        }

        if (iface.model[0]) {
            interfaceDesc.model = { };
            if (iface.model[0].$.type) {
                interfaceDesc.model.type = iface.model[0].$.type;
            }
        }

        return interfaceDesc;
    },

};

export const domainGraphicsXml = {

    serialize(graphicsDesc: DomainGraphicsDesc): { } {
        // tslint:disable-next-line:no-any
        const graphics: any = { $: { } };

        if (graphicsDesc.type) graphics.$.type = graphicsDesc.type;
        if (graphicsDesc.port) graphics.$.port = graphicsDesc.port;
        if (graphicsDesc.listen) graphics.$.listen = graphicsDesc.listen;
        if (graphicsDesc.passwd) graphics.$.passwd = graphicsDesc.passwd;

        return graphics;
    },

    // tslint:disable-next-line:no-any
    deserialize(graphics: any): DomainGraphicsDesc {
        const graphicsDesc: DomainGraphicsDesc = { };

        if (graphics.$.type) graphicsDesc.type = graphics.$.type;
        if (graphics.$.port) graphicsDesc.port = Number(graphics.$.port);
        if (graphics.$.listen) graphicsDesc.listen = graphics.$.listen;
        if (graphics.$.passwd) graphicsDesc.passwd = graphics.$.passwd;

        return graphicsDesc;
    },

};

export function domainDescToXml(desc: DomainDesc): string {
    // tslint:disable-next-line:no-any
    const domain: any = { $: { } };

    if (desc.type) domain.$.type = desc.type;
    if (desc.id) domain.$.id = desc.id;

    if (desc.name) domain.name = desc.name;
    if (desc.uuid) domain.uuid = desc.uuid;

    if (desc.memory) {
        domain.memory = { $: { } };
        if (desc.memory.unit) domain.memory.$.unit = desc.memory.unit;
        if (desc.memory.value) domain.memory._ = desc.memory.value;
    }

    if (desc.currentMemory) {
        domain.currentMemory = { $: { } };
        if (desc.currentMemory.unit) {
            domain.currentMemory.$.unit = desc.currentMemory.unit;
        }
        if (desc.currentMemory.value) {
            domain.currentMemory._ = desc.currentMemory.value;
        }
    }

    if (desc.vcpu) {
        domain.vcpu = { $: { } };
        if (desc.vcpu.placement) domain.vcpu.$.placement = desc.vcpu.placement;
        if (desc.vcpu.value) domain.vcpu._ = desc.vcpu.value;
    }

    if (desc.os) domain.os = domainOsXml.serialize(desc.os);

    if (desc.devices) {
        domain.devices = {
            emulator: [ ],
            disk: [ ],
            interface: [ ],
            console: [ ],
            graphics: [ ],
        };

        for (const deviceDesc of desc.devices) {
            // tslint:disable-next-line:no-any
            const device: any = { $: { } };

            switch (deviceDesc.type) {
                case "emulator":
                    const emulatorDesc = deviceDesc.emulator;
                    if (emulatorDesc.value) device._ = emulatorDesc.value;
                    domain.devices.emulator.push(device);
                    break;

                case "disk":
                    domain.devices.disk.push(
                        domainDiskXml.serialize(deviceDesc.disk));
                    break;

                case "interface":
                    domain.devices.interface.push(
                        domainInterfaceXml.serialize(deviceDesc.interface));
                    break;

                case "console":
                    const consoleDesc = deviceDesc.console;
                    if (consoleDesc.type) device.$.type = consoleDesc.type;
                    domain.devices.console.push(device);
                    break;

                case "graphics":
                    domain.devices.graphics.push(
                        domainGraphicsXml.serialize(deviceDesc.graphics));
                    break;

                default:
            }
        }
    }

    const builder = new xml2js.Builder({ headless: true });
    return builder.buildObject({ domain });
}

export async function domainDescFromXml(xml: string): Promise<DomainDesc> {
    const domainDesc: DomainDesc = { };
    const parsed = await xml2js.parseStringPromise(xml);

    if (!parsed.domain) throw new Error("Unable to parse domain xml");

    if (parsed.domain.$.type) domainDesc.type = parsed.domain.$.type;
    if (parsed.domain.$.id) domainDesc.id = parsed.domain.$.id;

    if (parsed.domain.name && parsed.domain.name[0]) {
        domainDesc.name = parsed.domain.name[0];
    }
    if (parsed.domain.uuid && parsed.domain.uuid[0]) {
        domainDesc.uuid = parsed.domain.uuid[0];
    }

    if (parsed.domain.memory && parsed.domain.memory[0]) {
        if (typeof parsed.domain.memory[0] === "object") {
            domainDesc.memory = {
                value: Number(parsed.domain.memory[0]._),
                unit: parsed.domain.memory[0].$.unit,
            };
        } else if (typeof parsed.domain.memory[0] === "string") {
            domainDesc.memory = {
                value: Number(parsed.domain.memory[0])
            };
        }
    }

    if (parsed.domain.currentMemory && parsed.domain.currentMemory[0]) {
        if (typeof parsed.domain.currentMemory[0] === "object") {
            domainDesc.currentMemory = {
                value: Number(parsed.domain.currentMemory[0]._),
                unit: parsed.domain.currentMemory[0].$.unit,
            };
        } else if (typeof parsed.domain.currentMemory[0] === "string") {
            domainDesc.currentMemory = {
                value: Number(parsed.domain.currentMemory[0]),
            };
        }
    }

    if (parsed.domain.vcpu && parsed.domain.vcpu[0]) {
        if (typeof parsed.domain.vcpu[0] === "object") {
            domainDesc.vcpu = {
                value: Number(parsed.domain.vcpu[0]._),
                placement: parsed.domain.vcpu[0].$.placement,
            };
        } else if (typeof parsed.domain.vcpu[0] === "string") {
            domainDesc.vcpu = {
                value: Number(parsed.domain.vcpu[0]),
            };
        }
    }

    if (parsed.domain.os && parsed.domain.os[0]) {
        domainDesc.os = domainOsXml.deserialize(parsed.domain.os[0]);
    }

    if (parsed.domain.devices && parsed.domain.devices[0]) {
        domainDesc.devices = [ ];

        if (parsed.domain.devices[0].emulator) {
            domainDesc.devices.push({
                type: "emulator",
                emulator: {
                    value: parsed.domain.devices[0].emulator[0]
                },
            });
        }

        if (parsed.domain.devices[0].disk) {
            for (const disk of parsed.domain.devices[0].disk) {
                domainDesc.devices.push({
                    type: "disk",
                    disk: domainDiskXml.deserialize(disk),
                });
            }
        }

        if (parsed.domain.devices[0].interface) {
            for (const iface of parsed.domain.devices[0].interface) {
                domainDesc.devices.push({
                    type: "interface",
                    interface: domainInterfaceXml.deserialize(iface),
                });
            }
        }

        if (parsed.domain.devices[0].console) {
            for (const cnsole of parsed.domain.devices[0].console) {
                domainDesc.devices.push({
                    type: "console",
                    console: { type: cnsole.$.type },
                });
            }
        }

        if (parsed.domain.devices[0].graphics) {
            for (const graphics of parsed.domain.devices[0].graphics) {
                domainDesc.devices.push({
                    type: "graphics",
                    graphics: domainGraphicsXml.deserialize(graphics),
                });
            }
        }
    }

    return domainDesc;
}
