struct VertexOutput {
    float4 position : SV_Position;
    float3 color;
};

struct Vertex {
    float2 position;
    float2 padding;
    float3 color;
};

struct Constants {
    Vertex* vertexBuffer;
    float2 padding;
    float4x4 transformMatrix;
};

[[vk::push_constant]]
Constants constants;

[shader("vertex")]

VertexOutput vertexMain(uint id : SV_VertexID) {
    Vertex vertex = constants.vertexBuffer[id];

    VertexOutput output;

    output.position = mul(constants.transformMatrix, float4(vertex.position, 0, 1));
    output.color = vertex.color;

    return output;
}

[shader("fragment")]

float4 fragmentMain(VertexOutput vertex) : SV_Target {
    return float4(vertex.color, 1);
}
