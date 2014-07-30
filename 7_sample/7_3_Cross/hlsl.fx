// -------------------------------------------------------------
// ũ�ν�����
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------

float4x4 mWVP;		// ��ǥ��ȯ���

float4 vLightDir;	// ��������
float4 vColor;		// ����*�޽û�
float3 vEyePos;		// ī�޶���ġ(������ǥ��)


static const int    MAX_SAMPLES = 16;    // �ִ���ø���
float2 g_avSampleOffsets[MAX_SAMPLES];	// ���ø���ġ
float4 g_avSampleWeights[MAX_SAMPLES];	// ���ø�����ġ

// -------------------------------------------------------------
// �ؽ���
// -------------------------------------------------------------
// �����ؽ�ó
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
// ������
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
// ���÷��� SetTexture ����
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
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;		// ������
    float2 Tex			: TEXCOORD0;	// �����ؽ�ó
    float3 L			: TEXCOORD1;	// ��������
    float3 E			: TEXCOORD2;	// ��������
};
// -------------------------------------------------------------
// ��鷻��
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos      : POSITION,          // ������ġ��ǥ
      float3 Normal   : NORMAL,            // ��������
      float3 Tangent  : TANGENT0,          // ��������
      float2 Texcoord : TEXCOORD0          // �ؽ�ó��ǥ
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
	
	// ��ǥ��ȯ
	Out.Pos = mul(Pos, mWVP);
	
	// �޽û�
	Out.Color = vColor;
	
	// ���ÿ� �ؽ�ó��ǥ
	Out.Tex = Texcoord;

	// ��ǥ�躯ȯ ����
	float3 N = Normal;
	float3 T = Tangent;
	float3 B = cross(N,T);

	// �ݿ��ݻ�� ����
	float3 E = vEyePos - Pos.xyz;	// �ü�����
	Out.E.x = dot(E,T);
	Out.E.y = dot(E,B);
	Out.E.z = dot(E,N);

	float3 L = -vLightDir.xyz;		// ��������
	Out.L.x = dot(L,T);
	Out.L.y = dot(L,B);
	Out.L.z = dot(L,N);
	
	return Out;
}
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
	float3 N = 2.0f*tex2D( NormalSamp, In.Tex ).xyz-1.0;// �����ʿ����� ����
	float3 L = normalize(In.L);						// ��������
	float3 R = reflect(-normalize(In.E), N);		// �ݻ纤��
	float amb = -vLightDir.w;						// ȯ�汤 ����
	
    return In.Color * tex2D( DecaleSamp, In.Tex )	// Ȯ�걤�� ȯ�汤��
			   * (max(0, dot(N, L))+amb)			// �������� �ؽ�ó���� �ռ��Ѵ�
			 + 2.0f * pow(max(0,dot(R, L)), 64);		// �� �ݿ��ݻ籤
}




//-----------------------------------------------------------------------------
// Name: DownScale4x4
// Desc: 1/4��ҹ��ۿ� ��� ����
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
// Desc: �����κи� ����
//-----------------------------------------------------------------------------
float4 BrightPassFilter(in float2 uv : TEXCOORD0) : COLOR
{
	float4 vSample = tex2D( s0, uv );
	
	// ��ο� �κ� ����
	vSample.rgb -= 1.5f;
	
	// ���Ѱ��� 0����
	vSample = 3.0f*max(vSample, 0.0f);

	return vSample;
}




//-----------------------------------------------------------------------------
// Name: GaussBlur5x5
// Desc: �߽� ��ó�� 13�� �ؼ��� ���ø�
//       (����� ���콺�����̹Ƿ� ��������� ���콺 ����� �ȴ�)
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
// Desc: 8���� ���ø��ؼ� ������ �����
//-----------------------------------------------------------------------------
float4 Star ( in float2 uv : TEXCOORD0 ) : COLOR
{
    float4 vColor = 0.0f;
    
    // ������ 8���� ���� ���ø�
    for(int i = 0; i < 8; i++) {
        vColor += g_avSampleWeights[i] * tex2D(s0, uv + g_avSampleOffsets[i]);
    }
    	
    return vColor;
}




//-----------------------------------------------------------------------------
// Name: MergeTextures_6
// Desc: 6���� ������ �ߺ��ռ�
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
// ��ũ��
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
// Desc: 1/4��ҹ��ۿ� ����� ����
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
// Desc: �����κи� ����
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
// Desc: 13�ؼ� ���ø����� ���콺 ������ ����
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
// Desc: 8���ø����� ���������
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
// Desc: 6���� ���� �ߺ��ռ�
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
