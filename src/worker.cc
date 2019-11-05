#include "worker.h"

void Worker::SetVirError(void)
{
    virErrorPtr err = virGetLastError();
    if (err == NULL) SetError("Unknown error");
    else SetError(std::string(err->message));
}

void Worker::OnError(const Napi::Error& err)
{
    Napi::HandleScope scope(Env());
    deferred.Reject(Napi::String::New(Env(), err.Message()));
    Callback().Call({});
}

void Worker::OnOK()
{
    Napi::HandleScope scope(Env());
    deferred.Resolve(Env().Undefined());
    Callback().Call({});
}
