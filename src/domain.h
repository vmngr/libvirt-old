#ifndef __LIBVIRT_DOMAIN_H
#define __LIBVIRT_DOMAIN_H

#include <napi.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <libvirt/libvirt-domain.h>

#include "hypervisor.h"

class Domain : public Napi::ObjectWrap<Domain>
{
public:

    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Domain(const Napi::CallbackInfo& info);
    ~Domain(void);

private:

    static Napi::FunctionReference constructor;

    virDomainPtr domainPtr = nullptr;

    friend class ConnectListAllDomainsWorker;

    friend class DomainCreateXMLWorker;
    friend class DomainDefineXMLWorker;
    friend class DomainGetInfoWorker;
    friend class DomainGetIDWorker;
    friend class DomainGetNameWorker;
    friend class DomainGetUUIDStringWorker;
    friend class DomainLookupByIDWorker;
    friend class DomainLookupByNameWorker;
    friend class DomainLookupByUUIDStringWorker;
    friend class DomainSaveWorker;
    friend class DomainCreateWorker;
    friend class DomainShutdownWorker;
    friend class DomainRestoreWorker;

};

#endif /* __LIBVIRT_DOMAIN_H */
