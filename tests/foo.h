#ifndef FOO_H
#define FOO_H

#include <nan.h>
#include <native_huron/emitter.h>

class Foo : public huron::Emitter {
public:
  static void Init(v8::Local<v8::Object> exports);

  static NAN_METHOD(New);
  static NAN_METHOD(Bar);
  static NAN_METHOD(CppOn);
  static NAN_METHOD(CppOff);
  static NAN_METHOD(CppOnce);

  void task ();
  //static NAN_METHOD(nocturne);

private:
  explicit Foo ();
  ~Foo ();

  static Nan::Persistent<v8::Function> constructor;
};

#endif
