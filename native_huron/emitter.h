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
  
  static void uv_free_handle(uv_handle_t* handle) {}

  template<typename... Args>
  void Emit (v8::Local<v8::Value> eventName, const Args&... args) {
    v8::String::Utf8Value name_(eventName->ToString());
    std::string name = std::string(*name_);

    std::vector<v8::Local<v8::Value> > converted_args = { args..., };

    if (m.find(name) != m.end()) {
      std::vector<internal::CopyablePersistentType> cb_vec_copy = m[name];
      for (std::vector<internal::CopyablePersistentType>::iterator
        it = cb_vec_copy.begin(); it != cb_vec_copy.end(); ++it) {
        node::MakeCallback(
          v8::Isolate::GetCurrent(), Nan::GetCurrentContext()->Global()
          , Nan::New(*it)
          , converted_args.size(), &converted_args.front());
      }
    }
    if (o.find(name) != o.end()) {
      std::vector<internal::CopyablePersistentType> cb_vec_copy = o[name];
      for (std::vector<internal::CopyablePersistentType>::iterator
        it = cb_vec_copy.begin(); it != cb_vec_copy.end(); ++it) {
        node::MakeCallback(
          v8::Isolate::GetCurrent(), Nan::GetCurrentContext()->Global()
          , Nan::New(*it)
          , converted_args.size(), &converted_args.front());
      }
      o[name].clear();
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

  static void AsyncEmit(uv_async_t *handle) {
    Nan::HandleScope scope;

    AsyncData data = *((AsyncData*) handle->data);

    v8::Isolate *iso = v8::Isolate::GetCurrent();
    huron::Dictionary dict = huron::Dictionary::CreateEmpty(iso);
    data.handler(dict);

    data.self->Emit(Nan::New(data.event_name).ToLocalChecked()
      , huron::ConvertToV8(iso, dict));

    uv_close((uv_handle_t*)handle, &Emitter::uv_free_handle);
  }

  static void Off (const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (!info[0]->IsString() || !info[1]->IsFunction()) return;

    Emitter *emitter = ObjectWrap::Unwrap<Emitter>(info.Holder());

    v8::String::Utf8Value name(info[0]->ToString());
    v8::Local<v8::Function> cb = info[1].As<v8::Function>();
    std::string eventName = std::string(*name);

    Nan::Persistent<v8::Function> persistent_cb;
    persistent_cb.Reset(cb);

    if (emitter->m.find(eventName) != emitter->m.end()) {
      for (std::vector<internal::CopyablePersistentType>::iterator
        it = emitter->m[eventName].begin(); it != emitter->m[eventName].end();) {
        if (Nan::New(*it) == persistent_cb)
          it = emitter->m[eventName].erase(it);
        else
          ++it;
      }
    }
    if (emitter->o.find(eventName) != emitter->o.end()) {
      for (std::vector<internal::CopyablePersistentType>::iterator
        it = emitter->o[eventName].begin(); it != emitter->o[eventName].end();) {
        if (Nan::New(*it) == persistent_cb)
          it = emitter->o[eventName].erase(it);
        else
          ++it;
      }
    }
  }
  
  static void RemoveEventListener (
      const Nan::FunctionCallbackInfo<v8::Value>& info) {
    Off(info);
  }

  void Off (std::string eventName, const v8::Local<v8::Function> cb) {
    Nan::Persistent<v8::Function> persistent_cb;
    persistent_cb.Reset(cb);

    if (m.find(eventName) != m.end()) {
      for (std::vector<internal::CopyablePersistentType>::iterator
        it = this->m[eventName].begin(); it != this->m[eventName].end();) {
        if (Nan::New(*it) == persistent_cb)
          it = this->m[eventName].erase(it);
        else
          ++it;
      }
    }
    if (o.find(eventName) != o.end()) {
      for (std::vector<internal::CopyablePersistentType>::iterator
        it = this->o[eventName].begin(); it != this->o[eventName].end();) {
        if (Nan::New(*it) == persistent_cb)
          it = this->o[eventName].erase(it);
        else
          ++it;
      }
    }
  }
  
  void RemoveEventListener (std::string eventName, 
                            const v8::Local<v8::Function> cb) {
    Off(eventName, cb);
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
  
  static void AddEventListener (
      const Nan::FunctionCallbackInfo<v8::Value>& info) {
    On(info);
  }

  void On (std::string eventName, const v8::Local<v8::Function> cb) {
    Nan::Persistent<v8::Function> persistent_cb;
    persistent_cb.Reset(cb);

    this->m[eventName].push_back(persistent_cb);
  }
  
  void AddEventListener (std::string eventName,
                         const v8::Local<v8::Function> cb) {
    On(eventName, cb);
  }

  static void Once (const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (!info[0]->IsString() || !info[1]->IsFunction()) return;

    Emitter *emitter = ObjectWrap::Unwrap<Emitter>(info.Holder());

    v8::String::Utf8Value name(info[0]->ToString());
    v8::Local<v8::Function> cb = info[1].As<v8::Function>();
    std::string eventName = std::string(*name);

    Nan::Persistent<v8::Function> persistent_cb;
    persistent_cb.Reset(cb);

    emitter->o[eventName].push_back(persistent_cb);
  }

  void Once (std::string eventName, const v8::Local<v8::Function> cb) {
    Nan::Persistent<v8::Function> persistent_cb;
    persistent_cb.Reset(cb);

    this->o[eventName].push_back(persistent_cb);
  }
  
  void RemoveAllListeners (std::string eventName) {
    this->m.erase(eventName);
    this->o.erase(eventName);
  }
  
  static void RemoveAllListeners (
      const Nan::FunctionCallbackInfo<v8::Value>& info) {
    Emitter *emitter = ObjectWrap::Unwrap<Emitter>(info.Holder());
    if (!info[0]->IsString()) {
      emitter->m.clear();
      emitter->o.clear();
    } else {
      v8::String::Utf8Value name(info[0]->ToString());
      std::string eventName = std::string(*name);

      emitter->m.erase(eventName);
      emitter->o.erase(eventName);
    }
  }
  
  void RemoveAllListeners () {
    m.clear();
    o.clear();
  }

  static void ListenerCount(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (!info[0]->IsString())
      return;
    Emitter* emitter = ObjectWrap::Unwrap<Emitter>(info.Holder());
    size_t counter = 0;
    v8::String::Utf8Value name(info[0]->ToString());
    std::string eventName = std::string(*name);

    if (emitter->m.find(eventName) != emitter->m.end()) {
      counter += emitter->m[eventName].size();
    }
    if (emitter->o.find(eventName) != emitter->o.end()) {
      counter += emitter->o[eventName].size();
    }
    info.GetReturnValue().Set(static_cast<unsigned int>(counter));
  }

  size_t ListenerCount(const std::string& eventName) {
    size_t counter = 0;
    if (m.find(eventName) != m.end()) {
      counter += m[eventName].size();
    }
    if (o.find(eventName) != o.end()) {
      counter += o[eventName].size();
    }
    return counter;
  }

 private:
  std::map<std::string, std::vector<internal::CopyablePersistentType> > m;
  std::map<std::string, std::vector<internal::CopyablePersistentType> > o;
};

}  // namespace huron

#endif  // NATIVE_HURON_EMITTER_H_
