#include "balisemsganalyse.h"
#include "balisedemod.h"
#include <QTextStream>

const UCHAR BaliseMsgAnalyse::GLX[76] =
{
	1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0,
	1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0,
	1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1,
	0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1,
	0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1,
	0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1,
};

//十一比特转十比特数组初始化
const short BaliseMsgAnalyse::CONVER_TABLE_OF_11BIT_TO_10BIT[] =
{
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,     0,     1,     2,     3,     4,     5,     6,
	7,     8,     9,    10,    11,    12,    13,    14,
	15,    16,    17,    18,    19,    20,    21,    22,
	23,    24,    25,    26,    27,    28,    -1,    -1,
	-1,    29,    30,    31,    32,    33,    34,    35,
	36,    37,    38,    39,    40,    41,    42,    43,
	44,    45,    46,    47,    48,    49,    50,    51,
	52,    53,    54,    55,    56,    57,    58,    -1,
	-1,    59,    -1,    -1,    -1,    -1,    60,    -1,
	-1,    61,    -1,    -1,    62,    -1,    63,    64,
	65,    -1,    66,    67,    68,    69,    70,    -1,
	-1,    71,    -1,    72,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    73,    74,    75,    -1,
	-1,    -1,    -1,    76,    -1,    -1,    -1,    77,
	78,    79,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    80,    81,    82,    83,    84,    -1,
	-1,    85,    -1,    86,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    87,    -1,    88,
	89,    90,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    91,    -1,    92,    -1,    -1,    -1,
	-1,    93,    94,    95,    96,    -1,    97,    -1,
	-1,    -1,    98,    99,    -1,    -1,    -1,   100,
	101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,   102,    -1,   103,    -1,
	-1,   104,    -1,   105,   106,   107,   108,   109,
	110,   111,   112,   113,    -1,    -1,   114,   115,
	116,    -1,    -1,    -1,   117,   118,   119,   120,
	-1,    -1,   121,   122,    -1,    -1,    -1,    -1,
	-1,    -1,   123,   124,    -1,   125,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   126,   127,
	128,   129,    -1,    -1,   130,   131,    -1,    -1,
	132,   133,   134,    -1,   135,   136,   137,    -1,
	-1,   138,   139,   140,   141,   142,   143,   144,
	-1,    -1,    -1,    -1,    -1,    -1,   145,   146,
	147,   148,   149,   150,   151,   152,    -1,    -1,
	153,   154,   155,   156,   157,   158,    -1,    -1,
	-1,    -1,    -1,    -1,   159,   160,   161,   162,
	163,   164,   165,   166,   167,   168,   169,   170,
	171,   172,   173,   174,    -1,    -1,    -1,    -1,
	-1,   175,    -1,   176,    -1,    -1,   177,    -1,
	-1,   178,   179,    -1,   180,   181,    -1,    -1,
	182,   183,   184,   185,   186,   187,   188,   189,
	190,   191,   192,   193,   194,   195,   196,   197,
	198,    -1,    -1,    -1,   199,   200,    -1,    -1,
	-1,    -1,    -1,    -1,   201,   202,   203,   204,
	205,   206,   207,   208,   209,   210,   211,   212,
	213,   214,   215,   216,    -1,    -1,   217,   218,
	-1,    -1,   219,    -1,   220,   221,   222,    -1,
	-1,   223,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,   224,   225,    -1,    -1,   226,   227,
	228,   229,   230,   231,    -1,    -1,    -1,    -1,
	232,   233,   234,   235,   236,   237,    -1,    -1,
	-1,    -1,   238,   239,   240,   241,   242,   243,
	244,   245,   246,   247,   248,   249,   250,   251,
	252,   253,    -1,    -1,   254,   255,   256,   257,
	-1,    -1,   258,   259,    -1,    -1,   260,    -1,
	-1,   261,    -1,    -1,   262,   263,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   264,   265,
	266,   267,   268,   269,   270,   271,    -1,    -1,
	272,   273,   274,   275,   276,   277,    -1,    -1,
	-1,    -1,    -1,   278,   279,   280,   281,   282,
	-1,    -1,    -1,    -1,   283,    -1,    -1,   284,
	285,   286,   287,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,   288,   289,    -1,
	-1,   290,   291,   292,    -1,    -1,    -1,    -1,
	293,    -1,    -1,    -1,   294,   295,   296,   297,
	298,   299,   300,   301,   302,   303,   304,   305,
	306,   307,   308,   309,    -1,    -1,    -1,    -1,
	-1,    -1,   310,   311,   312,   313,   314,   315,
	-1,   316,   317,   318,   319,   320,   321,   322,
	323,    -1,    -1,    -1,   324,    -1,   325,   326,
	-1,    -1,    -1,    -1,    -1,    -1,   327,    -1,
	-1,   328,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,   329,    -1,    -1,   330,
	331,   332,   333,   334,   335,   336,   337,   338,
	339,   340,   341,   342,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,   343,   344,   345,    -1,    -1,
	-1,    -1,    -1,   346,   347,   348,   349,   350,
	351,   352,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,   353,   354,   355,   356,   357,    -1,
	-1,   358,   359,   360,    -1,   361,   362,   363,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,   364,
	365,   366,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,   367,    -1,   368,   369,    -1,    -1,
	-1,    -1,   370,   371,   372,   373,    -1,    -1,
	374,   375,   376,   377,    -1,   378,   379,   380,
	381,   382,    -1,    -1,   383,   384,    -1,    -1,
	385,   386,   387,   388,   389,    -1,   390,    -1,
	-1,   391,    -1,   392,    -1,    -1,   393,   394,
	-1,    -1,    -1,    -1,   395,   396,   397,   398,
	399,    -1,    -1,    -1,   400,   401,    -1,    -1,
	-1,   402,    -1,   403,   404,   405,    -1,    -1,
	-1,    -1,    -1,   406,    -1,   407,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   408,   409,
	410,    -1,   411,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,   412,   413,   414,    -1,
	-1,   415,   416,   417,   418,   419,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   420,   421,
	422,    -1,    -1,    -1,   423,    -1,    -1,    -1,
	-1,    -1,   424,   425,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,   426,    -1,   427,    -1,
	428,   429,   430,   431,   432,    -1,    -1,   433,
	434,   435,   436,   437,    -1,    -1,   438,   439,
	-1,    -1,    -1,    -1,    -1,    -1,   440,    -1,
	-1,   441,    -1,   442,   443,   444,   445,   446,
	447,   448,   449,   450,   451,   452,   453,   454,
	455,   456,   457,   458,   459,   460,   461,    -1,
	462,   463,   464,   465,    -1,   466,    -1,    -1,
	-1,    -1,    -1,   467,   468,   469,    -1,    -1,
	470,   471,   472,   473,   474,   475,   476,   477,
	478,   479,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,   480,    -1,   481,   482,   483,    -1,
	-1,   484,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	485,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,   486,   487,   488,   489,
	490,   491,   492,   493,   494,   495,   496,   497,
	498,   499,   500,   501,   502,   503,   504,   505,
	506,   507,   508,   509,   510,   511,    -1,    -1,
	-1,    -1,   512,   513,   514,   515,   516,   517,
	518,   519,   520,   521,   522,   523,   524,   525,
	526,   527,   528,   529,   530,   531,   532,   533,
	534,   535,   536,   537,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,   538,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   539,    -1,
	-1,   540,   541,   542,    -1,   543,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   544,   545,
	546,   547,   548,   549,   550,   551,   552,   553,
	-1,    -1,   554,   555,   556,    -1,    -1,    -1,
	-1,    -1,   557,    -1,   558,   559,   560,   561,
	-1,   562,   563,   564,   565,   566,   567,   568,
	569,   570,   571,   572,   573,   574,   575,   576,
	577,   578,   579,   580,   581,    -1,   582,    -1,
	-1,   583,    -1,    -1,    -1,    -1,    -1,    -1,
	584,   585,    -1,    -1,   586,   587,   588,   589,
	590,    -1,    -1,   591,   592,   593,   594,   595,
	-1,   596,    -1,   597,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,   598,   599,    -1,    -1,
	-1,    -1,    -1,   600,    -1,    -1,    -1,   601,
	602,   603,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,   604,   605,   606,   607,   608,    -1,
	-1,   609,   610,   611,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,   612,    -1,   613,
	614,   615,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,   616,    -1,   617,    -1,    -1,    -1,
	-1,    -1,   618,   619,   620,    -1,   621,    -1,
	-1,    -1,   622,   623,    -1,    -1,    -1,   624,
	625,   626,   627,   628,    -1,    -1,    -1,    -1,
	629,   630,    -1,    -1,   631,    -1,   632,    -1,
	-1,   633,    -1,   634,   635,   636,   637,   638,
	-1,    -1,   639,   640,    -1,    -1,   641,   642,
	643,   644,   645,    -1,   646,   647,   648,   649,
	-1,    -1,   650,   651,   652,   653,    -1,    -1,
	-1,    -1,   654,   655,    -1,   656,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   657,   658,
	659,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	660,   661,   662,    -1,   663,   664,   665,    -1,
	-1,   666,   667,   668,   669,   670,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   671,   672,
	673,   674,   675,   676,   677,    -1,    -1,    -1,
	-1,    -1,   678,   679,   680,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,   681,   682,   683,   684,
	685,   686,   687,   688,   689,   690,   691,   692,
	693,    -1,    -1,   694,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   695,    -1,
	-1,   696,    -1,    -1,    -1,    -1,    -1,    -1,
	697,   698,    -1,   699,    -1,    -1,    -1,   700,
	701,   702,   703,   704,   705,   706,   707,    -1,
	708,   709,   710,   711,   712,   713,    -1,    -1,
	-1,    -1,    -1,    -1,   714,   715,   716,   717,
	718,   719,   720,   721,   722,   723,   724,   725,
	726,   727,   728,   729,    -1,    -1,    -1,   730,
	-1,    -1,    -1,    -1,   731,   732,   733,    -1,
	-1,   734,   735,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,   736,   737,   738,
	739,    -1,    -1,   740,    -1,    -1,    -1,    -1,
	741,   742,   743,   744,   745,    -1,    -1,    -1,
	-1,    -1,   746,   747,   748,   749,   750,   751,
	-1,    -1,   752,   753,   754,   755,   756,   757,
	758,   759,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,   760,   761,    -1,    -1,   762,    -1,
	-1,   763,    -1,    -1,   764,   765,    -1,    -1,
	766,   767,   768,   769,    -1,    -1,   770,   771,
	772,   773,   774,   775,   776,   777,   778,   779,
	780,   781,   782,   783,   784,   785,    -1,    -1,
	-1,    -1,   786,   787,   788,   789,   790,   791,
	-1,    -1,    -1,    -1,   792,   793,   794,   795,
	796,   797,    -1,    -1,   798,   799,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   800,    -1,
	-1,   801,   802,   803,    -1,   804,    -1,    -1,
	805,   806,    -1,    -1,   807,   808,   809,   810,
	811,   812,   813,   814,   815,   816,   817,   818,
	819,   820,   821,   822,    -1,    -1,    -1,    -1,
	-1,    -1,   823,   824,    -1,    -1,    -1,   825,
	826,   827,   828,   829,   830,   831,   832,   833,
	834,   835,   836,   837,   838,   839,   840,   841,
	-1,    -1,   842,   843,    -1,   844,   845,    -1,
	-1,   846,    -1,    -1,   847,    -1,   848,    -1,
	-1,    -1,    -1,    -1,   849,   850,   851,   852,
	853,   854,   855,   856,   857,   858,   859,   860,
	861,   862,   863,   864,    -1,    -1,    -1,    -1,
	-1,    -1,   865,   866,   867,   868,   869,   870,
	-1,    -1,   871,   872,   873,   874,   875,   876,
	877,   878,    -1,    -1,    -1,    -1,    -1,    -1,
	879,   880,   881,   882,   883,   884,   885,    -1,
	-1,   886,   887,   888,    -1,   889,   890,   891,
	-1,    -1,   892,   893,    -1,    -1,   894,   895,
	896,   897,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,   898,    -1,   899,   900,    -1,    -1,
	-1,    -1,    -1,    -1,   901,   902,    -1,    -1,
	903,   904,   905,   906,    -1,    -1,    -1,   907,
	908,   909,    -1,    -1,   910,   911,   912,   913,
	914,   915,   916,   917,   918,    -1,   919,    -1,
	-1,   920,    -1,   921,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,   922,
	923,    -1,    -1,    -1,   924,   925,    -1,    -1,
	-1,   926,    -1,   927,   928,   929,   930,    -1,
	-1,    -1,    -1,   931,    -1,   932,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   933,   934,
	935,    -1,   936,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,   937,    -1,   938,    -1,
	-1,   939,   940,   941,   942,   943,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,   944,   945,
	946,    -1,    -1,    -1,   947,    -1,    -1,    -1,
	-1,   948,   949,   950,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,   951,    -1,   952,    -1,
	-1,   953,   954,   955,   956,   957,    -1,   958,
	959,   960,    -1,   961,    -1,    -1,   962,    -1,
	-1,   963,    -1,    -1,    -1,    -1,   964,    -1,
	-1,   965,   966,   967,   968,   969,   970,   971,
	972,   973,   974,   975,   976,   977,   978,   979,
	980,   981,   982,   983,   984,   985,   986,   987,
	988,   989,   990,   991,   992,   993,   994,    -1,
	-1,    -1,   995,   996,   997,   998,   999,  1000,
	1001,  1002,  1003,  1004,  1005,  1006,  1007,  1008,
	1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,
	1017,  1018,  1019,  1020,  1021,  1022,  1023,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
	-1,    -1,    -1,    -1,    -1,    -1,    -1,    -1
};


BaliseMsgAnalyse::BaliseMsgAnalyse()
{
}

BaliseMsgAnalyse::~BaliseMsgAnalyse()
{
}

bool BaliseMsgAnalyse::analyse(Balise &balise, QVector<unsigned char> demodedBitVec)
{
	// 将FSK数据有比特形式的存储转换为字节形式的存储,由于接收到的FSK比特数据可能不是8的整数倍,
	// 为了便于后面的处理，增加一个字节
	size_t fskLenInByte = demodedBitVec.size() / 8 + 1;		// FSK数据长度，单位为字节
	balise.demodedByteVec.resize(fskLenInByte);
	for (size_t i = 0; i<demodedBitVec.size(); /*NULL*/)
		for (size_t j = i / 8, k = 0; k<8 && i<demodedBitVec.size(); ++k, ++i)
		{
			balise.demodedByteVec[j] <<= 1;
			balise.demodedByteVec[j] |= demodedBitVec[i];
		}

	// 判断FSK中是否存在正确的报文数据
	int		correctMSGPos = -1; // FSK中正确报文的起始位置
	UINT	controlBit = 0;	    // 控制比特的取值
	for (size_t i = 0; fskLenInByte >= LONG_CODE_MSG_LEN_IN_BYTE && i<fskLenInByte - LONG_CODE_MSG_LEN_IN_BYTE; ++i)
	{
		if (balise.demodedByteVec[i] != 0x00 && balise.demodedByteVec[i] != 0xFF && // 防止全零或者全1的数据
			true == isCodedMSGCorrect(balise.demodedByteVec.begin() + i, TRUE))
		{
			correctMSGPos = i;
			baliseMSGSynch(balise.demodedByteVec.begin() + correctMSGPos, balise.msgData);
			controlBit = (balise.msgData[114] >> 4) & 7;	// 控制比特的取值
			break;
		}
	}

	if ((correctMSGPos == -1) || ((controlBit != 1) && (controlBit != 6))
		|| !decode(balise.msgData, balise.userData, TRUE)) // FSK中没有正确的报文
	{
		return false;
	}
	else // 找到正确报文，进行同步，然后获取应答器编号
	{
		balise.baliseID = getBaliseID(balise.userData);

		// 通过报文校验判断FSK是否正确
		QVector<BYTE>		fskErrorBitData(fskLenInByte);			// 存放FSK数据错误比特信息的FSK
		memset(&fskErrorBitData[0], 0xFF, fskLenInByte);
		for (size_t i = correctMSGPos; fskLenInByte >= LONG_CODE_MSG_LEN_IN_BYTE
			&& i<fskLenInByte - LONG_CODE_MSG_LEN_IN_BYTE; ++i)
		{
			if (true == isCodedMSGCorrect(balise.demodedByteVec.begin() + i, TRUE))
			{
				memset(&fskErrorBitData[0] + i, 0, LONG_CODE_MSG_LEN_IN_BYTE);		// 连续1023比特正确,误码率均设为0
				// 判断第1024比特(即报文的第一个比特)是否正确
				fskErrorBitData[i + LONG_CODE_MSG_LEN_IN_BYTE - 1] |=
					(balise.demodedByteVec[i + LONG_CODE_MSG_LEN_IN_BYTE - 1] & 1) ^ (balise.demodedByteVec[i] >> 7);

				i += LONG_CODE_MSG_LEN_IN_BYTE - 1;
			}
		}
	
		// 获取连续八组正确1023比特报文数据，用于后续比较
		BYTE	cmpMSGBuffer[LONG_CODE_MSG_LEN_IN_BIT];			// 连续8组正确的FSK中同步前的编码后应答器报文数据
		for (size_t i = 0; i<LONG_CODE_MSG_LEN_IN_BIT * 8; /*NULL*/)
			for (int j = 0, k = 7; j<LONG_CODE_MSG_LEN_IN_BIT; ++j, ++i)
			{
				cmpMSGBuffer[i / 8] <<= 1;
				cmpMSGBuffer[i / 8] |= (balise.demodedByteVec[correctMSGPos + j / 8] >> k) & 1;
				k = k - 1 < 0 ? 7 : k - 1;
			}

		// 获取FSK误码率
		int		cmpBytePos(sizeof(cmpMSGBuffer) - (correctMSGPos % sizeof(cmpMSGBuffer)));	// 当前比较的报文字节在正确报文中的位置			
		for (size_t i = 0; i<fskLenInByte; ++i, cmpBytePos++)
		{
			if (fskErrorBitData[i] == 0xFF)		// 这段数据的报文内容校验错误，通过异或获取错误比特信息
			{
				fskErrorBitData[i] = balise.demodedByteVec[i]
					^ cmpMSGBuffer[cmpBytePos % sizeof(cmpMSGBuffer)];
			}
		}

        // 每个UCHAR型整数中每位上取值为1的比特个数
		const UCHAR ONE_BIT_NUM_TABLE[] =
		{
			0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
			1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
			1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
			2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
			1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
			2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
			2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
			3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
		};


		const size_t	BER_CALC_LEN(32);			// 误码率分组统计时每组的数据比特数据

		for (size_t i = 0; i<fskLenInByte; /*NULL*/)
		{
			size_t		berSum(0);

			// 每32比特进行一次统计
			for (size_t j = 0; j<BER_CALC_LEN / 8 && i + j<fskLenInByte; ++j, ++i)
			{
				berSum += ONE_BIT_NUM_TABLE[fskErrorBitData[j + i]];
			}

			balise.errorRateVec.push_back(berSum / (double)BER_CALC_LEN);
		}
	
		// 统计最大连续正确比特个数和有效报文个数
		size_t			totalCorrectBitNum(0);		// 所有的正确比特个数
		size_t			maxConCorrectBitNum(0);		// 最大连续正确比特个数
		size_t			curConCorrectBitNum(0);		// 当前统计所得连续正确比特个数
		size_t			correctMSGNum(0);			// 正确的应答器报文个数

		for (size_t i = 0; i<fskLenInByte; ++i)
			for (int j = 7; j >= 0; --j)
			{
				if (!((fskErrorBitData[i] >> j) & 1))
				{
					curConCorrectBitNum++;
					totalCorrectBitNum++;
				}// 碰到正确的比特，更新正确比特个数

				if (((fskErrorBitData[i] >> j) & 1) || (i == fskLenInByte - 1 && j == 0))
				{
					if (curConCorrectBitNum > maxConCorrectBitNum)
					{
						maxConCorrectBitNum = curConCorrectBitNum;
					}// 之前统计的连续正确比特数比之前统计的MaxConCorrectBitNum大，更新MaxConCorrectBitNum

					if (balise.firstCorrectBitPos <= 0
						&& curConCorrectBitNum >= LONG_CODE_MSG_LEN_IN_BIT)
					{
						balise.firstCorrectBitPos = i * 8 + (8 - j) - curConCorrectBitNum;
						balise.firstCorrectBitPos = balise.firstCorrectBitPos ?
							balise.firstCorrectBitPos : 1;		// 报文全部正确
					}// 统计第一个能解出应答器报文的正确比特

					 // 更新正确应答器报文个数
					correctMSGNum += curConCorrectBitNum / LONG_CODE_MSG_LEN_IN_BIT;

					curConCorrectBitNum = 0;
				}// 碰到错误的比特或者最后一个比特，重新开始进行统计
			}

		balise.maxRightBitNum = maxConCorrectBitNum;
		balise.correctMSGNum = correctMSGNum;
		balise.totalCorrectBitNum = totalCorrectBitNum;
		return true;
		////开始计算MTIE
		//calMTIE(&realDeviationOf1bitVec[correctMSGPos * 8], balise.MTIEVec);

		////开始计算数据速率
		//int temp = 0;
		//int sum_bit = 0;
		//int first_inflexion = 0;
		//int last_inflexion = 0;
		//for (int i = 0; i < bitNumVec.size(); i++)
		//{
		//	temp += bitNumVec[i];
		//	if (temp >= correctMSGPos * 8)
		//	{
		//		sum_bit += bitNumVec[i];
		//		if (first_inflexion == 0)
		//		{
		//			first_inflexion = inflexionVec[i];
		//		}
		//		if (sum_bit >= 500)
		//		{
		//			last_inflexion = inflexionVec[i];
  //                  balise.bps = sum_bit / ((last_inflexion - first_inflexion)*(1 / (sampleRate * 1e3)));
		//		}
		//	}
		//	
		//}
	}
}

bool BaliseMsgAnalyse::isCodedMSGCorrect(QVector<UCHAR>::iterator msgBufferIter, const bool isLong)
{
	ULONG			shiftRegster[3] = { 0 }; 			// 计算校验位的寄存器
	const size_t	GLX_LEN_IN_BIT(75);					// 循环码编码生成多项式的位数
	//循环码编码器生成多项式gL(x)，由高（75）到低（0）依次放在三个字中
	// gL(x) = x75 + x73 + x72 + x71 + x67 + x62 + x61 + x60       ;1011,1000,1000,0111
	//	     + x57 + x56 + x55 + x52 + x51 + x49                   ;0011,1001,1010
	//	     + x46 + x45 + x44 + x43 + x41 + x37 + x35 + x34 + x33 ;0111,1010,0010,1110
	//	     + x31 + x30 + x28 + x26 + x24 + x21 + x17 + x16       ;1101,0101,0010,0011
	//	     + x15 + x13 + x12 + x11 + x9  + x4  + x   + 1         ;1011,1010,0001,0011
	const ULONG 	GLX[3] = {
		0xB88,       	//1011,1000,1000
		0x739A7A2E,  	//0111,0011,1001,1010,0111,1010,0010,1110
		0xD523BA13,  	//1101,0101,0010,0011,1011,1010,0001,0011
	};
	//循环码编码器生成多项式gs(x)，由高（75）到低（0）依次放在三个字中
	const ULONG 	GSX[3] = {
		0x9F7,       	//100111110111
		0x90C2FEF7,  	//10010000110000101111111011110111
		0xCA4A3C4B,  	//11001010010010100011110001001011
	};
	// 检验是否满足奇偶校验条件，即能否被g(x)整除
	const size_t		MSG_LEN_IN_BIT(isLong == true ? LONG_CODE_MSG_LEN_IN_BIT : SHORT_CODE_MSG_LEN_IN_BIT);
	for (size_t i = 0; i<MSG_LEN_IN_BIT; /*NULL*/)
		for (int j = UCHAR_LEN_IN_BIT - 1; j >= 0 && i<MSG_LEN_IN_BIT; --j, ++i)
		{
			// 如果寄存器移出的数据为1则进行反馈运算
			if (leftShiftData(shiftRegster, GLX_LEN_IN_BIT, (*(msgBufferIter + i / UCHAR_LEN_IN_BIT) >> j) & 1))
			{
				xorData(shiftRegster, isLong == true ? GLX : GSX);
			}
		}
	shiftRegster[0] &= 0xFFF;
	return shiftRegster[0] | shiftRegster[1] | shiftRegster[2] ? false : true;
}

/*
功能：
将将85比特数组数据依次左移一位并追加数据

输入参数：
1. regData		：	当前寄存器的数值
2. regBitLen	:	移位寄存器的长度
2. bitAppend	:	移位后追加的比特

输入参数：
移出出的数据，即移位之后数组中第一个元素的第22比特上的数值
*/
UCHAR BaliseMsgAnalyse::leftShiftData(ULONG* regData, const size_t regBitLen, ULONG bitAppend)
{
	//assert(NULL != regData);

	regData[0] = (regData[0] << 1) | (regData[1] >> 31);
	regData[1] = (regData[1] << 1) | (regData[2] >> 31);
	regData[2] = (regData[2] << 1) | bitAppend;

	return (regData[0] >> (regBitLen % ULONG_LEN_IN_BIT)) & 1;
};

/*
功能：
对两个数组数据进行异或并将结果赋予第一个数组

输入参数：
1. DataArray1	：	第一个数组
2. DataArray2	:	第二个数组

输出参数：
无
*/
void BaliseMsgAnalyse::xorData(ULONG* dataArray1, const ULONG* dataArray2)
{
	dataArray1[0] ^= dataArray2[0];
	dataArray1[1] ^= dataArray2[1];
	dataArray1[2] ^= dataArray2[2];
};

bool BaliseMsgAnalyse::baliseMSGSynch(QVector<UCHAR>::iterator msgBeforeSynchIter, UCHAR* msgAfterSynch)
{
	//assert(NULL != msgAfterSynch);

	USHORT		shiftRegState = 0;				// 移位寄存器中存储的数值
	UCHAR		shiftRegOutput = 0;				// 移位寄存器移出的数值

	for (size_t i = 0, j = 8; i<LONG_CODE_MSG_LEN_IN_BIT; ++i)
	{
		shiftRegState = (shiftRegState << 1) | getBitValue(*(msgBeforeSynchIter + i / 8), j);
		shiftRegState ^= getBitValue(shiftRegState, 11) ? FLX : 0;
		j = j - 1 ? j - 1 : 8;
	}

	shiftRegState &= 0x7FF;
	int			fistBitPos = isValidSynchCode(shiftRegState);	// 同步前报文第一个比特在实际报文中的位置
	if (-1 != fistBitPos)
	{
		for (size_t i = LONG_CODE_MSG_LEN_IN_BIT - fistBitPos, j = 0; i<LONG_CODE_MSG_LEN_IN_BIT; ++i, ++j)
		{
			msgAfterSynch[j / 8] <<= 1;
			msgAfterSynch[j / 8] |= (*(msgBeforeSynchIter + i / 8) >> (7 - i % 8)) & 1;
		}

		for (size_t i = 0, j = fistBitPos; i<LONG_CODE_MSG_LEN_IN_BIT - fistBitPos; ++i, ++j)
		{
			msgAfterSynch[j / 8] <<= 1;
			msgAfterSynch[j / 8] |= (*(msgBeforeSynchIter + i / 8) >> (7 - i % 8)) & 1;
		}

		msgAfterSynch[LONG_CODE_MSG_LEN_IN_BYTE - 1] <<= 1;

		return true;
	}

	return false;
}

void BaliseMsgAnalyse::caculateMTIEAndBPS(Balise &balise, const QVector<float>& sampleDataVec)
{
	//找到正确bit对应的原始采样数据位置，并获取600个bit采样数据
	QVector<float> temp_sampleDataVec;
	temp_sampleDataVec.resize(POINTS1BIT_1 * 600);
	double b = sampleDataVec[0];
	int copypos = balise.firstCorrectBitPos*POINTS1BIT_1;
	for (auto &data : temp_sampleDataVec)
	{
		data = sampleDataVec.at(copypos++);
	}
	//memcpy(&temp_sampleDataVec[0], &sampleDataVec.at(balise.firstCorrectBitPos*POINTS1BIT_1), POINTS1BIT_1 * 600 * sizeof(double));
	QVector<float> phaseData;
	BaliseDemod::hilbertPhaseTrans(1, temp_sampleDataVec, phaseData);//对正确bit后面600个bit进行希尔伯特变换
	//QFile bittxt("hibert.txt");
	//if (bittxt.open(QIODevice::WriteOnly))
	//{
	//	QTextStream out(&bittxt);
	//	for (int i = 0; i < phaseData.size(); i++)
	//	{
	//		out << phaseData[i] << endl;
	//	}
	//	bittxt.close();
	//}
	QVector <double>  bps_err_Bit_1;
	QVector <double>  bps_err_Bit_2;
	//开始直线拟合
	int N = 32;
	//前后拟合使用的点数
	double a_r, b_r, a_l, b_l, x0;
	int pre_position = 0, start_idx = 0, demodedBitNum = 0, inflectionNum = 0, lengthOfSameBits = 0;
	double tempSuma = 0, tempSumb = 0, yinzi = 0;

	double	sum_x, sum_xx;
	double	sum_y_Left, sum_xy_Left;
	double	sum_y_Right, sum_xy_Right;
	double  idx_intersect_point = 0, start_true_point = 0, temp = 0;
	double  factBps, BPS, sum_BPS = 0;
	int cnt_Bit_Dem = 0;
	int		m_TermNum;
	for (int i = N; ; i++) //从相位的第20点开始找拐点
	{
		//计算右斜率
		tempSuma = 0, tempSumb = 0, yinzi = 0;
		for (int s = 1; s <= N; s++)
		{
			tempSuma += (2 * s - (N + 1))*phaseData[i + s];
			tempSumb += ((2 * N + 1) - 3 * s)*phaseData[i + s];
		}
		a_r = tempSuma * 6 / (N*(N*N - 1));
		b_r = tempSumb * 2 / (N*(N - 1));
		if (a_r > 0.008646448 && a_r < 0.04740639 || a_r > -0.0434310 && a_r < -0.01262182) //右斜率满足
		{
			//计算左斜率
			tempSuma = 0; tempSumb = 0;
			for (int s = -(N - 1); s <= 0; s++)
			{
				tempSuma += (2 * (s + N) - (N + 1))*phaseData[i + s];
				tempSumb += ((2 * N + 1) - 3 * (1 - s))*phaseData[i + s];
			}
			a_l = tempSuma * 6 / (N*(N*N - 1));
			b_l = tempSumb * 2 / (N*(N - 1));
			if ((a_r > 0.008646448 && a_r < 0.04740639 || a_r > -0.0434310 && a_r < -0.01262182) && a_r*a_l < 0)//左斜率满足
			{
				//计算横轴截距
				x0 = (b_l - b_r) / (a_r - a_l);
				if (x0 > -1 && x0 < 1) //截距满足
				{
					if (pre_position == 0) //第一个拐点，还无法进行后续判断
					{
						pre_position = i;
						continue;
					}
					if (i - pre_position >= POINTS1BIT_1 - deviationPointOf8bit_1)
					{
						int deviationPoint = (i - pre_position) % POINTS1BIT_1;
						if (deviationPoint >= POINTS1BIT_1 - deviationPointOf8bit_1)
							deviationPoint = POINTS1BIT_1 - deviationPoint;
						if (deviationPoint <= deviationPointOf8bit_1) //点数满足要求
						{
							//开始精确计算拐点位置
							m_TermNum = 80; // 计算点数
							start_idx = i;
							//DataSegment = LengthOfSameBits * 44 / 7;
							//不修正本长度内的相位偏差（2PI相位模糊度和干扰造成的偏差）
							//先求出直线拟合的系数
							sum_x = 3240;//m_TermNum*(m_TermNum+1)/2;
							sum_xx = 173880;//m_TermNum*(m_TermNum+1)*(2*m_TermNum+1)/6;

							//在两个极值点之间做for循环;折线平移到原点来计算折线方程;
							sum_y_Left = 0;
							sum_xy_Left = 0;
							sum_y_Right = 0;
							sum_xy_Right = 0;
							for (int k = 1; k <= m_TermNum; k++)
							{
								sum_y_Right += phaseData[start_idx + k];
								sum_xy_Right += k * phaseData[start_idx + k];
								sum_y_Left += phaseData[start_idx - k];
								sum_xy_Left += k * phaseData[start_idx - k];
							}
							double denominator = sum_x * sum_x - m_TermNum * sum_xx;
							a_r = (sum_x * sum_y_Right - m_TermNum * sum_xy_Right) / denominator;  //斜率
							b_r = (sum_xy_Right * sum_x - sum_y_Right * sum_xx) / denominator;//截距
							a_l = (-sum_x * sum_y_Left + m_TermNum * sum_xy_Left) / denominator;  //斜率
							b_l = (sum_xy_Left * sum_x - sum_y_Left * sum_xx) / denominator;//截距
							//求与前一条折线交点“坐标”并且计算相邻两个交点之间有多少个比特Bit_Num  
							if ((a_r > 0.008646448 && a_r<0.04740639 && a_l>-0.0434310  && a_l < -0.01262182)   //折线拐点
								|| (a_l > 0.008646448  && a_l<0.04740639  && a_r>-0.0434310 && a_r < -0.01262182))
							{
								idx_intersect_point = i + (b_r - b_l) / (a_l - a_r);
								if (start_true_point == 0) //第一个精确计算的拐点
								{
									pre_position = i;
									start_true_point = idx_intersect_point;
									continue;
								}
								temp = (idx_intersect_point - start_true_point) / bpsTrue;// 
								if (temp - (int)temp > 0.8)
								{
									lengthOfSameBits = (int)temp + 1;
								}
								else
								{
									lengthOfSameBits = (int)temp;
								}
								BPS = (idx_intersect_point - start_true_point) / lengthOfSameBits; //准确的长度,平均到每个bit
								int j;
								for (j = cnt_Bit_Dem; j < cnt_Bit_Dem + lengthOfSameBits; j++)
								{
									bps_err_Bit_1.push_back(BPS); //计算每个比特的实际采样点数
									sum_BPS += BPS;//统计BPS
									if (63221 / BPS < balise.minTransSpeed)
									{
										balise.minTransSpeed = 63221 / BPS;
									}
									if (63221 / BPS > balise.maxTransSpeed)
									{
										balise.maxTransSpeed = 63221 / BPS;
									}
								}
								cnt_Bit_Dem = j;
								if (cnt_Bit_Dem > 500)//窗口长度，只计算500个点长度
								{
									break;
								}
								start_true_point = idx_intersect_point;
							}
						}
					}
					pre_position = i;
				}
				
			}
		}
	}
	//计算实际波特率
	factBps = sum_BPS / (cnt_Bit_Dem - 1);
    //计算实际数据速率
	balise.averTranSpeed = 63221 / factBps;

    //开始计算MTIE

	//统计每个bit对应的波特率偏差
	bps_err_Bit_1.resize(500);
	bps_err_Bit_2.resize(500);

	for (int j = 0; j<500; j++)
	{
		bps_err_Bit_1[j] = bps_err_Bit_1[j] - factBps;
		bps_err_Bit_2[j] = bps_err_Bit_1[j] - bpsTrue;
	}
	balise.MTIEVec_1.resize(500);
	balise.MTIEVec_2.resize(500);
	calMTIE(&bps_err_Bit_1[0], balise.MTIEVec_1);
	calMTIE(&bps_err_Bit_2[0], balise.MTIEVec_2);
}

int BaliseMsgAnalyse::isValidSynchCode(const USHORT synchCode)
{
	// 同步位移表格，具体含义如下：
	// 对于角标为i的元素，如果SYNCH_TABLE[i]为-1表明i不是有效的同步结果，否则SYNCH_TABLE[i]
	// 即为报文移位的长度，即S=SYNCH_TABLE[i]
	static bool		isInitSynchTable = false;					// 是否已初始化同步表格判断变量
	static int		SYNCH_TABLE[FLX] = { 0 };

	// 初始化同步位移表格
	if (false == isInitSynchTable)
	{
		USHORT		shiftRegState = 0;				// 移位寄存器中存储的数值
		UCHAR		shiftRegOutput = 0;				// 移位寄存器移出的数值

		isInitSynchTable = true;
		memset(SYNCH_TABLE, 0xFF, sizeof SYNCH_TABLE);

		for (size_t i = 0; i<sizeof(GLX) / sizeof(GLX[0]); i++)
		{
			shiftRegState = (shiftRegState << 1) | GLX[i];
			shiftRegState ^= getBitValue(shiftRegState, 11) ? FLX : 0;
		}

		SYNCH_TABLE[shiftRegState & 0x7FF] = 0;

		for (size_t i = 1; i<1023; i++)
		{
			shiftRegState <<= 1;
			shiftRegState ^= getBitValue(shiftRegState, 11) ? FLX : 0;
			SYNCH_TABLE[shiftRegState & 0x7FF] = i;
		}
	}

	return SYNCH_TABLE[synchCode];
}


bool BaliseMsgAnalyse::decode(const UCHAR* srcPtr, UCHAR* dstPtr, const bool isLong)
{
	//assert(NULL != srcPtr || NULL != dstPtr);

	// 判断报文是否符奇偶校验
	if (false == isCodedMSGCorrect((BYTE*)srcPtr, isLong))
	{
		return false;
	}

	// 将编码后的报文按照每个元素11比特重新组合
	USHORT			msgIn11Bit[LONG_CODE_MSG_LEN_IN_BIT / CODE_MSG_WORD_LEN_IN_BIT] = { 0 };		// 以11比特为一组存储的报文	
	ULONG			dataBuffer = 0;			// 数据存储缓冲区
	size_t			dataLeft = 0;			// 数据缓冲区中剩余的比特数
	const size_t	MSG_WORD_NUM(true == isLong ? LONG_CODE_MSG_LEN_IN_BIT / CODE_MSG_WORD_LEN_IN_BIT
		: SHORT_CODE_MSG_LEN_IN_BIT / CODE_MSG_WORD_LEN_IN_BIT);

	for (size_t i = 0, j = 0; i<MSG_WORD_NUM; ++i)
	{
		while (dataLeft < CODE_MSG_WORD_LEN_IN_BIT)
		{
			dataBuffer = (dataBuffer << 8) | srcPtr[j++];
			dataLeft += 8;
		}

		msgIn11Bit[i] = getHigh11Bit(dataBuffer, dataLeft);
		dataLeft -= CODE_MSG_WORD_LEN_IN_BIT;
	}

	if (false == checkValidWord(msgIn11Bit, MSG_WORD_NUM))
	{
		return false;
	}


	// 判断校验比特是否正确
	ULONG				checkBits[3] = { 0 };
	const ULONG 		GLX[3] = { 0xB88, 0x739A7A2E, 0xD523BA13 };
	const ULONG 		GSX[3] = { 0x9F7, 0x90C2FEF7, 0xCA4A3C4B };

	getCheckBits(msgIn11Bit, true == isLong ? LONG_CODE_MSG_LEN_IN_BIT : SHORT_CODE_MSG_LEN_IN_BIT,
		checkBits);
	xorData(checkBits, true == isLong ? GLX : GSX);

	if (checkBits[0] || checkBits[1] || checkBits[2])
	{
		return false;
	}

	// 判断cb是否正确，如果反转比特为1则要反转报文内容
	if (msgIn11Bit[true == isLong ? 83 : 21] >> 8 != 1 && msgIn11Bit[true == isLong ? 83 : 21] >> 8 != 6)
	{
		return false;
	}

	if (msgIn11Bit[true == isLong ? 83 : 21] >> 10)
	{
		for (size_t i = 0; i<MSG_WORD_NUM; ++i)
		{
			msgIn11Bit[i] ^= 0x7FF;
		}
	}// 反转比特为1，反转报文内容
	

	// 将11比特转为10比特用户数据
	USHORT		msgIn10Bit[LONG_MSG_LEN_IN_BIT / DECODE_MSG_WORD_LEN_IN_BIT] = { 0 };		// 以11比特为一组存储的报文	

	// 对报文进行解扰
																							
	USHORT		scrambleCode(((msgIn11Bit[true == isLong ? 83 : 21] << 4) & 0xFFF)
		| ((msgIn11Bit[true == isLong ? 84 : 22] >> 7) & 0xF));	// 报文选用的扰码
	descramble10BitData(scrambleCode, msgIn11Bit, msgIn10Bit, isLong);
	
	USHORT			sum = 0;
	const size_t	USER_WORD_NUM((isLong == true ? LONG_MSG_LEN_IN_BIT : SHORT_MSG_LEN_IN_BIT) / DECODE_MSG_WORD_LEN_IN_BIT);
	for (size_t i = 1; i<USER_WORD_NUM; ++i)
	{
		sum += msgIn10Bit[i];
	}

	msgIn10Bit[0] = getLow10Bit((msgIn10Bit[0] | (1 << 10)) - getLow10Bit(sum));



	// 报文编码成功，将10比特一组的报文数据转换为8比特一组的报文数据
	for (size_t i = 0; i<(true == isLong ? LONG_MSG_LEN_IN_BIT : SHORT_MSG_LEN_IN_BIT); ++i)
	{
		dstPtr[i / UCHAR_LEN_IN_BIT] <<= 1;
		dstPtr[i / UCHAR_LEN_IN_BIT] |= msgIn10Bit[i / DECODE_MSG_WORD_LEN_IN_BIT] >>
			(DECODE_MSG_WORD_LEN_IN_BIT - (i%DECODE_MSG_WORD_LEN_IN_BIT) - 1);
	}
	dstPtr[(true == isLong ? LONG_MSG_LEN_IN_BYTE : SHORT_MSG_LEN_IN_BYTE) - 1] <<=
		true == isLong ? 2 : 6;

	return true;
}

bool BaliseMsgAnalyse::checkValidWord(const USHORT* msgPtr, const size_t number)
{
	//assert(NULL != msgPtr);

	for (size_t i = 0; i<number; ++i)
	{
		if (CONVER_TABLE_OF_11BIT_TO_10BIT[*msgPtr++] < 0)
		{
			return false;
		}
	}

	return true;
}

void BaliseMsgAnalyse::getCheckBits(const USHORT* srcPtr, size_t bitNum, ULONG* checkBitPtr)
{
	//assert(NULL != srcPtr && checkBitPtr != NULL);

	const size_t	FX_AND_GX_LEN_IN_BIT(85);										// F(X)*G(X)多项式的位数
	const ULONG		FLX_AND_GLX[] = { 0x3EC171, 0x890C6F72, 0xC063B091 };		// FL(X)*GL(X)多项式
	const ULONG		FSX_AND_GSX[] = { 0x2BB94D, 0x87757959, 0x021B6D65 };		// FS(X)*GS(X)多项式
	const size_t	BIT_NUM_PER_ITEM(CODE_MSG_WORD_LEN_IN_BIT);						// 每个元素的有效位

	for (size_t i = 0; i<bitNum; /*NULL*/)
		for (int j = BIT_NUM_PER_ITEM - 1; j >= 0 && i<bitNum; --j, ++i)
		{
			//1==================================================================================================
			// 如果寄存器移出的数据为1则进行反馈运算
			if (leftShiftData(checkBitPtr, FX_AND_GX_LEN_IN_BIT, (srcPtr[i / BIT_NUM_PER_ITEM] >> j) & 1))
			{
				xorData(checkBitPtr, bitNum == LONG_CODE_MSG_LEN_IN_BIT ? FLX_AND_GLX : FSX_AND_GSX);
			}
			//1==================================================================================================
		}

	checkBitPtr[0] &= 0x1FFFFF;
}

void BaliseMsgAnalyse::descramble10BitData(const USHORT scrambleCode, const USHORT* srcPtr, USHORT* dstPtr, const bool isLong)
{
	//assert(NULL != srcPtr && NULL != dstPtr);

	const size_t	BIT_NUM_PER_ITEM(DECODE_MSG_WORD_LEN_IN_BIT);		// 每个元素的有效位
	const size_t	ITEM_NUM(isLong == true ? LONG_MSG_LEN_IN_BIT / BIT_NUM_PER_ITEM
		: SHORT_MSG_LEN_IN_BIT / BIT_NUM_PER_ITEM);						// 元素的个数
	const ULONG		SHIFT_REG_WEIGHT(0XEA000001);						// 移位寄存器的权重
	const ULONG		RANDOM_GENERATOR_NUM(2801775573);					// 随机数生成常数
	ULONG			shiftRegState((RANDOM_GENERATOR_NUM * scrambleCode) & 0xFFFFFFFF);	// 移位寄存器的状态

	for (size_t i = 0; i<ITEM_NUM; dstPtr[i] = getLow10Bit(dstPtr[i]), ++i, ++srcPtr)
		for (int j = BIT_NUM_PER_ITEM - 1; j >= 0; --j)
		{
			UCHAR		curInputBit((CONVER_TABLE_OF_11BIT_TO_10BIT[*srcPtr] >> j) & 1);	// 当前输入寄存器的比特
			dstPtr[i] = (dstPtr[i] << 1) | (curInputBit ^ (shiftRegState >> 31));

			// 将移位寄存器进行移位，如果移位寄存器移出的数值为1则进行反馈运算，否则保持不变
			shiftRegState = (shiftRegState << 1) ^ (curInputBit ? SHIFT_REG_WEIGHT : 0);
		}
}

QString BaliseMsgAnalyse::getBaliseID(const BYTE* msgBuffer)
{
	//assert(NULL != msgBuffer);

	ULONG		buffer[2] = { 0 };
	for (size_t i = 0; i<8; ++i)
	{
		buffer[i / 4] <<= 8;
		buffer[i / 4] |= msgBuffer[i];
	}

	BYTE			N_PIG((buffer[0] >> 20) & 0x7);
	BYTE			N_TOTAL((buffer[0] >> 17) & 0x7);
	BYTE			NID_C1(buffer[0] & 0x7F);
	BYTE			NID_C2((buffer[1] >> 29) & 0x7);
	BYTE			NID_BG1((buffer[1] >> 23) & 0x3F);
	BYTE			NID_BG2((buffer[1] >> 15) & 0xFF);
	QString			ID;
	
	ID = N_TOTAL > 0 ? QString("%1-%2-%3-%4-%5")
		.arg(QString::number(NID_C1))
		.arg(QString::number(NID_C2))
		.arg(QString::number(NID_BG1))
		.arg(QString::number(NID_BG2))
		.arg(QString::number(N_PIG + 1)) :
		QString("%1-%2-%3-%4")
		.arg(QString::number(NID_C1))
		.arg(QString::number(NID_C2))
		.arg(QString::number(NID_BG1))
		.arg(QString::number(NID_BG2));

	return ID;
}

/*******************************************************************************************************
* 函 数 名：calMTIE(const vector<int> &posVec, const vector<int> &bitNumVec, vector<double> &tieVec, vector<double> &MTIEVec)
* 函数功能：计算MTIE
* 入口参数：posVec, bitNumVec, tieVec, MTIEVec
* 返    回：无
********************************************************************************************************/
void BaliseMsgAnalyse::calMTIE(const double realDeviationOf1bitVec[], QVector<double> &MTIEVec) 
{
	MTIEVec[0] = 0;
	QVector<double> TIEPointsVec;
	for (int windowSize = 2; windowSize < 500; windowSize++)
	{
		for (int p = 0; p < 500 - windowSize; )
		{
			double max, min;
			int maxPos, minPos;
			findMax(&realDeviationOf1bitVec[p], windowSize, max, maxPos);
			findMin(&realDeviationOf1bitVec[p], windowSize, min, minPos);
			int frontPos = max < min ? maxPos : minPos;
			TIEPointsVec.push_back(max - min);
			if (frontPos != 0) //第一个极值点在区间内
			{
				p += frontPos;
			}
			else //第一个极值点在区间头，区间后移一个
			{
				p += 1;
			}
		}
		int MTIEPointsPos;
		double MTIEPoints;
		findMax(&TIEPointsVec[0], TIEPointsVec.size(), MTIEPoints, MTIEPointsPos);
		double MTIE = MTIEPoints * (1 / (sampleRate * 1e6));
		MTIEVec[windowSize] = MTIE;
		TIEPointsVec.clear();
	}
}

void BaliseMsgAnalyse::findMax(const double * vector, const int vecSize, double & max, int & maxPos)
{
	max = vector[0];
	for (int i = 0; i < vecSize; i++)
	{
		if (vector[i] > max)
		{
			max = vector[i];
			maxPos = i;
		}
	}
}

void BaliseMsgAnalyse::findMin(const double * vector, const int vecSize, double & min, int & minPos)
{
	min = vector[0];
	for (int i = 0; i < vecSize; i++)
	{
		if (vector[i] < min)
		{
			min = vector[i];
			minPos = i;
		}
	}
}

