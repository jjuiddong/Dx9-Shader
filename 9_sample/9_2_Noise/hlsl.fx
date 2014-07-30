// -------------------------------------------------------------
// Perlin Noise
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWVP;		// ��ǥ��ȯ ���
float4 LightDir;		// ��������

// -------------------------------------------------------------
// �ؽ�ó
// -------------------------------------------------------------
// ���� �ؽ�ó
texture Tex;
sampler Samp = sampler_state
{
    Texture = <Tex>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Wrap;
};
texture WoodTex;
sampler WoodSamp = sampler_state
{
    Texture = <WoodTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Clamp;
};

// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Col			: COLOR0;
    float3 Tex			: TEXCOORD0;	// ���� �ؽ�ó��ǥ
};
// -------------------------------------------------------------
// ��ǥ��ȯ
// -------------------------------------------------------------
float2 Q(float3 v)
{
	float2 Out = (float2)0;
	
	float pv = 7.0f/8.0f;
	float ar = 1.0/(2.0*(1.000000001+v.y));

	Out.x = pv * ar * v.x + 0.5;
	Out.y = pv * ar * v.z + 0.5;

	return Out;
}

// -------------------------------------------------------------
// ��鷻��
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos      : POSITION,         // ������ġ��ǥ
      float3 Normal   : NORMAL            // ��������
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
	
	// ��ǥ��ȯ
	Out.Pos = mul(Pos, mWVP);
	
	// ����
	Out.Col = max(0,dot(Normal, LightDir.xyz)) - LightDir.w;

	//��ġ�� ������ ��ȯ�Ѵ�
	Out.Tex.xy = Q(normalize(Pos));

	return Out;
}
// -------------------------------------------------------------
// �� ��Ÿ���� ������ �Լ�
// -------------------------------------------------------------
float noise( float gap, float3 pos )
{
	float2 x = gap * pos;
	float2 ix = floor(x)/128.0f;
	float2 fx = frac(x);

	float x00 = tex2D( Samp, ix + float2(0.f/128.f, 0.f/128.f) ).x;
	float x10 = tex2D( Samp, ix + float2(1.f/128.f, 0.f/128.f) ).x;
	float x01 = tex2D( Samp, ix + float2(0.f/128.f, 1.f/128.f) ).x;
	float x11 = tex2D( Samp, ix + float2(1.f/128.f, 1.f/128.f) ).x;
	
	return lerp(lerp(x00, x01, fx.y), lerp(x10, x11, fx.y), fx.x);
}
// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
	float4 color = {167.f/256.f, 105.f/256.f, 61.f/256.f, 0};

	float n = 0.5000f * noise(  8.f, In.Tex )
			+ 0.2500f * noise( 16.f, In.Tex )
			+ 0.1250f * noise( 32.f, In.Tex )
			+ 0.0625f * noise( 64.f, In.Tex );

	return In.Col * tex2D( WoodSamp, float2(0.5, n) );
//	return color * (0.8f*fmod(10*n,1)+0.2);
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
