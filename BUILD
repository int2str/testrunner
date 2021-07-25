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
    "-std=c++17",
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
    hdrs = ["//:public_headers"],
    strip_include_prefix = "/include",
)

cc_library(
    name = "main",
    srcs = ["testrunner_main.cc"],
    hdrs = ["//:public_headers"],
    copts = COPTS,
    linkstatic = 1,
    strip_include_prefix = "/include",
)

cc_test(
    name = "selftest",
    size = "small",
    testonly = 1,
    srcs = [
        "testrunner_selftest.cc",
        "//:public_headers",
    ],
    copts = COPTS,
    deps = ["//:main"],
)
