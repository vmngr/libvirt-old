import chalk from "chalk";
import process from "process";

import libvirt, { ConnectListAllDomainsFlags } from "../";

import { Parser, Builder } from 'xml2js'

(async () => {

    const uri = "qemu+ssh://root@libvirt_dev/system";
    const hypervisor = new libvirt.Hypervisor({ uri });

    await hypervisor.connectOpen();

    const hostname = await hypervisor.connectGetHostname();
    process.stdout.write(`Connected to ${hostname}!\n\n`);

    const debianDomain = await hypervisor.domainLookupByName('debian9-vm1');
    
    const domainXML = await hypervisor.domainGetXMLDesc(debianDomain);

    const xmlParser = new Parser({ headless: true, explicitRoot: false, rootName: 'domain'});
    const xmlBuilder = new Builder({ headless: true, explicitRoot: false, rootName: "domain"});

    const domainObj = await xmlParser.parseStringPromise(domainXML);

    let domainVNCIndex = domainObj.devices[0].graphics.findIndex((graphics:any) => graphics.$.type === 'vnc');

    const newVNCPort = parseInt(domainObj.devices[0].graphics[domainVNCIndex].$.port) + 1
    domainObj.devices[0].graphics[domainVNCIndex].$.port = String(newVNCPort);


    const domainChangeVNCXML = xmlBuilder.buildObject(domainObj);

    await hypervisor.domainShutdown(debianDomain).then(()=>{
        console.log("DOMAIN SHUTDOWN OK");
    }).catch(err => {
        console.log("shutdown err:", err);
    });
    await new Promise(resolve => setTimeout(resolve, 5000));
    
    await hypervisor.domainDefineXML(domainChangeVNCXML).then(()=>{
        console.log("DOMAIN DEFINE OK")
    });

    await hypervisor.domainCreate(debianDomain).then(()=>{
        console.log("DOMAIN START OK");
        console.log(`New VNC port : ${newVNCPort}`)
    });
    
    await hypervisor.connectClose();

})();
