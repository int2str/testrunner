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
    "-Wnon-virtual-dtor",
]

COPTS = WARNING_COPTS + [
    "-std=c++23",
]

package(
    default_visibility = ["//visibility:public"],
)

filegroup(
    name = "public_headers",
    srcs = ["include/testrunner/testrunner.h"],
)

cc_library(
    name = "lib",
    srcs = ["src/testrunner.cc"],
    hdrs = ["//:public_headers"],
    copts = COPTS,
    strip_include_prefix = "/include",
)

cc_library(
    name = "main",
    srcs = ["src/testrunner_main.cc"],
    hdrs = ["//:public_headers"],
    copts = COPTS,
    deps = ["//:lib"],
    linkstatic = 1,
    strip_include_prefix = "/include",
)

cc_test(
    name = "selftest",
    size = "small",
    testonly = 1,
    srcs = [
        "src/testrunner_selftest.cc",
        "//:public_headers",
    ],
    copts = COPTS,
    deps = ["//:main"],
)
