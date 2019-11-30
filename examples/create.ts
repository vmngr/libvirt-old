import chalk from "chalk";
import process from "process";

import libvirt, { Domain } from "../"
import { domainDescToXml, DomainDesc, DomainBuilder } from "../dist";
import uuidv1 from 'uuid/v1';

(async ()=>{
    const uri = "qemu:///system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    // Connecting to our hypervisor
    await hypervisor.connectOpen();

    const builDomainDesc = createDomainDesc("Example VM");

    builDomainDesc.addDisk({
        type: "file", device: "disk",
        driver: { name: "qemu", type: "qcow2" },
        source: { file: "/" }, // Define path of your qcow2 disk
        target: { dev: "vda", bus: "virtio" }
    });

    const domainXML = domainDescToXml(builDomainDesc.build());

    const domain: Domain = await hypervisor.domainDefineXML(domainXML);
    // Our domain has been defined or edited if DomainDesc have existing uuid 
    // Then we can start domain

    const domainName = await hypervisor.domainGetName(domain);

    await hypervisor.domainCreate(domain).then(() => {
        process.stdout.write(`Domain ${chalk.green(domainName)} started \n\n`)
    }).catch((err) => {
        process.stdout.write(`Error on starting ${chalk.red(domainName)} domain  \n\n`);
    })

    await hypervisor.connectClose();

})()

function createDomainDesc(domainName: string){

    const template: DomainDesc = {
    
        type: "kvm",
    
        memory: { value: 1048576 },
        currentMemory: { value: 1048576 },
    
        vcpu: { value: 1 },
    
        os: {
            type: { arch: "x86_64", machine: "q35", value: "hvm" },
            boot: { dev: "hd" },
        },
    
        devices: [
    
            {
                type: "emulator",
                emulator: { value: "/usr/bin/qemu-system-x86_64" },
            },
    
            {
                type: "console",
                console: { type: "pty" },
            },
    
            {
                type: "graphics",
                graphics: {
                    type: "vnc",
                    port: -1,
                    listen: "0.0.0.0",
                    passwd: "test1",
                }
            },
    
        ],
    
    };
    
    const domainUUID = uuidv1();
    const domain = new DomainBuilder()
        .fromTemplate(template)
        .setName(domainName)
        .setUUID(domainUUID)
        .addInterface({
            type: "network",
            source: { network: "default" },
            mac: { address: "52:54:00:8e:c6:5f" },
            model: { type: "virtio" },
        })

    return domain;
}