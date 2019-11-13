import chalk from "chalk";
import process from "process";

import * as libvirt from "../";

(async () => {

    const uri = "qemu:///system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    // Connecting to our hypervisor
    await hypervisor.connectOpen();

    const activeDomains = await hypervisor.connectListAllDomains(
        libvirt.ConnectListAllDomainsFlags.ACTIVE);

    if (activeDomains.length === 0) {
        process.stdout.write("No domains for shutdown :(");
        return;
    }

    for (const activeDomain of activeDomains) {
        const domainName = await hypervisor.domainGetName(activeDomain);

        process.stdout.write(`Shutting down domain: `
            + `${chalk.blue(domainName)} ... `);

        await hypervisor.domainShutdown(activeDomain).then(() => {
            process.stdout.write(`domain ${chalk.green(domainName)} `
                + "shutdown success!\n\n");
        }).catch((err: Error) => {
            process.stderr.write(`domain ${chalk.red(domainName)} shutdown `
                + `ERROR: ${err}\n\n`);
        });
    }

})();
