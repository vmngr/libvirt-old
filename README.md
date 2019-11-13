![Cover image][cover]

# vmngr / libvirt

![CI badge][cibadge]
![License badge][licensebadge]
![Contributors badge][contribadge]
![Issues badge][issuesbadge]
![PR badge][prbadge]

Libvirt bindings for Node.js®.

## Example Usage

```typescript
import * as libvirt from "@vmngr/libvirt";

const uri = "qemu+ssh:///";
const hypervisor = new libvirt.Hypervisor({ uri });

(async function() {

    await hypervisor.connectOpen();

    const activeDomains = await hypervisor.connectListAllDomains(
            libvirt.ConnectListAllDomainsFlags.ACTIVE);

    const activeDomainNames = await Promise.all(activeDomains
        .map((domain) => hypervisor.domainGetName(domain)));

    console.log(activeDomainNames);

    await hypervisor.connectClose();

})();
```

---

MIT License

Copyright (c) 2019 Leon Rinkel <leon@rinkel.me> and vmngr/libvirt contributers

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


[cover]: cover.png "Cover image"

[licensebadge]: https://img.shields.io/github/license/vmngr/libvirt "License badge"
[cibadge]: https://github.com/vmngr/libvirt/workflows/CI/badge.svg "CI badge"
[contribadge]: https://img.shields.io/github/contributors/vmngr/libvirt "Contributors badge"
[issuesbadge]: https://img.shields.io/github/issues/vmngr/libvirt "Issues badge"
[prbadge]: https://img.shields.io/github/issues-pr/vmngr/libvirt "PR badge"
