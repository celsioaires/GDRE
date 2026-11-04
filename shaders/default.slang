struct VertexOutput {
    float4 position : SV_Position;
    float3 color;
};

struct Vertex {
    float2 position;
    float2 padding;
    float3 color;
};

struct MeshConstants {
    Vertex* vertexBuffer;
    float2 padding;
    float4x4 transformMatrix;
};

struct CameraData {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

[[vk::push_constant]]
MeshConstants meshConstants;

[[vk::binding(0, 0)]]
ConstantBuffer<CameraData> cameraData;

[shader("vertex")]

VertexOutput vertexMain(uint id : SV_VertexID) {
    Vertex vertex = meshConstants.vertexBuffer[id];

    VertexOutput output;

    output.position = mul(meshConstants.transformMatrix, float4(vertex.position, 0, 1));

    /*
    output.position =
        mul(cameraData.projectionMatrix,
        mul(cameraData.viewMatrix,
        mul(meshConstants.transformMatrix, float4(vertex.position, 0, 1))));
    */

    output.color = vertex.color;

    return output;
}

[shader("fragment")]

float4 fragmentMain(VertexOutput vertex) : SV_Target {
    return float4(vertex.color, 1);
}
