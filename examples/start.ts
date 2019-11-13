import chalk from "chalk";
import process from "process";

import * as libvirt from "../";

(async () => {

    const uri = "qemu:///system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    // Connecting to our hypervisor
    await hypervisor.connectOpen();

    const inactiveDomains = await hypervisor.connectListAllDomains(
        libvirt.ConnectListAllDomainsFlags.INACTIVE);

    if (inactiveDomains.length === 0) {
        process.stdout.write("No domains to start :(");
        return;
    }

    for (const inactiveDomain of inactiveDomains) {
        const domainName = await hypervisor.domainGetName(inactiveDomain);

        process.stdout.write(`Starting domain: ${chalk.blue(domainName)}`);

        await hypervisor.domainCreate(inactiveDomain).then(() => {
            process.stdout.write(
                `Domain ${chalk.green(domainName)} has been started!`);
        }).catch((err: any) => {
            process.stderr.write(
                `Domain ${chalk.red(domainName)} shutdown ERROR:`, err);
        });
    }

})();
