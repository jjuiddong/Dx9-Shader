// ------------------------------------------------------------
// �߾Ӱ�����
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

static const float TEX_SIZE = 256.0;

// ------------------------------------------------------------
// �ؽ�ó
// ------------------------------------------------------------
texture SrcMap;
sampler SrcSamp = sampler_state
{
    Texture = <SrcMap>;
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
	float2 Tex			: TEXCOORD0;
};

// ------------------------------------------------------------
// �������̴�
// ------------------------------------------------------------
VS_OUTPUT VS (
      float4 Pos    : POSITION,          // ������
      float4 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    // ��ġ��ǥ
    Out.Pos = Pos;
    
    Out.Tex = Tex + float2( 0.5/TEX_SIZE, 0.5/TEX_SIZE);
    
    return Out;
}

// ------------------------------------------------------------
// �ȼ����̴�
// ------------------------------------------------------------
float4 PS (VS_OUTPUT In) : COLOR
{   
    float4 Out = (float4)0;
	const float3 RGB2Y = {0.299, 0.587, 0.114};
	
	float2 s1 = float2( 0.0/TEX_SIZE, 1.0/TEX_SIZE);
	float2 s2 = float2( 1.0/TEX_SIZE, 0.0/TEX_SIZE);
	
	// �ؽ�ó�� �ణ �÷��� ���ø�
	float3 col0 = tex2D( SrcSamp, In.Tex );
	float3 col1 = tex2D( SrcSamp, In.Tex + s1 );
	float3 col2 = tex2D( SrcSamp, In.Tex - s1 );
	float3 col3 = tex2D( SrcSamp, In.Tex + s2 );
	float3 col4 = tex2D( SrcSamp, In.Tex - s2 );
	
	// �������� ���Ѵ�
	float b0 = dot( col0, RGB2Y );
	float b1 = dot( col1, RGB2Y );
	float b2 = dot( col2, RGB2Y );
	float b3 = dot( col3, RGB2Y );
	float b4 = dot( col4, RGB2Y );

	// �������� ���Ѵ� ([4-��]�� ������ ����)
	float flag0 = ((b0< b1)?1.0:0.0) + ((b0< b2)?1.0:0.0) + ((b0< b3)?1.0:0.0) + ((b0< b4)?1.0:0.0);
	float flag1 = ((b1<=b0)?1.0:0.0) + ((b1< b2)?1.0:0.0) + ((b1< b3)?1.0:0.0) + ((b1< b4)?1.0:0.0);
	float flag2 = ((b2<=b0)?1.0:0.0) + ((b2<=b1)?1.0:0.0) + ((b2< b3)?1.0:0.0) + ((b2< b4)?1.0:0.0);
	float flag3 = ((b3<=b0)?1.0:0.0) + ((b3<=b1)?1.0:0.0) + ((b3<=b2)?1.0:0.0) + ((b3< b4)?1.0:0.0);

	// �߽ɻ��� �߾Ӱ����� ���
	Out.xyz =  ( flag0 ==2.0 ) ? col0 : 
	          (( flag1 ==2.0 ) ? col1 :
	          (( flag2 ==2.0 ) ? col2 :
	          (( flag3 ==2.0 ) ? col3 : col4 )));

    return Out;
}
// ------------------------------------------------------------
// ��ũ��
// ------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // ���̴�
        VertexShader = compile vs_1_1  VS();
        PixelShader  = compile ps_2_0 PS();
        
		Sampler[0] = (SrcSamp);
    }
}
