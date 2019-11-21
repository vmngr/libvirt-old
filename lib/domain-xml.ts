/**
 * Copyright 2019 Leon Rinkel <leon@rinkel.me> and vmngr/libvirt contributers.
 *
 * This file is part of the vmngr/libvirt project and is subject to the MIT
 * license as in the LICENSE file in the project root.
 *
 * @brief Contains helper functions to serialize domain descriptions to XML.
 * @todo Add functions to load and parse XML to domain descriptions.
 */

import xml2js from "xml2js";

import {
    DomainDesc,
    DomainOsDesc,
    DomainDiskDesc,
    DomainInterfaceDesc,
    DomainGraphicsDesc,
} from "./domain-desc";

/**
 * @todo Not complete nor 100% correct.
 */
function serializeDomainOs(osDesc: DomainOsDesc): { } {
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
}

/**
 * @todo Not complete nor 100% correct.
 */
function serializeDomainDisk(diskDesc: DomainDiskDesc): { } {
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
}

/**
 * @todo Not complete nor 100% correct.
 */
function serializeDomainInterface(interfaceDesc: DomainInterfaceDesc): { } {
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
}

/**
 * @todo Not complete nor 100% correct.
 */
function serializeDomainGraphics(graphicsDesc: DomainGraphicsDesc): { } {
    // tslint:disable-next-line:no-any
    const graphics: any = { $: { } };

    if (graphicsDesc.type) graphics.$.type = graphicsDesc.type;
    if (graphicsDesc.port) graphics.$.port = graphicsDesc.port;
    if (graphicsDesc.listen) graphics.$.listen = graphicsDesc.listen;
    if (graphicsDesc.passwd) graphics.$.passwd = graphicsDesc.passwd;

    return graphics;
}

/**
 * @todo Not complete nor 100% correct.
 */
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

    if (desc.os) domain.os = serializeDomainOs(desc.os);

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
                        serializeDomainDisk(deviceDesc.disk));
                    break;

                case "interface":
                    domain.devices.interface.push(
                        serializeDomainInterface(deviceDesc.interface));
                    break;

                case "console":
                    const consoleDesc = deviceDesc.console;
                    if (consoleDesc.type) device.$.type = consoleDesc.type;
                    domain.devices.console.push(device);
                    break;

                case "graphics":
                    domain.devices.graphics.push(
                        serializeDomainGraphics(deviceDesc.graphics));
                    break;

                default:
            }
        }
    }

    const builder = new xml2js.Builder();
    return builder.buildObject({ domain });

}
