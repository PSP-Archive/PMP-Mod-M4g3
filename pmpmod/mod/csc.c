/*
PMP Mod
Copyright (C) 2006 jonny

Homepage: http://jonny.leffe.dnsalias.com
E-mail:   jonny@leffe.dnsalias.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
color space conversion, asm and c version
*/


#include "csc.h"



unsigned char color_space_conversion_fix[812] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161,
 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187,
 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213,
 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
 255, 255, 255, 255, 255, 255, 255, 255, 255};

int           color_space_conversion_my [256] = {0, 9539, 19078, 28617, 38156, 47695, 57234, 66773, 76312, 85851, 95390, 104929,
 114468, 124007, 133546, 143085, 152624, 162163, 171702, 181241, 190780, 200319, 209858, 219397, 228936, 238475, 248014, 257553,
 267092, 276631, 286170, 295709, 305248, 314787, 324326, 333865, 343404, 352943, 362482, 372021, 381560, 391099, 400638, 410177,
 419716, 429255, 438794, 448333, 457872, 467411, 476950, 486489, 496028, 505567, 515106, 524645, 534184, 543723, 553262, 562801,
 572340, 581879, 591418, 600957, 610496, 620035, 629574, 639113, 648652, 658191, 667730, 677269, 686808, 696347, 705886, 715425,
 724964, 734503, 744042, 753581, 763120, 772659, 782198, 791737, 801276, 810815, 820354, 829893, 839432, 848971, 858510, 868049,
 877588, 887127, 896666, 906205, 915744, 925283, 934822, 944361, 953900, 963439, 972978, 982517, 992056, 1001595, 1011134, 1020673,
 1030212, 1039751, 1049290, 1058829, 1068368, 1077907, 1087446, 1096985, 1106524, 1116063, 1125602, 1135141, 1144680, 1154219,
 1163758, 1173297, 1182836, 1192375, 1201914, 1211453, 1220992, 1230531, 1240070, 1249609, 1259148, 1268687, 1278226, 1287765,
 1297304, 1306843, 1316382, 1325921, 1335460, 1344999, 1354538, 1364077, 1373616, 1383155, 1392694, 1402233, 1411772, 1421311,
 1430850, 1440389, 1449928, 1459467, 1469006, 1478545, 1488084, 1497623, 1507162, 1516701, 1526240, 1535779, 1545318, 1554857,
 1564396, 1573935, 1583474, 1593013, 1602552, 1612091, 1621630, 1631169, 1640708, 1650247, 1659786, 1669325, 1678864, 1688403,
 1697942, 1707481, 1717020, 1726559, 1736098, 1745637, 1755176, 1764715, 1774254, 1783793, 1793332, 1802871, 1812410, 1821949,
 1831488, 1841027, 1850566, 1860105, 1869644, 1879183, 1888722, 1898261, 1907800, 1917339, 1926878, 1936417, 1945956, 1955495,
 1965034, 1974573, 1984112, 1993651, 2003190, 2012729, 2022268, 2031807, 2041346, 2050885, 2060424, 2069963, 2079502, 2089041,
 2098580, 2108119, 2117658, 2127197, 2136736, 2146275, 2155814, 2165353, 2174892, 2184431, 2193970, 2203509, 2213048, 2222587,
 2232126, 2241665, 2251204, 2260743, 2270282, 2279821, 2289360, 2298899, 2308438, 2317977, 2327516, 2337055, 2346594, 2356133,
 2365672, 2375211, 2384750, 2394289, 2403828, 2413367, 2422906, 2432445};

int           color_space_conversion_mu1[256] = {5441, 21966, 38491, 55016, 71541, 88066, 104591, 121116, 137641, 154166, 170691,
 187216, 203741, 220266, 236791, 253316, 269841, 286366, 302891, 319416, 335941, 352466, 368991, 385516, 402041, 418566, 435091,
 451616, 468141, 484666, 501191, 517716, 534241, 550766, 567291, 583816, 600341, 616866, 633391, 649916, 666441, 682966, 699491,
 716016, 732541, 749066, 765591, 782116, 798641, 815166, 831691, 848216, 864741, 881266, 897791, 914316, 930841, 947366, 963891,
 980416, 996941, 1013466, 1029991, 1046516, 1063041, 1079566, 1096091, 1112616, 1129141, 1145666, 1162191, 1178716, 1195241,
 1211766, 1228291, 1244816, 1261341, 1277866, 1294391, 1310916, 1327441, 1343966, 1360491, 1377016, 1393541, 1410066, 1426591,
 1443116, 1459641, 1476166, 1492691, 1509216, 1525741, 1542266, 1558791, 1575316, 1591841, 1608366, 1624891, 1641416, 1657941,
 1674466, 1690991, 1707516, 1724041, 1740566, 1757091, 1773616, 1790141, 1806666, 1823191, 1839716, 1856241, 1872766, 1889291,
 1905816, 1922341, 1938866, 1955391, 1971916, 1988441, 2004966, 2021491, 2038016, 2054541, 2071066, 2087591, 2104116, 2120641,
 2137166, 2153691, 2170216, 2186741, 2203266, 2219791, 2236316, 2252841, 2269366, 2285891, 2302416, 2318941, 2335466, 2351991,
 2368516, 2385041, 2401566, 2418091, 2434616, 2451141, 2467666, 2484191, 2500716, 2517241, 2533766, 2550291, 2566816, 2583341,
 2599866, 2616391, 2632916, 2649441, 2665966, 2682491, 2699016, 2715541, 2732066, 2748591, 2765116, 2781641, 2798166, 2814691,
 2831216, 2847741, 2864266, 2880791, 2897316, 2913841, 2930366, 2946891, 2963416, 2979941, 2996466, 3012991, 3029516, 3046041,
 3062566, 3079091, 3095616, 3112141, 3128666, 3145191, 3161716, 3178241, 3194766, 3211291, 3227816, 3244341, 3260866, 3277391,
 3293916, 3310441, 3326966, 3343491, 3360016, 3376541, 3393066, 3409591, 3426116, 3442641, 3459166, 3475691, 3492216, 3508741,
 3525266, 3541791, 3558316, 3574841, 3591366, 3607891, 3624416, 3640941, 3657466, 3673991, 3690516, 3707041, 3723566, 3740091,
 3756616, 3773141, 3789666, 3806191, 3822716, 3839241, 3855766, 3872291, 3888816, 3905341, 3921866, 3938391, 3954916, 3971441,
 3987966, 4004491, 4021016, 4037541, 4054066, 4070591, 4087116, 4103641, 4120166, 4136691, 4153216, 4169741, 4186266, 4202791,
 4219316};

int           color_space_conversion_mv1[256] = {447107, 460182, 473257, 486332, 499407, 512482, 525557, 538632, 551707, 564782,
 577857, 590932, 604007, 617082, 630157, 643232, 656307, 669382, 682457, 695532, 708607, 721682, 734757, 747832, 760907, 773982,
 787057, 800132, 813207, 826282, 839357, 852432, 865507, 878582, 891657, 904732, 917807, 930882, 943957, 957032, 970107, 983182,
 996257, 1009332, 1022407, 1035482, 1048557, 1061632, 1074707, 1087782, 1100857, 1113932, 1127007, 1140082, 1153157, 1166232,
 1179307, 1192382, 1205457, 1218532, 1231607, 1244682, 1257757, 1270832, 1283907, 1296982, 1310057, 1323132, 1336207, 1349282,
 1362357, 1375432, 1388507, 1401582, 1414657, 1427732, 1440807, 1453882, 1466957, 1480032, 1493107, 1506182, 1519257, 1532332,
 1545407, 1558482, 1571557, 1584632, 1597707, 1610782, 1623857, 1636932, 1650007, 1663082, 1676157, 1689232, 1702307, 1715382,
 1728457, 1741532, 1754607, 1767682, 1780757, 1793832, 1806907, 1819982, 1833057, 1846132, 1859207, 1872282, 1885357, 1898432,
 1911507, 1924582, 1937657, 1950732, 1963807, 1976882, 1989957, 2003032, 2016107, 2029182, 2042257, 2055332, 2068407, 2081482,
 2094557, 2107632, 2120707, 2133782, 2146857, 2159932, 2173007, 2186082, 2199157, 2212232, 2225307, 2238382, 2251457, 2264532,
 2277607, 2290682, 2303757, 2316832, 2329907, 2342982, 2356057, 2369132, 2382207, 2395282, 2408357, 2421432, 2434507, 2447582,
 2460657, 2473732, 2486807, 2499882, 2512957, 2526032, 2539107, 2552182, 2565257, 2578332, 2591407, 2604482, 2617557, 2630632,
 2643707, 2656782, 2669857, 2682932, 2696007, 2709082, 2722157, 2735232, 2748307, 2761382, 2774457, 2787532, 2800607, 2813682,
 2826757, 2839832, 2852907, 2865982, 2879057, 2892132, 2905207, 2918282, 2931357, 2944432, 2957507, 2970582, 2983657, 2996732,
 3009807, 3022882, 3035957, 3049032, 3062107, 3075182, 3088257, 3101332, 3114407, 3127482, 3140557, 3153632, 3166707, 3179782,
 3192857, 3205932, 3219007, 3232082, 3245157, 3258232, 3271307, 3284382, 3297457, 3310532, 3323607, 3336682, 3349757, 3362832,
 3375907, 3388982, 3402057, 3415132, 3428207, 3441282, 3454357, 3467432, 3480507, 3493582, 3506657, 3519732, 3532807, 3545882,
 3558957, 3572032, 3585107, 3598182, 3611257, 3624332, 3637407, 3650482, 3663557, 3676632, 3689707, 3702782, 3715857, 3728932,
 3742007, 3755082, 3768157, 3781232};

int           color_space_conversion_mu2[256] = {0, -3209, -6418, -9627, -12836, -16045, -19254, -22463, -25672, -28881, -32090,
 -35299, -38508, -41717, -44926, -48135, -51344, -54553, -57762, -60971, -64180, -67389, -70598, -73807, -77016, -80225, -83434,
 -86643, -89852, -93061, -96270, -99479, -102688, -105897, -109106, -112315, -115524, -118733, -121942, -125151, -128360, -131569,
 -134778, -137987, -141196, -144405, -147614, -150823, -154032, -157241, -160450, -163659, -166868, -170077, -173286, -176495,
 -179704, -182913, -186122, -189331, -192540, -195749, -198958, -202167, -205376, -208585, -211794, -215003, -218212, -221421,
 -224630, -227839, -231048, -234257, -237466, -240675, -243884, -247093, -250302, -253511, -256720, -259929, -263138, -266347,
 -269556, -272765, -275974, -279183, -282392, -285601, -288810, -292019, -295228, -298437, -301646, -304855, -308064, -311273,
 -314482, -317691, -320900, -324109, -327318, -330527, -333736, -336945, -340154, -343363, -346572, -349781, -352990, -356199,
 -359408, -362617, -365826, -369035, -372244, -375453, -378662, -381871, -385080, -388289, -391498, -394707, -397916, -401125,
 -404334, -407543, -410752, -413961, -417170, -420379, -423588, -426797, -430006, -433215, -436424, -439633, -442842, -446051,
 -449260, -452469, -455678, -458887, -462096, -465305, -468514, -471723, -474932, -478141, -481350, -484559, -487768, -490977,
 -494186, -497395, -500604, -503813, -507022, -510231, -513440, -516649, -519858, -523067, -526276, -529485, -532694, -535903,
 -539112, -542321, -545530, -548739, -551948, -555157, -558366, -561575, -564784, -567993, -571202, -574411, -577620, -580829,
 -584038, -587247, -590456, -593665, -596874, -600083, -603292, -606501, -609710, -612919, -616128, -619337, -622546, -625755,
 -628964, -632173, -635382, -638591, -641800, -645009, -648218, -651427, -654636, -657845, -661054, -664263, -667472, -670681,
 -673890, -677099, -680308, -683517, -686726, -689935, -693144, -696353, -699562, -702771, -705980, -709189, -712398, -715607,
 -718816, -722025, -725234, -728443, -731652, -734861, -738070, -741279, -744488, -747697, -750906, -754115, -757324, -760533,
 -763742, -766951, -770160, -773369, -776578, -779787, -782996, -786205, -789414, -792623, -795832, -799041, -802250, -805459,
 -808668, -811877, -815086, -818295};

int           color_space_conversion_mv2[256] = {3383922, 3377262, 3370602, 3363942, 3357282, 3350622, 3343962, 3337302, 3330642,
 3323982, 3317322, 3310662, 3304002, 3297342, 3290682, 3284022, 3277362, 3270702, 3264042, 3257382, 3250722, 3244062, 3237402,
 3230742, 3224082, 3217422, 3210762, 3204102, 3197442, 3190782, 3184122, 3177462, 3170802, 3164142, 3157482, 3150822, 3144162,
 3137502, 3130842, 3124182, 3117522, 3110862, 3104202, 3097542, 3090882, 3084222, 3077562, 3070902, 3064242, 3057582, 3050922,
 3044262, 3037602, 3030942, 3024282, 3017622, 3010962, 3004302, 2997642, 2990982, 2984322, 2977662, 2971002, 2964342, 2957682,
 2951022, 2944362, 2937702, 2931042, 2924382, 2917722, 2911062, 2904402, 2897742, 2891082, 2884422, 2877762, 2871102, 2864442,
 2857782, 2851122, 2844462, 2837802, 2831142, 2824482, 2817822, 2811162, 2804502, 2797842, 2791182, 2784522, 2777862, 2771202,
 2764542, 2757882, 2751222, 2744562, 2737902, 2731242, 2724582, 2717922, 2711262, 2704602, 2697942, 2691282, 2684622, 2677962,
 2671302, 2664642, 2657982, 2651322, 2644662, 2638002, 2631342, 2624682, 2618022, 2611362, 2604702, 2598042, 2591382, 2584722,
 2578062, 2571402, 2564742, 2558082, 2551422, 2544762, 2538102, 2531442, 2524782, 2518122, 2511462, 2504802, 2498142, 2491482,
 2484822, 2478162, 2471502, 2464842, 2458182, 2451522, 2444862, 2438202, 2431542, 2424882, 2418222, 2411562, 2404902, 2398242,
 2391582, 2384922, 2378262, 2371602, 2364942, 2358282, 2351622, 2344962, 2338302, 2331642, 2324982, 2318322, 2311662, 2305002,
 2298342, 2291682, 2285022, 2278362, 2271702, 2265042, 2258382, 2251722, 2245062, 2238402, 2231742, 2225082, 2218422, 2211762,
 2205102, 2198442, 2191782, 2185122, 2178462, 2171802, 2165142, 2158482, 2151822, 2145162, 2138502, 2131842, 2125182, 2118522,
 2111862, 2105202, 2098542, 2091882, 2085222, 2078562, 2071902, 2065242, 2058582, 2051922, 2045262, 2038602, 2031942, 2025282,
 2018622, 2011962, 2005302, 1998642, 1991982, 1985322, 1978662, 1972002, 1965342, 1958682, 1952022, 1945362, 1938702, 1932042,
 1925382, 1918722, 1912062, 1905402, 1898742, 1892082, 1885422, 1878762, 1872102, 1865442, 1858782, 1852122, 1845462, 1838802,
 1832142, 1825482, 1818822, 1812162, 1805502, 1798842, 1792182, 1785522, 1778862, 1772202, 1765542, 1758882, 1752222, 1745562,
 1738902, 1732242, 1725582, 1718922, 1712262, 1705602, 1698942, 1692282, 1685622};





#define make_uv() \
	"lbu $13, 0($15)\n"   /* $13 <- u     */ \
	"lbu $14, 0($16)\n"   /* $14 <- v     */ \
	"addiu $15, $15, 1\n" /* cu = cu + 1  */ \
	"addiu $16, $16, 1\n" /* cv = cv + 1  */ \
	"sll $13, $13, 2\n"   /* $13 <- 4 * u */ \
	"sll $14, $14, 2\n"   /* $14 <- 4 * v */ \
	"lw $21, color_space_conversion_mu2($13)\n" /* $21 <- MU2  */ \
	"lw $22, color_space_conversion_mv2($14)\n" /* $22 <- MV2  */ \
	"lw $19, color_space_conversion_mu1($13)\n" /* $19 <- MU1  */ \
	"lw $20, color_space_conversion_mv1($14)\n" /* $20 <- MV1  */ \
	"add $21, $21, $22\n"                       /* $21 <- ADDG */





#define make_y_high(y, out) \
	"lbu $7, "#y"+0($2)\n"                       /*  $7 <- y     */ \
	"lbu $25, "#y"+1($2)\n"                      /* $25 <- y     */ \
	"sll $7, $7, 2\n"                            /*  $7 <- 4 * y */ \
	"lw $7, color_space_conversion_my($7)\n"     /*  $7 <- Y     */ \
	"add $7, $7, $26\n"						     /* $7 <- Y + (y_offset<<13)*/ \
	"sll $25, $25, 2\n"                          /* $25 <- 4 * y */ \
	"lw $25, color_space_conversion_my($25)\n"   /* $25 <- Y     */ \
	"add $25, $25, $26\n"						 /* $25 <- Y + (y_offset<<13)*/ \
	"add $23, $7, $20\n"                         /* R            */ \
	"srl $23, $23, 13\n"                         /* R >> 13      */ \
	"lbu $23, color_space_conversion_fix($23)\n" /* R            */ \
	"sb $23, "#out"+0($3)\n"                     /* out <- R     */ \
	"add $24, $7, $21\n"                         /* G            */ \
	"srl $24, $24, 13\n"                         /* G >> 13      */ \
	"lbu $24, color_space_conversion_fix($24)\n" /* G            */ \
	"sb $24, "#out"+1($3)\n"                     /* out <- G     */ \
	"add $22, $7, $19\n"                         /* B            */ \
	"srl $22, $22, 13\n"                         /* B >> 13      */ \
	"lbu $22, color_space_conversion_fix($22)\n" /* B            */ \
	"sb $22, "#out"+2($3)\n"                     /* out <- B     */ \
	"add $23, $25, $20\n"                        /* R            */ \
	"srl $23, $23, 13\n"                         /* R >> 13      */ \
	"lbu $23, color_space_conversion_fix($23)\n" /* R            */ \
	"sb $23, "#out"+4($3)\n"                     /* out <- R     */ \
	"add $24, $25, $21\n"                        /* G            */ \
	"srl $24, $24, 13\n"                         /* G >> 13      */ \
	"lbu $24, color_space_conversion_fix($24)\n" /* G            */ \
	"sb $24, "#out"+5($3)\n"                     /* out <- G     */ \
	"add $22, $25, $19\n"                        /* B            */ \
	"srl $22, $22, 13\n"                         /* B >> 13      */ \
	"lbu $22, color_space_conversion_fix($22)\n" /* B            */ \
	"sb $22, "#out"+6($3)\n"                     /* out <- B     */




#define make_y_low(y, out) \
	"addu $13, $2, $12\n"                        /* $13 <- new cy  */ \
	"addu $14, $3, $9\n"                         /* $14 <- new out */ \
	"lbu $7, "#y"+0($13)\n"                      /*  $7 <- y       */ \
	"lbu $25, "#y"+1($13)\n"                     /* $25 <- y       */ \
	"sll $7, $7, 2\n"                            /*  $7 <- 4 * y   */ \
	"lw $7, color_space_conversion_my($7)\n"     /*  $7 <- Y       */ \
	"add $7, $7, $26\n"							 /* $7 <- Y + (y_offset<<13)*/ \
	"sll $25, $25, 2\n"                          /* $25 <- 4 * y   */ \
	"lw $25, color_space_conversion_my($25)\n"   /* $25 <- Y       */ \
	"add $25, $25, $26\n"						 /* $25 <- Y + (y_offset<<13)*/ \
	"add $23, $7, $20\n"                         /* R              */ \
	"srl $23, $23, 13\n"                         /* R >> 13        */ \
	"lbu $23, color_space_conversion_fix($23)\n" /* R              */ \
	"sb $23, "#out"+0($14)\n"                    /* out <- R       */ \
	"add $24, $7, $21\n"                         /* G              */ \
	"srl $24, $24, 13\n"                         /* G >> 13        */ \
	"lbu $24, color_space_conversion_fix($24)\n" /* G              */ \
	"sb $24, "#out"+1($14)\n"                    /* out <- G       */ \
	"add $22, $7, $19\n"                         /* B              */ \
	"srl $22, $22, 13\n"                         /* B >> 13        */ \
	"lbu $22, color_space_conversion_fix($22)\n" /* B              */ \
	"sb $22, "#out"+2($14)\n"                    /* out <- B       */ \
	"add $23, $25, $20\n"                        /* R              */ \
	"srl $23, $23, 13\n"                         /* R >> 13        */ \
	"lbu $23, color_space_conversion_fix($23)\n" /* R              */ \
	"sb $23, "#out"+4($14)\n"                    /* out <- R       */ \
	"add $24, $25, $21\n"                        /* G              */ \
	"srl $24, $24, 13\n"                         /* G >> 13        */ \
	"lbu $24, color_space_conversion_fix($24)\n" /* G              */ \
	"sb $24, "#out"+5($14)\n"                    /* out <- G       */ \
	"add $22, $25, $19\n"                        /* B              */ \
	"srl $22, $22, 13\n"                         /* B >> 13        */ \
	"lbu $22, color_space_conversion_fix($22)\n" /* B              */ \
	"sb $22, "#out"+6($14)\n"                    /* out <- B       */






void csc_asm(struct csc_struct *p)
	{
	// (width  % 8) == 0
	// (height % 2) == 0


	asm __volatile__
		(
		"lw $2, 0(%0)\n"  // $2 <- cy
		"lw $3, 24(%0)\n" // $3 <- out


		"lw $4, 28(%0)\n" // $4 <- width
		"srl $5, $4, 1\n" // $5 <- width / 2


		"lw $8, 32(%0)\n" // $8 <- height
		"srl $8, $8, 1\n" // $8 <- height / 2


		"lw $9, 36(%0)\n" // $9 <- line_size
		"sll $9, $9, 2\n" // $9 <- 4 * line_size


		"sll $10, $9, 1\n"    // $10 <- 2 * 4 * line_size
		"subu $10, $10, $4\n"
		"subu $10, $10, $4\n"
		"subu $10, $10, $4\n"
		"subu $10, $10, $4\n" // $10 <- 2 * 4 * line_size - 4 * width (add_out)


		"lw $12, 12(%0)\n"    // $12 <- l0
		"sll $11, $12, 1\n"   // $11 <- 2 * l0
		"subu $11, $11, $4\n" // $11 <- 2 * l0 - width (add_cy)


		"lw $15, 4(%0)\n" // $15 <- cu
		"lw $16, 8(%0)\n" // $16 <- cv

		"lw $17, 16(%0)\n" // $17 <- l1
		"lw $18, 20(%0)\n" // $18 <- l2

		"subu $17, $17, $5\n" // $17 <- l1 - width / 2 (add_cu)
		"subu $18, $18, $5\n" // $18 <- l2 - width / 2 (add_cv)

		"lw $26, 40(%0)\n"		// $26 <- y_offset
		"sll $26, $26, 13\n"	// $26 <- y_offset << 13



		"h_loop:\n"


		"  move $6, $5\n" // $6 <- width / 2
		"  w_loop:\n"




		// tmp -> $7 $13 $14 $19 $20 $21 $22 $23 $24 $25
		make_uv()
		// tmp -> $7 $13 $14 $22 $23 $24 $25
		make_y_high(0, 0)
		make_y_low (0, 0)


		// tmp -> $7 $13 $14 $19 $20 $21 $22 $23 $24 $25
		make_uv()
		// tmp -> $7 $13 $14 $22 $23 $24 $25
		make_y_high(2, 8)
		make_y_low (2, 8)


		// tmp -> $7 $13 $14 $19 $20 $21 $22 $23 $24 $25
		make_uv()
		// tmp -> $7 $13 $14 $22 $23 $24 $25
		make_y_high(4, 16)
		make_y_low (4, 16)


		// tmp -> $7 $13 $14 $19 $20 $21 $22 $23 $24 $25
		make_uv()
		// tmp -> $7 $13 $14 $22 $23 $24 $25
		make_y_high(6, 24)
		make_y_low (6, 24)





		"    addiu $3, $3, 32\n" // out = out + 32
		"    addiu $2, $2, 8\n"  // cy = cy + 8



		"    subu $6, $6, 4\n" // $6 = $6 - 4
		"    bnez $6, w_loop\n"
		"    nop\n"





		"  addu $3, $3, $10\n" // out = out + add_out
		"  addu $2, $2, $11\n" // cy = cy + add_cy
		"  addu $15, $15, $17\n" // cu = cu + add_cu
		"  addu $16, $16, $18\n" // cv = cv + add_cv


		"  subu $8, $8, 1\n" // $8 = $8 - 1
		"  bnez $8, h_loop\n"
		"  nop\n"



		:
		: "r" (p)
		: "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23", "$24", "$25"
		);
	}











void csc_c(struct csc_struct *p)
	{
	#define BIT 13


	unsigned char *cy  = p->cy;
	unsigned char *cu  = p->cu;
	unsigned char *cv  = p->cv;
	int l0             = p->l0;
	int l1             = p->l1;
	int l2             = p->l2;
	unsigned char *out = p->out;
	int width          = p->width;
	int height         = p->height;
	int line_size      = p->line_size;
	int y_offset       = p->y_offset << BIT;


	int Y, U, V, R, G, B, y, x;
	int MU1, MU2, MV1, MV2, ADDG, VAL;





	for (y = 0; y < height; y += 2)
		{
		for (x = 0; x < width; x += 2)
			{
			U = cu[x >> 1];
			V = cv[x >> 1];

			MU1 = color_space_conversion_mu1[U];
			MU2 = color_space_conversion_mu2[U];
			MV1 = color_space_conversion_mv1[V];
			MV2 = color_space_conversion_mv2[V];
			ADDG = MU2 + MV2;




			Y = color_space_conversion_my[cy[x]] + y_offset;

			B = Y + MU1;
			R = Y + MV1;
			G = Y + ADDG;

            R >>= BIT;
            G >>= BIT;
            B >>= BIT;


            VAL = (color_space_conversion_fix[B] << 16) + (color_space_conversion_fix[G] << 8) + color_space_conversion_fix[R];
            ((int *)(out))[x] = VAL;




			Y = color_space_conversion_my[cy[x + 1]] + y_offset;

			B = Y + MU1;
			R = Y + MV1;
			G = Y + ADDG;

            R >>= BIT;
            G >>= BIT;
            B >>= BIT;

            VAL = (color_space_conversion_fix[B] << 16) + (color_space_conversion_fix[G] << 8) + color_space_conversion_fix[R];
            ((int *)(out))[x + 1] = VAL;





			Y = color_space_conversion_my[cy[x + l0]] + y_offset;

			B = Y + MU1;
			R = Y + MV1;
			G = Y + ADDG;

            R >>= BIT;
            G >>= BIT;
            B >>= BIT;

            VAL = (color_space_conversion_fix[B] << 16) + (color_space_conversion_fix[G] << 8) + color_space_conversion_fix[R];
            ((int *)(out))[x + line_size] = VAL;





			Y = color_space_conversion_my[cy[x + l0 + 1]] + y_offset;

			B = Y + MU1;
			R = Y + MV1;
			G = Y + ADDG;

            R >>= BIT;
            G >>= BIT;
            B >>= BIT;

            VAL = (color_space_conversion_fix[B] << 16) + (color_space_conversion_fix[G] << 8) + color_space_conversion_fix[R];
            ((int *)(out))[x + line_size + 1] = VAL;
			}


		cy  += l0 << 1;
		out += line_size << 3;


		cu += l1;
		cv += l2;
		}
	}
