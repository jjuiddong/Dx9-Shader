// -------------------------------------------------------------
// ��ũ������
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------

float  MAP_WIDTH;	// SAT ��
float  MAP_HEIGHT;	// SAT ����

float SIZE = 40.0f;	// ������
float INV_SIZE2=1.0f/(40.0f*40.0f);	// �������� ������ ����

// -------------------------------------------------------------
// �ؽ�ó
// -------------------------------------------------------------
texture SrcMap;
sampler SrcSamp = sampler_state
{
    Texture = <SrcMap>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
texture WeightMap;
sampler WeightSamp = sampler_state
{
    Texture = <WeightMap>;
    MinFilter = POINT;
    MagFilter = POINT;
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
    float2 ofset		: COLOR0;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
	float2 Tex2			: TEXCOORD2;
	float2 Tex3			: TEXCOORD3;
};

// -------------------------------------------------------------
// -------------------------------------------------------------
// ���� ���� ���̺� �����
// -------------------------------------------------------------
// -------------------------------------------------------------

// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_sat_x (
      float4 Pos    : POSITION,          // ������
      float4 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    // ��ġ��ǥ
    Out.Pos = Pos;
    
    // �ؽ�ó��ǥ // �߽�
    Out.Tex0 = Tex + float2( 0.5f/MAP_WIDTH, 0.5f/MAP_HEIGHT );
    Out.Tex1 = Tex + float2(-0.5f/MAP_WIDTH, 0.5f/MAP_HEIGHT );// �
    
    return Out;
}
// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
float4 PS_sat_x(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    float4 weight = tex2D( WeightSamp, In.Tex0 );
    
    Color  = tex2D( SrcSamp, In.Tex0 ) * weight
           + tex2D( SrcSamp, In.Tex1 );
           
    return Color;
}

// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_sat_y (
      float4 Pos    : POSITION,          // ������
      float4 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    // ��ġ��ǥ
    Out.Pos = Pos;
    
    // �ؽ�ó��ǥ
    Out.Tex0 = Tex + float2( 0.5f/MAP_WIDTH, 0.5f/MAP_HEIGHT );
    Out.Tex1 = Tex + float2( 0.5f/MAP_WIDTH,-0.5f/MAP_HEIGHT );// ߾
    
    return Out;
}

// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
float4 PS_sat_y(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    
    Color  = tex2D( SrcSamp, In.Tex0 )
           + tex2D( SrcSamp, In.Tex1 );
           
    return Color;
}

// -------------------------------------------------------------
// -------------------------------------------------------------
// �������
// -------------------------------------------------------------
// -------------------------------------------------------------

// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_out (
      float4 Pos    : POSITION,          // ������
      float4 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    // ��ġ��ǥ
    Out.Pos = Pos;
    
    Out.Tex0 = Tex + float2( (0.5f-0.5f*SIZE)/MAP_WIDTH, (0.5f-0.5f*SIZE)/MAP_HEIGHT );
    Out.Tex1 = Tex + float2( (0.5f-0.5f*SIZE)/MAP_WIDTH, (0.5f+0.5f*SIZE)/MAP_HEIGHT );
    Out.Tex2 = Tex + float2( (0.5f+0.5f*SIZE)/MAP_WIDTH, (0.5f-0.5f*SIZE)/MAP_HEIGHT );
    Out.Tex3 = Tex + float2( (0.5f+0.5f*SIZE)/MAP_WIDTH, (0.5f+0.5f*SIZE)/MAP_HEIGHT );
    
    return Out;
}

// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
float4 PS_out(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    float  weight;
    
    // ����ġ������ ���Ѵ�
    weight =  tex2D( SrcSamp, In.Tex0 ).a
            - tex2D( SrcSamp, In.Tex1 ).a
            - tex2D( SrcSamp, In.Tex2 ).a
            + tex2D( SrcSamp, In.Tex3 ).a;
            
    // SAT���� ����ġ�������� ������ ���� ���Ѵ�
    Color  =( tex2D( SrcSamp, In.Tex0 )
            - tex2D( SrcSamp, In.Tex1 )
            - tex2D( SrcSamp, In.Tex2 )
            + tex2D( SrcSamp, In.Tex3 ))/weight;

    return Color;
}

// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
		// -----------------------------------------------------
		// X�� ������
		// -----------------------------------------------------
        VertexShader = compile vs_1_1 VS_sat_x();
        PixelShader  = compile ps_2_0 PS_sat_x();
    }
    pass P1
    {
		// -----------------------------------------------------
		// Y�� ������
		// -----------------------------------------------------
        VertexShader = compile vs_1_1 VS_sat_y();
        PixelShader  = compile ps_2_0 PS_sat_y();
    }
    pass P2
    {
		// -----------------------------------------------------
		// ���
		// -----------------------------------------------------
        VertexShader = compile vs_1_1 VS_out();
        PixelShader  = compile ps_2_0 PS_out();
    }
}
