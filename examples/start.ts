import chalk from "chalk";
import process from "process";

import libvirt from "../";

(async () => {

    const uri = process.env.LIBVIRT_URI || "qemu:///system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    // Connecting to our hypervisor
    await hypervisor.connectOpen();

    const inactiveDomains = await hypervisor.connectListAllDomains(
        libvirt.ConnectListAllDomainsFlags.INACTIVE);

    if (inactiveDomains.length === 0) {
        process.stdout.write("No domains to start :( \n\n");
        return;
    }

    for (const inactiveDomain of inactiveDomains) {
        const domainName = await hypervisor.domainGetName(inactiveDomain);

        process.stdout.write(`Starting domain: ${chalk.blue(domainName)} ... \n\n`);

        await hypervisor.domainCreate(inactiveDomain).then(() => {
            process.stdout.write(
                `domain ${chalk.green(domainName)} has been started!\n\n`);
        }).catch((err: Error) => {
            process.stderr.write(
                `domain ${chalk.red(domainName)} shutdown ERROR: ${err} \n\n`);
        });
    }

})();
