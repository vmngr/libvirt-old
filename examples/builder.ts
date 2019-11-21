import { domainDescToXml, DomainDesc, DomainBuilder } from "../dist";

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

const domain = new DomainBuilder()
    .fromTemplate(template)

    .setName("test1")
    .setUUID("148d0864-2354-4c27-b82c-731bdd3f320c")

    .addDisk({
        type: "file", device: "disk",
        driver: { name: "qemu", type: "qcow2" },
        source: { file: "/home/leon/test1.img" },
        target: { dev: "vda", bus: "virtio" },
    })

    .addInterface({
        type: "network",
        source: { network: "default" },
        mac: { address: "52:54:00:8e:c6:5f" },
        model: { type: "virtio" },
    })

    .build();

const xml = domainDescToXml(domain);
console.log(xml);
