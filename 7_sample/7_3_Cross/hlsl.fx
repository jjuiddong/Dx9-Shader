// -------------------------------------------------------------
// 크로스필터
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------

float4x4 mWVP;		// 좌표변환행렬

float4 vLightDir;	// 광원방향
float4 vColor;		// 조명*메시색
float3 vEyePos;		// 카메라위치(로컬좌표계)


static const int    MAX_SAMPLES = 16;    // 최대샘플링수
float2 g_avSampleOffsets[MAX_SAMPLES];	// 샘플링위치
float4 g_avSampleWeights[MAX_SAMPLES];	// 샘플링가중치

// -------------------------------------------------------------
// 텍스쳐
// -------------------------------------------------------------
// 디컬텍스처
texture DecaleTex;
sampler DecaleSamp = sampler_state
{
    Texture = <DecaleTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
// 법선맵
texture NormalMap;
sampler NormalSamp = sampler_state
{
    Texture = <NormalMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Wrap;
};

//-----------------------------------------------------------------------------
// 샘플러와 SetTexture 대응
//-----------------------------------------------------------------------------
sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);
sampler s5 : register(s5);
sampler s6 : register(s6);
sampler s7 : register(s7);

// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;		// 정점색
    float2 Tex			: TEXCOORD0;	// 디컬텍스처
    float3 L			: TEXCOORD1;	// 광원벡터
    float3 E			: TEXCOORD2;	// 법선벡터
};
// -------------------------------------------------------------
// 장면렌더
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos      : POSITION,          // 로컬위치좌표
      float3 Normal   : NORMAL,            // 법선벡터
      float3 Tangent  : TANGENT0,          // 접선벡터
      float2 Texcoord : TEXCOORD0          // 텍스처좌표
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
	
	// 좌표변환
	Out.Pos = mul(Pos, mWVP);
	
	// 메시색
	Out.Color = vColor;
	
	// 디컬용 텍스처좌표
	Out.Tex = Texcoord;

	// 좌표계변환 기저
	float3 N = Normal;
	float3 T = Tangent;
	float3 B = cross(N,T);

	// 반영반사용 벡터
	float3 E = vEyePos - Pos.xyz;	// 시선벡터
	Out.E.x = dot(E,T);
	Out.E.y = dot(E,B);
	Out.E.z = dot(E,N);

	float3 L = -vLightDir.xyz;		// 광원벡터
	Out.L.x = dot(L,T);
	Out.L.y = dot(L,B);
	Out.L.z = dot(L,N);
	
	return Out;
}
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
	float3 N = 2.0f*tex2D( NormalSamp, In.Tex ).xyz-1.0;// 법선맵에서의 법선
	float3 L = normalize(In.L);						// 광원벡터
	float3 R = reflect(-normalize(In.E), N);		// 반사벡터
	float amb = -vLightDir.w;						// 환경광 강도
	
    return In.Color * tex2D( DecaleSamp, In.Tex )	// 확산광과 환경광은
			   * (max(0, dot(N, L))+amb)			// 정점색과 텍스처색을 합성한다
			 + 2.0f * pow(max(0,dot(R, L)), 64);		// 퐁 반영반사광
}




//-----------------------------------------------------------------------------
// Name: DownScale4x4
// Desc: 1/4축소버퍼에 장면 복사
//-----------------------------------------------------------------------------
float4 DownScale4x4 ( in float2 uv : TEXCOORD0 ) : COLOR
{
    float4 sample = 0.0f;

	for( int i=0; i < 16; i++ ) {
		sample += tex2D( s0, uv + g_avSampleOffsets[i] );
	}
    
	return sample / 16;
}



//-----------------------------------------------------------------------------
// Name: BrightPassFilter
// Desc: 밝은부분만 추출
//-----------------------------------------------------------------------------
float4 BrightPassFilter(in float2 uv : TEXCOORD0) : COLOR
{
	float4 vSample = tex2D( s0, uv );
	
	// 어두운 부분 제외
	vSample.rgb -= 1.5f;
	
	// 하한값을 0으로
	vSample = 3.0f*max(vSample, 0.0f);

	return vSample;
}




//-----------------------------------------------------------------------------
// Name: GaussBlur5x5
// Desc: 중심 근처의 13개 텍셀을 샘플링
//       (계수가 가우스분포이므로 결과적으로 가우스 평균이 된다)
//-----------------------------------------------------------------------------
float4 GaussBlur5x5 (in float2 uv : TEXCOORD0) : COLOR
{
    float4 sample = 0.0f;

	for( int i=0; i < 13; i++ ) {
		sample += g_avSampleWeights[i]
					 * tex2D( s0, uv + g_avSampleOffsets[i] );
	}

	return sample;
}




//-----------------------------------------------------------------------------
// Name: Star
// Desc: 8개를 샘플링해서 광선을 만든다
//-----------------------------------------------------------------------------
float4 Star ( in float2 uv : TEXCOORD0 ) : COLOR
{
    float4 vColor = 0.0f;
    
    // 광선에 8개의 점을 샘플링
    for(int i = 0; i < 8; i++) {
        vColor += g_avSampleWeights[i] * tex2D(s0, uv + g_avSampleOffsets[i]);
    }
    	
    return vColor;
}




//-----------------------------------------------------------------------------
// Name: MergeTextures_6
// Desc: 6장의 광선을 중복합성
//-----------------------------------------------------------------------------
float4 MergeTextures_6(in float2 uv : TEXCOORD0 ) : COLOR
{
	float4 vColor = 0.0f;
	
	vColor = ( tex2D(s0, uv)
	         + tex2D(s1, uv)
	         + tex2D(s2, uv)
	         + tex2D(s3, uv)
	         + tex2D(s4, uv)
	         + tex2D(s5, uv) )/6.0f;
		
	return vColor;
}





// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}




//-----------------------------------------------------------------------------
// Name: DownScale4x4
// Type: Technique                                     
// Desc: 1/4축소버퍼에 장면을 복사
//-----------------------------------------------------------------------------
technique DownScale4x4
{
    pass P0
    {
        PixelShader  = compile ps_2_0 DownScale4x4();
        MinFilter[0] = Point;
        AddressU[0] = Clamp;
        AddressV[0] = Clamp;
    }
}




//-----------------------------------------------------------------------------
// Name: BrightPassFilter
// Desc: 밝은부분만 추출
//-----------------------------------------------------------------------------
technique BrightPassFilter
{
    pass P0
    {
        PixelShader  = compile ps_2_0 BrightPassFilter();
        MinFilter[0] = Point;
        MagFilter[0] = Point;
    }
}





//-----------------------------------------------------------------------------
// Name: GaussBlur5x5
// Desc: 13텍셀 샘플링으로 가우스 뭉개기 실행
//-----------------------------------------------------------------------------
technique GaussBlur5x5
{
    pass P0
    {
        PixelShader  = compile ps_2_0 GaussBlur5x5();
        MinFilter[0] = Point;
        AddressU[0] = Clamp;
        AddressV[0] = Clamp;
    }
}




//-----------------------------------------------------------------------------
// Name: Star
// Desc: 8샘플링으로 광선만들기
//-----------------------------------------------------------------------------
technique Star
{
    pass P0
    {
        PixelShader  = compile ps_2_0 Star();
        MagFilter[0] = Linear;
        MinFilter[0] = Linear;
    }

}




//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Desc: 6개의 광선 중복합성
//-----------------------------------------------------------------------------
technique MergeTextures
{
    pass P0
    {
        PixelShader  = compile ps_2_0 MergeTextures_6();
        MagFilter[0] = Point;
        MinFilter[0] = Point;
        MagFilter[1] = Point;
        MinFilter[1] = Point;
        MagFilter[2] = Point;
        MinFilter[2] = Point;
        MagFilter[3] = Point;
        MinFilter[3] = Point;
        MagFilter[4] = Point;
        MinFilter[4] = Point;
        MagFilter[5] = Point;
        MinFilter[5] = Point;
    }
}
