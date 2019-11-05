#ifndef __LIBVIRT_WORKER_H
#define __LIBVIRT_WORKER_H

#include <napi.h>

#include "hypervisor.h"

class Worker : public Napi::AsyncWorker {
protected:

    Napi::Promise::Deferred deferred;
    Hypervisor* hypervisor;

    void SetVirError(void);

public:

    Worker(
        Napi::Function& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor
    ) : Napi::AsyncWorker(callback),
        deferred(deferred), hypervisor(hypervisor) {}

    void OnError(const Napi::Error& err);
    void OnOK(void);

};

#endif /* __LIBVIRT_WORKER_H */
