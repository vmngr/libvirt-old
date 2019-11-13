import * as libvirt from "../";

(async ()=>{

    const uri = "qemu:///system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    //Connecting to our hypervisor
    await hypervisor.connectOpen();

    const activeDomains = await hypervisor.connectListAllDomains(
        libvirt.ConnectListAllDomainsFlags.ACTIVE);

    if (activeDomains.length == 0) {
        console.log("No domains for shutdown :(");
        return;
    }

    for (let inactiveDomain of activeDomains){
        const domainName = await hypervisor.domainGetName(inactiveDomain);
        const domainLookup = await hypervisor.domainLookupByName(domainName);
        
        console.log(`Shutting down domain: ${domainName}`);

        await hypervisor.domainShutdown(inactiveDomain).then(()=>{
            console.log(`Domain ${domainName} shutdown success!`)
        }).catch((err: any)=>{
            console.error(`Domain ${domainName} shutdown ERROR:`, err);
        })
    }

})()