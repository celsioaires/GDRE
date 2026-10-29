struct Vertex {
    float3 color;
    float4 svPosition : SV_Position;
};

static float2 positions[3] = float2[](
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
);

static float3 colors[3] = float3[](
    float3(1, 0, 0),
    float3(0, 1, 0),
    float3(0, 0, 1)
);

[shader("vertex")]

Vertex vertexMain(uint id : SV_VertexID) {
    Vertex vertex;

    vertex.svPosition = float4(positions[id], 0, 1);
    vertex.color = colors[id];

    return vertex;
}

[shader("fragment")]

float4 fragmentMain(Vertex vertex) : SV_Target {
    return float4(vertex.color, 1);
}
