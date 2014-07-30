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
float    z_max;		// �Ĺ� Ŭ��������� �Ÿ�
float    z_focus;	// ������ �´� �Ÿ�
float    d_max;		// f2/F*L : ���ѿ������� ������

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
sampler FinalSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = POINT;
    MagFilter = POINT;
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
	float2 Tex			: TEXCOORD0;
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
	
	Out.Color = pos.w / z_max;// �����п� ���̰�
	
    Out.Tex = Tex;		// �ؽ�ó��ǥ

    return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(�������)
// ------------------------------------------------------------
float4 PS_pass0 (VS_OUTPUT In) : COLOR
{
	float4 Out = tex2D( FloorSamp, In.Tex );
	
	Out.a = In.Color.w;		// ���ļ��п� ���̰�
	
	return Out;
}
// ------------------------------------------------------------
// �������̴�(��������)
// ------------------------------------------------------------
VS_OUTPUT VS_pass1 (
      float4 Pos    : POSITION           // ������
    , float4 Normal : NORMAL             // ��������
     ,float4 Tex   : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    float4 pos = mul( Pos, mWVP );
    
	Out.Pos = pos;						// ��ġ��ǥ
	
	// ����
	Out.Color.xyz = 0.5f*Normal.xyz+0.5f;
	// ����
	Out.Color.w = pos.w / z_max;						// ���̰�
    Out.Tex = Tex;										// �ؽ�ó��ǥ

    return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(��������)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	float4 tex = tex2D( SrcSamp, In.Tex );
	float4 N = 2.0f*In.Color - 1.0f;// ��������
	N.w = 0;
	
	// ��
	Out = tex * vCol * (vLightDir.w + saturate(dot(vLightDir, N)));
	// ����
	Out.w = In.Color.w;
	
	return Out;
}


// ------------------------------------------------------------
// �ȼ����̴�(�ռ�)
// ------------------------------------------------------------
#define NUM_DOF_TAPS 6					// ���ø� ����
float2   filterTaps[NUM_DOF_TAPS];		// ���ø����� ��ġ
float    filterDistance[NUM_DOF_TAPS];	// ���ø����� �Ÿ�
// ------------------------------------------------------------
float4 PS_pass3 (VS_OUTPUT In) : COLOR
{
	float4 col0 = tex2D( FinalSamp,  In.Tex ); // �߽ɻ��� ���̰�
	float4 ret = col0;

	for(int i = 0; i < NUM_DOF_TAPS; i++){
		// ���� ���̰��� ���ø��Ѵ�
		float4 col = tex2D( FinalSamp, In.Tex + filterTaps[i] );
		// �������� ����Ѵ�
		float z = col.a;
		float a = z - z_focus;		// �������� �Ÿ�
		float d =  a * d_max / z;	// ���ʿ� �������� ������
		if(a<0) d = -d;				// ���տ� �������� ������
		
		// �������� ũ�Ⱑ filterTaps�� Ȯ���� �纸�� ũ�ٸ�
		// �������� �������� ���� �ִ°��̹Ƿ�, �� ���� ����Ѵ�.
		if(filterDistance[i]<d){
			ret += col;
		}else{
			ret += col0;
		}
	}
	
	return ret / (NUM_DOF_TAPS+1);
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
    pass P2	// �ռ�
    {
        PixelShader  = compile ps_2_0 PS_pass3();
	}
}
