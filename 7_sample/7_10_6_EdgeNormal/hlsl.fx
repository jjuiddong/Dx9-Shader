// ------------------------------------------------------------
// À±°ûÃßÃâ
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// Àü¿ªº¯¼ö
// ------------------------------------------------------------
float4x4 mWVP;
float4   vCol;
float4	 vLightDir;	// ±¤¿ø¹æÇâ
float    fNear=1.0f;
float    fFar =7.0f;

// ------------------------------------------------------------
// ÅØ½ºÃ³
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
// Á¤Á¡¼ÎÀÌ´õ¿¡¼­ ÇÈ¼¿¼ÎÀÌ´õ·Î ³Ñ±â´Â µ¥ÀÌÅÍ
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
// ÇÈ¼¿¼ÎÀÌ´õ Ãâ·Âµ¥ÀÌÅÍ
// ------------------------------------------------------------
struct PS_OUTPUT
{
    float4 Color		: COLOR0;
    float4 Normal		: COLOR1;
};

// ------------------------------------------------------------
// Á¤Á¡¼ÎÀÌ´õ(Á¶¸í¾øÀ½)
// ------------------------------------------------------------
VS_OUTPUT VS_pass0 (
      float4 Pos    : POSITION           // ¸ğµ¨Á¤Á¡
     ,float4 Normal : NORMAL             // ¹ı¼±º¤ÅÍ
     ,float4 Tex0   : TEXCOORD0	         // ÅØ½ºÃ³ÁÂÇ¥
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // Ãâ·Âµ¥ÀÌÅÍ
    
    float4 pos = mul( Pos, mWVP );
    
	// À§Ä¡ÁÂÇ¥
	Out.Pos = pos;
	
	// ¹ı¼±
	Out.Normal = 0.5f*Normal+0.5f;// ¿ùµåº¯È¯
	
    Out.Tex0 = Tex0;

    return Out;
}
// ------------------------------------------------------------
// ÇÈ¼¿¼ÎÀÌ´õ(Á¶¸í¾øÀ½)
// ------------------------------------------------------------
PS_OUTPUT PS_pass0 (VS_OUTPUT In) : COLOR
{
	PS_OUTPUT Out;
	
	// ƒŒƒ“ƒ_ƒŠƒ“ƒOƒ^[ƒQƒbƒg‚PFF
	Out.Color = tex2D( FloorSamp, In.Tex0 );
	
	// ƒŒƒ“ƒ_ƒŠƒ“ƒOƒ^[ƒQƒbƒg‚QF–@ü
	Out.Normal =In.Normal;

	return Out;
}
// ------------------------------------------------------------
// Á¤Á¡¼ÎÀÌ´õ(Á¶¸íÀÖÀ½)
// ------------------------------------------------------------
VS_OUTPUT VS_pass1 (
      float4 Pos    : POSITION           // ¸ğµ¨Á¤Á¡
    , float4 Normal : NORMAL             // ¹ı¼±º¤ÅÍ
     ,float4 Tex0   : TEXCOORD0	         // ÅØ½ºÃ³ÁÂÇ¥
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // Ãâ·Âµ¥ÀÌÅÍ
    
    float4 pos = mul( Pos, mWVP );
    
	// À§Ä¡ÁÂÇ¥
	Out.Pos = pos;
	
	// »ö
	float diffuse = max(dot(vLightDir.xyz, Normal.xyz), 0);//È®»ê»ö
	float ambient = vLightDir.w;						   //È¯°æ»ö
	Out.Color = vCol * ( diffuse + ambient );
	
	// ÅØ½ºÃ³ÁÂÇ¥
    Out.Tex0 = Tex0;

	// ¹ı¼±
	Out.Normal = 0.5f*Normal+0.5f;// ¿ùµåº¯È¯

    return Out;
}

// ------------------------------------------------------------
// ÇÈ¼¿¼ÎÀÌ´õ(Á¶¸íÀÖÀ½)
// ------------------------------------------------------------
PS_OUTPUT PS_pass1 (VS_OUTPUT In) : COLOR
{
	PS_OUTPUT Out;
	
	// ·»´õ¸µÅ¸°Ù 1:»ö
	Out.Color = In.Color * tex2D( FloorSamp, In.Tex0 );
	
	// ·»´õ¸µÅ¸°Ù 2:¹ı¼±
	Out.Normal =In.Normal;
	
	return Out;
}
// ------------------------------------------------------------
// ÇÈ¼¿¼ÎÀÌ´õ(À±°ûÃßÃâ)
// ------------------------------------------------------------
PixelShader PS_NormalEdge = asm
{
    ps_1_1
    
	tex t0	// ÇöÀçÇÁ·¹ÀÓ(ÁÂÃø»ó´Ü)
	tex t1	// ÇöÀçÇÁ·¹ÀÓ(¿ìÃøÇÏ´Ü)
	tex t2	// ÇöÀçÇÁ·¹ÀÓ(ÁÂÃøÇÏ´Ü)
	tex t3	// ÇöÀçÇÁ·¹ÀÓ(¿ìÃø»ó´Ü)

	// RGB °¢ ÈÖµµÂ÷¸¦ ¾ò´Â´Ù
	sub_x2 r0,  t0, t1    // r0 = 2*(t0-t1)
	sub_x2 r1,  t2, t3    // r1 = 2*(t2-t3)

	// °¢°¢ÀÇ ÈÖµµÂ÷ÀÌ °ªÀÇ Àı´ë°ªÀ» ¾ò±âÀ§ÇØ Á¦°öÇÑ´Ù
	dp3_x4 r0,  r0, r0    // r0 = 16*(t0-t1)^2
	dp3_x4 r1,  r1, r1    // r1 = 16*(t2-t3)^2

	// (1 - ÈÖµµ) ÀÇ °á°ú´Â Èò¹ÙÅÁ¿¡ °ËÀº ¿§Áö
	add r0, 1-r0, -r1     // r0 = 1-16*((t0-t1)^2+(t2-t3)^2)
};

// ------------------------------------------------------------
// Å×Å©´Ğ
// ------------------------------------------------------------
technique TShader
{
    pass P0 // Á¶¸í°è»ê¾øÀ½
    {
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_2_0 PS_pass0();
    }
    pass P1 // Á¶¸í°è»êÀÖÀ½
    {
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_2_0 PS_pass1();
    }
    pass P2 // ¿§Áö
    {
        // ¼ÎÀÌ´õ
        PixelShader  = <PS_NormalEdge>;
		// ÅØ½ºÃ³
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
	}
}
