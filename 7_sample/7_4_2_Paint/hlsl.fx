// -------------------------------------------------------------
// 잉크렌더링
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------

float  MAP_WIDTH;	// SAT 폭
float  MAP_HEIGHT;	// SAT 높이

float SIZE = 40.0f;	// 뭉갤양
float INV_SIZE2=1.0f/(40.0f*40.0f);	// 뭉갤양의 역수의 제곱

// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
texture SrcMap;
sampler SrcSamp = sampler_state
{
    Texture = <SrcMap>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
texture WeightMap;
sampler WeightSamp = sampler_state
{
    Texture = <WeightMap>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float2 ofset		: COLOR0;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
	float2 Tex2			: TEXCOORD2;
	float2 Tex3			: TEXCOORD3;
};

// -------------------------------------------------------------
// -------------------------------------------------------------
// 영역 총합 테이블 만들기
// -------------------------------------------------------------
// -------------------------------------------------------------

// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_sat_x (
      float4 Pos    : POSITION,          // 모델정점
      float4 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    
    // 텍스처좌표 // 중심
    Out.Tex0 = Tex + float2( 0.5f/MAP_WIDTH, 0.5f/MAP_HEIGHT );
    Out.Tex1 = Tex + float2(-0.5f/MAP_WIDTH, 0.5f/MAP_HEIGHT );// 左
    
    return Out;
}
// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS_sat_x(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    float4 weight = tex2D( WeightSamp, In.Tex0 );
    
    Color  = tex2D( SrcSamp, In.Tex0 ) * weight
           + tex2D( SrcSamp, In.Tex1 );
           
    return Color;
}

// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_sat_y (
      float4 Pos    : POSITION,          // 모델정점
      float4 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    
    // 텍스처좌표
    Out.Tex0 = Tex + float2( 0.5f/MAP_WIDTH, 0.5f/MAP_HEIGHT );
    Out.Tex1 = Tex + float2( 0.5f/MAP_WIDTH,-0.5f/MAP_HEIGHT );// 上
    
    return Out;
}

// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS_sat_y(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    
    Color  = tex2D( SrcSamp, In.Tex0 )
           + tex2D( SrcSamp, In.Tex1 );
           
    return Color;
}

// -------------------------------------------------------------
// -------------------------------------------------------------
// 최종출력
// -------------------------------------------------------------
// -------------------------------------------------------------

// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_out (
      float4 Pos    : POSITION,          // 모델정점
      float4 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    
    Out.Tex0 = Tex + float2( (0.5f-0.5f*SIZE)/MAP_WIDTH, (0.5f-0.5f*SIZE)/MAP_HEIGHT );
    Out.Tex1 = Tex + float2( (0.5f-0.5f*SIZE)/MAP_WIDTH, (0.5f+0.5f*SIZE)/MAP_HEIGHT );
    Out.Tex2 = Tex + float2( (0.5f+0.5f*SIZE)/MAP_WIDTH, (0.5f-0.5f*SIZE)/MAP_HEIGHT );
    Out.Tex3 = Tex + float2( (0.5f+0.5f*SIZE)/MAP_WIDTH, (0.5f+0.5f*SIZE)/MAP_HEIGHT );
    
    return Out;
}

// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS_out(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    float  weight;
    
    // 가중치총합을 구한다
    weight =  tex2D( SrcSamp, In.Tex0 ).a
            - tex2D( SrcSamp, In.Tex1 ).a
            - tex2D( SrcSamp, In.Tex2 ).a
            + tex2D( SrcSamp, In.Tex3 ).a;
            
    // SAT값을 가중치총합으로 나누어 색을 구한다
    Color  =( tex2D( SrcSamp, In.Tex0 )
            - tex2D( SrcSamp, In.Tex1 )
            - tex2D( SrcSamp, In.Tex2 )
            + tex2D( SrcSamp, In.Tex3 ))/weight;

    return Color;
}

// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
		// -----------------------------------------------------
		// X축 뭉개기
		// -----------------------------------------------------
        VertexShader = compile vs_1_1 VS_sat_x();
        PixelShader  = compile ps_2_0 PS_sat_x();
    }
    pass P1
    {
		// -----------------------------------------------------
		// Y축 뭉개기
		// -----------------------------------------------------
        VertexShader = compile vs_1_1 VS_sat_y();
        PixelShader  = compile ps_2_0 PS_sat_y();
    }
    pass P2
    {
		// -----------------------------------------------------
		// 출력
		// -----------------------------------------------------
        VertexShader = compile vs_1_1 VS_out();
        PixelShader  = compile ps_2_0 PS_out();
    }
}
