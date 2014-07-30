// ------------------------------------------------------------
// ��������
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// ��������
// ------------------------------------------------------------
float4x4 mWVP;
float4   vCol;
float4	 vLightDir;	// ��������
float    fNear=1.0f;
float    fFar =7.0f;

// ------------------------------------------------------------
// �ؽ�ó
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
// �������̴����� �ȼ����̴��� �ѱ�� ������
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
    float4 Normal		: COLOR1;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
	float2 Tex2			: TEXCOORD2;
	float2 Tex3			: TEXCOORD3;
};
// ------------------------------------------------------------
// �ȼ����̴� ��µ�����
// ------------------------------------------------------------
struct PS_OUTPUT
{
    float4 Color		: COLOR0;
    float4 Normal		: COLOR1;
};

// ------------------------------------------------------------
// �������̴�(�������)
// ------------------------------------------------------------
VS_OUTPUT VS_pass0 (
      float4 Pos    : POSITION           // ������
     ,float4 Normal : NORMAL             // ��������
     ,float4 Tex0   : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    float4 pos = mul( Pos, mWVP );
    
	// ��ġ��ǥ
	Out.Pos = pos;
	
	// ����
	Out.Normal = 0.5f*Normal+0.5f;// ���庯ȯ
	
    Out.Tex0 = Tex0;

    return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(�������)
// ------------------------------------------------------------
PS_OUTPUT PS_pass0 (VS_OUTPUT In) : COLOR
{
	PS_OUTPUT Out;
	
	// �����_�����O�^�[�Q�b�g�P�F�F
	Out.Color = tex2D( FloorSamp, In.Tex0 );
	
	// �����_�����O�^�[�Q�b�g�Q�F�@��
	Out.Normal =In.Normal;

	return Out;
}
// ------------------------------------------------------------
// �������̴�(��������)
// ------------------------------------------------------------
VS_OUTPUT VS_pass1 (
      float4 Pos    : POSITION           // ������
    , float4 Normal : NORMAL             // ��������
     ,float4 Tex0   : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    float4 pos = mul( Pos, mWVP );
    
	// ��ġ��ǥ
	Out.Pos = pos;
	
	// ��
	float diffuse = max(dot(vLightDir.xyz, Normal.xyz), 0);//Ȯ���
	float ambient = vLightDir.w;						   //ȯ���
	Out.Color = vCol * ( diffuse + ambient );
	
	// �ؽ�ó��ǥ
    Out.Tex0 = Tex0;

	// ����
	Out.Normal = 0.5f*Normal+0.5f;// ���庯ȯ

    return Out;
}

// ------------------------------------------------------------
// �ȼ����̴�(��������)
// ------------------------------------------------------------
PS_OUTPUT PS_pass1 (VS_OUTPUT In) : COLOR
{
	PS_OUTPUT Out;
	
	// ������Ÿ�� 1:��
	Out.Color = In.Color * tex2D( FloorSamp, In.Tex0 );
	
	// ������Ÿ�� 2:����
	Out.Normal =In.Normal;
	
	return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(��������)
// ------------------------------------------------------------
PixelShader PS_NormalEdge = asm
{
    ps_1_1
    
	tex t0	// ����������(�������)
	tex t1	// ����������(�����ϴ�)
	tex t2	// ����������(�����ϴ�)
	tex t3	// ����������(�������)

	// RGB �� �ֵ����� ��´�
	sub_x2 r0,  t0, t1    // r0 = 2*(t0-t1)
	sub_x2 r1,  t2, t3    // r1 = 2*(t2-t3)

	// ������ �ֵ����� ���� ���밪�� ������� �����Ѵ�
	dp3_x4 r0,  r0, r0    // r0 = 16*(t0-t1)^2
	dp3_x4 r1,  r1, r1    // r1 = 16*(t2-t3)^2

	// (1 - �ֵ�) �� ����� ������� ���� ����
	add r0, 1-r0, -r1     // r0 = 1-16*((t0-t1)^2+(t2-t3)^2)
};

// ------------------------------------------------------------
// ��ũ��
// ------------------------------------------------------------
technique TShader
{
    pass P0 // ���������
    {
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_2_0 PS_pass0();
    }
    pass P1 // ����������
    {
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_2_0 PS_pass1();
    }
    pass P2 // ����
    {
        // ���̴�
        PixelShader  = <PS_NormalEdge>;
		// �ؽ�ó
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
	}
}
