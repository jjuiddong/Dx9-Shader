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

// ------------------------------------------------------------
// �ؽ�ó
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
// �������̴����� �ȼ����̴��� �ѱ�� ������
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float2 Tex0			: TEXCOORD0;
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
	
    Out.Tex0 = Tex0;

    return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(�������)
// ------------------------------------------------------------
float4 PS_pass0 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// ������Ÿ�� 1:��
	Out = tex2D( FloorSamp, In.Tex0 );
	
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
	Out.Color = vCol * ( vLightDir.w					 //ȯ�汤
			   + max(dot(vLightDir.xyz, Normal.xyz), 0));//Ȯ�걤
	
    Out.Tex0 = Tex0;

    return Out;
}

// ------------------------------------------------------------
// �ȼ����̴�(��������)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// ������Ÿ�� 1:��
	Out = In.Color;
	Out.rgb *= tex2D( FloorSamp, In.Tex0 );
	
	return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(��������)
// ------------------------------------------------------------
PixelShader PS_Edge = asm
{
    ps_1_1
    
    def c0, 0.299f, 0.587f, 0.114f, 0.0f    ; �ֵ� ����ġ
    
	tex t0	// ����������(�������)
	tex t1	// ����������(�����ϴ�)
	tex t2	// ����������(�����ϴ�)
	tex t3	// ����������(�������)

	dp3 r0,      t0, c0         ;         rgb        a
	dp3 r1,      t1, c0         ; r0 = (t3�ֵ�, t0�ֵ�)
	dp3 r0.rgb,  t3, c0         ; r1 = (t2�ֵ�, t1�ֵ�)
	dp3 r1.rgb,  t2, c0
	                            ;             rgb               a
	add_x4     r0,   r0,-r1     ; r0 =  4( t3-t2�ֵ�,     t0-t1�ֵ�)
	mul_x4     r0,   r0, r0     ; r0 = 64((t3-t2�ֵ�)^2, (t0-t1�ֵ�)^2)
	add_x4     r0, 1-r0,-r0.a   ; r0 = 4*(1-64((t3-t2�ֵ�)^2+(t0-t1�ֵ�)^2))
};

// ------------------------------------------------------------
// ��ũ��
// ------------------------------------------------------------
technique TShader
{
    pass P0 // ���������
    {
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_1_1 PS_pass0();
    }
    pass P1 // ����������
    {
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_1_1 PS_pass1();
    }
    pass P2 // ����
    {
        // ���̴�
        PixelShader  = <PS_Edge>;
		// �ؽ�ó
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
	}
}
