// -------------------------------------------------------------
// ��������
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWVPT;		// ���ÿ��� �ؽ�ó���������� ��ǥ��ȯ
float4	 vLightDir;	// ��������
float4   vCol;		// �޽û�

// -------------------------------------------------------------
// �ؽ�ó
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
// -------------------------------------------------------------
// �н�1:�����ۿ� ���̹��� ����
// -------------------------------------------------------------
// -------------------------------------------------------------

// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos		: POSITION;
	float4 Col		: COLOR0;
	float2 Tex		: TEXCOORD0;
	float2 Depth	: TEXCOORD1;
};

// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT VS (
	  float4 Pos	: POSITION          // ������ġ
	, float4 Normal	: NORMAL			// ��������
	, float4 Tex	: TEXCOORD0			// �ؽ�ó��ǥ
){
	VS_OUTPUT Out = (VS_OUTPUT)0;       // ��µ�����
	
	float4 pos = mul( Pos, mWVP );		// ��ǥ��ȯ
	
	Out.Pos = pos;						// ��ġ��ǥ
	
	Out.Col = vCol * max( dot(vLightDir, Normal), 0);	// ������
	
	Out.Tex = Tex;						// �ؽ�ó��ǥ
	
	Out.Depth = 0.1f*pos.w;				// ����
	
	return Out;
}
// -------------------------------------------------------------
// �ȼ����̴��� ��µ�����
// -------------------------------------------------------------
struct PS_OUTPUT {
	float4 Color : COLOR0;
	float4 Depth : COLOR1;
};
// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
PS_OUTPUT PS ( VS_OUTPUT In ) {
	
	PS_OUTPUT Out = ( PS_OUTPUT ) 0;
	
	// �⺻��
	Out.Color = In.Col * tex2D( DecaleMapSamp, In.Tex );
	
	//����
	Out.Depth.x = In.Depth;

    return Out;
}

// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();

		Sampler[0] = (DecaleMapSamp);
    }
}


// -------------------------------------------------------------
// -------------------------------------------------------------
// �н�2:���׸� ����
// -------------------------------------------------------------
// -------------------------------------------------------------

// -------------------------------------------------------------
// �ؽ�ó
// -------------------------------------------------------------
texture DepthMap;
sampler DepthMapSamp = sampler_state
{
    Texture = <DepthMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
texture FrameBuffer;
sampler FrameBufferSamp = sampler_state
{
    Texture = <FrameBuffer>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT_VOLUME
{
	float4 Pos		: POSITION;
	float4 Depth	: COLOR0;
	float4 Tex		: TEXCOORD0;
};
// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT_VOLUME VS_VOLUME (
	  float4 Pos	: POSITION          // ������ǥ
	, float4 Tex	: TEXCOORD0			// �ؽ�ó��ǥ
){
	VS_OUTPUT_VOLUME Out = (VS_OUTPUT_VOLUME)0;        // ��µ�����
	
	float4 pos = mul( Pos, mWVP );	// ��ǥ��ȯ
	
	Out.Pos = pos;					// ��ġ��ǥ
	
	Out.Tex = mul(Pos, mWVPT);		// �ؽ�ó��ǥ
	
	Out.Depth = 0.1f*pos.w;			// ����
	
	return Out;
}
// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
float4 PS_VOLUME1( VS_OUTPUT_VOLUME In) : COLOR
{
	float depth_map    = tex2Dproj(    DepthMapSamp, In.Tex ).x;
	float frame_buffer = tex2Dproj( FrameBufferSamp, In.Tex ).x;
	
	return frame_buffer
	 + 10.0f*((depth_map < In.Depth.x) ? depth_map : In.Depth.x);
}

// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
float4 PS_VOLUME2 ( VS_OUTPUT_VOLUME In) : COLOR
{
	float depth_map    = tex2Dproj(    DepthMapSamp, In.Tex ).x;
	float frame_buffer = tex2Dproj( FrameBufferSamp, In.Tex ).x;
	
	return frame_buffer
	 - 10.0f*((depth_map < In.Depth.x) ? depth_map : In.Depth.x);
}

// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TVolume
{
    pass P0
    {
		Sampler[0] = (DepthMapSamp);
		Sampler[1] = (FrameBufferSamp);
		
		// ������ ������Ʈ
        CullMode = CW;// ���ʂ������_�����O
		Zenable = False;
        
        // ���̴�
        VertexShader = compile vs_1_1 VS_VOLUME ();
        PixelShader  = compile ps_2_0 PS_VOLUME1();

    }
    pass P1
    {
		Sampler[0] = (DepthMapSamp);
		Sampler[1] = (FrameBufferSamp);
		
		// ������ ������Ʈ
        CullMode = CCW;// �ո鷻����
		Zenable = False;
        
        // ���̴�
        VertexShader = compile vs_1_1 VS_VOLUME ();
        PixelShader  = compile ps_2_0 PS_VOLUME2();
    }
}

// -------------------------------------------------------------
// -------------------------------------------------------------
// �н�4:�����ռ�
// -------------------------------------------------------------
// -------------------------------------------------------------

// -------------------------------------------------------------
// �ؽ�ó
// -------------------------------------------------------------
texture FogMap;
sampler FogMapSamp = sampler_state
{
    Texture = <FogMap>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
texture ColorMap;
sampler ColorMapSamp = sampler_state
{
    Texture = <ColorMap>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT_FINAL
{
	float4 Pos		: POSITION;
	float2 Tex		: TEXCOORD0;
};
// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT_FINAL VS_FINAL (
	  float4 Pos	: POSITION          // ������ġ
	, float2 Tex	: TEXCOORD0			// �ؽ�ó��ǥ
){
	VS_OUTPUT_FINAL Out;        // ��µ�����
	
	// ��ġ��ǥ
	Out.Pos = Pos;
	
	// �ؽ�ó��ǥ
	Out.Tex = Tex;
	
	return Out;
}
// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
float4 PS_FINAL ( VS_OUTPUT_FINAL In) : COLOR
{
	float  fog_map = tex2D( FogMapSamp,   In.Tex ).x;
	float4 col_map = tex2D( ColorMapSamp, In.Tex );
	float4 fog_color = {0.84f, 0.88f, 1.0f, 1.0f};
	
	return lerp(col_map, fog_color, fog_map);
}

// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TFinal
{
    pass P0
    {
		Sampler[0] = (FogMapSamp);
		Sampler[1] = (ColorMapSamp);
		
		// ������ ������Ʈ
        AlphaBlendEnable = False;

        // ���̴�
        VertexShader = compile vs_1_1 VS_FINAL();
        PixelShader  = compile ps_2_0 PS_FINAL();
    }
}
