/**
 * Copyright 2019 Leon Rinkel <leon@rinkel.me> and vmngr/libvirt contributers.
 * 
 * This file is part of the vmngr/libvirt project and is subject to the MIT
 * license as in the LICENSE file in the project root.
 */

#include <glib-2.0/glib.h>
#include "src/hypervisor.h"


Napi::FunctionReference Hypervisor::constructor;
Napi::Object Hypervisor::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Hypervisor", {
        InstanceMethod("connectOpen", &Hypervisor::ConnectOpen),
        InstanceMethod("connectClose", &Hypervisor::ConnectClose),
        InstanceMethod("connectListAllDomains",
            &Hypervisor::ConnectListAllDomains),
        InstanceMethod("connectListDomains", &Hypervisor::ConnectListDomains),
        InstanceMethod("connectListDefinedDomains",
            &Hypervisor::ConnectListDefinedDomains),
        InstanceMethod("connectGetMaxVcpus", &Hypervisor::ConnectGetMaxVcpus),
        InstanceMethod("connectGetHostname", &Hypervisor::ConnectGetHostname),

        InstanceMethod("domainCreateXML", &Hypervisor::DomainCreateXML),
        InstanceMethod("domainDefineXML", &Hypervisor::DomainDefineXML),
        InstanceMethod("domainGetInfo", &Hypervisor::DomainGetInfo),
        InstanceMethod("domainGetID", &Hypervisor::DomainGetID),
        InstanceMethod("domainGetName", &Hypervisor::DomainGetName),
        InstanceMethod("domainGetUUIDString", &Hypervisor::DomainGetUUIDString),
        InstanceMethod("domainLookupByID", &Hypervisor::DomainLookupByID),
        InstanceMethod("domainLookupByName", &Hypervisor::DomainLookupByName),
        InstanceMethod("domainLookupByUUIDString",
            &Hypervisor::DomainLookupByUUIDString),
        InstanceMethod("domainSave", &Hypervisor::DomainSave),
        InstanceMethod("domainCreate", &Hypervisor::DomainCreate),
        InstanceMethod("domainShutdown", &Hypervisor::DomainShutdown),
        InstanceMethod("domainReboot", &Hypervisor::DomainReboot),
        InstanceMethod("domainRestore", &Hypervisor::DomainRestore),
        InstanceMethod("domainGetXMLDesc", &Hypervisor::DomainGetXMLDesc),
        InstanceMethod("domainInterfaceStats",
            &Hypervisor::DomainInterfaceStats),
        InstanceMethod("domainInterfaceTune",
            &Hypervisor::DomainInterfaceTune),
        InstanceMethod("domainInterfaceTuneCurrent",
            &Hypervisor::DomainInterfaceTuneCurrent),
        InstanceMethod("domainMemoryStats",
            &Hypervisor::DomainMemoryStats),

        InstanceMethod("nodeGetInfo", &Hypervisor::NodeGetInfo),
        InstanceMethod("nodeGetMemoryStats", &Hypervisor::NodeGetMemoryStats),
        InstanceMethod("nodeCpuStats", &Hypervisor::NodeCpuStats),
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Hypervisor", func);
    return exports;
}

/******************************************************************************
 * Constructor                                                                *
 ******************************************************************************/

Hypervisor::Hypervisor(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Hypervisor>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() <= 0 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected an object.")
            .ThrowAsJavaScriptException();
        return;
    }

    Napi::Object options = info[0].As<Napi::Object>();

    if (!options.HasOwnProperty("uri")) {
        Napi::TypeError::New(env, "Expected options to have property 'uri'.")
            .ThrowAsJavaScriptException();
        return;
    }

    Napi::String uri = options.Get("uri").As<Napi::String>();
    this->uri = std::string(uri.Utf8Value());
}

char * getTypedParamValue(virTypedParameterPtr item) {
    char *str = NULL;
    switch (item->type) {
    case VIR_TYPED_PARAM_INT:
        str = g_strdup_printf("%d", item->value.i);
        break;

    case VIR_TYPED_PARAM_UINT:
        str = g_strdup_printf("%u", item->value.ui);
        break;

    case VIR_TYPED_PARAM_LLONG:
        str = g_strdup_printf("%lld", item->value.l);
        break;

    case VIR_TYPED_PARAM_ULLONG:
        str = g_strdup_printf("%llu", item->value.ul);
        break;

    case VIR_TYPED_PARAM_DOUBLE:
        str = g_strdup_printf("%f", item->value.d);
        break;

    case VIR_TYPED_PARAM_BOOLEAN:
        str = g_strdup(item->value.b ? "yes" : "no");
        break;

    case VIR_TYPED_PARAM_STRING:
        str = g_strdup(item->value.s);
        break;
    default:
        str = "Not known type";
    }

    return str;
}
