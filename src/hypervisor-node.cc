/**
 * Copyright 2019 Leon Rinkel <leon@rinkel.me> and vmngr/libvirt contributers.
 * 
 * This file is part of the vmngr/libvirt project and is subject to the MIT
 * license as in the LICENSE file in the project root.
 */

#include "src/hypervisor.h"

#include "src/worker.h"
#include <glib-2.0/glib.h>

static Napi::Value dummyCallback(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
}

/******************************************************************************
 * NodeGetInfo                                                                *
 ******************************************************************************/

class NodeGetInfoWorker : public Worker {
 public:
    using Worker::Worker;

    void Execute(void) override {
        int ret = virNodeGetInfo(hypervisor->conn, &nodeInfo);
        if (ret < 0) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object info = Napi::Object::New(Env());
        info.Set("model", Napi::String::New(Env(), nodeInfo.model));
        info.Set("memory", Napi::Number::New(Env(), nodeInfo.memory));
        info.Set("cpus", Napi::Number::New(Env(), nodeInfo.cpus));
        info.Set("mhz", Napi::Number::New(Env(), nodeInfo.mhz));
        info.Set("nodes", Napi::Number::New(Env(), nodeInfo.nodes));
        info.Set("sockets", Napi::Number::New(Env(), nodeInfo.sockets));
        info.Set("cores", Napi::Number::New(Env(), nodeInfo.cores));
        info.Set("threads", Napi::Number::New(Env(), nodeInfo.threads));

        deferred.Resolve(info);
        Callback().Call({});
    }

 private:
    virNodeInfo nodeInfo;
};

Napi::Value Hypervisor::NodeGetInfo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);

    NodeGetInfoWorker* worker = new NodeGetInfoWorker(callback, deferred, this);
    worker->Queue();

    return deferred.Promise();
}
/******************************************************************************
 * NodeGetMemoryStats                                                         *
 ******************************************************************************/

class NodeGetMemoryStatsWorker : public Worker {
 public:
    NodeGetMemoryStatsWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor)
        : Worker(callback, deferred, hypervisor) {}

    void Execute(void) override {
        int cellNum = -1; // Get all field
        int ret = virNodeGetMemoryStats(hypervisor->conn, cellNum, NULL, &nparams,0);
        if (ret != 0) SetVirError();
        params = (virNodeMemoryStatsPtr) g_malloc0_n(nparams, sizeof(*params));
        ret = virNodeGetMemoryStats(hypervisor->conn, cellNum, params, &nparams, 0);
        if (ret != 0) SetVirError();
    }

    void OnOK(void) override {
        Napi::HandleScope scope(Env());

        Napi::Object info = Napi::Object::New(Env());
        for (int i = 0; i < nparams; i++){
            info.Set(params[i].field, Napi::Number::New(Env(), params[i].value));
        }
        deferred.Resolve(info);
        Callback().Call({});
    }

 private:
    virNodeMemoryStatsPtr params;
    int nparams = 0;
};

Napi::Value Hypervisor::NodeGetMemoryStats(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    NodeGetMemoryStatsWorker* worker = new NodeGetMemoryStatsWorker(callback, deferred, this);
    worker->Queue();

    return deferred.Promise();
}

/******************************************************************************
 * NodeCpuStatsWorker                                                         *
 ******************************************************************************/
typedef enum {
    VIRSH_CPU_USER,
    VIRSH_CPU_SYSTEM,
    VIRSH_CPU_IDLE,
    VIRSH_CPU_IOWAIT,
    VIRSH_CPU_INTR,
    VIRSH_CPU_USAGE,
    VIRSH_CPU_LAST
} virshCPUStats;
#define G_N_ELEMENTS(Array) (sizeof(Array) / sizeof(*(Array)))
#define STREQ(a, b) (strcmp(a, b) == 0)
int
virEnumFromString(const char * const *types,
                  unsigned int ntypes,
                  const char *type)
{
    size_t i;
    if (!type)
        return -1;

    for (i = 0; i < ntypes; i++)
        if (STREQ(types[i], type))
            return i;

    return -1;
}
#define VIR_ENUM_IMPL(name, lastVal, ...) \
    static const char *const name ## TypeList[] = { __VA_ARGS__ }; \
    int name ## TypeFromString(const char *type) { \
        return virEnumFromString(name ## TypeList, \
                                 G_N_ELEMENTS(name ## TypeList), \
                                 type); \
    }

VIR_ENUM_IMPL(virshCPUStats, VIRSH_CPU_LAST,
              VIR_NODE_CPU_STATS_USER,
              VIR_NODE_CPU_STATS_KERNEL,
              VIR_NODE_CPU_STATS_IDLE,
              VIR_NODE_CPU_STATS_IOWAIT,
              VIR_NODE_CPU_STATS_INTR,
              VIR_NODE_CPU_STATS_UTILIZATION);
const char *virshCPUOutput[] = {
    "user",
    "system",
    "idle",
    "iowait",
    "intr",
    "usage"
};

class NodeCpuStatsWorker : public Worker {
 public:
    NodeCpuStatsWorker(
        Napi::Function const& callback,
        Napi::Promise::Deferred deferred,
        Hypervisor* hypervisor,
        bool flag_percent)
        : Worker(callback, deferred, hypervisor), flag_percent(flag_percent) {}

    void Execute(void) override {
        size_t i, j;
        cpu_stats[VIRSH_CPU_LAST] = { 0 };
        present[VIRSH_CPU_LAST] = { false };
        int ret = virNodeGetCPUStats(hypervisor->conn, cpuNum, NULL, &nparams, 0);
        if (ret != 0) SetVirError();

        memset(cpu_stats, 0, sizeof(cpu_stats));

        params = (virNodeCPUStatsPtr) g_malloc0_n(nparams, sizeof(*params));

        for (i = 0; i < 2; i++) {
            virNodeGetCPUStats(hypervisor->conn, cpuNum, params, &nparams, 0);
            for (j = 0; j < nparams; j++) {
                int field = virshCPUStatsTypeFromString(params[j].field);

                if (field < 0){
                    continue;
                }

                if (i == 0){
                    cpu_stats[field] = params[j].value;
                    present[field] = true;
                }else if (present[field]) {
                    cpu_stats[field] = params[j].value - cpu_stats[field];
                }
            }
            if (present[VIRSH_CPU_USAGE] || !flag_percent){
                break;
            }
            g_usleep(1000000); // 1s
        }  
    }
    void OnOK(void) override {
        size_t i;
        Napi::HandleScope scope(Env());

        Napi::Object info = Napi::Object::New(Env());
        if (!flag_percent){
            for (i = 0; i < VIRSH_CPU_USAGE; i++) {
                if (present[i]) {
                    info.Set(virshCPUOutput[i], Napi::Number::New(Env(), cpu_stats[i]));
                    total_time += cpu_stats[i];
                }
                info.Set("total", Napi::Number::New(Env(), total_time));
                
            }
        }else {
            if (present[VIRSH_CPU_USAGE]) {
                info.Set("usage", cpu_stats[VIRSH_CPU_USAGE]);
                info.Set("idle", 100 - cpu_stats[VIRSH_CPU_USAGE]);
            } else {
                double usage = 0;
                for (i = 0; i < VIRSH_CPU_USAGE; i++){
                    total_time += cpu_stats[i];
                }

                usage = (cpu_stats[VIRSH_CPU_USER] + cpu_stats[VIRSH_CPU_SYSTEM])
                / total_time * 100;

                info.Set("usage", Napi::Number::New(Env(), usage));
                for (i = 0; i < VIRSH_CPU_USAGE; i++) {
                    if (present[i]) {
                        info.Set(virshCPUOutput[i], Napi::Number::New(Env(), cpu_stats[i] / total_time * 100));
                    }
                }
            }
        }
        deferred.Resolve(info);
        Callback().Call({});
    }

 private:
    bool flag_percent;
    double usage = 0;
    double total_time = 0;
    virNodeCPUStatsPtr params;
    unsigned long long cpu_stats[VIRSH_CPU_LAST];
    bool present[VIRSH_CPU_LAST];
    int cpuNum = VIR_NODE_CPU_STATS_ALL_CPUS; // -1
    int nparams = 0;
};

Napi::Value Hypervisor::NodeCpuStats(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    bool flag_percent = false;
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::Function callback = Napi::Function::New(env, dummyCallback);
    if (info.Length() == 1){
        if (!info[0].IsBoolean()){
            deferred.Reject(Napi::String::New(env, "Expected an boolean [percent]."));
            return deferred.Promise();
        }else{
            flag_percent = info[0].As<Napi::Boolean>().ToBoolean();
        }
    }

    NodeCpuStatsWorker* worker = new NodeCpuStatsWorker(callback, deferred, this, flag_percent);
    worker->Queue();

    return deferred.Promise();
}