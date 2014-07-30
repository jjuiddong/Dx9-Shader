// -------------------------------------------------------------
// 부드러운 그림자
// 
// Copyright (c) 2002-2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWLP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWLPB;		// 텍스처좌표계로의 투영
float4   vCol;		// 메시색
float4	 vLightDir;	// 광원방향

static float MAP_SIZE = 512.0f;

// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
texture ShadowMap;
sampler ShadowMapSamp = sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
texture SrcMap;
sampler SrcSamp = sampler_state
{
    Texture = <SrcMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
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
	float4 Diffuse		: COLOR0;
	float4 Ambient		: COLOR1;
	float4 Depth		: TEXCOORD0;
	float4 ShadowMapUV	: TEXCOORD1;
};
// -------------------------------------------------------------
struct VS_OUTPUT_QUAD
{
    float4 Pos			: POSITION;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
	float2 Tex2			: TEXCOORD2;
	float2 Tex3			: TEXCOORD3;
	float2 Tex4			: TEXCOORD4;
	float2 Tex5			: TEXCOORD5;
	float2 Tex6			: TEXCOORD6;
	float2 Tex7			: TEXCOORD7;
};

// -------------------------------------------------------------
// 1패스:깊이맵 생성
// -------------------------------------------------------------

// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL	         // 모델법선
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 좌표변환
	float4 pos = mul( Pos, mWLP );
	
    // 위치좌표
    Out.Pos = pos;
    
    // 카메라좌표계에서의 깊이를 텍스처에 넣는다
    Out.Depth = pos.zzzw;

    return Out;
}
// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{   
    float4 Color = In.Depth / In.Depth.w;

    return Color;
}  

// -------------------------------------------------------------
// 2패스:윤곽추출
// -------------------------------------------------------------

// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT_QUAD VS_pass1(
      float4 Pos    : POSITION,          // 모델정점
      float4 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT_QUAD Out = (VS_OUTPUT_QUAD)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    
    Out.Tex0 = Tex + float2(-0.5f/MAP_SIZE, -0.5f/MAP_SIZE);
    Out.Tex1 = Tex + float2(+0.5f/MAP_SIZE, +0.5f/MAP_SIZE);
    Out.Tex2 = Tex + float2(-0.5f/MAP_SIZE, +0.5f/MAP_SIZE);
    Out.Tex3 = Tex + float2(+0.5f/MAP_SIZE, -0.5f/MAP_SIZE);

    return Out;
}
// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT_QUAD In) : COLOR
{   
    float4 Color;
	
	float d0 = tex2D( SrcSamp, In.Tex0 ) - tex2D( SrcSamp, In.Tex1 );
	float d1 = tex2D( SrcSamp, In.Tex2 ) - tex2D( SrcSamp, In.Tex3 );
	
	Color = d0*d0 + d1*d1;
	
    return Color;
}
// -------------------------------------------------------------
// 3패스:흐리게 뭉개기
// -------------------------------------------------------------

// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT_QUAD VS_pass2(
      float4 Pos    : POSITION,          // 모델정점
      float4 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT_QUAD Out = (VS_OUTPUT_QUAD)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    
    Out.Tex0 = Tex + float2(-3.0f/MAP_SIZE, -3.0f/MAP_SIZE);
    Out.Tex1 = Tex + float2(-3.0f/MAP_SIZE, -1.0f/MAP_SIZE);
    Out.Tex2 = Tex + float2(-3.0f/MAP_SIZE,  1.0f/MAP_SIZE);
    Out.Tex3 = Tex + float2(-3.0f/MAP_SIZE,  3.0f/MAP_SIZE);
    Out.Tex4 = Tex + float2(-1.0f/MAP_SIZE, -3.0f/MAP_SIZE);
    Out.Tex5 = Tex + float2(-1.0f/MAP_SIZE, -1.0f/MAP_SIZE);
    Out.Tex6 = Tex + float2(-1.0f/MAP_SIZE,  1.0f/MAP_SIZE);
    Out.Tex7 = Tex + float2(-1.0f/MAP_SIZE,  3.0f/MAP_SIZE);

    return Out;
}

// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT_QUAD In) : COLOR
{   
    float4 Color;
    float2 dvu = float2( 4.0f/MAP_SIZE, 0 );
	
	Color =   tex2D( SrcSamp, In.Tex0 )
			+ tex2D( SrcSamp, In.Tex1 )
			+ tex2D( SrcSamp, In.Tex2 )
			+ tex2D( SrcSamp, In.Tex3 )
			+ tex2D( SrcSamp, In.Tex4 )
			+ tex2D( SrcSamp, In.Tex5 )
			+ tex2D( SrcSamp, In.Tex6 )
			+ tex2D( SrcSamp, In.Tex7 )
			+ tex2D( SrcSamp, In.Tex0 + dvu)
			+ tex2D( SrcSamp, In.Tex1 + dvu)
			+ tex2D( SrcSamp, In.Tex2 + dvu)
			+ tex2D( SrcSamp, In.Tex3 + dvu)
			+ tex2D( SrcSamp, In.Tex4 + dvu)
			+ tex2D( SrcSamp, In.Tex5 + dvu)
			+ tex2D( SrcSamp, In.Tex6 + dvu)
			+ tex2D( SrcSamp, In.Tex7 + dvu)
			;
	
    return 0.7f * Color;
}
// -------------------------------------------------------------
// 4패스:합성
// -------------------------------------------------------------

// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass3(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL	         // 모델법선
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
	float4	uv;
	
	// 좌표변환
    Out.Pos = mul(Pos, mWVP);
	// 색
	Out.Diffuse = vCol * max( dot(vLightDir, Normal), 0);// 확산광
	Out.Ambient = vCol * 0.3f;                     // 환경광
	
	// 텍스처좌표
	uv = mul(Pos, mWLPB);
	Out.ShadowMapUV = uv;
	uv = mul(Pos, mWLP);
	Out.Depth       = uv.zzzw;
		
    return Out;
}
// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass3(VS_OUTPUT In) : COLOR
{   
    float4 Color = In.Ambient;
    
	float  shadow_map = tex2Dproj( ShadowMapSamp, In.ShadowMapUV ).x;
    
    Color += In.Diffuse*((shadow_map < In.Depth.z/In.Depth.w-0.01)
					 ? tex2Dproj( SrcSamp, In.ShadowMapUV) : 1 );

    return Color;
}  
// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_2_0 PS_pass0();
    }
    pass P1
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_2_0 PS_pass1();
        
		Sampler[0] = (SrcSamp);
    }
    pass P2
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS_pass2();
        PixelShader  = compile ps_2_0 PS_pass2();
        
		Sampler[0] = (SrcSamp);
    }
    pass P3
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS_pass3();
        PixelShader  = compile ps_2_0 PS_pass3();
        
		Sampler[0] = (ShadowMapSamp);
		Sampler[1] = (SrcSamp);
    }
}
