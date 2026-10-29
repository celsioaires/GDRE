#!/bin/bash

set -e

SLANGC="$VULKAN_SDK/bin/slangc"

"$SLANGC" default.slang \
    -target spirv \
    -profile spirv_1_4 \
    -emit-spirv-directly \
    -fvk-use-entrypoint-name \
    -entry vertexMain \
    -entry fragmentMain \
    -o compiled/default.spv
