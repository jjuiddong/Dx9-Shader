// ------------------------------------------------------------
// 피사계심도
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// 전역변수
// ------------------------------------------------------------
float4x4 mWVP;		// 로컬-투영행렬
float4   vCol;		// 메시색
float4	 vLightDir;	// 광원방향
float    z_max;		// 후방 클립면까지의 거리
float    z_focus;	// 초점이 맞는 거리
float    d_max;		// f2/F*L : 무한원에서의 착란원

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
sampler FinalSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// ------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float2 Tex			: TEXCOORD0;
};

// ------------------------------------------------------------
// 정점셰이더(조명없음)
// ------------------------------------------------------------
VS_OUTPUT VS_pass0 (
      float4 Pos    : POSITION           // 모델정점
     ,float4 Tex   : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    float4 pos = mul( Pos, mWVP );
    
	Out.Pos = pos;			// 위치좌표
	
	Out.Color = pos.w / z_max;// 색성분에 깊이값
	
    Out.Tex = Tex;		// 텍스처좌표

    return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더(조명없음)
// ------------------------------------------------------------
float4 PS_pass0 (VS_OUTPUT In) : COLOR
{
	float4 Out = tex2D( FloorSamp, In.Tex );
	
	Out.a = In.Color.w;		// 알파성분에 깊이값
	
	return Out;
}
// ------------------------------------------------------------
// 정점셰이더(조명있음)
// ------------------------------------------------------------
VS_OUTPUT VS_pass1 (
      float4 Pos    : POSITION           // 모델정점
    , float4 Normal : NORMAL             // 법선벡터
     ,float4 Tex   : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    float4 pos = mul( Pos, mWVP );
    
	Out.Pos = pos;						// 위치좌표
	
	// 법선
	Out.Color.xyz = 0.5f*Normal.xyz+0.5f;
	// 깊이
	Out.Color.w = pos.w / z_max;						// 깊이값
    Out.Tex = Tex;										// 텍스처좌표

    return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더(조명있음)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	float4 tex = tex2D( SrcSamp, In.Tex );
	float4 N = 2.0f*In.Color - 1.0f;// 법선벡터
	N.w = 0;
	
	// 색
	Out = tex * vCol * (vLightDir.w + saturate(dot(vLightDir, N)));
	// 깊이
	Out.w = In.Color.w;
	
	return Out;
}


// ------------------------------------------------------------
// 픽셀셰이더(합성)
// ------------------------------------------------------------
#define NUM_DOF_TAPS 6					// 샘플링 개수
float2   filterTaps[NUM_DOF_TAPS];		// 샘플링점의 위치
float    filterDistance[NUM_DOF_TAPS];	// 샘플링점의 거리
// ------------------------------------------------------------
float4 PS_pass3 (VS_OUTPUT In) : COLOR
{
	float4 col0 = tex2D( FinalSamp,  In.Tex ); // 중심색과 깊이값
	float4 ret = col0;

	for(int i = 0; i < NUM_DOF_TAPS; i++){
		// 색과 깊이값을 샘플링한다
		float4 col = tex2D( FinalSamp, In.Tex + filterTaps[i] );
		// 착란원을 계싼한다
		float z = col.a;
		float a = z - z_focus;		// 초점과의 거리
		float d =  a * d_max / z;	// 안쪽에 있을때의 착란원
		if(a<0) d = -d;				// 눈앞에 있을때의 착란원
		
		// 착란원의 크기가 filterTaps로 확대한 양보다 크다면
		// 현재점은 뭉개야할 곳에 있는것이므로, 그 값을 사용한다.
		if(filterDistance[i]<d){
			ret += col;
		}else{
			ret += col0;
		}
	}
	
	return ret / (NUM_DOF_TAPS+1);
}



// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique TShader
{
    pass P0 // 일반렌더링 + 깊이렌더링
    {		// 조명없음
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_1_1 PS_pass0();
    }
    pass P1	// 일반렌더링 + 깊이렌더링
    {		// 조명있음
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_1_1 PS_pass1();
    }
    pass P2	// 합성
    {
        PixelShader  = compile ps_2_0 PS_pass3();
	}
}
