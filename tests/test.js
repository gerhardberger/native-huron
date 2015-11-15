var Foo = require('./build/Release/foo').Foo
var foo = new Foo()

console.log(foo.bar())
console.log(foo.bar().play());
