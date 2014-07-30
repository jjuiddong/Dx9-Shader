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
float4   vCenter;
float4   vScale;

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
texture BlurTex;
sampler BlurSamp = sampler_state
{
    Texture = <BlurTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
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
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
	float2 Tex2			: TEXCOORD2;
	float2 Tex3			: TEXCOORD3;
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
	
	Out.Color = pos.z/pos.w;// 색성분에 깊이값
	
    Out.Tex0 = Tex;		// 텍스처좌표

    return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더(조명없음)
// ------------------------------------------------------------
float4 PS_pass0 (VS_OUTPUT In) : COLOR
{
	float4 Out = tex2D( FloorSamp, In.Tex0 );
	
	Out.a = In.Color.w;		// 알파성분에 깊이값
	
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
    
	Out.Pos = pos;						// 위치좌표
	
	// 색
	Out.Color = vCol * ( vLightDir.w					 //환경광
			   + max(dot(vLightDir.xyz, Normal.xyz), 0));//확산광
	
	Out.Color.w = pos.z/pos.w;			// 깊이값
    Out.Tex0 = Tex0;					// 텍스처좌표

    return Out;
}

// ------------------------------------------------------------
// 픽셀셰이더(조명있음)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out = In.Color * tex2D( SrcSamp, In.Tex0 );
	
	return Out;
}
// ------------------------------------------------------------
// 정점셰이더(뭉개기)
// ------------------------------------------------------------
VS_OUTPUT VS_pass2 (
      float4 Pos    : POSITION           // 모델정점
     ,float4 Tex0   : TEXCOORD0	         // 텍스처좌표
     ,float4 Tex1   : TEXCOORD1	         // 텍스처좌표
     ,float4 Tex2   : TEXCOORD2	         // 텍스처좌표
     ,float4 Tex3   : TEXCOORD3	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    // 테스처좌표
    Out.Tex0 = Tex0;
    Out.Tex1 = Tex1;
    Out.Tex2 = Tex2;
    Out.Tex3 = Tex3;
    
    return Out;
}

// ------------------------------------------------------------
// 픽셀셰이더(뭉개기)
// ------------------------------------------------------------
float4 PS_pass2 (VS_OUTPUT In) : COLOR
{
	float4 col0 = tex2D( SrcSamp, In.Tex0 );
	float4 col1 = tex2D( SrcSamp, In.Tex1 );
	float4 col2 = tex2D( SrcSamp, In.Tex2 );
	float4 col3 = tex2D( SrcSamp, In.Tex3 );
	
	return 0.25 * (col0 + col1 + col2 + col3);
}


// ------------------------------------------------------------
// 픽셀셰이더(합성)
// ------------------------------------------------------------
float4 PS_pass3 (VS_OUTPUT In) : COLOR
{
	float t1, t2, t3;
	
	float4 col0 = tex2D( SrcSamp,  In.Tex0 ); // 중심
	float4 col1 = tex2D( BlurSamp, In.Tex1 ); // 샘플링점1
	float4 col2 = tex2D( BlurSamp, In.Tex2 ); // 샘플링점2
	float4 col3 = tex2D( BlurSamp, In.Tex3 ); // 샘플링점3
	float a0 = col0.a-vCenter.w; // 중심의 뭉개짐 상태
	float a1 = col1.a-vCenter.w; // 샘플링점 1의 뭉개짐 상태
	float a2 = col2.a-vCenter.w; // 샘플링점 2의 뭉개짐 상태
	float a3 = col3.a-vCenter.w; // 샘플링점 3의 뭉개짐 상태
	
	// 합성가중치 계산
	t1 = a1;
	// 샘플링점 1이 안쪽에 있으면 중심의 뭉개진 상태를 고려해 넣는다
	if(a0<a1) t1 *= a0;
	t2 = a2;
	// 샘플링점 2가 안쪽에 있으면 중심의 뭉개진 상태를 고려해 넣는다
	if(a0<a2) t2 *= a0;
	t3 = a3;
	// 샘플링점 3이 안쪽에 있으면 중심의 뭉개진 상태를 고려해 넣는다
	if(a0<a3) t3 *= a0;
	
	// 적당히 스케일링
	float s = 10000.0f*vScale.w;
	t1 = saturate(s * t1*t1);
	t2 = saturate(s * t2*t2);
	t3 = saturate(s * t3*t3);
	
	// 합성
	return  ( lerp(col0, col1, t1)
			+ lerp(col0, col2, t2)
			+ lerp(col0, col3, t3))/3;
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
    pass P2	// 뭉개기
    {
        VertexShader = compile vs_1_1 VS_pass2();
        PixelShader  = compile ps_1_1 PS_pass2();
        AddressU[0] = Clamp;
        AddressV[0] = Clamp;
        AddressU[1] = Clamp;
        AddressV[1] = Clamp;
        AddressU[2] = Clamp;
        AddressV[2] = Clamp;
        AddressU[3] = Clamp;
        AddressV[3] = Clamp;
    }
    pass P3	// 합성
    {
        PixelShader  = compile ps_2_0 PS_pass3();
        MinFilter[0] = Point;
        MagFilter[0] = Point;
        MinFilter[1] = Linear;
        MagFilter[1] = Linear;
        MinFilter[2] = Linear;
        MagFilter[2] = Linear;
        MinFilter[3] = Linear;
        MagFilter[3] = Linear;
	}
}
