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
// �������̴����� �ȼ����̴��� �ѱ�� ������
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float2 Tex0			: TEXCOORD0;
};

// ------------------------------------------------------------
// �������̴�(��������)
// ------------------------------------------------------------
VS_OUTPUT VS (
      float4 Pos    : POSITION           // ������
    , float4 Normal : NORMAL             // ��������
     ,float4 Tex0   : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
	// ��ġ��ǥ
	Out.Pos = mul( Pos, mWVP );
	
	// ��
	float diffuse = max(dot(vLightDir.xyz, Normal.xyz), 0);//ȯ�汤
	float ambient = vLightDir.w;						   //Ȯ�걤
	Out.Color = vCol * ( diffuse + ambient );
	
	// �ؽ�ó��ǥ
    Out.Tex0 = Tex0;

    return Out;
}

// ------------------------------------------------------------
// �������̴�(��������)
// ------------------------------------------------------------
VS_OUTPUT VS_Edge (
      float4 Pos    : POSITION           // ������
    , float4 Normal : NORMAL             // ��������
     ,float4 Tex0   : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    float4 pos = mul( Pos, mWVP );

    float4 n = mul( Normal, mWVP );
    n.zw=0;
    
	// ��ġ��ǥ
	Out.Pos = pos + 0.02f*n;
	
	// ��
	Out.Color = 0;
	
    return Out;
}

// ------------------------------------------------------------
// ��ũ��
// ------------------------------------------------------------
technique TShader
{
    pass P0 // ����������
    {
        VertexShader = compile vs_1_1 VS();
    }
    pass P1 // ��������
    {
        VertexShader = compile vs_1_1 VS_Edge();
    }
}
