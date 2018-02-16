cxx_binary(
    name='asobitomo',
    srcs=["src/main.cc"],
    headers=glob(['src/*.h']),
    compiler_flags=['-std=c++14'],
    deps=['//asobitomo:libasobitomo']
)

cxx_library(
    name='libasobitomo',
    srcs=glob(["src/*.cc"]),
    exported_headers=subdir_glob([
        ('src', '*.h')
    ]),
    headers=subdir_glob([
        ('src', '*.h')
    ]),
    compiler_flags=['-std=c++14', '-g'],
)
