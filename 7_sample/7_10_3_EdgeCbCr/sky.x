xof 0303txt 0032

template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template VertexDuplicationIndices {
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}


Frame Frame_SCENE_ROOT {

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Frame Frame1_sky {

  FrameTransformMatrix {
   1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
  }

  Mesh Mesh_sky {
   177;
   -3.000000;0.000000;-3.000000;,
   3.000000;0.000000;-3.000000;,
   3.000000;0.000000;3.000000;,
   -3.000000;0.000000;3.000000;,
   3.000000;0.000000;0.000000;,
   3.000000;0.000000;0.000000;,
   2.897778;0.000000;0.776457;,
   2.897778;0.000000;0.776457;,
   2.598076;0.000000;1.500000;,
   2.598076;0.000000;1.500000;,
   2.121320;0.000000;2.121320;,
   2.121320;0.000000;2.121320;,
   1.500000;0.000000;2.598076;,
   1.500000;0.000000;2.598076;,
   0.776457;0.000000;2.897778;,
   0.776457;0.000000;2.897778;,
   0.000000;0.000000;3.000000;,
   0.000000;0.000000;3.000000;,
   -0.776457;0.000000;2.897778;,
   -0.776457;0.000000;2.897778;,
   -1.500000;0.000000;2.598076;,
   -1.500000;0.000000;2.598076;,
   -2.121320;0.000000;2.121320;,
   -2.121320;0.000000;2.121320;,
   -2.598076;0.000000;1.500000;,
   -2.598076;0.000000;1.500000;,
   -2.897778;0.000000;0.776457;,
   -2.897778;0.000000;0.776457;,
   -3.000000;0.000000;0.000000;,
   -3.000000;0.000000;0.000000;,
   -2.897778;0.000000;-0.776457;,
   -2.897778;0.000000;-0.776457;,
   -2.598076;0.000000;-1.500000;,
   -2.598076;0.000000;-1.500000;,
   -2.121320;0.000000;-2.121320;,
   -2.121320;0.000000;-2.121320;,
   -1.500000;0.000000;-2.598076;,
   -1.500000;0.000000;-2.598076;,
   -0.776457;0.000000;-2.897778;,
   -0.776457;0.000000;-2.897778;,
   0.000000;0.000000;-3.000000;,
   0.000000;0.000000;-3.000000;,
   0.776457;0.000000;-2.897778;,
   0.776457;0.000000;-2.897778;,
   1.500000;0.000000;-2.598076;,
   1.500000;0.000000;-2.598076;,
   2.121320;0.000000;-2.121320;,
   2.121320;0.000000;-2.121320;,
   2.598076;0.000000;-1.500000;,
   2.598076;0.000000;-1.500000;,
   2.897778;0.000000;-0.776457;,
   2.897778;0.000000;-0.776457;,
   3.000000;3.550000;0.000000;,
   3.000000;3.550000;0.000000;,
   2.897778;3.550000;-0.776457;,
   2.897778;3.550000;-0.776457;,
   2.598076;3.550000;-1.500000;,
   2.598076;3.550000;-1.500000;,
   2.121320;3.550000;-2.121320;,
   2.121320;3.550000;-2.121320;,
   1.500000;3.550000;-2.598076;,
   1.500000;3.550000;-2.598076;,
   0.776457;3.550000;-2.897778;,
   0.776457;3.550000;-2.897778;,
   0.000000;3.550000;-3.000000;,
   0.000000;3.550000;-3.000000;,
   -0.776457;3.550000;-2.897778;,
   -0.776457;3.550000;-2.897778;,
   -1.500000;3.550000;-2.598076;,
   -1.500000;3.550000;-2.598076;,
   -2.121320;3.550000;-2.121320;,
   -2.121320;3.550000;-2.121320;,
   -2.598076;3.550000;-1.500000;,
   -2.598076;3.550000;-1.500000;,
   -2.897778;3.550000;-0.776457;,
   -2.897778;3.550000;-0.776457;,
   -3.000000;3.550000;0.000000;,
   -3.000000;3.550000;0.000000;,
   -2.897778;3.550000;0.776457;,
   -2.897778;3.550000;0.776457;,
   -2.598076;3.550000;1.500000;,
   -2.598076;3.550000;1.500000;,
   -2.121320;3.550000;2.121320;,
   -2.121320;3.550000;2.121320;,
   -1.500000;3.550000;2.598076;,
   -1.500000;3.550000;2.598076;,
   -0.776457;3.550000;2.897778;,
   -0.776457;3.550000;2.897778;,
   0.000000;3.550000;3.000000;,
   0.000000;3.550000;3.000000;,
   0.776457;3.550000;2.897778;,
   0.776457;3.550000;2.897778;,
   1.500000;3.550000;2.598076;,
   1.500000;3.550000;2.598076;,
   2.121320;3.550000;2.121320;,
   2.121320;3.550000;2.121320;,
   2.598076;3.550000;1.500000;,
   2.598076;3.550000;1.500000;,
   2.897778;3.550000;0.776457;,
   2.897778;3.550000;0.776457;,
   -3.000000;0.000000;0.000000;,
   0.000000;0.000000;3.000000;,
   3.000000;0.000000;0.000000;,
   0.000000;0.000000;-3.000000;,
   0.000000;0.000000;0.000000;,
   -1.500000;0.000000;-3.000000;,
   -3.000000;0.000000;-1.500000;,
   -1.500000;0.000000;0.000000;,
   0.000000;0.000000;-1.500000;,
   -1.500000;0.000000;-1.500000;,
   -3.000000;0.000000;1.500000;,
   -1.500000;0.000000;3.000000;,
   0.000000;0.000000;1.500000;,
   -1.500000;0.000000;1.500000;,
   1.500000;0.000000;3.000000;,
   3.000000;0.000000;1.500000;,
   1.500000;0.000000;0.000000;,
   1.500000;0.000000;1.500000;,
   3.000000;0.000000;-1.500000;,
   1.500000;0.000000;-3.000000;,
   1.500000;0.000000;-1.500000;,
   -0.750000;0.000000;-3.000000;,
   -1.500000;0.000000;-2.250000;,
   -0.750000;0.000000;-1.500000;,
   0.000000;0.000000;-2.250000;,
   -0.750000;0.000000;-2.250000;,
   -2.250000;0.000000;-3.000000;,
   -3.000000;0.000000;-2.250000;,
   -2.250000;0.000000;-1.500000;,
   -2.250000;0.000000;-2.250000;,
   -3.000000;0.000000;-0.750000;,
   -2.250000;0.000000;0.000000;,
   -1.500000;0.000000;-0.750000;,
   -2.250000;0.000000;-0.750000;,
   -0.750000;0.000000;0.000000;,
   0.000000;0.000000;-0.750000;,
   -0.750000;0.000000;-0.750000;,
   -1.500000;0.000000;0.750000;,
   -0.750000;0.000000;1.500000;,
   0.000000;0.000000;0.750000;,
   -0.750000;0.000000;0.750000;,
   -3.000000;0.000000;0.750000;,
   -2.250000;0.000000;1.500000;,
   -2.250000;0.000000;0.750000;,
   -3.000000;0.000000;2.250000;,
   -2.250000;0.000000;3.000000;,
   -1.500000;0.000000;2.250000;,
   -2.250000;0.000000;2.250000;,
   -0.750000;0.000000;3.000000;,
   0.000000;0.000000;2.250000;,
   -0.750000;0.000000;2.250000;,
   0.750000;0.000000;1.500000;,
   1.500000;0.000000;0.750000;,
   0.750000;0.000000;0.000000;,
   0.750000;0.000000;0.750000;,
   0.750000;0.000000;3.000000;,
   1.500000;0.000000;2.250000;,
   0.750000;0.000000;2.250000;,
   2.250000;0.000000;3.000000;,
   3.000000;0.000000;2.250000;,
   2.250000;0.000000;1.500000;,
   2.250000;0.000000;2.250000;,
   3.000000;0.000000;0.750000;,
   2.250000;0.000000;0.000000;,
   2.250000;0.000000;0.750000;,
   0.750000;0.000000;-1.500000;,
   1.500000;0.000000;-2.250000;,
   0.750000;0.000000;-3.000000;,
   0.750000;0.000000;-2.250000;,
   1.500000;0.000000;-0.750000;,
   0.750000;0.000000;-0.750000;,
   3.000000;0.000000;-0.750000;,
   2.250000;0.000000;-1.500000;,
   2.250000;0.000000;-0.750000;,
   3.000000;0.000000;-2.250000;,
   2.250000;0.000000;-3.000000;,
   2.250000;0.000000;-2.250000;;
   176;
   3;175,174,1;,
   3;175,176,174;,
   3;176,172,118;,
   3;166,120,172;,
   3;119,166,176;,
   3;172,173,171;,
   3;173,163,102;,
   3;169,116,163;,
   3;120,169,173;,
   3;165,170,169;,
   3;170,153,116;,
   3;135,104,153;,
   3;108,135,170;,
   3;167,168,166;,
   3;168,165,120;,
   3;124,108,165;,
   3;103,124,168;,
   3;163,164,162;,
   3;164,160,115;,
   3;152,117,160;,
   3;116,152,164;,
   3;160,161,159;,
   3;161,158,2;,
   3;156,114,158;,
   3;103,125,124;,
   3;121,122,125;,
   3;125,109,123;,
   3;124,123,108;,
   3;105,129,122;,
   3;126,127,129;,
   3;129,106,128;,
   3;122,128,109;,
   3;109,133,132;,
   3;128,130,133;,
   3;133,100,131;,
   3;132,131,107;,
   3;108,136,135;,
   3;123,132,136;,
   3;136,107,134;,
   3;135,134,104;,
   3;104,140,139;,
   3;134,137,140;,
   3;140,113,138;,
   3;139,138,112;,
   3;107,143,137;,
   3;131,141,143;,
   3;143,110,142;,
   3;137,142,113;,
   3;113,147,146;,
   3;142,144,147;,
   3;147,3,145;,
   3;146,145,111;,
   3;112,150,149;,
   3;138,146,150;,
   3;150,111,148;,
   3;149,148,101;,
   3;104,154,153;,
   3;139,151,154;,
   3;154,117,152;,
   3;153,152,116;,
   3;112,157,151;,
   3;149,155,157;,
   3;157,114,156;,
   3;151,156,117;,
   3;117,161,160;,
   3;156,158,161;,
   3;161,2,159;,
   3;160,159,115;,
   3;116,164,163;,
   3;152,160,164;,
   3;164,115,162;,
   3;163,162,102;,
   3;103,168,167;,
   3;124,165,168;,
   3;168,120,166;,
   3;167,166,119;,
   3;108,170,165;,
   3;135,153,170;,
   3;170,116,169;,
   3;165,169,120;,
   3;120,173,172;,
   3;169,163,173;,
   3;173,102,171;,
   3;172,171,118;,
   3;119,176,175;,
   3;166,172,176;,
   3;176,118,174;,
   3;117,156,161;,
   3;109,128,133;,
   3;122,129,128;,
   3;151,157,156;,
   3;157,155,114;,
   3;149,101,155;,
   3;112,149,157;,
   3;153,154,152;,
   3;154,151,117;,
   3;139,112,151;,
   3;104,139,154;,
   3;149,150,148;,
   3;150,146,111;,
   3;138,113,146;,
   3;112,138,150;,
   3;146,147,145;,
   3;147,144,3;,
   3;142,110,144;,
   3;113,142,147;,
   3;137,143,142;,
   3;143,141,110;,
   3;131,100,141;,
   3;107,131,143;,
   3;139,140,138;,
   3;140,137,113;,
   3;103,121,125;,
   3;121,105,122;,
   3;125,122,109;,
   3;124,125,123;,
   3;105,126,129;,
   3;126,0,127;,
   3;129,127,106;,
   3;104,134,140;,
   3;134,107,137;,
   3;128,106,130;,
   3;133,130,100;,
   3;132,133,131;,
   3;108,123,136;,
   3;123,109,132;,
   3;136,132,107;,
   3;135,136,134;,
   3;6,8,96;,
   3;9,10,94;,
   3;11,12,92;,
   3;13,14,90;,
   3;15,16,88;,
   3;18,86,89;,
   3;20,84,87;,
   3;22,82,85;,
   3;24,80,83;,
   3;26,78,81;,
   3;28,76,79;,
   3;30,74,77;,
   3;32,72,75;,
   3;34,70,73;,
   3;36,68,71;,
   3;38,66,69;,
   3;40,64,67;,
   3;41,42,62;,
   3;43,44,60;,
   3;45,46,58;,
   3;47,48,56;,
   3;49,50,54;,
   3;51,4,52;,
   3;6,96,98;,
   3;9,94,97;,
   3;11,92,95;,
   3;13,90,93;,
   3;15,88,91;,
   3;18,89,17;,
   3;20,87,19;,
   3;22,85,21;,
   3;24,83,23;,
   3;26,81,25;,
   3;28,79,27;,
   3;30,77,29;,
   3;32,75,31;,
   3;34,73,33;,
   3;36,71,35;,
   3;38,69,37;,
   3;40,67,39;,
   3;41,62,65;,
   3;43,60,63;,
   3;45,58,61;,
   3;47,56,59;,
   3;49,54,57;,
   3;51,52,55;,
   3;5,99,53;,
   3;5,7,99;;

   MeshNormals {
    177;
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    -0.991445;0.000000;0.130526;,
    -0.991445;0.000000;-0.130526;,
    -0.923879;0.000000;-0.382684;,
    -0.991445;0.000000;-0.130526;,
    -0.923879;0.000000;-0.382684;,
    -0.793353;0.000000;-0.608761;,
    -0.793353;0.000000;-0.608761;,
    -0.608761;0.000000;-0.793353;,
    -0.608761;0.000000;-0.793353;,
    -0.382684;0.000000;-0.923879;,
    -0.382684;0.000000;-0.923879;,
    -0.130526;0.000000;-0.991445;,
    -0.130526;0.000000;-0.991445;,
    0.130526;0.000000;-0.991445;,
    0.130526;0.000000;-0.991445;,
    0.382684;0.000000;-0.923879;,
    0.382684;0.000000;-0.923879;,
    0.608761;0.000000;-0.793353;,
    0.608761;0.000000;-0.793353;,
    0.793353;0.000000;-0.608761;,
    0.793353;0.000000;-0.608761;,
    0.923879;0.000000;-0.382684;,
    0.923879;0.000000;-0.382684;,
    0.991445;0.000000;-0.130526;,
    0.991445;0.000000;-0.130526;,
    0.991445;0.000000;0.130526;,
    0.991445;0.000000;0.130526;,
    0.923879;0.000000;0.382684;,
    0.923879;0.000000;0.382684;,
    0.793353;0.000000;0.608761;,
    0.793353;0.000000;0.608761;,
    0.608761;0.000000;0.793353;,
    0.608761;0.000000;0.793353;,
    0.382684;0.000000;0.923879;,
    0.382684;0.000000;0.923879;,
    0.130526;0.000000;0.991445;,
    0.130526;0.000000;0.991445;,
    -0.130526;0.000000;0.991445;,
    -0.130526;0.000000;0.991445;,
    -0.382684;0.000000;0.923879;,
    -0.382684;0.000000;0.923879;,
    -0.608761;0.000000;0.793353;,
    -0.608761;0.000000;0.793353;,
    -0.793353;0.000000;0.608761;,
    -0.793353;0.000000;0.608761;,
    -0.923879;0.000000;0.382684;,
    -0.923879;0.000000;0.382684;,
    -0.991445;0.000000;0.130526;,
    -0.991445;0.000000;0.130526;,
    -0.991445;0.000000;-0.130526;,
    -0.923879;0.000000;0.382684;,
    -0.991445;0.000000;0.130526;,
    -0.793353;0.000000;0.608761;,
    -0.923879;0.000000;0.382684;,
    -0.608761;0.000000;0.793353;,
    -0.793353;0.000000;0.608761;,
    -0.382684;0.000000;0.923879;,
    -0.608761;0.000000;0.793353;,
    -0.130526;0.000000;0.991445;,
    -0.382684;0.000000;0.923879;,
    0.130526;0.000000;0.991445;,
    -0.130526;0.000000;0.991445;,
    0.382684;0.000000;0.923879;,
    0.130526;0.000000;0.991445;,
    0.608761;0.000000;0.793353;,
    0.382684;0.000000;0.923879;,
    0.793353;0.000000;0.608761;,
    0.608761;0.000000;0.793353;,
    0.923879;0.000000;0.382684;,
    0.793353;0.000000;0.608761;,
    0.991445;0.000000;0.130526;,
    0.923879;0.000000;0.382684;,
    0.991445;0.000000;-0.130526;,
    0.991445;0.000000;0.130526;,
    0.923879;0.000000;-0.382684;,
    0.991445;0.000000;-0.130526;,
    0.793353;0.000000;-0.608761;,
    0.923879;0.000000;-0.382684;,
    0.608761;0.000000;-0.793353;,
    0.793353;0.000000;-0.608761;,
    0.382684;0.000000;-0.923879;,
    0.608761;0.000000;-0.793353;,
    0.130526;0.000000;-0.991445;,
    0.382684;0.000000;-0.923879;,
    -0.130526;0.000000;-0.991445;,
    0.130526;0.000000;-0.991445;,
    -0.382684;0.000000;-0.923879;,
    -0.130526;0.000000;-0.991445;,
    -0.608761;0.000000;-0.793353;,
    -0.382684;0.000000;-0.923879;,
    -0.793353;0.000000;-0.608761;,
    -0.608761;0.000000;-0.793353;,
    -0.923879;0.000000;-0.382684;,
    -0.793353;0.000000;-0.608761;,
    -0.923879;0.000000;-0.382684;,
    -0.991445;0.000000;-0.130526;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;;
    176;
    3;175,174,1;,
    3;175,176,174;,
    3;176,172,118;,
    3;166,120,172;,
    3;119,166,176;,
    3;172,173,171;,
    3;173,163,102;,
    3;169,116,163;,
    3;120,169,173;,
    3;165,170,169;,
    3;170,153,116;,
    3;135,104,153;,
    3;108,135,170;,
    3;167,168,166;,
    3;168,165,120;,
    3;124,108,165;,
    3;103,124,168;,
    3;163,164,162;,
    3;164,160,115;,
    3;152,117,160;,
    3;116,152,164;,
    3;160,161,159;,
    3;161,158,2;,
    3;156,114,158;,
    3;103,125,124;,
    3;121,122,125;,
    3;125,109,123;,
    3;124,123,108;,
    3;105,129,122;,
    3;126,127,129;,
    3;129,106,128;,
    3;122,128,109;,
    3;109,133,132;,
    3;128,130,133;,
    3;133,100,131;,
    3;132,131,107;,
    3;108,136,135;,
    3;123,132,136;,
    3;136,107,134;,
    3;135,134,104;,
    3;104,140,139;,
    3;134,137,140;,
    3;140,113,138;,
    3;139,138,112;,
    3;107,143,137;,
    3;131,141,143;,
    3;143,110,142;,
    3;137,142,113;,
    3;113,147,146;,
    3;142,144,147;,
    3;147,3,145;,
    3;146,145,111;,
    3;112,150,149;,
    3;138,146,150;,
    3;150,111,148;,
    3;149,148,101;,
    3;104,154,153;,
    3;139,151,154;,
    3;154,117,152;,
    3;153,152,116;,
    3;112,157,151;,
    3;149,155,157;,
    3;157,114,156;,
    3;151,156,117;,
    3;117,161,160;,
    3;156,158,161;,
    3;161,2,159;,
    3;160,159,115;,
    3;116,164,163;,
    3;152,160,164;,
    3;164,115,162;,
    3;163,162,102;,
    3;103,168,167;,
    3;124,165,168;,
    3;168,120,166;,
    3;167,166,119;,
    3;108,170,165;,
    3;135,153,170;,
    3;170,116,169;,
    3;165,169,120;,
    3;120,173,172;,
    3;169,163,173;,
    3;173,102,171;,
    3;172,171,118;,
    3;119,176,175;,
    3;166,172,176;,
    3;176,118,174;,
    3;117,156,161;,
    3;109,128,133;,
    3;122,129,128;,
    3;151,157,156;,
    3;157,155,114;,
    3;149,101,155;,
    3;112,149,157;,
    3;153,154,152;,
    3;154,151,117;,
    3;139,112,151;,
    3;104,139,154;,
    3;149,150,148;,
    3;150,146,111;,
    3;138,113,146;,
    3;112,138,150;,
    3;146,147,145;,
    3;147,144,3;,
    3;142,110,144;,
    3;113,142,147;,
    3;137,143,142;,
    3;143,141,110;,
    3;131,100,141;,
    3;107,131,143;,
    3;139,140,138;,
    3;140,137,113;,
    3;103,121,125;,
    3;121,105,122;,
    3;125,122,109;,
    3;124,125,123;,
    3;105,126,129;,
    3;126,0,127;,
    3;129,127,106;,
    3;104,134,140;,
    3;134,107,137;,
    3;128,106,130;,
    3;133,130,100;,
    3;132,133,131;,
    3;108,123,136;,
    3;123,109,132;,
    3;136,132,107;,
    3;135,136,134;,
    3;6,8,96;,
    3;9,10,94;,
    3;11,12,92;,
    3;13,14,90;,
    3;15,16,88;,
    3;18,86,89;,
    3;20,84,87;,
    3;22,82,85;,
    3;24,80,83;,
    3;26,78,81;,
    3;28,76,79;,
    3;30,74,77;,
    3;32,72,75;,
    3;34,70,73;,
    3;36,68,71;,
    3;38,66,69;,
    3;40,64,67;,
    3;41,42,62;,
    3;43,44,60;,
    3;45,46,58;,
    3;47,48,56;,
    3;49,50,54;,
    3;51,4,52;,
    3;6,96,98;,
    3;9,94,97;,
    3;11,92,95;,
    3;13,90,93;,
    3;15,88,91;,
    3;18,89,17;,
    3;20,87,19;,
    3;22,85,21;,
    3;24,83,23;,
    3;26,81,25;,
    3;28,79,27;,
    3;30,77,29;,
    3;32,75,31;,
    3;34,73,33;,
    3;36,71,35;,
    3;38,69,37;,
    3;40,67,39;,
    3;41,62,65;,
    3;43,60,63;,
    3;45,58,61;,
    3;47,56,59;,
    3;49,54,57;,
    3;51,52,55;,
    3;5,99,53;,
    3;5,7,99;;
   }

   MeshTextureCoords {
    177;
    -2.500000;3.500000;,
    3.500000;3.500000;,
    3.500000;-2.500000;,
    -2.500000;-2.500000;,
    0.750000;1.000000;,
    0.750000;1.000000;,
    0.791667;1.000000;,
    0.791667;1.000000;,
    0.833333;1.000000;,
    0.833333;1.000000;,
    0.875000;1.000000;,
    0.875000;1.000000;,
    0.916667;1.000000;,
    0.916667;1.000000;,
    0.958333;1.000000;,
    0.958333;1.000000;,
    1.000000;1.000000;,
    0.000000;1.000000;,
    0.041667;1.000000;,
    0.041667;1.000000;,
    0.083333;1.000000;,
    0.083333;1.000000;,
    0.125000;1.000000;,
    0.125000;1.000000;,
    0.166667;1.000000;,
    0.166667;1.000000;,
    0.208333;1.000000;,
    0.208333;1.000000;,
    0.250000;1.000000;,
    0.250000;1.000000;,
    0.291667;1.000000;,
    0.291667;1.000000;,
    0.333333;1.000000;,
    0.333333;1.000000;,
    0.375000;1.000000;,
    0.375000;1.000000;,
    0.416667;1.000000;,
    0.416667;1.000000;,
    0.458333;1.000000;,
    0.458333;1.000000;,
    0.500000;1.000000;,
    0.500000;1.000000;,
    0.541667;1.000000;,
    0.541667;1.000000;,
    0.583333;1.000000;,
    0.583333;1.000000;,
    0.625000;1.000000;,
    0.625000;1.000000;,
    0.666667;1.000000;,
    0.666667;1.000000;,
    0.708333;1.000000;,
    0.708333;1.000000;,
    0.750000;0.000000;,
    0.750000;0.000000;,
    0.708333;0.000000;,
    0.708333;0.000000;,
    0.666667;0.000000;,
    0.666667;0.000000;,
    0.625000;0.000000;,
    0.625000;0.000000;,
    0.583333;0.000000;,
    0.583333;0.000000;,
    0.541667;0.000000;,
    0.541667;0.000000;,
    0.500000;0.000000;,
    0.500000;0.000000;,
    0.458333;0.000000;,
    0.458333;0.000000;,
    0.416667;0.000000;,
    0.416667;0.000000;,
    0.375000;0.000000;,
    0.375000;0.000000;,
    0.333333;0.000000;,
    0.333333;0.000000;,
    0.291667;0.000000;,
    0.291667;0.000000;,
    0.250000;0.000000;,
    0.250000;0.000000;,
    0.208333;0.000000;,
    0.208333;0.000000;,
    0.166667;0.000000;,
    0.166667;0.000000;,
    0.125000;0.000000;,
    0.125000;0.000000;,
    0.083333;0.000000;,
    0.083333;0.000000;,
    0.041667;0.000000;,
    0.041667;0.000000;,
    1.000000;0.000000;,
    0.000000;0.000000;,
    0.958333;0.000000;,
    0.958333;0.000000;,
    0.916667;0.000000;,
    0.916667;0.000000;,
    0.875000;0.000000;,
    0.875000;0.000000;,
    0.833333;0.000000;,
    0.833333;0.000000;,
    0.791667;0.000000;,
    0.791667;0.000000;,
    -2.500000;0.500000;,
    0.500000;-2.500000;,
    3.500000;0.500000;,
    0.500000;3.500000;,
    0.500000;0.500000;,
    -1.000000;3.500000;,
    -2.500000;2.000000;,
    -1.000000;0.500000;,
    0.500000;2.000000;,
    -1.000000;2.000000;,
    -2.500000;-1.000000;,
    -1.000000;-2.500000;,
    0.500000;-1.000000;,
    -1.000000;-1.000000;,
    2.000000;-2.500000;,
    3.500000;-1.000000;,
    2.000000;0.500000;,
    2.000000;-1.000000;,
    3.500000;2.000000;,
    2.000000;3.500000;,
    2.000000;2.000000;,
    -0.250000;3.500000;,
    -1.000000;2.750000;,
    -0.250000;2.000000;,
    0.500000;2.750000;,
    -0.250000;2.750000;,
    -1.750000;3.500000;,
    -2.500000;2.750000;,
    -1.750000;2.000000;,
    -1.750000;2.750000;,
    -2.500000;1.250000;,
    -1.750000;0.500000;,
    -1.000000;1.250000;,
    -1.750000;1.250000;,
    -0.250000;0.500000;,
    0.500000;1.250000;,
    -0.250000;1.250000;,
    -1.000000;-0.250000;,
    -0.250000;-1.000000;,
    0.500000;-0.250000;,
    -0.250000;-0.250000;,
    -2.500000;-0.250000;,
    -1.750000;-1.000000;,
    -1.750000;-0.250000;,
    -2.500000;-1.750000;,
    -1.750000;-2.500000;,
    -1.000000;-1.750000;,
    -1.750000;-1.750000;,
    -0.250000;-2.500000;,
    0.500000;-1.750000;,
    -0.250000;-1.750000;,
    1.250000;-1.000000;,
    2.000000;-0.250000;,
    1.250000;0.500000;,
    1.250000;-0.250000;,
    1.250000;-2.500000;,
    2.000000;-1.750000;,
    1.250000;-1.750000;,
    2.750000;-2.500000;,
    3.500000;-1.750000;,
    2.750000;-1.000000;,
    2.750000;-1.750000;,
    3.500000;-0.250000;,
    2.750000;0.500000;,
    2.750000;-0.250000;,
    1.250000;2.000000;,
    2.000000;2.750000;,
    1.250000;3.500000;,
    1.250000;2.750000;,
    2.000000;1.250000;,
    1.250000;1.250000;,
    3.500000;1.250000;,
    2.750000;2.000000;,
    2.750000;1.250000;,
    3.500000;2.750000;,
    2.750000;3.500000;,
    2.750000;2.750000;;
   }

   VertexDuplicationIndices {
    177;
    177;
    0,
    1,
    2,
    3,
    4,
    4,
    6,
    6,
    8,
    8,
    10,
    10,
    12,
    12,
    14,
    14,
    16,
    16,
    18,
    18,
    20,
    20,
    22,
    22,
    24,
    24,
    26,
    26,
    28,
    28,
    30,
    30,
    32,
    32,
    34,
    34,
    36,
    36,
    38,
    38,
    40,
    40,
    42,
    42,
    44,
    44,
    46,
    46,
    48,
    48,
    50,
    50,
    52,
    52,
    54,
    54,
    56,
    56,
    58,
    58,
    60,
    60,
    62,
    62,
    64,
    64,
    66,
    66,
    68,
    68,
    70,
    70,
    72,
    72,
    74,
    74,
    76,
    76,
    78,
    78,
    80,
    80,
    82,
    82,
    84,
    84,
    86,
    86,
    88,
    88,
    90,
    90,
    92,
    92,
    94,
    94,
    96,
    96,
    98,
    98,
    100,
    101,
    102,
    103,
    104,
    105,
    106,
    107,
    108,
    109,
    110,
    111,
    112,
    113,
    114,
    115,
    116,
    117,
    118,
    119,
    120,
    121,
    122,
    123,
    124,
    125,
    126,
    127,
    128,
    129,
    130,
    131,
    132,
    133,
    134,
    135,
    136,
    137,
    138,
    139,
    140,
    141,
    142,
    143,
    144,
    145,
    146,
    147,
    148,
    149,
    150,
    151,
    152,
    153,
    154,
    155,
    156,
    157,
    158,
    159,
    160,
    161,
    162,
    163,
    164,
    165,
    166,
    167,
    168,
    169,
    170,
    171,
    172,
    173,
    174,
    175,
    176;
   }

   MeshMaterialList {
    2;
    176;
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1;

    Material {
     0.784314;0.784314;0.784314;1.000000;;
     51.200001;
     0.000000;0.000000;0.000000;;
     0.000000;0.000000;0.000000;;
     TextureFilename {
      "tile.bmp";
     }
    }

    Material {
     1.000000;1.000000;1.000000;1.000000;;
     51.200001;
     0.000000;0.000000;0.000000;;
     0.000000;0.000000;0.000000;;
     TextureFilename {
      "sky.bmp";
     }
    }
   }
  }
 }
}