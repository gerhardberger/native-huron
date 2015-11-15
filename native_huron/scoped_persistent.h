// Copyright 2014 Cheng Zhao. All rights reserved.
// Use of this source code is governed by MIT license that can be found in the
// LICENSE file.

#ifndef NATIVE_HURON_SCOPED_PERSISTENT_H_
#define NATIVE_HURON_SCOPED_PERSISTENT_H_


#include "native_huron/converter.h"
#include <v8.h>

namespace huron {

// A v8::Persistent handle to a V8 value which destroys and clears the
// underlying handle on destruction.
template <typename T>
class ScopedPersistent {
 public:
  ScopedPersistent() : isolate_(v8::Isolate::GetCurrent()) {}

  ScopedPersistent(v8::Isolate* isolate, v8::Local<v8::Value> handle)
      : isolate_(isolate) {
    reset(isolate, v8::Local<T>::Cast(handle));
  }

  ~ScopedPersistent() {
    reset();
  }

  void reset(v8::Isolate* isolate, v8::Local<T> handle) {
    if (!handle.IsEmpty()) {
      isolate_ = isolate;
      HURON_PERSISTENT_ASSIGN(T, isolate, handle_, handle);
    } else {
      reset();
    }
  }

  void reset() {
    HURON_PERSISTENT_RESET(handle_);
  }

  bool IsEmpty() const {
    return handle_.IsEmpty();
  }

  v8::Local<T> NewHandle() const {
    return NewHandle(isolate_);
  }

  v8::Local<T> NewHandle(v8::Isolate* isolate) const {
    if (handle_.IsEmpty())
      return v8::Local<T>();
    return HURON_PERSISTENT_TO_LOCAL(T, isolate, handle_);
  }

  template<typename P, typename C>
  void SetWeak(P* parameter, C callback) {
    HURON_PERSISTENT_SET_WEAK(handle_, parameter, callback);
  }

  v8::Isolate* isolate() const { return isolate_; }

 private:
  v8::Isolate* isolate_;
  v8::Persistent<T> handle_;

  //DISALLOW_COPY_AND_ASSIGN(ScopedPersistent);
};

template<typename T>
struct Converter<ScopedPersistent<T> > {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    const ScopedPersistent<T>& val) {
    return val.NewHandle(isolate);
  }

  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     ScopedPersistent<T>* out) {
    v8::Local<T> converted;
    if (!Converter<v8::Local<T> >::FromV8(isolate, val, &converted))
      return false;

    out->reset(isolate, converted);
    return true;
  }
};

}  // namespace huron

#endif  // NATIVE_HURON_SCOPED_PERSISTENT_H_
