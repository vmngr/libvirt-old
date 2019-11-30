import chalk from "chalk";
import process from "process";

import libvirt from "../";

(async () => {

    const uri = "qemu+ssh://root@libvirt_dev/system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    // Connecting to our hypervisor
    await hypervisor.connectOpen();

    const activeDomains = await hypervisor.connectListAllDomains(
        libvirt.ConnectListAllDomainsFlags.ACTIVE);

    if (activeDomains.length === 0) {
        process.stdout.write("No domains for reboot :( \n\n");
        return;
    }

    for (const activeDomain of activeDomains) {
        const domainName = await hypervisor.domainGetName(activeDomain);

        process.stdout.write(`Rebooting domain: `
            + `${chalk.blue(domainName)} ... \n\n`);

        await hypervisor.domainReboot(activeDomain).then(() => {
            process.stdout.write(`domain ${chalk.green(domainName)} `
                + "is being to be rebooted!\n\n");
        }).catch((err: Error) => {
            process.stderr.write(`domain ${chalk.red(domainName)} reboot `
                + `ERROR: ${err}\n\n`);
        });
    }

})();
