# third_party/external/openssl/BUILD

load("@bazel_skylib//rules:repositories.bzl", "local_repository")

exports_files(
    glob(["**"]),
    visibility = ["//visibility:public"],
)
load("//third_party/external/openssl/extension.bzl", "openssl_repo")

# Expose the 'openssl_repo' rule defined in extension.bzl
openssl_repo(
    name = "openssl_repo",
    path = "/usr/include/openssl-riscv/include",  # Path to OpenSSL headers
    # Add any other necessary attributes specific to your use case
)
