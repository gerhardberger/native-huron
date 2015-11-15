# native-huron

A set of utility classes to make writing native addons easier. See `tests` folder for example. Current list of classes:
- **Converter:** Converting from primitive and basic C++ variables to V8 variables and from V8 variables to C++ variables.
- **Dictionary:** Handle dictionaries that represents a Javascript Object.
- **PersistentDictionary:** Same as Dictionaries but stored on the heap.
- **ScopedPersistent:** Store V8 objects on the heap.

## Install
```
$ npm i native-huron
```

## Usage
In your `binding.gyp`:
``` gyp
{
  'includes': [ 'path/to/native-huron/module/native_huron_files.gypi' ],
  'targets': [ {
    'target_name': 'test'
    , 'sources': [ '<@(native_huron_files)' ]
    , 'include_dirs': [ "<!(node -e \"require('native-huron')\")" ]
  } ]
}
```

In your C++ files:
``` c++
#include <native_huron/converter.h>
#include <native_huron/dictionary.h>

// Use utility classes...
```

## API

### Converter

#### ConvertToV8(isolate, value)
Converts the value, a primitive or simple C++ variable (e.g. `double` or `std::string`) to a V8 variable. **Returns** the converted V8 variable.

#### TryConvertToV8(isolate, value, result)
Tries to convert the value, a primitive or simple C++ variable (e.g. `double` or `std::string`) to a V8 variable. **Returns** a `boolean` wether the conversion was successful, if it was, the result is written to the `result` address.

#### ConvertFromV8(isolate, value, result)
Tries to convert the value, a V8 variable, primitive or simple C++ variable (e.g. `double` or `std::string`). **Returns** a `boolean` wether the conversion was successful, if it was, the result is written to the `result` address.

#### StringToV8(isolate, input)
Convenience function to convert a `std::string` to a `v8::String`. **Returns** the converted string.

#### V8ToString(isolate, input)
Convenience function to convert a `v8::String` to a `std::string`. **Returns** the converted string.

### Dictionary
This class can also be used with Converter's `ConvertTo/FromV8` functions.

#### Dictionary::Dictionary(isolate, object)
Creates a `Dictionary` object from the `object` parameter, which is a `v8::Local<v8::Object>`.

#### Dictionary::CreateEmpty(isolate)
Creates an empty `Dictionary` object. **Returns** the created dictionary.

#### dict.Get(key, out)
**Returns** `true`, if the `key` (`std::string`) exists in the dictionary and writes it to the `out` address.

#### dict.GetHidden(key, out)
**Returns** `true`, if the `key` (`std::string`) exists as a hidden value in the dictionary and writes it to the `out` address.

#### dict.Set(key, value)
**Returns** `true`, if the `key` (`std::string`) is successfully set in the dictionary with the given `value`.

#### dict.SetHidden(key, value)
**Returns** `true`, if the `key` (`std::string`) is successfully set as hidden value in the dictionary with the given `value`.

#### dict.SetMethod(key, callback)
**Returns** `true`, if the `key` (`std::string`) method is successfully set in the dictionary with the given `callback`, which is a C++ method with V8 `FunctionCallback` signature (standard V8 addon method signature).

#### dict.Delete(key)
**Returns** `true`, if the `key` (`std::string`) is successfully deleted from the dictionary.

#### dict.IsEmpty()
**Returns** if the dictionary is empty.

### PersistentDictionary
Same functionality as `Dictionary`, but it is stored on the heap. *Use only if really neccessary.*
