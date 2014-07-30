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
float time;

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
    AddressV = Wrap;
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
	Out.Col = max(0,dot(Normal, LightDir.xyz)) + LightDir.w;

	//��ġ�� ������ ��ȯ�Ѵ�
	Out.Tex = 0.5f*Pos+0.5;

	return Out;
}
// -------------------------------------------------------------
// �� ��Ÿ���� ������ �Լ�
// -------------------------------------------------------------
float noise( float gap, float3 pos )
{
	float3 x = gap * pos;
	float3 ix = floor(x)/32.0f;
	float3 fx = frac(x);
	
	float4 x0, x1;

	x0.x = tex3D( Samp, ix + float3(0.f/32.f, 0.f/32.f, 0.f/32.f) ).x;
	x0.y = tex3D( Samp, ix + float3(1.f/32.f, 0.f/32.f, 0.f/32.f) ).x;
	x0.z = tex3D( Samp, ix + float3(0.f/32.f, 1.f/32.f, 0.f/32.f) ).x;
	x0.w = tex3D( Samp, ix + float3(1.f/32.f, 1.f/32.f, 0.f/32.f) ).x;
	
	x1.x = tex3D( Samp, ix + float3(0.f/32.f, 0.f/32.f, 1.f/32.f) ).x;
	x1.y = tex3D( Samp, ix + float3(1.f/32.f, 0.f/32.f, 1.f/32.f) ).x;
	x1.z = tex3D( Samp, ix + float3(0.f/32.f, 1.f/32.f, 1.f/32.f) ).x;
	x1.w = tex3D( Samp, ix + float3(1.f/32.f, 1.f/32.f, 1.f/32.f) ).x;
	
	x0    = lerp(x0.xyzw, x1.xyzw, fx.z);
	x0.xy = lerp(x0.xy,   x0.zw,   fx.y);
	return  lerp(x0.x,    x0.y,    fx.x);
}
// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
	float4 color = {167.f/256.f, 105.f/256.f, 61.f/256.f, 0};

	float n = 0.5000f * noise( 12.f, In.Tex )
			+ 0.2500f * noise( 24.f, In.Tex );

	return In.Col * tex2D( WoodSamp, n+time );
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
