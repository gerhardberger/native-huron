// Copyright 2014 Cheng Zhao. All rights reserved.
// Use of this source code is governed by MIT license that can be found in the
// LICENSE file.

#ifndef NATIVE_HURON_COMPAT_H_
#define NATIVE_HURON_COMPAT_H_

#include "node_version.h"

#if NODE_VERSION_AT_LEAST(0, 11, 0)

#define HURON_HANDLE_SCOPE(isolate) v8::HandleScope handle_scope(isolate)

#define HURON_METHOD_ARGS_TYPE   v8::FunctionCallbackInfo<v8::Value>
#define HURON_METHOD_RETURN_TYPE void

#define HURON_METHOD_RETURN_VALUE(value) return info.GetReturnValue().Set(value)
#define HURON_METHOD_RETURN_UNDEFINED()  return
#define HURON_METHOD_RETURN_NULL()       return info.GetReturnValue().SetNull()
#define HURON_METHOD_RETURN(value)       args.Return(value)

#define HURON_STRING_NEW(isolate, data) \
    v8::String::NewFromUtf8(isolate, data, v8::String::kNormalString)
#define HURON_STRING_NEW_FROM_UTF8(isolate, data, length) \
    v8::String::NewFromUtf8(isolate, data, v8::String::kNormalString, length)
#define HURON_STRING_NEW_FROM_UTF16(isolate, data, length) \
    v8::String::NewFromTwoByte(isolate, data, v8::String::kNormalString, length)
#define HURON_STRING_NEW_SYMBOL(isolate, data, length) \
    v8::String::NewFromUtf8(isolate, data, v8::String::kInternalizedString, length)

#define HURON_UNDEFINED(isolate) v8::Undefined(isolate)
#define HURON_TRUE(isolate) v8::True(isolate)
#define HURON_FALSE(isolate) v8::False(isolate)
#define HURON_ARRAY_NEW(isolate, size) v8::Array::New(isolate, size)
#define HURON_NUMBER_NEW(isolate, data) v8::Number::New(isolate, data)
#define HURON_INTEGER_NEW(isolate, data) v8::Integer::New(isolate, data)
#define HURON_INTEGER_NEW_UNSIGNED(isolate, data) \
  v8::Integer::NewFromUnsigned(isolate, data)
#define HURON_EXTERNAL_NEW(isolate, data) v8::External::New(isolate, data)
#define HURON_BOOLEAN_NEW(isolate, data) v8::Boolean::New(isolate, data)
#define HURON_OBJECT_NEW(isolate) v8::Object::New(isolate)

#define HURON_SET_INTERNAL_FIELD_POINTER(object, index, value) \
    object->SetAlignedPointerInInternalField(index, value)
#define HURON_GET_INTERNAL_FIELD_POINTER(object, index) \
    object->GetAlignedPointerFromInternalField(index)

#define HURON_PERSISTENT_INIT(isolate, handle, value) \
    handle(isolate, value)
#define HURON_PERSISTENT_ASSIGN(type, isolate, handle, value) \
    handle.Reset(isolate, value)
#define HURON_PERSISTENT_RESET(handle) \
    handle.Reset()
#define HURON_PERSISTENT_TO_LOCAL(type, isolate, handle) \
    v8::Local<type>::New(isolate, handle)
#define HURON_PERSISTENT_SET_WEAK(handle, parameter, callback) \
    handle.SetWeak(parameter, callback)

#define HURON_WEAK_CALLBACK(name, v8_type, c_type) \
  void name(const v8::WeakCallbackData<v8_type, c_type>& data)
#define HURON_WEAK_CALLBACK_INIT(c_type) \
  c_type* self = data.GetParameter()

#define HURON_THROW_EXCEPTION(isolate, value) \
    isolate->ThrowException(value)

#else  // Node 0.8 and 0.10

#define HURON_HANDLE_SCOPE(isolate) v8::HandleScope handle_scope

#define HURON_METHOD_ARGS_TYPE   v8::Arguments
#define HURON_METHOD_RETURN_TYPE v8::Local<v8::Value>

#define HURON_METHOD_RETURN_VALUE(value) return value
#define HURON_METHOD_RETURN_UNDEFINED()  return v8::Undefined()
#define HURON_METHOD_RETURN_NULL()       return v8::Null()
#define HURON_METHOD_RETURN(value) \
    HURON_METHOD_RETURN_VALUE(ConvertToV8(args.isolate(), value))

#define HURON_STRING_NEW(isolate, data) \
    v8::String::New(data)
#define HURON_STRING_NEW_FROM_UTF8(isolate, data, length) \
    v8::String::New(data, length)
#define HURON_STRING_NEW_FROM_UTF16(isolate, data, length) \
    v8::String::NewFromTwoByte(data, v8::String::kNormalString, length)
#define HURON_STRING_NEW_SYMBOL(isolate, data, length) \
    v8::String::NewSymbol(data, length)

#define HURON_UNDEFINED(isolate) v8::Undefined()
#define HURON_TRUE(isolate) v8::True()
#define HURON_FALSE(isolate) v8::False()
#define HURON_ARRAY_NEW(isolate, size) v8::Array::New(size)
#define HURON_NUMBER_NEW(isolate, data) v8::Number::New(data)
#define HURON_INTEGER_NEW(isolate, data) v8::Integer::New(data)
#define HURON_INTEGER_NEW_UNSIGNED(isolate, data) \
  v8::Integer::NewFromUnsigned(data)
#define HURON_EXTERNAL_NEW(isolate, data) v8::External::New(data)
#define HURON_BOOLEAN_NEW(isolate, data) v8::Boolean::New(data)
#define HURON_OBJECT_NEW(isolate) v8::Object::New()

#define HURON_SET_INTERNAL_FIELD_POINTER(object, index, value) \
    object->SetPointerInInternalField(index, value)
#define HURON_GET_INTERNAL_FIELD_POINTER(object, index) \
    object->GetPointerFromInternalField(index)

#define HURON_PERSISTENT_INIT(isolate, handle, value) \
    handle(value)
#define HURON_PERSISTENT_ASSIGN(type, isolate, handle, value) \
    handle = v8::Persistent<type>::New(value)
#define HURON_PERSISTENT_RESET(handle) \
    handle.Dispose(); \
    handle.Clear()
#define HURON_PERSISTENT_TO_LOCAL(type, isolate, handle) \
    v8::Local<type>::New(handle)
#define HURON_PERSISTENT_SET_WEAK(handle, parameter, callback) \
    handle.MakeWeak(parameter, callback)

#define HURON_WEAK_CALLBACK(name, v8_type, c_type) \
  void name(v8::Persistent<v8::Value> object, void* parameter)
#define HURON_WEAK_CALLBACK_INIT(c_type) \
  c_type* self = static_cast<c_type*>(parameter)

#define HURON_THROW_EXCEPTION(isolate, value) \
    v8::ThrowException(value)

#endif  // (NODE_MODULE_VERSION > 0x000B)


// Generally we should not provide utility macros, but this just makes things
// much more comfortable so we keep it.
#define HURON_METHOD(name) \
    HURON_METHOD_RETURN_TYPE name(const HURON_METHOD_ARGS_TYPE& info)

#endif  // NATIVE_HURON_COMPAT_H_
