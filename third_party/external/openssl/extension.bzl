
load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

def _openssl_repo_impl(ctx):
    ctx.file("BUILD.bazel", """
cc_library(
    name = "openssl",
    hdrs = glob(["openssl/**/*.h"]),
    includes = ["."],
    visibility = ["//visibility:public"],
)
    """)
    ctx.symlink(ctx.path(ctx.attr.path), "openssl")

openssl_repo = rule(
    implementation = _openssl_repo_impl,
    attrs = {
        "path": attr.string(mandatory = True),
    },
)
