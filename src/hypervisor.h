#ifndef __LIBVIRT_HYPERVISOR_H
#define __LIBVIRT_HYPERVISOR_H

#include <string>

#include <napi.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <libvirt/libvirt-domain.h>

class ConnectOpenWorker;
class ConnectCloseWorker;
class ConnectListAllDomainsWorker;
class ConnectListDomainsWorker;
class ConnectListDefinedDomainsWorker;
class ConnectGetMaxVcpusWorker;
class ConnectGetHostnameWorker;

class DomainCreateXMLWorker;
class DomainDefineXMLWorker;
class DomainGetInfoWorker;
class DomainGetIDWorker;
class DomainGetNameWorker;
class DomainGetUUIDStringWorker;
class DomainLookupByIDWorker;
class DomainLookupByNameWorker;
class DomainLookupByUUIDStringWorker;
class DomainSaveWorker;
class DomainRestoreWorker;
class DomainCreateWorker;
class DomainShutdownWorker;

class NodeGetInfoWorker;

class Hypervisor : public Napi::ObjectWrap<Hypervisor>
{
public:

    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Hypervisor(const Napi::CallbackInfo& info);

    Napi::Value ConnectOpen(const Napi::CallbackInfo& info);
    Napi::Value ConnectClose(const Napi::CallbackInfo& info);
    Napi::Value ConnectListAllDomains(const Napi::CallbackInfo& info);
    Napi::Value ConnectListDomains(const Napi::CallbackInfo& info);
    Napi::Value ConnectListDefinedDomains(const Napi::CallbackInfo& info);
    Napi::Value ConnectGetMaxVcpus(const Napi::CallbackInfo& info);
    Napi::Value ConnectGetHostname(const Napi::CallbackInfo& info);

    Napi::Value DomainCreateXML(const Napi::CallbackInfo& info);
    Napi::Value DomainDefineXML(const Napi::CallbackInfo& info);
    Napi::Value DomainGetInfo(const Napi::CallbackInfo& info);
    Napi::Value DomainGetID(const Napi::CallbackInfo& info);
    Napi::Value DomainGetName(const Napi::CallbackInfo& info);
    Napi::Value DomainGetUUIDString(const Napi::CallbackInfo& info);
    Napi::Value DomainLookupByID(const Napi::CallbackInfo& info);
    Napi::Value DomainLookupByName(const Napi::CallbackInfo& info);
    Napi::Value DomainLookupByUUIDString(const Napi::CallbackInfo& info);
    Napi::Value DomainSave(const Napi::CallbackInfo& info);
    Napi::Value DomainCreate(const Napi::CallbackInfo& info);
    Napi::Value DomainShutdown(const Napi::CallbackInfo& info);
    Napi::Value DomainRestore(const Napi::CallbackInfo& info);

    Napi::Value NodeGetInfo(const Napi::CallbackInfo& info);

private:

    static Napi::FunctionReference constructor;

    std::string uri;
    virConnectPtr conn = NULL;

    friend class ConnectOpenWorker;
    friend class ConnectCloseWorker;
    friend class ConnectListAllDomainsWorker;
    friend class ConnectListDomainsWorker;
    friend class ConnectListDefinedDomainsWorker;
    friend class ConnectGetMaxVcpusWorker;
    friend class ConnectGetHostnameWorker;

    friend class DomainCreateXMLWorker;
    friend class DomainDefineXMLWorker;
    friend class DomainGetInfoWorker;
    friend class DomainGetIDWorker;
    friend class DomainGetNameWorker;
    friend class DomainGetUUIDStringWorker;
    friend class DomainLookupByIDWorker;
    friend class DomainLookupByNameWorker;
    friend class DomainLookupByUUIDStringWorker;
    friend class DomainRestoreWorker;
    friend class DomainCreateWorker;
    friend class DomainShutdown;

    friend class NodeGetInfoWorker;

};

#endif /* __LIBVIRT_HYPERVISOR_H */
