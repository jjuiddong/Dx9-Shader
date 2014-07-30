// -------------------------------------------------------------
// 우선순위버퍼 그림자
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWLP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWVPT;		// 텍스처좌표계로의 투영
float4   vCol;		// 메시색
float4   vId;		// 우선순위버퍼 번호
float4	 vLightDir;	// 광원방향

// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
texture IdMap;
sampler IdMapSamp = sampler_state
{
    Texture = <IdMap>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
texture DecaleMap;
sampler DecaleMapSamp = sampler_state
{
    Texture = <DecaleMap>;
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
	float4 ShadowMapUV	: TEXCOORD0;
	float4 ID			: TEXCOORD1;
	float2 TexDecale	: TEXCOORD2;
};

// -------------------------------------------------------------
// 1패스:정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL	         // 모델법선
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 좌표변환
    Out.Pos =  mul( Pos, mWLP );
    
    // ID를 색으로 출력
    Out.Diffuse = vId;

    return Out;
}
// -------------------------------------------------------------
// 1패스:픽셀셰이더
// -------------------------------------------------------------
PIXELSHADER PS_pass0 = asm
{
    ps.1.1
    
    mov r0, v0	// 색을 ID로 해서 출력
};
// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL,	         // 모델법선
      float2 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
	float4	uv;
	
	// 좌표변환
    Out.Pos = mul(Pos, mWVP);
	// 색
	Out.Diffuse = vCol * max( dot(vLightDir, Normal), 0);// 확산광
	Out.Ambient = vCol * 0.3f;							 // 환경광
	
	// 텍스처좌표
	uv = mul(Pos, mWVPT);
	Out.ShadowMapUV = uv;

	// ID값
	Out.ID = vId;
	
	// 디컬 텍스처
	Out.TexDecale = Tex;
		
    return Out;
}
// -------------------------------------------------------------
// 2패스:픽셀셰이더(텍스처있음)
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{   
    float4 Color = In.Ambient;
    float4 zero = {0,0,0,0};
    float ofset = 1.0f/256.0f;
    
	float  id_map = tex2Dproj( IdMapSamp, In.ShadowMapUV );
	float4 decale = tex2D( DecaleMapSamp, In.TexDecale );
    
    Color += (id_map.x < In.ID.x+ofset && In.ID.x-ofset < id_map.x)
				 ? In.Diffuse : zero;

    return decale * Color;
}  
// -------------------------------------------------------------
// 2패스:픽셀셰이더(텍스처없음)
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT In) : COLOR
{   
    float4 Color = In.Ambient;
    float4 zero = {0,0,0,0};
    float ofset = 0.01f;
    
	float  id_map = tex2Dproj( IdMapSamp, In.ShadowMapUV );
	float4 decale = tex2D( DecaleMapSamp, In.TexDecale );
    
    Color += (id_map.x < In.ID.x+ofset && In.ID.x-ofset < id_map.x)
				 ? In.Diffuse : zero;

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
        PixelShader  = <PS_pass0>;
    }
    pass P1
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_pass1();
    }
    pass P2
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_pass2();
    }
}
