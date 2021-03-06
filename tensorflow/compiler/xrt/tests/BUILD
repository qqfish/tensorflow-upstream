licenses(["notice"])  # Apache 2.0

package(
    default_visibility = [
        "//learning/brain:__subpackages__",
        "//tensorflow/compiler:__subpackages__",
    ],
)

load("//tensorflow:tensorflow.bzl", "tf_gpu_cc_test", "tf_cc_test")
load(
    "//tensorflow/core:platform/default/build_config_root.bzl",
    "tf_gpu_tests_tags",
)

cc_library(
    name = "raw_api_test_lib",
    testonly = 1,
    srcs = [
        "raw_api_test.cc",
    ],
    deps = [
        "//tensorflow/cc:cc_ops",
        "//tensorflow/cc:client_session",
        "//tensorflow/cc:ops",
        "//tensorflow/cc:scope",
        "//tensorflow/compiler/tf2xla:xla_compiler",
        "//tensorflow/compiler/xla:literal",
        "//tensorflow/compiler/xla:literal_util",
        "//tensorflow/compiler/xla:shape_util",
        "//tensorflow/compiler/xla:xla_data_proto",
        "//tensorflow/compiler/xla/client:client_library",
        "//tensorflow/compiler/xla/client:local_client",
        "//tensorflow/compiler/xla/client:xla_builder",
        "//tensorflow/compiler/xla/client:xla_computation",
        "//tensorflow/compiler/xla/service:platform_util",
        "//tensorflow/compiler/xrt:xrt_proto",
        "//tensorflow/compiler/xrt:xrt_server",
        "//tensorflow/compiler/xrt/cc:xrt_ops",
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:framework",
        "//tensorflow/core:framework_internal",
        "//tensorflow/core:lib",
        "//tensorflow/core:tensorflow_opensource",
        "//tensorflow/core:test",
        "//tensorflow/core:test_main",
        "@com_google_absl//absl/strings",
    ],
)

tf_cc_test(
    name = "raw_api_test_cpu",
    size = "medium",
    srcs = [],
    args = [
        "--xla_test_device=XLA_CPU",
        "--xla_platform=CPU",
    ],
    deps = [
        ":raw_api_test_lib",
        "//tensorflow/compiler/jit:xla_cpu_device",
    ],
)

tf_gpu_cc_test(
    name = "raw_api_test_gpu",
    size = "medium",
    srcs = [],
    args = [
        "--xla_test_device=XLA_GPU",
        "--xla_platform=GPU",
    ],
    deps = [
        ":raw_api_test_lib",
        "//tensorflow/compiler/jit:xla_gpu_device",
    ],
)
