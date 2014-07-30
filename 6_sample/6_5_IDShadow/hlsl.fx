// -------------------------------------------------------------
// �켱�������� �׸���
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWLP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWVPT;		// �ؽ�ó��ǥ����� ����
float4   vCol;		// �޽û�
float4   vId;		// �켱�������� ��ȣ
float4	 vLightDir;	// ��������

// -------------------------------------------------------------
// �ؽ�ó
// -------------------------------------------------------------
texture IdMap;
sampler IdMapSamp = sampler_state
{
    Texture = <IdMap>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
texture DecaleMap;
sampler DecaleMapSamp = sampler_state
{
    Texture = <DecaleMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
	float4 Diffuse		: COLOR0;
	float4 Ambient		: COLOR1;
	float4 ShadowMapUV	: TEXCOORD0;
	float4 ID			: TEXCOORD1;
	float2 TexDecale	: TEXCOORD2;
};

// -------------------------------------------------------------
// 1�н�:�������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL	         // �𵨹���
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    // ��ǥ��ȯ
    Out.Pos =  mul( Pos, mWLP );
    
    // ID�� ������ ���
    Out.Diffuse = vId;

    return Out;
}
// -------------------------------------------------------------
// 1�н�:�ȼ����̴�
// -------------------------------------------------------------
PIXELSHADER PS_pass0 = asm
{
    ps.1.1
    
    mov r0, v0	// ���� ID�� �ؼ� ���
};
// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL,	         // �𵨹���
      float2 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
	float4	uv;
	
	// ��ǥ��ȯ
    Out.Pos = mul(Pos, mWVP);
	// ��
	Out.Diffuse = vCol * max( dot(vLightDir, Normal), 0);// Ȯ�걤
	Out.Ambient = vCol * 0.3f;							 // ȯ�汤
	
	// �ؽ�ó��ǥ
	uv = mul(Pos, mWVPT);
	Out.ShadowMapUV = uv;

	// ID��
	Out.ID = vId;
	
	// ���� �ؽ�ó
	Out.TexDecale = Tex;
		
    return Out;
}
// -------------------------------------------------------------
// 2�н�:�ȼ����̴�(�ؽ�ó����)
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{   
    float4 Color = In.Ambient;
    float4 zero = {0,0,0,0};
    float ofset = 1.0f/256.0f;
    
	float  id_map = tex2Dproj( IdMapSamp, In.ShadowMapUV );
	float4 decale = tex2D( DecaleMapSamp, In.TexDecale );
    
    Color += (id_map.x < In.ID.x+ofset && In.ID.x-ofset < id_map.x)
				 ? In.Diffuse : zero;

    return decale * Color;
}  
// -------------------------------------------------------------
// 2�н�:�ȼ����̴�(�ؽ�ó����)
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT In) : COLOR
{   
    float4 Color = In.Ambient;
    float4 zero = {0,0,0,0};
    float ofset = 0.01f;
    
	float  id_map = tex2Dproj( IdMapSamp, In.ShadowMapUV );
	float4 decale = tex2D( DecaleMapSamp, In.TexDecale );
    
    Color += (id_map.x < In.ID.x+ofset && In.ID.x-ofset < id_map.x)
				 ? In.Diffuse : zero;

    return Color;
}  
// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = <PS_pass0>;
    }
    pass P1
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_pass1();
    }
    pass P2
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_pass2();
    }
}
