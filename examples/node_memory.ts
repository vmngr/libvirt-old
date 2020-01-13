import chalk from "chalk";
import process from "process";
import util from 'util';

import libvirt from "../";

(async () => {
    const vmName = "debian9-vm1", // VM name
          ifName = "vnet0";        // VM Interface Name

    const uri = "qemu+ssh://root@libvirt_dev/system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    await hypervisor.connectOpen();

    const hostname = await hypervisor.connectGetHostname();
    process.stdout.write(`Connected to ${hostname}!\n\n`);
    console.log(await hypervisor.nodeGetMemoryStats());
})();
