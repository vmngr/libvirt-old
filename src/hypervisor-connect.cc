#include "hypervisor.h"

#include "worker.h"
#include "domain.h"

static void dummyErrorFunc(void* userData, virErrorPtr err) {}

static Napi::Value dummyCallback(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
}

/******************************************************************************
 * ConnectOpen                                                                *
 ******************************************************************************/

class ConnectOpenWorker : public Worker {
public:

    using Worker::Worker;

    void Execute(void) override
    {
        virSetErrorFunc(NULL, dummyErrorFunc);
        hypervisor->conn = virConnectOpen(hypervisor->uri.c_str());
        if (hypervisor->conn == NULL) SetVirError();
    }

};

Napi::Value Hypervisor::ConnectOpen(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    ConnectOpenWorker* worker = new ConnectOpenWorker(callback, deferred, this);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * ConnectClose                                                               *
 ******************************************************************************/

class ConnectCloseWorker : public Worker {
public:

    using Worker::Worker;

    void Execute(void) override
    {
        virConnectClose(hypervisor->conn);
    }

};

Napi::Value Hypervisor::ConnectClose(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    ConnectCloseWorker* worker =
        new ConnectCloseWorker(callback, deferred, this);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * ConnectListAllDomains                                                      *
 ******************************************************************************/

class ConnectListAllDomainsWorker : public Worker {
public:

    ConnectListAllDomainsWorker(
        Napi::Function& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        unsigned int flags
    ) : Worker(callback, deferred, hypervisor),
        flags(flags) {}

    void Execute(void) override
    {
        numDomains = virConnectListAllDomains(
            hypervisor->conn, &domainPtrs, flags);
        if (numDomains < 0) SetVirError();
    }

    void OnOK(void) override
    {
        Napi::HandleScope scope(Env());

        Napi::Array domains = Napi::Array::New(Env()); 
        for (int i = 0; i < numDomains; i++)
        {
            Napi::Object domain = Domain::constructor.New({
                Napi::External<virDomain>::New(Env(), domainPtrs[i]) });
            domains.Set(i, domain);
        }
        free(domainPtrs);

        deferred.Resolve(domains);
        Callback().Call({});
    }

private:

    unsigned int flags;
    virDomainPtr* domainPtrs;
    int numDomains;

};

Napi::Value Hypervisor::ConnectListAllDomains(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    unsigned int flags = 0;

    if (info.Length() >= 1 && info[0].IsNumber())
        flags = info[0].As<Napi::Number>();
    else if (info.Length() >= 1 && !info[0].IsNumber())
    {
        deferred.Reject(Napi::String::New(env, "Expected a number."));
        return deferred.Promise();
    }

    ConnectListAllDomainsWorker* worker =
        new ConnectListAllDomainsWorker(callback, deferred, this, flags);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * ConnectListDomains                                                         *
 ******************************************************************************/

class ConnectListDomainsWorker : public Worker {
public:

    using Worker::Worker;

    void Execute(void) override
    {
        numActiveDomains = virConnectNumOfDomains(hypervisor->conn);
        activeDomainIds = (int*) malloc(sizeof(int) * numActiveDomains);
        numActiveDomains = virConnectListDomains(hypervisor->conn,
            activeDomainIds, numActiveDomains);
    }

    void OnOK(void) override
    {
        Napi::HandleScope scope(Env());

        Napi::Array activeDomains = Napi::Array::New(Env()); 
        for (int i = 0; i < numActiveDomains; i++)
            activeDomains.Set(i, activeDomainIds[i]);
        free(activeDomainIds);

        deferred.Resolve(activeDomains);
        Callback().Call({});
    }

private:

    int numActiveDomains;
    int* activeDomainIds;

};

Napi::Value Hypervisor::ConnectListDomains(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    ConnectListDomainsWorker* worker =
        new ConnectListDomainsWorker(callback, deferred, this);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * ConnectListDefinedDomains                                                  *
 ******************************************************************************/

class ConnectListDefinedDomainsWorker : public Worker {
public:

    using Worker::Worker;

    void Execute(void) override
    {
        numInactiveDomains =
            virConnectNumOfDefinedDomains(hypervisor->conn);
        inactiveDomainNames = (char**) malloc(
            sizeof(char*) * numInactiveDomains);
        numInactiveDomains = virConnectListDefinedDomains(hypervisor->conn,
            inactiveDomainNames, numInactiveDomains);
    }

    void OnOK(void) override
    {
        Napi::HandleScope scope(Env());

        Napi::Array inactiveDomains = Napi::Array::New(Env()); 
        for (int i = 0; i < numInactiveDomains; i++)
        {
            inactiveDomains.Set(i, inactiveDomainNames[i]);
            free(inactiveDomainNames[i]);
        }
        free(inactiveDomainNames);

        deferred.Resolve(inactiveDomains);
        Callback().Call({});
    }

private:

    int numInactiveDomains;
    char** inactiveDomainNames;

};

Napi::Value Hypervisor::ConnectListDefinedDomains(
    const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    ConnectListDefinedDomainsWorker* worker =
        new ConnectListDefinedDomainsWorker(callback, deferred, this);
    worker->Queue();

    return deferred.Promise();
}
