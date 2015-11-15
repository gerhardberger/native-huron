{
  'includes': [
      '../native_huron_files.gypi'
  ],
  'targets': [
    {
      'target_name': 'foo'
      , 'sources': [ 'addon.cc', 'foo.cc', '<@(native_huron_files)' ]
      , 'include_dirs': [
          "<!(node -e \"require('nan')\")"
        , "<!(node -e \"require('..')\")"
      ]
      , 'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [
                '-mmacosx-version-min=10.7'
              , '-std=c++11'
              , '-stdlib=libc++'
            ]
          }
        }]
      ]
    }
  ]
}
