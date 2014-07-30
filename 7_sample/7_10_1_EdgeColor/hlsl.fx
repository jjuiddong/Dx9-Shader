// ------------------------------------------------------------
// 윤곽추출
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// 전역변수
// ------------------------------------------------------------
float4x4 mWVP;
float4   vCol;
float4	 vLightDir;	// 광원방향

// ------------------------------------------------------------
// 텍스처
// ------------------------------------------------------------
texture SrcTex;
sampler SrcSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// ------------------------------------------------------------
sampler FloorSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Wrap;
};
// ------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float2 Tex0			: TEXCOORD0;
};

// ------------------------------------------------------------
// 정점셰이더(조명없음)
// ------------------------------------------------------------
VS_OUTPUT VS_pass0 (
      float4 Pos    : POSITION           // 모델정점
     ,float4 Normal : NORMAL             // 법선벡터
     ,float4 Tex0   : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
	// 위치좌표
	Out.Pos = mul( Pos, mWVP );
	
	// 텍스처좌표
    Out.Tex0 = Tex0;

    return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더(조명없음)
// ------------------------------------------------------------
float4 PS_pass0 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// 색
	Out = tex2D( FloorSamp, In.Tex0 );
	
	return Out;
}
// ------------------------------------------------------------
// 정점셰이더(조명있음)
// ------------------------------------------------------------
VS_OUTPUT VS_pass1 (
      float4 Pos    : POSITION           // 모델정점
    , float4 Normal : NORMAL             // 법선벡터
     ,float4 Tex0   : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
	// 위치좌표
	Out.Pos = mul( Pos, mWVP );
	
	// 색
	float diffuse = max(dot(vLightDir.xyz, Normal.xyz), 0);//확산광
	float ambient = vLightDir.w;						   //환경광
	Out.Color = vCol * ( diffuse + ambient );
	
	// 텍스처좌표
    Out.Tex0 = Tex0;

    return Out;
}

// ------------------------------------------------------------
// 픽셀셰이더(조명있음)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// 색
	Out = In.Color * tex2D( FloorSamp, In.Tex0 );
	
	return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더(윤곽추출)
// ------------------------------------------------------------
PixelShader PS_NormalEdge = asm
{
    ps_1_1
    
	tex t0	// 현재프레임(좌측상단)
	tex t1	// 현재프레임(우측하단)
	tex t2	// 현재프레임(좌측하단)
	tex t3	// 현재프레임(우측상단)

	// RGB 각각의 휘도차
	sub_x2 r0,  t0, t1
	sub_x2 r1,  t2, t3

	// 절대값의 근사값을 얻기위해서 제곱
	dp3_x4 r0,  r0, r0
	dp3_x4 r1,  r1, r1

	// (1 - 휘도차)로 흰색바탕에 검은엣지
	add r0, 1-r0, -r1	// 1 - r0 - r1
};

// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique TShader
{
    pass P0 // 조명계산없음
    {
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_1_1 PS_pass0();
    }
    pass P1 // 조명계산있음
    {
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_1_1 PS_pass1();
    }
    pass P2 // 엣지
    {
        // 셰이더
        PixelShader  = <PS_NormalEdge>;
		// 텍스처
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
	}
}
