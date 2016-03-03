#include "foo.h"

#include <thread>

#include <native_huron/converter.h>
#include <native_huron/dictionary.h>
#include <native_huron/emitter.h>

Nan::Persistent<v8::Function> Foo::constructor;

Foo::Foo  () { }
Foo::~Foo () { }

void Foo::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Foo").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "bar", Bar);
  Nan::SetPrototypeMethod(tpl, "cppOn", CppOn);

  Nan::SetPrototypeMethod(tpl, "on", huron::Emitter::On);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("Foo").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(Foo::New) {
  if (info.IsConstructCall()) {
    Foo *foo = new Foo();
    foo->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { info[0] };
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void nocturne(const v8::FunctionCallbackInfo<v8::Value> &info) {
  info.GetReturnValue().Set(huron::ConvertToV8(info.GetIsolate()
    , "Pretty music playing!"));
}

void Foo::task() {
  int j = 2;

  Emit("sus", [=](huron::Dictionary &dict) {
    dict.Set("num", j);
  });
}

NAN_METHOD(Foo::Bar) {
  Foo *foo = ObjectWrap::Unwrap<Foo>(info.Holder());
  v8::Isolate *isolate = info.GetIsolate();

  huron::Dictionary dict = huron::Dictionary::CreateEmpty(isolate);

  dict.Set("composer", "Erik Satie");
  dict.Set("sonata", 15);
  dict.SetMethod("play", &nocturne);

  v8::Local<v8::Value> comp;
  if (dict.Get("text", &comp)) { }

  std::thread t(std::bind(&Foo::task, foo));
  t.detach();

  foo->Emit("jingo");

  info.GetReturnValue().Set(huron::ConvertToV8(isolate, dict));
}

NAN_METHOD(Foo::CppOn) {
  Foo *foo = ObjectWrap::Unwrap<Foo>(info.Holder());

  v8::String::Utf8Value name(info[0]->ToString());
  std::string eventName = std::string(*name);

  v8::Local<v8::Function> cb = info[1].As<v8::Function>();

  foo->On(eventName, cb);
}
