var Foo = require('./build/Release/foo').Foo
var foo = new Foo()

foo.on('sus', function (event) {
  console.log(event.num)
})

console.log(foo.bar())
setTimeout(function () {
  console.log(foo.bar())
}, 500)
