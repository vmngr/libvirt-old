import chalk from "chalk";
import process from "process";
import _ from "lodash";
import bytes from "bytes";
import { prettifyMemoryUsage } from '../dist/domain-utils'

import libvirt, { Domain } from "../";

(async () => {
    const vmName = "debian9-vm1", // VM name
          ifName = "vnet0";        // VM Interface Name

    const uri = "qemu+ssh://root@libvirt_dev/system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    await hypervisor.connectOpen();

    const hostname = await hypervisor.connectGetHostname();
    process.stdout.write(`Connected to ${hostname}!\n\n`);

    const activeDomain = await hypervisor.domainLookupByName(vmName).then(domain => domain).catch(() => null); // Domain not exist or error
    if (!activeDomain){
        console.log(`Error on lookup for ${chalk.blue(vmName)}`);
        return;
    }   

    setInterval(async () => {
        const memoryStats = await hypervisor.domainMemoryStats(activeDomain);
        const pretty = prettifyMemoryUsage(memoryStats);
        console.log(pretty);
        // console.log(memoryStats);

        // Pretty is just an example.
    }, 1000)
})();
