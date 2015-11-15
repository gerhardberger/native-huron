#include <nan.h>
#include "foo.h"

void InitAll(v8::Local<v8::Object> exports) {
  Foo::Init(exports);
}

NODE_MODULE(bindings, InitAll)
