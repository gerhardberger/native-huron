var Foo = require('./build/Release/foo').Foo
var foo = new Foo()

foo.on('sus', function (event) {
  console.log(event.num)
})

foo.cppOn('jingo', function () {
  console.log('Listened on CPP')
})

console.log(foo.bar())
setTimeout(function () {
  console.log(foo.bar())
}, 500)
