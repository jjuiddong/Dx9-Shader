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
float    fNear=1.0f;
float    fFar =7.0f;

// ------------------------------------------------------------
// 텍스처
// ------------------------------------------------------------
texture SrcTex;
sampler SrcSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = POINT;
    MagFilter = POINT;
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
	
	// 법선
	Out.Color.w = (pos.w-fNear)/(fFar-fNear);
	
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
	// ID
	Out.a = In.Color.w;
	
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
	float diffuse = max(dot(vLightDir.xyz, Normal.xyz), 0);//확산색
	float ambient = vLightDir.w;						   //환경색
	Out.Color = vCol * ( diffuse + ambient );
	
    Out.Tex0 = Tex0;

	// 법선
	Out.Color.w = (pos.w-fNear)/(fFar-fNear);

    return Out;
}

// ------------------------------------------------------------
// 픽셀셰이더(조명있음)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// 색
	Out.rgb = In.Color * tex2D( FloorSamp, In.Tex0 );
	// ID
	Out.a = In.Color.w;
	
	return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더(윤곽추출)
// ------------------------------------------------------------
PixelShader PS_ZEdge = asm
{
    ps_1_1
    
	tex t0	// 현재프레임(좌측상단)
	tex t1	// 현재프레임(우측하단)
	tex t2	// 현재프레임(좌측하단)
	tex t3	// 현재프레임(우측상단)
	
	// 인수로 t#레지스터를 2개 사용할수 없으므로 일단 복사
	mov r0, t0
	mov r1, t2
	
	add_x4     r0,   r0,  -t1	;       r0                  r1
	add_x4     r1,   r1,  -t3	; 4*(t0.a-t1.a)        4*(t2.a-t3.a)
	mul_x4     r0,   r0,   r0	;
	mul_x4     r1,   r1,   r1	; 16*(t0-t1깊이)^2,  16*(t2-t3깊이)^2)
	add_x4     r0,   r0,   r1	; r0.a = 64((t0-t0깊이)^2+(t3-t1깊이)^2)
	mov        r0,   1-r0.a		; r0.a = (1-64((t0-t0깊이)^2+(t3-t1깊이)^2))
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
        PixelShader  = <PS_ZEdge>;
		// 텍스처
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
	}
}
