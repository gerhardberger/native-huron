// Copyright 2014 Cheng Zhao. All rights reserved.
// Use of this source code is governed by MIT license that can be found in the
// LICENSE file.

#ifndef NATIVE_HURON_PERSISTENT_DICTIONARY_H_
#define NATIVE_HURON_PERSISTENT_DICTIONARY_H_

#include "native_huron/dictionary.h"
#include "native_huron/scoped_persistent.h"

namespace huron {

// Like Dictionary, but stores object in persistent handle so you can keep it
// safely on heap.
class PersistentDictionary : public Dictionary {
 public:
  PersistentDictionary();
  PersistentDictionary(v8::Isolate* isolate, v8::Local<v8::Object> object);
  virtual ~PersistentDictionary();

  v8::Local<v8::Object> GetHandle() const override;

 private:
  // TODO: maybe something instead of scoped_ptr<>
  ScopedPersistent<v8::Object> *handle_;
};

template<>
struct Converter<PersistentDictionary> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     PersistentDictionary* out);
};

}  // namespace huron

#endif  // NATIVE_HURON_PERSISTENT_DICTIONARY_H_
