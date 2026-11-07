struct VertexOutput {
    float4 position : SV_Position;
    float3 color;
};

struct Vertex {
    float3 position;
    float padding0;
    float3 color;
    float padding1;
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

[[vk::push_constant]]
PushConstants drawConstants;

[[vk::binding(0, 0)]]
ConstantBuffer<CameraData> cameraData;

[shader("vertex")]

VertexOutput vertexMain(uint id : SV_VertexID) {
    Vertex vertex = drawConstants.vertexBuffer[id];

    VertexOutput output;

    output.position =
        mul(cameraData.projectionMatrix,
        mul(cameraData.viewMatrix,
        mul(drawConstants.transformMatrix, float4(vertex.position, 1))));

    output.color = vertex.color;

    return output;
}

[shader("fragment")]

float4 fragmentMain(VertexOutput vertex) : SV_Target {
    return float4(vertex.color, 1);
}
