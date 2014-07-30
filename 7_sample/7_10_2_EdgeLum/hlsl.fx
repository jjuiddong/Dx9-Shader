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
    
    float4 pos = mul( Pos, mWVP );
    
	// 위치좌표
	Out.Pos = pos;
	
    Out.Tex0 = Tex0;

    return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더(조명없음)
// ------------------------------------------------------------
float4 PS_pass0 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// 렌더링타겟 1:색
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
    
    float4 pos = mul( Pos, mWVP );
    
	// 위치좌표
	Out.Pos = pos;
	
	// 색
	Out.Color = vCol * ( vLightDir.w					 //환경광
			   + max(dot(vLightDir.xyz, Normal.xyz), 0));//확산광
	
    Out.Tex0 = Tex0;

    return Out;
}

// ------------------------------------------------------------
// 픽셀셰이더(조명있음)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// 렌더링타겟 1:색
	Out = In.Color;
	Out.rgb *= tex2D( FloorSamp, In.Tex0 );
	
	return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더(윤곽추출)
// ------------------------------------------------------------
PixelShader PS_Edge = asm
{
    ps_1_1
    
    def c0, 0.299f, 0.587f, 0.114f, 0.0f    ; 휘도 가중치
    
	tex t0	// 현재프레임(좌측상단)
	tex t1	// 현재프레임(우측하단)
	tex t2	// 현재프레임(좌측하단)
	tex t3	// 현재프레임(우측상단)

	dp3 r0,      t0, c0         ;         rgb        a
	dp3 r1,      t1, c0         ; r0 = (t3휘도, t0휘도)
	dp3 r0.rgb,  t3, c0         ; r1 = (t2휘도, t1휘도)
	dp3 r1.rgb,  t2, c0
	                            ;             rgb               a
	add_x4     r0,   r0,-r1     ; r0 =  4( t3-t2휘도,     t0-t1휘도)
	mul_x4     r0,   r0, r0     ; r0 = 64((t3-t2휘도)^2, (t0-t1휘도)^2)
	add_x4     r0, 1-r0,-r0.a   ; r0 = 4*(1-64((t3-t2휘도)^2+(t0-t1휘도)^2))
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
        PixelShader  = <PS_Edge>;
		// 텍스처
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
	}
}
