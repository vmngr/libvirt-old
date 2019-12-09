/**
 * Copyright 2019 Leon Rinkel <leon@rinkel.me> and vmngr/libvirt contributers.
 * 
 * This file is part of the vmngr/libvirt project and is subject to the MIT
 * license as in the LICENSE file in the project root.
 */
#include "src/hypervisor.h"
#include <glib-2.0/glib.h>
#include "src/worker.h"
#include "src/domain.h"

static Napi::Value dummyCallback(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
}

/******************************************************************************
 * DomainCreateXML                                                            *
 ******************************************************************************/

class DomainCreateXMLWorker : public Worker {
 public:
    DomainCreateXMLWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        std::string domainXml)
        : Worker(callback, deferred, hypervisor), domainXml(domainXml) {}

    void Execute(void) override {
        domainPtr = virDomainCreateXML(hypervisor->conn, domainXml.c_str(), 0);
        if (!domainPtr) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object domain = Domain::constructor.New({
            Napi::External<virDomain>::New(Env(), domainPtr) });

        deferred.Resolve(domain);
        Callback().Call({});
    }

 private:
    std::string domainXml;
    virDomainPtr domainPtr;
};

Napi::Value Hypervisor::DomainCreateXML(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsString()) {
        deferred.Reject(Napi::String::New(env, "Expected a string"));
        return deferred.Promise();
    }

    Napi::String domainXml = info[0].As<Napi::String>();

    DomainCreateXMLWorker* worker =
        new DomainCreateXMLWorker(callback, deferred, this, domainXml);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainDefineXML                                                            *
 ******************************************************************************/

class DomainDefineXMLWorker : public Worker {
 public:
    DomainDefineXMLWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        std::string domainXml)
        : Worker(callback, deferred, hypervisor), domainXml(domainXml) {}

    void Execute(void) override {
        domainPtr = virDomainDefineXML(hypervisor->conn, domainXml.c_str());
        if (!domainPtr) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object domain = Domain::constructor.New({
            Napi::External<virDomain>::New(Env(), domainPtr) });

        deferred.Resolve(domain);
        Callback().Call({});
    }

 private:
    std::string domainXml;
    virDomainPtr domainPtr;
};

Napi::Value Hypervisor::DomainDefineXML(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsString()) {
        deferred.Reject(Napi::String::New(env, "Expected a string"));
        return deferred.Promise();
    }

    Napi::String domainXml = info[0].As<Napi::String>();

    DomainDefineXMLWorker* worker =
        new DomainDefineXMLWorker(callback, deferred, this, domainXml);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainGetInfo                                                              *
 ******************************************************************************/

class DomainGetInfoWorker : public Worker {
 public:
    DomainGetInfoWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain)
        : Worker(callback, deferred, hypervisor), domain(domain) {}

    void Execute(void) override {
        int ret = virDomainGetInfo(domain->domainPtr, &domainInfo);
        if (ret < 0) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object info = Napi::Object::New(Env());
        info.Set("state", Napi::Number::New(Env(), domainInfo.state));
        info.Set("maxMem", Napi::Number::New(Env(), domainInfo.maxMem));
        info.Set("memory", Napi::Number::New(Env(), domainInfo.memory));
        info.Set("nrVirtCpu", Napi::Number::New(Env(), domainInfo.nrVirtCpu));
        info.Set("cpuTime", Napi::Number::New(Env(), domainInfo.cpuTime));

        deferred.Resolve(info);
        Callback().Call({});
    }

 private:
    Domain* domain;
    virDomainInfo domainInfo;
};

Napi::Value Hypervisor::DomainGetInfo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    DomainGetInfoWorker* worker = new DomainGetInfoWorker(
        callback, deferred, this, domain);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainGetID                                                                *
 ******************************************************************************/

class DomainGetIDWorker : public Worker {
 public:
    DomainGetIDWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain)
        : Worker(callback, deferred, hypervisor), domain(domain) {}

    void Execute(void) override {
        id = virDomainGetID(domain->domainPtr);

        /* Inactive domains dont't have an id, so virDomainGetID will return -1,
         * but not set an error. This will cause SetVirtError to throw an
         * "Unknown error". But in this case it might be better to just return
         * null as id. */

        if (id == static_cast<unsigned int>(-1)) {
            virErrorPtr err = virGetLastError();
            if (err != NULL) SetError(std::string(err->message));
        }
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        // See comment above.
        if (id != static_cast<unsigned int>(-1))
            deferred.Resolve(Napi::Number::New(Env(), this->id));
        else
            deferred.Resolve(Env().Undefined());

        Callback().Call({});
    }

 private:
    Domain* domain;
    unsigned int id;
};

Napi::Value Hypervisor::DomainGetID(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    DomainGetIDWorker* worker = new DomainGetIDWorker(
        callback, deferred, this, domain);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainGetName                                                              *
 ******************************************************************************/

class DomainGetNameWorker : public Worker {
 public:
    DomainGetNameWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain)
        : Worker(callback, deferred, hypervisor), domain(domain) {}

    void Execute(void) override {
        domainName = virDomainGetName(domain->domainPtr);
        if (domainName == NULL) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());
        deferred.Resolve(Napi::String::New(Env(), this->domainName));
        Callback().Call({});
    }

 private:
    Domain* domain;
    const char* domainName;
};

Napi::Value Hypervisor::DomainGetName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    DomainGetNameWorker* worker = new DomainGetNameWorker(
        callback, deferred, this, domain);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainGetUUIDString                                                        *
 ******************************************************************************/

class DomainGetUUIDStringWorker : public Worker {
 public:
    DomainGetUUIDStringWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain)
        : Worker(callback, deferred, hypervisor), domain(domain) {}

    void Execute(void) override {
        int ret = virDomainGetUUIDString(domain->domainPtr, uuid);
        if (ret < 0) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());
        deferred.Resolve(Napi::String::New(Env(), this->uuid,
            VIR_UUID_STRING_BUFLEN - 1));
        Callback().Call({});
    }

 private:
    Domain* domain;
    char uuid[VIR_UUID_STRING_BUFLEN];
};

Napi::Value Hypervisor::DomainGetUUIDString(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    DomainGetUUIDStringWorker* worker = new DomainGetUUIDStringWorker(
        callback, deferred, this, domain);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainLookupByID                                                           *
 ******************************************************************************/

class DomainLookupByIDWorker : public Worker {
 public:
    DomainLookupByIDWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        int id)
        : Worker(callback, deferred, hypervisor), id(id) {}

    void Execute(void) override {
        domainPtr = virDomainLookupByID(hypervisor->conn, id);
        if (domainPtr == NULL) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object domain = Domain::constructor.New({
            Napi::External<virDomain>::New(Env(), domainPtr) });

        deferred.Resolve(domain);
        Callback().Call({});
    }

 private:
    int id;
    virDomainPtr domainPtr;
};

Napi::Value Hypervisor::DomainLookupByID(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsNumber()) {
        deferred.Reject(Napi::String::New(env, "Expected a number."));
        return deferred.Promise();
    }

    int id = info[0].As<Napi::Number>();

    DomainLookupByIDWorker* worker = new DomainLookupByIDWorker(
        callback, deferred, this, id);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainLookupByName                                                         *
 ******************************************************************************/

class DomainLookupByNameWorker : public Worker {
 public:
    DomainLookupByNameWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        std::string name)
        : Worker(callback, deferred, hypervisor), name(name) {}

    void Execute(void) override {
        domainPtr = virDomainLookupByName(hypervisor->conn, name.c_str());
        if (domainPtr == NULL) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object domain = Domain::constructor.New({
            Napi::External<virDomain>::New(Env(), domainPtr) });

        deferred.Resolve(domain);
        Callback().Call({});
    }

 private:
    std::string name;
    virDomainPtr domainPtr;
};

Napi::Value Hypervisor::DomainLookupByName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsString()) {
        deferred.Reject(Napi::String::New(env, "Expected a string."));
        return deferred.Promise();
    }

    std::string name = info[0].As<Napi::String>();

    DomainLookupByNameWorker* worker = new DomainLookupByNameWorker(
        callback, deferred, this, name);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainLookupByUUIDString                                                   *
 ******************************************************************************/

class DomainLookupByUUIDStringWorker : public Worker {
 public:
    DomainLookupByUUIDStringWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        std::string uuid)
        : Worker(callback, deferred, hypervisor), uuid(uuid) {}

    void Execute(void) override {
        domainPtr = virDomainLookupByUUIDString(hypervisor->conn, uuid.c_str());
        if (domainPtr == NULL) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object domain = Domain::constructor.New({
            Napi::External<virDomain>::New(Env(), domainPtr) });

        deferred.Resolve(domain);
        Callback().Call({});
    }

 private:
    std::string uuid;
    virDomainPtr domainPtr;
};

Napi::Value Hypervisor::DomainLookupByUUIDString(
    const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsString()) {
        deferred.Reject(Napi::String::New(env, "Expected a string."));
        return deferred.Promise();
    }

    std::string uuid = info[0].As<Napi::String>();

    DomainLookupByUUIDStringWorker* worker = new DomainLookupByUUIDStringWorker(
        callback, deferred, this, uuid);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainSave                                                                 *
 ******************************************************************************/

class DomainSaveWorker : public Worker {
 public:
    DomainSaveWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain,
        std::string filename)
        : Worker(callback, deferred, hypervisor), domain(domain),
          filename(filename) {}

    void Execute(void) override {
        int ret = virDomainSave(domain->domainPtr, filename.c_str());
        if (ret < 0) SetVirError();
    }

 private:
    Domain* domain;
    std::string filename;
};

Napi::Value Hypervisor::DomainSave(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }
    if (info.Length() <= 1 || !info[1].IsString()) {
        deferred.Reject(Napi::String::New(env, "Expected a string."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());
    std::string filename = info[1].As<Napi::String>();

    DomainSaveWorker* worker = new DomainSaveWorker(
        callback, deferred, this, domain, filename);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainRestore                                                              *
 ******************************************************************************/

class DomainRestoreWorker : public Worker {
 public:
    DomainRestoreWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        std::string filename)
        : Worker(callback, deferred, hypervisor), filename(filename) {}

    void Execute(void) override {
        int ret = virDomainRestore(hypervisor->conn, filename.c_str());
        if (ret < 0) SetVirError();
    }

 private:
    std::string filename;
};

Napi::Value Hypervisor::DomainRestore(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsString()) {
        deferred.Reject(Napi::String::New(env, "Expected a string."));
        return deferred.Promise();
    }

    std::string filename = info[0].As<Napi::String>();

    DomainRestoreWorker* worker = new DomainRestoreWorker(
        callback, deferred, this, filename);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainCreate                                                               *
 ******************************************************************************/

class DomainCreateWorker : public Worker {
 public:
    DomainCreateWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain)
        : Worker(callback, deferred, hypervisor), domain(domain) {}

    void Execute(void) override {
        int create = virDomainCreate(domain->domainPtr);
        if (create < 0) SetVirError();
    }

 private:
    Domain* domain;
};

Napi::Value Hypervisor::DomainCreate(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    DomainCreateWorker* worker = new DomainCreateWorker(
        callback, deferred, this, domain);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainShutDown                                                             *
 ******************************************************************************/

class DomainShutdownWorker : public Worker {
 public:
    DomainShutdownWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain)
        : Worker(callback, deferred, hypervisor), domain(domain) {}

    void Execute(void) override {
        int shutdown = virDomainShutdown(domain->domainPtr);
        if (shutdown < 0) SetVirError();
    }

 private:
    Domain* domain;
};

Napi::Value Hypervisor::DomainShutdown(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    DomainShutdownWorker* worker = new DomainShutdownWorker(
        callback, deferred, this, domain);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainReboot                                                            *
 ******************************************************************************/

class DomainRebootWorker : public Worker {
 public:
    DomainRebootWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain,
        unsigned int flags)
        : Worker(callback, deferred, hypervisor),
          domain(domain), flags(flags) {}

    void Execute(void) override {
        int reboot = virDomainReboot(domain->domainPtr, flags);
        if (reboot < 0) SetVirError();
    }

 private:
    Domain* domain;
    unsigned int flags;
};

Napi::Value Hypervisor::DomainReboot(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    unsigned int flags = 0;

    DomainRebootWorker* worker = new DomainRebootWorker(
        callback, deferred, this, domain, flags);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainGetXMLDesc                                                           *
 ******************************************************************************/

class DomainGetXMLDescWorker : public Worker {
 public:
    DomainGetXMLDescWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain,
        unsigned int flags)
        : Worker(callback, deferred, hypervisor), domain(domain),
          flags(flags) {}

    void Execute(void) override {
        xmlDesc = virDomainGetXMLDesc(domain->domainPtr, flags);
        if (xmlDesc == NULL) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::String xmlDescStr = Napi::String::New(Env(), xmlDesc);
        free(xmlDesc);

        deferred.Resolve(xmlDescStr);
        Callback().Call({});
    }

 private:
    Domain* domain;
    unsigned int flags;
    char* xmlDesc;
};

Napi::Value Hypervisor::DomainGetXMLDesc(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    unsigned int flags = 0;

    if (info.Length() >= 2 && info[1].IsNumber()) {
        flags = info[1].As<Napi::Number>();
    } else if (info.Length() >= 2 && !info[1].IsNumber()) {
        deferred.Reject(Napi::String::New(env, "Expected a number."));
        return deferred.Promise();
    }

    DomainGetXMLDescWorker* worker =
        new DomainGetXMLDescWorker(callback, deferred, this, domain, flags);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainInterfaceStats                                                       *
 ******************************************************************************/
class DomainInterfaceStatsWorker : public Worker {
 public:
    DomainInterfaceStatsWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain,
        std::string device)
        : Worker(callback, deferred, hypervisor),
         domain(domain), device(device) {}

    void Execute(void) override {
        int ret = virDomainInterfaceStats(domain->domainPtr,
                    device.c_str(), &ifstats, sizeof(ifstats));
        if (ret < 0) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object info = Napi::Object::New(Env());

        info.Set("rx_bytes", Napi::Number::New(Env(), ifstats.rx_bytes));
        info.Set("rx_packets", Napi::Number::New(Env(), ifstats.rx_packets));
        info.Set("rx_errs", Napi::Number::New(Env(), ifstats.rx_errs));
        info.Set("rx_drop", Napi::Number::New(Env(), ifstats.rx_drop));
        info.Set("tx_bytes", Napi::Number::New(Env(), ifstats.tx_bytes));
        info.Set("tx_packets", Napi::Number::New(Env(), ifstats.tx_packets));
        info.Set("tx_errs", Napi::Number::New(Env(), ifstats.tx_errs));
        info.Set("tx_drop", Napi::Number::New(Env(), ifstats.tx_drop));

        deferred.Resolve(info);
        Callback().Call({});
    }

 private:
    Domain* domain;
    std::string device;
    virDomainInterfaceStatsStruct ifstats;
};

Napi::Value Hypervisor::DomainInterfaceStats(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    if (info.Length() == 1) {
        deferred.Reject(Napi::String::New(env, "Expected a domain ptr."));
        return deferred.Promise();
    } else if (info.Length() == 2 && !info[1].IsString()) {
        deferred.Reject(Napi::String::New(env,
        "Expected a interface device name at 2nd arg."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    std::string device = info[1].As<Napi::String>().Utf8Value();

    DomainInterfaceStatsWorker* worker =
        new DomainInterfaceStatsWorker(callback, deferred, this,
        domain, device);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainInterfaceTuneWorker                                                       *
 ******************************************************************************/
class DomainInterfaceTuneWorker : public Worker {
 public:
    DomainInterfaceTuneWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain,
        std::string device,
        virTypedParameterPtr params,
        int nparams,
        unsigned int flags)
        : Worker(callback, deferred, hypervisor),
         domain(domain), device(device), params(params),
         nparams(nparams), flags(flags) {}

    void Execute(void) override {
        int ret = virDomainSetInterfaceParameters(domain->domainPtr,
                  device.c_str(), params, nparams, flags);
        if (ret < 0) SetVirError();
    }

 private:
    Domain* domain;
    std::string device;
    virTypedParameterPtr params;
    int nparams;
    unsigned int flags;
};

Napi::Value Hypervisor::DomainInterfaceTune(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    virTypedParameterPtr params = NULL;
    virNetDevBandwidthRate inbound;
    unsigned int flags = VIR_DOMAIN_AFFECT_CURRENT;
    flags |= VIR_DOMAIN_AFFECT_LIVE;

    int nparams = 0;
    int maxparams = 0;

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    if (info.Length() == 1) {
        deferred.Reject(Napi::String::New(env, "Expected a domain ptr."));
        return deferred.Promise();
    } else if (info.Length() >= 2 && !info[1].IsString()) {
        deferred.Reject(Napi::String::New(env,
        "Expected a device interface name by second arg."));
        return deferred.Promise();
    } else if (info.Length() >= 3 && !info[2].IsObject()) {
        deferred.Reject(Napi::String::New(env,
        "Expected tune object (inbound?, outbound?) by 3rd arg."));
        return deferred.Promise();
    }
    std::string device = info[1].As<Napi::String>().Utf8Value();

    Napi::Object interfaceTune = info[2].As<Napi::Object>();

    if (interfaceTune.Has("inbound") &&
        interfaceTune.Get("inbound").IsObject() ) {
        Napi::Object inboundObj = interfaceTune.Get("inbound")
                                  .As<Napi::Object>();

        inbound.average = inboundObj.Get("average")
                          .As<Napi::Number>().ToNumber().Uint32Value();
        inbound.peak = inboundObj.Get("peak")
                          .As<Napi::Number>().ToNumber().Uint32Value();

        inbound.burst = inboundObj.Get("burst")
                        .As<Napi::Number>().ToNumber().Uint32Value();
        inbound.floor = inboundObj.Get("floor")
                        .As<Napi::Number>().ToNumber().Uint32Value();

        virTypedParamsAddUInt(&params, &nparams,
                          &maxparams, VIR_DOMAIN_BANDWIDTH_IN_AVERAGE,
                          inbound.average);

        virTypedParamsAddUInt(&params, &nparams,
                          &maxparams, VIR_DOMAIN_BANDWIDTH_IN_PEAK,
                          inbound.peak);
        virTypedParamsAddUInt(&params, &nparams,
                          &maxparams, VIR_DOMAIN_BANDWIDTH_IN_BURST,
                          inbound.burst);
        virTypedParamsAddUInt(&params, &nparams,
                          &maxparams, VIR_DOMAIN_BANDWIDTH_IN_FLOOR,
                          inbound.floor);
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    DomainInterfaceTuneWorker* worker =
        new DomainInterfaceTuneWorker(callback, deferred, this,
        domain, device, params, nparams, flags);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainInterfaceTuneCurrentWorker                                           *
 ******************************************************************************/
class DomainInterfaceTuneCurrentWorker : public Worker {
 public:
    DomainInterfaceTuneCurrentWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain,
        std::string device,
        virTypedParameterPtr params,
        int nparams,
        unsigned int flags)
        : Worker(callback, deferred, hypervisor),
         domain(domain), device(device), params(params),
         nparams(nparams), flags(flags) {}

    void Execute(void) override {
        int ret = virDomainGetInterfaceParameters(domain->domainPtr,
                          device.c_str(), NULL, &nparams, flags);
        if (ret != 0) {
            throw Napi::Error::New(Env(),
                "Unable to get number of interface parameters");
        }

        // params = new virTypedParameterPtr;
        params = (virTypedParameterPtr) g_malloc0_n(nparams, sizeof(*params));

        int reret = virDomainGetInterfaceParameters(domain->domainPtr,
                 device.c_str(), params, &nparams, flags);
        if (reret != 0){
            SetVirError();
        }
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object currentTune = Napi::Object::New(Env());
        currentTune.Set("inbound", Napi::Object::New(Env()));
        currentTune.Set("outbound", Napi::Object::New(Env()));

        for (int i = 0; i < nparams; i++) {
            char *token;
            std::string search = ".";
            char *rest = params[i].field;

            token = strtok_r(params[i].field, search.c_str(), &rest);
            std::string bound = std::string(token);
            token = strtok_r(NULL, search.c_str(), &rest);
            std::string type = std::string(token);
            // No more things to tokenize
            Napi::Object boundObject = currentTune.Get(bound)
                                       .As<Napi::Object>();
            char *str = getTypedParamValue(&params[i]);
            Napi::Number tuneValue = Napi::Number::New(Env(), atof(str));

            boundObject.Set(std::string(type), tuneValue);
        }

        deferred.Resolve(currentTune);
        Callback().Call({});
    }

 private:
    Domain* domain;
    std::string device;
    virTypedParameterPtr params;
    int nparams;
    unsigned int flags;
};

Napi::Value Hypervisor::DomainInterfaceTuneCurrent
            (const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    virTypedParameterPtr params = NULL;
    unsigned int flags = VIR_DOMAIN_AFFECT_CURRENT;

    int nparams = 0;

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    if (info.Length() == 1) {
        deferred.Reject(Napi::String::New(env,
                        "Expected a device name by 2nd arg."));

        return deferred.Promise();
    } else if (info.Length() >= 2 && !info[1].IsString()) {
        deferred.Reject(Napi::String::New(env,
        "Expected a device interface name by second arg."));
        return deferred.Promise();
    }

    std::string device = info[1].As<Napi::String>().Utf8Value();


    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    DomainInterfaceTuneCurrentWorker* worker =
        new DomainInterfaceTuneCurrentWorker(callback, deferred, this,
        domain, device, params, nparams, flags);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * DomainMemoryStats                                                          *
 ******************************************************************************/
class DomainMemoryStatsWorker : public Worker {
 public:
    DomainMemoryStatsWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        Domain* domain,
        unsigned int flags)
        : Worker(callback, deferred, hypervisor),
         domain(domain), flags(flags) {}

    void Execute(void) override {
        int ret = virDomainSetMemoryStatsPeriod(domain->domainPtr, 1, flags);
        if (ret < 0 ) SetVirError();

        nr_stats = virDomainMemoryStats(domain->domainPtr, memstats,
                    VIR_DOMAIN_MEMORY_STAT_NR, 0);
        if (nr_stats < 0) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object info = Napi::Object::New(Env());
        unsigned long long actual = 0;
        unsigned long long available = 0;
        for (int i = 0; i < nr_stats; i++) {
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_SWAP_IN)
                info.Set("swap_in", Napi::Number::New(Env(), memstats[i].val));
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_SWAP_OUT)
                info.Set("swap_out", Napi::Number::New(Env(), memstats[i].val));
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_MAJOR_FAULT)
                info.Set("major_fault", Napi::Number::New(Env(), memstats[i].val));
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_MINOR_FAULT)
                info.Set("minor_fault", Napi::Number::New(Env(), memstats[i].val));
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_UNUSED)
                info.Set("unused", Napi::Number::New(Env(), memstats[i].val));
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_AVAILABLE) {
                info.Set("available", Napi::Number::New(Env(), memstats[i].val));
                available = memstats[i].val;
            }
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_USABLE)
                info.Set("usable", Napi::Number::New(Env(), memstats[i].val));
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_ACTUAL_BALLOON) {
                info.Set("actual", Napi::Number::New(Env(), memstats[i].val));
                actual = memstats[i].val;
            }
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_RSS)
                info.Set("rss", Napi::Number::New(Env(), memstats[i].val));
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_LAST_UPDATE)
                info.Set("last_update", Napi::Number::New(Env(), memstats[i].val));
            if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_DISK_CACHES)
                info.Set("disk_caches", Napi::Number::New(Env(), memstats[i].val));
            // if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_HUGETLB_PGALLOC)
            //     info.Set("hugetlb_pgalloc", Napi::Number::New(Env(), memstats[i].val));
            // if (memstats[i].tag == VIR_DOMAIN_MEMORY_STAT_HUGETLB_PGFAIL)
            //     info.Set("hugetlb_pgfail", Napi::Number::New(Env(), memstats[i].val));
        }
        
        info.Set("used", Napi::Number::New(Env(), actual - available));
        deferred.Resolve(info);
        Callback().Call({});
    }

 private:
    Domain* domain;
    unsigned int flags;
    int nr_stats;
    virDomainMemoryStatStruct memstats[VIR_DOMAIN_MEMORY_STAT_NR];
};

Napi::Value Hypervisor::DomainMemoryStats(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        deferred.Reject(Napi::String::New(env, "Expected an object."));
        return deferred.Promise();
    }

    if (info.Length() < 1) {
        deferred.Reject(Napi::String::New(env, "Expected a domain ptr."));
        return deferred.Promise();
    } else if (info.Length() > 1) {
        deferred.Reject(Napi::String::New(env, "Method doesn't require more args."));
        return deferred.Promise();
    }

    Domain* domain = Napi::ObjectWrap<Domain>::Unwrap(
        info[0].As<Napi::Object>());

    unsigned int flags = VIR_DOMAIN_AFFECT_CURRENT;

    DomainMemoryStatsWorker* worker =
        new DomainMemoryStatsWorker(callback, deferred, this, domain, flags);
    worker->Queue();

    return deferred.Promise();
}