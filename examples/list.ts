import chalk from "chalk";
import process from "process";

import * as libvirt from "../";

(async () => {

    const uri = "qemu:///system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    await hypervisor.connectOpen();

    const hostname = await hypervisor.connectGetHostname();
    process.stdout.write(`Connected to ${hostname}!\n\n`);

    const activeDomains = await hypervisor.connectListAllDomains(
        libvirt.ConnectListAllDomainsFlags.ACTIVE);
    const inactiveDomains = await hypervisor.connectListAllDomains(
        libvirt.ConnectListAllDomainsFlags.INACTIVE);

    const activeDomainNames = await Promise.all(activeDomains
        .map((domain) => hypervisor.domainGetName(domain)));

    const inactiveDomainNames = await Promise.all(inactiveDomains
        .map((domain) => hypervisor.domainGetName(domain)));
    await hypervisor.connectClose();

    process.stdout.write("Active Domains\n");
    process.stdout.write("--------------\n");
    for (const name of activeDomainNames) {
        process.stdout.write(chalk.green(`  ${name}\n`));
    }
    process.stdout.write("\n");

    process.stdout.write("Inactive Domains\n");
    process.stdout.write("-----------------\n");
    for (const name of inactiveDomainNames) {
        process.stdout.write(chalk.red(`  ${name}\n`));
    }
    process.stdout.write("\n");

})();
