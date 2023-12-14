-- <DUMPING MAP DATABASE>
database = {
	water = {
		data = {
			[1] = {
				dst=88,
				src=18,
				range=7,
			},
			[2] = {
				dst=18,
				src=25,
				range=70,
			},
		},
		label = {
			dst="light",
			src="water",
		},
	},
	temperature = {
		data = {
			[1] = {
				dst=0,
				src=69,
				range=1,
			},
			[2] = {
				dst=1,
				src=0,
				range=69,
			},
		},
		label = {
			dst="humidity",
			src="temperature",
		},
	},
	seed = {
		data = {
			[1] = {
				dst=50,
				src=98,
				range=2,
			},
			[2] = {
				dst=52,
				src=50,
				range=48,
			},
		},
		label = {
			dst="soil",
			src="seed",
		},
	},
	fertilizer = {
		data = {
			[1] = {
				dst=49,
				src=53,
				range=8,
			},
			[2] = {
				dst=0,
				src=11,
				range=42,
			},
			[3] = {
				dst=42,
				src=0,
				range=7,
			},
			[4] = {
				dst=57,
				src=7,
				range=4,
			},
		},
		label = {
			dst="water",
			src="fertilizer",
		},
	},
	humidity = {
		data = {
			[1] = {
				dst=60,
				src=56,
				range=37,
			},
			[2] = {
				dst=56,
				src=93,
				range=4,
			},
		},
		label = {
			dst="location",
			src="humidity",
		},
	},
	soil = {
		data = {
			[1] = {
				dst=0,
				src=15,
				range=37,
			},
			[2] = {
				dst=37,
				src=52,
				range=2,
			},
			[3] = {
				dst=39,
				src=0,
				range=15,
			},
		},
		label = {
			dst="fertilizer",
			src="soil",
		},
	},
	light = {
		data = {
			[1] = {
				dst=45,
				src=77,
				range=23,
			},
			[2] = {
				dst=81,
				src=45,
				range=19,
			},
			[3] = {
				dst=68,
				src=64,
				range=13,
			},
		},
		label = {
			dst="temperature",
			src="light",
		},
	},
}
-- <DUMPING SEED MAPS>
local seeds = {}

seeds[1] = {start = 79, range = 14}
seeds[1].mappings = {
	-- seed-to-soil map: 100 elements
	seed = {
		[0] = 0,		[1] = 1,		[2] = 2,		[3] = 3,
		[4] = 4,		[5] = 5,		[6] = 6,		[7] = 7,
		[8] = 8,		[9] = 9,		[10] = 10,		[11] = 11,
		[12] = 12,		[13] = 13,		[14] = 14,		[15] = 15,
		[16] = 16,		[17] = 17,		[18] = 18,		[19] = 19,
		[20] = 20,		[21] = 21,		[22] = 22,		[23] = 23,
		[24] = 24,		[25] = 25,		[26] = 26,		[27] = 27,
		[28] = 28,		[29] = 29,		[30] = 30,		[31] = 31,
		[32] = 32,		[33] = 33,		[34] = 34,		[35] = 35,
		[36] = 36,		[37] = 37,		[38] = 38,		[39] = 39,
		[40] = 40,		[41] = 41,		[42] = 42,		[43] = 43,
		[44] = 44,		[45] = 45,		[46] = 46,		[47] = 47,
		[48] = 48,		[49] = 49,		[50] = 52,		[51] = 53,
		[52] = 54,		[53] = 55,		[54] = 56,		[55] = 57,
		[56] = 58,		[57] = 59,		[58] = 60,		[59] = 61,
		[60] = 62,		[61] = 63,		[62] = 64,		[63] = 65,
		[64] = 66,		[65] = 67,		[66] = 68,		[67] = 69,
		[68] = 70,		[69] = 71,		[70] = 72,		[71] = 73,
		[72] = 74,		[73] = 75,		[74] = 76,		[75] = 77,
		[76] = 78,		[77] = 79,		[78] = 80,		[79] = 81,
		[80] = 82,		[81] = 83,		[82] = 84,		[83] = 85,
		[84] = 86,		[85] = 87,		[86] = 88,		[87] = 89,
		[88] = 90,		[89] = 91,		[90] = 92,		[91] = 93,
		[92] = 94,		[93] = 95,		[94] = 96,		[95] = 97,
		[96] = 98,		[97] = 99,		[98] = 50,		[99] = 51,
	},
	-- soil-to-fertilizer map: 54 elements
	soil = {
		[0] = 39,		[1] = 40,		[2] = 41,		[3] = 42,
		[4] = 43,		[5] = 44,		[6] = 45,		[7] = 46,
		[8] = 47,		[9] = 48,		[10] = 49,		[11] = 50,
		[12] = 51,		[13] = 52,		[14] = 53,		[15] = 0,
		[16] = 1,		[17] = 2,		[18] = 3,		[19] = 4,
		[20] = 5,		[21] = 6,		[22] = 7,		[23] = 8,
		[24] = 9,		[25] = 10,		[26] = 11,		[27] = 12,
		[28] = 13,		[29] = 14,		[30] = 15,		[31] = 16,
		[32] = 17,		[33] = 18,		[34] = 19,		[35] = 20,
		[36] = 21,		[37] = 22,		[38] = 23,		[39] = 24,
		[40] = 25,		[41] = 26,		[42] = 27,		[43] = 28,
		[44] = 29,		[45] = 30,		[46] = 31,		[47] = 32,
		[48] = 33,		[49] = 34,		[50] = 35,		[51] = 36,
		[52] = 37,		[53] = 38,
	},
	-- fertilizer-to-water map: 61 elements
	fertilizer = {
		[0] = 42,		[1] = 43,		[2] = 44,		[3] = 45,
		[4] = 46,		[5] = 47,		[6] = 48,		[7] = 57,
		[8] = 58,		[9] = 59,		[10] = 60,		[11] = 0,
		[12] = 1,		[13] = 2,		[14] = 3,		[15] = 4,
		[16] = 5,		[17] = 6,		[18] = 7,		[19] = 8,
		[20] = 9,		[21] = 10,		[22] = 11,		[23] = 12,
		[24] = 13,		[25] = 14,		[26] = 15,		[27] = 16,
		[28] = 17,		[29] = 18,		[30] = 19,		[31] = 20,
		[32] = 21,		[33] = 22,		[34] = 23,		[35] = 24,
		[36] = 25,		[37] = 26,		[38] = 27,		[39] = 28,
		[40] = 29,		[41] = 30,		[42] = 31,		[43] = 32,
		[44] = 33,		[45] = 34,		[46] = 35,		[47] = 36,
		[48] = 37,		[49] = 38,		[50] = 39,		[51] = 40,
		[52] = 41,		[53] = 49,		[54] = 50,		[55] = 51,
		[56] = 52,		[57] = 53,		[58] = 54,		[59] = 55,
		[60] = 56,
	},
	-- water-to-light map: 95 elements
	water = {
		[0] = 42,		[1] = 43,		[2] = 44,		[3] = 45,
		[4] = 46,		[5] = 47,		[6] = 48,		[7] = 57,
		[8] = 58,		[9] = 59,		[10] = 60,		[11] = 0,
		[12] = 1,		[13] = 2,		[14] = 3,		[15] = 4,
		[16] = 5,		[17] = 6,		[18] = 88,		[19] = 89,
		[20] = 90,		[21] = 91,		[22] = 92,		[23] = 93,
		[24] = 94,		[25] = 18,		[26] = 19,		[27] = 20,
		[28] = 21,		[29] = 22,		[30] = 23,		[31] = 24,
		[32] = 25,		[33] = 26,		[34] = 27,		[35] = 28,
		[36] = 29,		[37] = 30,		[38] = 31,		[39] = 32,
		[40] = 33,		[41] = 34,		[42] = 35,		[43] = 36,
		[44] = 37,		[45] = 38,		[46] = 39,		[47] = 40,
		[48] = 41,		[49] = 42,		[50] = 43,		[51] = 44,
		[52] = 45,		[53] = 46,		[54] = 47,		[55] = 48,
		[56] = 49,		[57] = 50,		[58] = 51,		[59] = 52,
		[60] = 53,		[61] = 54,		[62] = 55,		[63] = 56,
		[64] = 57,		[65] = 58,		[66] = 59,		[67] = 60,
		[68] = 61,		[69] = 62,		[70] = 63,		[71] = 64,
		[72] = 65,		[73] = 66,		[74] = 67,		[75] = 68,
		[76] = 69,		[77] = 70,		[78] = 71,		[79] = 72,
		[80] = 73,		[81] = 74,		[82] = 75,		[83] = 76,
		[84] = 77,		[85] = 78,		[86] = 79,		[87] = 80,
		[88] = 81,		[89] = 82,		[90] = 83,		[91] = 84,
		[92] = 85,		[93] = 86,		[94] = 87,
	},
	-- light-to-temperature map: 100 elements
	light = {
		[0] = 42,		[1] = 43,		[2] = 44,		[3] = 45,
		[4] = 46,		[5] = 47,		[6] = 48,		[7] = 57,
		[8] = 58,		[9] = 59,		[10] = 60,		[11] = 0,
		[12] = 1,		[13] = 2,		[14] = 3,		[15] = 4,
		[16] = 5,		[17] = 6,		[18] = 88,		[19] = 89,
		[20] = 90,		[21] = 91,		[22] = 92,		[23] = 93,
		[24] = 94,		[25] = 18,		[26] = 19,		[27] = 20,
		[28] = 21,		[29] = 22,		[30] = 23,		[31] = 24,
		[32] = 25,		[33] = 26,		[34] = 27,		[35] = 28,
		[36] = 29,		[37] = 30,		[38] = 31,		[39] = 32,
		[40] = 33,		[41] = 34,		[42] = 35,		[43] = 36,
		[44] = 37,		[45] = 81,		[46] = 82,		[47] = 83,
		[48] = 84,		[49] = 85,		[50] = 86,		[51] = 87,
		[52] = 88,		[53] = 89,		[54] = 90,		[55] = 91,
		[56] = 92,		[57] = 93,		[58] = 94,		[59] = 95,
		[60] = 96,		[61] = 97,		[62] = 98,		[63] = 99,
		[64] = 68,		[65] = 69,		[66] = 70,		[67] = 71,
		[68] = 72,		[69] = 73,		[70] = 74,		[71] = 75,
		[72] = 76,		[73] = 77,		[74] = 78,		[75] = 79,
		[76] = 80,		[77] = 45,		[78] = 46,		[79] = 47,
		[80] = 48,		[81] = 49,		[82] = 50,		[83] = 51,
		[84] = 52,		[85] = 53,		[86] = 54,		[87] = 55,
		[88] = 56,		[89] = 57,		[90] = 58,		[91] = 59,
		[92] = 60,		[93] = 61,		[94] = 62,		[95] = 63,
		[96] = 64,		[97] = 65,		[98] = 66,		[99] = 67,
	},
	-- temperature-to-humidity map: 70 elements
	temperature = {
		[0] = 1,		[1] = 2,		[2] = 3,		[3] = 4,
		[4] = 5,		[5] = 6,		[6] = 7,		[7] = 8,
		[8] = 9,		[9] = 10,		[10] = 11,		[11] = 12,
		[12] = 13,		[13] = 14,		[14] = 15,		[15] = 16,
		[16] = 17,		[17] = 18,		[18] = 19,		[19] = 20,
		[20] = 21,		[21] = 22,		[22] = 23,		[23] = 24,
		[24] = 25,		[25] = 26,		[26] = 27,		[27] = 28,
		[28] = 29,		[29] = 30,		[30] = 31,		[31] = 32,
		[32] = 33,		[33] = 34,		[34] = 35,		[35] = 36,
		[36] = 37,		[37] = 38,		[38] = 39,		[39] = 40,
		[40] = 41,		[41] = 42,		[42] = 43,		[43] = 44,
		[44] = 45,		[45] = 46,		[46] = 47,		[47] = 48,
		[48] = 49,		[49] = 50,		[50] = 51,		[51] = 52,
		[52] = 53,		[53] = 54,		[54] = 55,		[55] = 56,
		[56] = 57,		[57] = 58,		[58] = 59,		[59] = 60,
		[60] = 61,		[61] = 62,		[62] = 63,		[63] = 64,
		[64] = 65,		[65] = 66,		[66] = 67,		[67] = 68,
		[68] = 69,		[69] = 0,
	},
	-- humidity-to-location map: 97 elements
	humidity = {
		[0] = 1,		[1] = 2,		[2] = 3,		[3] = 4,
		[4] = 5,		[5] = 6,		[6] = 7,		[7] = 8,
		[8] = 9,		[9] = 10,		[10] = 11,		[11] = 12,
		[12] = 13,		[13] = 14,		[14] = 15,		[15] = 16,
		[16] = 17,		[17] = 18,		[18] = 19,		[19] = 20,
		[20] = 21,		[21] = 22,		[22] = 23,		[23] = 24,
		[24] = 25,		[25] = 26,		[26] = 27,		[27] = 28,
		[28] = 29,		[29] = 30,		[30] = 31,		[31] = 32,
		[32] = 33,		[33] = 34,		[34] = 35,		[35] = 36,
		[36] = 37,		[37] = 38,		[38] = 39,		[39] = 40,
		[40] = 41,		[41] = 42,		[42] = 43,		[43] = 44,
		[44] = 45,		[45] = 46,		[46] = 47,		[47] = 48,
		[48] = 49,		[49] = 50,		[50] = 51,		[51] = 52,
		[52] = 53,		[53] = 54,		[54] = 55,		[55] = 56,
		[56] = 60,		[57] = 61,		[58] = 62,		[59] = 63,
		[60] = 64,		[61] = 65,		[62] = 66,		[63] = 67,
		[64] = 68,		[65] = 69,		[66] = 70,		[67] = 71,
		[68] = 72,		[69] = 73,		[70] = 74,		[71] = 75,
		[72] = 76,		[73] = 77,		[74] = 78,		[75] = 79,
		[76] = 80,		[77] = 81,		[78] = 82,		[79] = 83,
		[80] = 84,		[81] = 85,		[82] = 86,		[83] = 87,
		[84] = 88,		[85] = 89,		[86] = 90,		[87] = 91,
		[88] = 92,		[89] = 93,		[90] = 94,		[91] = 95,
		[92] = 96,		[93] = 56,		[94] = 57,		[95] = 58,
		[96] = 59,
	},
}

seeds[2] = {start = 55, range = 13}
seeds[2].mappings = {
	-- seed-to-soil map: 100 elements
	seed = {
		[0] = 0,		[1] = 1,		[2] = 2,		[3] = 3,
		[4] = 4,		[5] = 5,		[6] = 6,		[7] = 7,
		[8] = 8,		[9] = 9,		[10] = 10,		[11] = 11,
		[12] = 12,		[13] = 13,		[14] = 14,		[15] = 15,
		[16] = 16,		[17] = 17,		[18] = 18,		[19] = 19,
		[20] = 20,		[21] = 21,		[22] = 22,		[23] = 23,
		[24] = 24,		[25] = 25,		[26] = 26,		[27] = 27,
		[28] = 28,		[29] = 29,		[30] = 30,		[31] = 31,
		[32] = 32,		[33] = 33,		[34] = 34,		[35] = 35,
		[36] = 36,		[37] = 37,		[38] = 38,		[39] = 39,
		[40] = 40,		[41] = 41,		[42] = 42,		[43] = 43,
		[44] = 44,		[45] = 45,		[46] = 46,		[47] = 47,
		[48] = 48,		[49] = 49,		[50] = 52,		[51] = 53,
		[52] = 54,		[53] = 55,		[54] = 56,		[55] = 57,
		[56] = 58,		[57] = 59,		[58] = 60,		[59] = 61,
		[60] = 62,		[61] = 63,		[62] = 64,		[63] = 65,
		[64] = 66,		[65] = 67,		[66] = 68,		[67] = 69,
		[68] = 70,		[69] = 71,		[70] = 72,		[71] = 73,
		[72] = 74,		[73] = 75,		[74] = 76,		[75] = 77,
		[76] = 78,		[77] = 79,		[78] = 80,		[79] = 81,
		[80] = 82,		[81] = 83,		[82] = 84,		[83] = 85,
		[84] = 86,		[85] = 87,		[86] = 88,		[87] = 89,
		[88] = 90,		[89] = 91,		[90] = 92,		[91] = 93,
		[92] = 94,		[93] = 95,		[94] = 96,		[95] = 97,
		[96] = 98,		[97] = 99,		[98] = 50,		[99] = 51,
	},
	-- soil-to-fertilizer map: 54 elements
	soil = {
		[0] = 39,		[1] = 40,		[2] = 41,		[3] = 42,
		[4] = 43,		[5] = 44,		[6] = 45,		[7] = 46,
		[8] = 47,		[9] = 48,		[10] = 49,		[11] = 50,
		[12] = 51,		[13] = 52,		[14] = 53,		[15] = 0,
		[16] = 1,		[17] = 2,		[18] = 3,		[19] = 4,
		[20] = 5,		[21] = 6,		[22] = 7,		[23] = 8,
		[24] = 9,		[25] = 10,		[26] = 11,		[27] = 12,
		[28] = 13,		[29] = 14,		[30] = 15,		[31] = 16,
		[32] = 17,		[33] = 18,		[34] = 19,		[35] = 20,
		[36] = 21,		[37] = 22,		[38] = 23,		[39] = 24,
		[40] = 25,		[41] = 26,		[42] = 27,		[43] = 28,
		[44] = 29,		[45] = 30,		[46] = 31,		[47] = 32,
		[48] = 33,		[49] = 34,		[50] = 35,		[51] = 36,
		[52] = 37,		[53] = 38,
	},
	-- fertilizer-to-water map: 61 elements
	fertilizer = {
		[0] = 42,		[1] = 43,		[2] = 44,		[3] = 45,
		[4] = 46,		[5] = 47,		[6] = 48,		[7] = 57,
		[8] = 58,		[9] = 59,		[10] = 60,		[11] = 0,
		[12] = 1,		[13] = 2,		[14] = 3,		[15] = 4,
		[16] = 5,		[17] = 6,		[18] = 7,		[19] = 8,
		[20] = 9,		[21] = 10,		[22] = 11,		[23] = 12,
		[24] = 13,		[25] = 14,		[26] = 15,		[27] = 16,
		[28] = 17,		[29] = 18,		[30] = 19,		[31] = 20,
		[32] = 21,		[33] = 22,		[34] = 23,		[35] = 24,
		[36] = 25,		[37] = 26,		[38] = 27,		[39] = 28,
		[40] = 29,		[41] = 30,		[42] = 31,		[43] = 32,
		[44] = 33,		[45] = 34,		[46] = 35,		[47] = 36,
		[48] = 37,		[49] = 38,		[50] = 39,		[51] = 40,
		[52] = 41,		[53] = 49,		[54] = 50,		[55] = 51,
		[56] = 52,		[57] = 53,		[58] = 54,		[59] = 55,
		[60] = 56,
	},
	-- water-to-light map: 95 elements
	water = {
		[0] = 42,		[1] = 43,		[2] = 44,		[3] = 45,
		[4] = 46,		[5] = 47,		[6] = 48,		[7] = 57,
		[8] = 58,		[9] = 59,		[10] = 60,		[11] = 0,
		[12] = 1,		[13] = 2,		[14] = 3,		[15] = 4,
		[16] = 5,		[17] = 6,		[18] = 88,		[19] = 89,
		[20] = 90,		[21] = 91,		[22] = 92,		[23] = 93,
		[24] = 94,		[25] = 18,		[26] = 19,		[27] = 20,
		[28] = 21,		[29] = 22,		[30] = 23,		[31] = 24,
		[32] = 25,		[33] = 26,		[34] = 27,		[35] = 28,
		[36] = 29,		[37] = 30,		[38] = 31,		[39] = 32,
		[40] = 33,		[41] = 34,		[42] = 35,		[43] = 36,
		[44] = 37,		[45] = 38,		[46] = 39,		[47] = 40,
		[48] = 41,		[49] = 42,		[50] = 43,		[51] = 44,
		[52] = 45,		[53] = 46,		[54] = 47,		[55] = 48,
		[56] = 49,		[57] = 50,		[58] = 51,		[59] = 52,
		[60] = 53,		[61] = 54,		[62] = 55,		[63] = 56,
		[64] = 57,		[65] = 58,		[66] = 59,		[67] = 60,
		[68] = 61,		[69] = 62,		[70] = 63,		[71] = 64,
		[72] = 65,		[73] = 66,		[74] = 67,		[75] = 68,
		[76] = 69,		[77] = 70,		[78] = 71,		[79] = 72,
		[80] = 73,		[81] = 74,		[82] = 75,		[83] = 76,
		[84] = 77,		[85] = 78,		[86] = 79,		[87] = 80,
		[88] = 81,		[89] = 82,		[90] = 83,		[91] = 84,
		[92] = 85,		[93] = 86,		[94] = 87,
	},
	-- light-to-temperature map: 100 elements
	light = {
		[0] = 42,		[1] = 43,		[2] = 44,		[3] = 45,
		[4] = 46,		[5] = 47,		[6] = 48,		[7] = 57,
		[8] = 58,		[9] = 59,		[10] = 60,		[11] = 0,
		[12] = 1,		[13] = 2,		[14] = 3,		[15] = 4,
		[16] = 5,		[17] = 6,		[18] = 88,		[19] = 89,
		[20] = 90,		[21] = 91,		[22] = 92,		[23] = 93,
		[24] = 94,		[25] = 18,		[26] = 19,		[27] = 20,
		[28] = 21,		[29] = 22,		[30] = 23,		[31] = 24,
		[32] = 25,		[33] = 26,		[34] = 27,		[35] = 28,
		[36] = 29,		[37] = 30,		[38] = 31,		[39] = 32,
		[40] = 33,		[41] = 34,		[42] = 35,		[43] = 36,
		[44] = 37,		[45] = 81,		[46] = 82,		[47] = 83,
		[48] = 84,		[49] = 85,		[50] = 86,		[51] = 87,
		[52] = 88,		[53] = 89,		[54] = 90,		[55] = 91,
		[56] = 92,		[57] = 93,		[58] = 94,		[59] = 95,
		[60] = 96,		[61] = 97,		[62] = 98,		[63] = 99,
		[64] = 68,		[65] = 69,		[66] = 70,		[67] = 71,
		[68] = 72,		[69] = 73,		[70] = 74,		[71] = 75,
		[72] = 76,		[73] = 77,		[74] = 78,		[75] = 79,
		[76] = 80,		[77] = 45,		[78] = 46,		[79] = 47,
		[80] = 48,		[81] = 49,		[82] = 50,		[83] = 51,
		[84] = 52,		[85] = 53,		[86] = 54,		[87] = 55,
		[88] = 56,		[89] = 57,		[90] = 58,		[91] = 59,
		[92] = 60,		[93] = 61,		[94] = 62,		[95] = 63,
		[96] = 64,		[97] = 65,		[98] = 66,		[99] = 67,
	},
	-- temperature-to-humidity map: 70 elements
	temperature = {
		[0] = 1,		[1] = 2,		[2] = 3,		[3] = 4,
		[4] = 5,		[5] = 6,		[6] = 7,		[7] = 8,
		[8] = 9,		[9] = 10,		[10] = 11,		[11] = 12,
		[12] = 13,		[13] = 14,		[14] = 15,		[15] = 16,
		[16] = 17,		[17] = 18,		[18] = 19,		[19] = 20,
		[20] = 21,		[21] = 22,		[22] = 23,		[23] = 24,
		[24] = 25,		[25] = 26,		[26] = 27,		[27] = 28,
		[28] = 29,		[29] = 30,		[30] = 31,		[31] = 32,
		[32] = 33,		[33] = 34,		[34] = 35,		[35] = 36,
		[36] = 37,		[37] = 38,		[38] = 39,		[39] = 40,
		[40] = 41,		[41] = 42,		[42] = 43,		[43] = 44,
		[44] = 45,		[45] = 46,		[46] = 47,		[47] = 48,
		[48] = 49,		[49] = 50,		[50] = 51,		[51] = 52,
		[52] = 53,		[53] = 54,		[54] = 55,		[55] = 56,
		[56] = 57,		[57] = 58,		[58] = 59,		[59] = 60,
		[60] = 61,		[61] = 62,		[62] = 63,		[63] = 64,
		[64] = 65,		[65] = 66,		[66] = 67,		[67] = 68,
		[68] = 69,		[69] = 0,
	},
	-- humidity-to-location map: 97 elements
	humidity = {
		[0] = 1,		[1] = 2,		[2] = 3,		[3] = 4,
		[4] = 5,		[5] = 6,		[6] = 7,		[7] = 8,
		[8] = 9,		[9] = 10,		[10] = 11,		[11] = 12,
		[12] = 13,		[13] = 14,		[14] = 15,		[15] = 16,
		[16] = 17,		[17] = 18,		[18] = 19,		[19] = 20,
		[20] = 21,		[21] = 22,		[22] = 23,		[23] = 24,
		[24] = 25,		[25] = 26,		[26] = 27,		[27] = 28,
		[28] = 29,		[29] = 30,		[30] = 31,		[31] = 32,
		[32] = 33,		[33] = 34,		[34] = 35,		[35] = 36,
		[36] = 37,		[37] = 38,		[38] = 39,		[39] = 40,
		[40] = 41,		[41] = 42,		[42] = 43,		[43] = 44,
		[44] = 45,		[45] = 46,		[46] = 47,		[47] = 48,
		[48] = 49,		[49] = 50,		[50] = 51,		[51] = 52,
		[52] = 53,		[53] = 54,		[54] = 55,		[55] = 56,
		[56] = 60,		[57] = 61,		[58] = 62,		[59] = 63,
		[60] = 64,		[61] = 65,		[62] = 66,		[63] = 67,
		[64] = 68,		[65] = 69,		[66] = 70,		[67] = 71,
		[68] = 72,		[69] = 73,		[70] = 74,		[71] = 75,
		[72] = 76,		[73] = 77,		[74] = 78,		[75] = 79,
		[76] = 80,		[77] = 81,		[78] = 82,		[79] = 83,
		[80] = 84,		[81] = 85,		[82] = 86,		[83] = 87,
		[84] = 88,		[85] = 89,		[86] = 90,		[87] = 91,
		[88] = 92,		[89] = 93,		[90] = 94,		[91] = 95,
		[92] = 96,		[93] = 56,		[94] = 57,		[95] = 58,
		[96] = 59,
	},
}
