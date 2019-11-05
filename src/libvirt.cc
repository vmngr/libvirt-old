#include <napi.h>
#include <libvirt/libvirt.h>

#include "hypervisor.h"
#include "domain.h"

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Hypervisor::Init(env, exports);
    Domain::Init(env, exports);
    return exports;
}

NODE_API_MODULE(libvirt, Init)
