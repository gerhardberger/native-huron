#ifndef NATIVE_HURON_EMITTER_H_
#define NATIVE_HURON_EMITTER_H_

#include <map>
#include <vector>
#include <functional>

#include <nan.h>

#include <native_huron/converter.h>
#include <native_huron/dictionary.h>

namespace huron {

namespace internal {

typedef Nan::CopyablePersistentTraits<v8::Function>::CopyablePersistent
  CopyablePersistentType;

}

class Emitter : public Nan::ObjectWrap {
 public:
  Emitter  () { }
  ~Emitter () { }

  struct AsyncData {
  	Emitter *self;
  	std::string event_name;
    std::function<void(huron::Dictionary&)> handler;
  };

  template<typename... Args>
  void Emit (v8::Local<v8::Value> eventName, const Args&... args) {
    v8::String::Utf8Value name_(eventName->ToString());
    std::string name = std::string(*name_);

    if (m[name].empty()) return;

    std::vector<v8::Local<v8::Value> > converted_args = { args..., };

    for (std::vector<internal::CopyablePersistentType>::iterator
      it = m[name].begin(); it != m[name].end(); ++it) {
        node::MakeCallback(
          v8::Isolate::GetCurrent(), Nan::GetCurrentContext()->Global()
        , Nan::New(*it)
        , converted_args.size(), &converted_args.front());
    }
  }

  template<typename... Args>
  void Emit (std::string eventName, const Args&... args) {
    Emit(Nan::New(eventName).ToLocalChecked(), args...);
  }

  template<typename... Args>
  void Emit (const char *eventName, const Args&... args) {
    Emit(Nan::New(eventName).ToLocalChecked(), args...);
  }

  template<typename Func>
  void Emit (std::string eventName, Func fn) {
    uv_async_t *async = new uv_async_t();
    uv_async_init(uv_default_loop()
      , async
      , Emitter::AsyncEmit);

    AsyncData *data = new AsyncData();
		data->self = this;
    data->event_name = eventName;
    data->handler = fn;

		async->data = (void*)data;
		uv_async_send(async);
  }

  template<typename Func>
  void Emit (const char *eventName, Func fn) {
    Emit(std::string(eventName), fn);
  }

  static void On (const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (!info[0]->IsString() || !info[1]->IsFunction()) return;

    Emitter *emitter = ObjectWrap::Unwrap<Emitter>(info.Holder());

    v8::String::Utf8Value name(info[0]->ToString());
    v8::Local<v8::Function> cb = info[1].As<v8::Function>();
    std::string eventName = std::string(*name);

    Nan::Persistent<v8::Function> persistent_cb;
    persistent_cb.Reset(cb);

    emitter->m[eventName].push_back(persistent_cb);
  }

  static void AsyncEmit(uv_async_t *handle) {
    Nan::HandleScope scope;

    AsyncData data = *((AsyncData*) handle->data);

    v8::Isolate *iso = v8::Isolate::GetCurrent();
    huron::Dictionary dict = huron::Dictionary::CreateEmpty(iso);
    data.handler(dict);

    data.self->Emit(Nan::New(data.event_name).ToLocalChecked()
      , huron::ConvertToV8(iso, dict));

    delete handle;
  }

 private:
  std::map<std::string, std::vector<internal::CopyablePersistentType> > m;
};

}  // namespace huron

#endif  // NATIVE_HURON_EMITTER_H_
