var Foo = require('./build/Release/foo').Foo
var foo = new Foo()

var onlyonce = function () {
  console.log('I shall see this only once')
}

var neversee = function () {
  console.log('I shall never see this')
}

foo.on('sus', function (event) {
  console.log(event.num)
})

foo.cppOn('jingo', onlyonce)

foo.cppOn('jingo', function () {
  console.log('Listened on CPP')
})

foo.cppOnce('jingo', function () {
  console.log('Listened on CPP once')
  foo.cppOff('jingo', onlyonce)
})

foo.cppOnce('jingo', neversee)
foo.cppOff('jingo', neversee)

console.log(foo.bar())
setTimeout(function () {
  console.log(foo.bar())
}, 500)
