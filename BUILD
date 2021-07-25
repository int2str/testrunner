WARNING_COPTS = [
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Wconversion",
    "-Wunused",
    "-Wimplicit-fallthrough",
    "-Wshadow",
    "-Wold-style-cast",
    "-Wcast-align",
    "-Wnon-virtual-dtor"
]

COPTS = WARNING_COPTS + [
    "-std=c++17"
]

cc_library(
    name = "main",
    copts = COPTS,
    srcs = [
        "testrunner_main.cc",
        "testrunner_core.h"
    ]
)

cc_test(
    name = "selftest",
    size = "small",
    copts = COPTS,
    srcs = [
        "testrunner_selftest.cc",
        "testrunner_core.h"
    ],
    deps = [ "//:main" ]
)
