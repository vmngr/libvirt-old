import * as libvirt from "../";

(async function() {

    const uri = "qemu+ssh://napoles/system?socket=/var/run/libvirt/libvirt-sock";
    const hypervisor = new libvirt.Hypervisor({ uri });

    await hypervisor.connectOpen();

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
    for (const name of activeDomainNames)
        process.stdout.write(`  \x1b[32m${name}\x1b[0m\n`);
    process.stdout.write('\n');

    process.stdout.write("Inactive Domains\n");
    process.stdout.write("-----------------\n");
    for (const name of inactiveDomainNames)
        process.stdout.write(`  \x1b[31m${name}\x1b[0m\n`);
    process.stdout.write('\n');

})();
