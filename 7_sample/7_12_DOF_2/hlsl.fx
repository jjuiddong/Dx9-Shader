// ------------------------------------------------------------
// �ǻ��ɵ�
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// ��������
// ------------------------------------------------------------
float4x4 mWVP;		// ����-�������
float4   vCol;		// �޽û�
float4	 vLightDir;	// ��������
float4   vCenter;
float4   vScale;

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
// �������̴����� �ȼ����̴��� �ѱ�� ������
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
// �������̴�(�������)
// ------------------------------------------------------------
VS_OUTPUT VS_pass0 (
      float4 Pos    : POSITION           // ������
     ,float4 Tex   : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    float4 pos = mul( Pos, mWVP );
    
	Out.Pos = pos;			// ��ġ��ǥ
	
	Out.Color = pos.z/pos.w;// �����п� ���̰�
	
    Out.Tex0 = Tex;		// �ؽ�ó��ǥ

    return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(�������)
// ------------------------------------------------------------
float4 PS_pass0 (VS_OUTPUT In) : COLOR
{
	float4 Out = tex2D( FloorSamp, In.Tex0 );
	
	Out.a = In.Color.w;		// ���ļ��п� ���̰�
	
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
    
	Out.Pos = pos;						// ��ġ��ǥ
	
	// ��
	Out.Color = vCol * ( vLightDir.w					 //ȯ�汤
			   + max(dot(vLightDir.xyz, Normal.xyz), 0));//Ȯ�걤
	
	Out.Color.w = pos.z/pos.w;			// ���̰�
    Out.Tex0 = Tex0;					// �ؽ�ó��ǥ

    return Out;
}

// ------------------------------------------------------------
// �ȼ����̴�(��������)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out = In.Color * tex2D( SrcSamp, In.Tex0 );
	
	return Out;
}
// ------------------------------------------------------------
// �������̴�(������)
// ------------------------------------------------------------
VS_OUTPUT VS_pass2 (
      float4 Pos    : POSITION           // ������
     ,float4 Tex0   : TEXCOORD0	         // �ؽ�ó��ǥ
     ,float4 Tex1   : TEXCOORD1	         // �ؽ�ó��ǥ
     ,float4 Tex2   : TEXCOORD2	         // �ؽ�ó��ǥ
     ,float4 Tex3   : TEXCOORD3	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    // ��ġ��ǥ
    Out.Pos = Pos;
    // �׽�ó��ǥ
    Out.Tex0 = Tex0;
    Out.Tex1 = Tex1;
    Out.Tex2 = Tex2;
    Out.Tex3 = Tex3;
    
    return Out;
}

// ------------------------------------------------------------
// �ȼ����̴�(������)
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
// �ȼ����̴�(�ռ�)
// ------------------------------------------------------------
float4 PS_pass3 (VS_OUTPUT In) : COLOR
{
	float t1, t2, t3;
	
	float4 col0 = tex2D( SrcSamp,  In.Tex0 ); // �߽�
	float4 col1 = tex2D( BlurSamp, In.Tex1 ); // ���ø���1
	float4 col2 = tex2D( BlurSamp, In.Tex2 ); // ���ø���2
	float4 col3 = tex2D( BlurSamp, In.Tex3 ); // ���ø���3
	float a0 = col0.a-vCenter.w; // �߽��� ������ ����
	float a1 = col1.a-vCenter.w; // ���ø��� 1�� ������ ����
	float a2 = col2.a-vCenter.w; // ���ø��� 2�� ������ ����
	float a3 = col3.a-vCenter.w; // ���ø��� 3�� ������ ����
	
	// �ռ�����ġ ���
	t1 = a1;
	// ���ø��� 1�� ���ʿ� ������ �߽��� ������ ���¸� ����� �ִ´�
	if(a0<a1) t1 *= a0;
	t2 = a2;
	// ���ø��� 2�� ���ʿ� ������ �߽��� ������ ���¸� ����� �ִ´�
	if(a0<a2) t2 *= a0;
	t3 = a3;
	// ���ø��� 3�� ���ʿ� ������ �߽��� ������ ���¸� ����� �ִ´�
	if(a0<a3) t3 *= a0;
	
	// ������ �����ϸ�
	float s = 10000.0f*vScale.w;
	t1 = saturate(s * t1*t1);
	t2 = saturate(s * t2*t2);
	t3 = saturate(s * t3*t3);
	
	// �ռ�
	return  ( lerp(col0, col1, t1)
			+ lerp(col0, col2, t2)
			+ lerp(col0, col3, t3))/3;
}



// ------------------------------------------------------------
// ��ũ��
// ------------------------------------------------------------
technique TShader
{
    pass P0 // �Ϲݷ����� + ���̷�����
    {		// �������
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_1_1 PS_pass0();
    }
    pass P1	// �Ϲݷ����� + ���̷�����
    {		// ��������
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_1_1 PS_pass1();
    }
    pass P2	// ������
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
    pass P3	// �ռ�
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
