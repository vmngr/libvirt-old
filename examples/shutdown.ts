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

    for (let activeDomain of activeDomains){
        const domainName = await hypervisor.domainGetName(activeDomain);
        
        console.log(`Shutting down domain: ${domainName}`);

        await hypervisor.domainShutdown(activeDomain).then(()=>{
            console.log(`Domain ${domainName} shutdown success!`)
        }).catch((err: any)=>{
            console.error(`Domain ${domainName} shutdown ERROR:`, err);
        })
    }

})()
