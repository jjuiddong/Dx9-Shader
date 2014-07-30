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
	
	// ����
	Out.Color.w = (pos.w-fNear)/(fFar-fNear);
	
    Out.Tex0 = Tex0;

    return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(�������)
// ------------------------------------------------------------
float4 PS_pass0 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// ��
	Out = tex2D( FloorSamp, In.Tex0 );
	// ID
	Out.a = In.Color.w;
	
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
	
    Out.Tex0 = Tex0;

	// ����
	Out.Color.w = (pos.w-fNear)/(fFar-fNear);

    return Out;
}

// ------------------------------------------------------------
// �ȼ����̴�(��������)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// ��
	Out.rgb = In.Color * tex2D( FloorSamp, In.Tex0 );
	// ID
	Out.a = In.Color.w;
	
	return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(��������)
// ------------------------------------------------------------
PixelShader PS_ZEdge = asm
{
    ps_1_1
    
	tex t0	// ����������(�������)
	tex t1	// ����������(�����ϴ�)
	tex t2	// ����������(�����ϴ�)
	tex t3	// ����������(�������)
	
	// �μ��� t#�������͸� 2�� ����Ҽ� �����Ƿ� �ϴ� ����
	mov r0, t0
	mov r1, t2
	
	add_x4     r0,   r0,  -t1	;       r0                  r1
	add_x4     r1,   r1,  -t3	; 4*(t0.a-t1.a)        4*(t2.a-t3.a)
	mul_x4     r0,   r0,   r0	;
	mul_x4     r1,   r1,   r1	; 16*(t0-t1����)^2,  16*(t2-t3����)^2)
	add_x4     r0,   r0,   r1	; r0.a = 64((t0-t0����)^2+(t3-t1����)^2)
	mov        r0,   1-r0.a		; r0.a = (1-64((t0-t0����)^2+(t3-t1����)^2))
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
        PixelShader  = <PS_ZEdge>;
		// �ؽ�ó
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
	}
}
