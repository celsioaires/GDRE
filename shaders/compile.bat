@echo off

setlocal

set "SLANGC=%VULKAN_SDK%\Bin\slangc.exe"

"%SLANGC%" default.slang ^
    -target spirv ^
    -profile spirv_1_4 ^
    -emit-spirv-directly ^
    -fvk-use-entrypoint-name ^
    -entry vertexMain ^
    -entry fragmentMain ^
    -o compiled/default.spv

