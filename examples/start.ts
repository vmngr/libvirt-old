import * as libvirt from "../";
import chalk from "chalk";

(async ()=>{

    const uri = "qemu:///system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    //Connecting to our hypervisor
    await hypervisor.connectOpen();

    const inactiveDomains = await hypervisor.connectListAllDomains(
        libvirt.ConnectListAllDomainsFlags.INACTIVE);

    if (inactiveDomains.length == 0) {
        console.log("No domains for shutdown :(");
        return;
    }

    for (let inactiveDomain of inactiveDomains){
        const domainName = await hypervisor.domainGetName(inactiveDomain);
        
        console.log(`Starting domain: ${chalk.blue(domainName)}`);

        await hypervisor.domainCreate(inactiveDomain).then(()=>{
            console.log(`Domain ${chalk.green(domainName)} has been started!`);
        }).catch((err: any)=>{
            console.error(`Domain ${chalk.red(domainName)} shutdown ERROR:`, err);
        })
    }

})()
