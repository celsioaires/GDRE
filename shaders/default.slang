struct VertexOutput {
    float4 position : SV_Position;
    float3 color;
    float2 uv;
};

struct Vertex {
    float3 position;
    float padding0;
    float3 color;
    float padding1;
    float2 uv;
    float2 padding2;
};

struct PushConstants {
    Vertex* vertexBuffer;
    float2 padding;
    float4x4 transformMatrix;
};

struct CameraData {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

struct MaterialData {
    float4 colorFactors;
    float4 metalRoughFactors;
    float4 padding0;
};

[[vk::push_constant]]
PushConstants gDrawConstants;

[[vk::binding(0, 0)]]
ConstantBuffer<CameraData> gCameraData;

[shader("vertex")]

VertexOutput vertexMain(uint id : SV_VertexID) {
    Vertex vertex = gDrawConstants.vertexBuffer[id];

    VertexOutput output;

    output.position =
        mul(gCameraData.projectionMatrix,
        mul(gCameraData.viewMatrix,
        mul(gDrawConstants.transformMatrix, float4(vertex.position, 1))));

    output.color = vertex.color;
    output.uv = vertex.uv;

    return output;
}

[[vk::binding(0, 1)]]
ConstantBuffer<MaterialData> gMaterialData;

[[vk::binding(1, 1)]]
Sampler2D gColorTexture;

[[vk::binding(2, 1)]]
Sampler2D gMetalRoughTexture;

[shader("fragment")]

float4 fragmentMain(VertexOutput vertex) : SV_Target {
    float4 texel = gColorTexture.Sample(vertex.uv);

    texel *= gMaterialData.colorFactors;

    return texel;
}
