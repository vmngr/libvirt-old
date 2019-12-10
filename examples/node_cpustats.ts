import chalk from "chalk";
import process from "process";
import _ from "lodash";
import bytes from "bytes";

import libvirt, { Domain, prettifyCpuUsage } from "../";

(async () => {
    const vmName = "debian9-vm1", // VM name
          ifName = "vnet0";        // VM Interface Name

    const uri = "qemu+ssh://root@libvirt_dev/system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    await hypervisor.connectOpen();

    const hostname = await hypervisor.connectGetHostname();
    process.stdout.write(`Connected to ${hostname}!\n\n`);

    const prev = await hypervisor.nodeCpuStats();
    await new Promise(resolve => setTimeout(resolve, 1000));
    const current = await hypervisor.nodeCpuStats();
    const usage = prettifyCpuUsage(prev, current);
    console.log(usage);
})();
