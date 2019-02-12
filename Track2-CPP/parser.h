// Copyright 2019, Nikita Kazeev, Higher School of Economics
#pragma once
#include <ctype.h>
#include <cstdint>
#include <cmath>

#include <iostream>
#include <vector>
#include <array>
#include <limits>

const size_t N_STATIONS = 4;
const size_t FOI_FEATURES_PER_STATION = 11;
// The structure of .csv is the following:
// id, <62 float features>, number of hits in FOI, <9 arrays of FOI hits features>, <2 float features>
const size_t N_RAW_FEATURES = 65;
const size_t N_RAW_FEATURES_TAIL = 2;

const std::vector<float> MATCHED_HIT_Z_MEAN = { 15270, 16470, 17670, 18870 };

// simple features indexes
const size_t FOI_HITS_N_INDEX = 62;
const size_t LEXTRA_X_INDEX = 45;
const size_t LEXTRA_Y_INDEX = 49;
const size_t MATCHED_HIT_X_INDEX = 13;
const size_t MATCHED_HIT_Y_INDEX = 17;
const size_t MATCHED_HIT_Z_INDEX = 21;
const size_t MATCHED_HIT_DX_INDEX = 25;
const size_t MATCHED_HIT_DY_INDEX = 29;
const size_t MATCHED_HIT_DZ_INDEX = 33;
const size_t MEXTRA_DX_INDEX = 54;
const size_t MEXTRA_DY_INDEX = 58;
const size_t P_INDEX = 64;
const size_t PT_INDEX = 63;
const size_t N_SHARED_INDEX = 53;
const size_t NCL_INDEX = 0;
const size_t AVG_CS_INDEX = 4;

// custom features offsets
const size_t FO_CUSTOM_00 = 0;                              // foi counters per station
const size_t FO_CUSTOM_01 = N_STATIONS;                     // compute_polar_distances_matched_hit
const size_t FO_CUSTOM_02 = FO_CUSTOM_01 + 2 * N_STATIONS;  // distance_dist_lextra_matched
const size_t FO_CUSTOM_03 = FO_CUSTOM_02 + N_STATIONS + 2;  // ratio_radius_matched_hit
const size_t FO_CUSTOM_04 = FO_CUSTOM_03 + 3 * N_STATIONS;  // ratio_mextra_matched
const size_t FO_CUSTOM_05 = FO_CUSTOM_04 + N_STATIONS;      // momentum
const size_t FO_CUSTOM_06 = FO_CUSTOM_05 + 1;               // nshared_foihits
const size_t FO_CUSTOM_07 = FO_CUSTOM_06 + 1;               // cluster_size
const size_t FO_CUSTOM_08 = FO_CUSTOM_07 + N_STATIONS;

// number of features per group
const size_t N_FEATURES_BASELINE = N_RAW_FEATURES + N_STATIONS * FOI_FEATURES_PER_STATION;
const size_t N_FEATURES_CUSTOM = FO_CUSTOM_08;
const size_t N_FEATURES = N_FEATURES_BASELINE + N_FEATURES_CUSTOM;

// custom features indexes
const size_t MATCHED_HIT_RADIUS_INDEX = N_FEATURES_BASELINE + FO_CUSTOM_01;

const size_t FOI_FEATURES_START = N_RAW_FEATURES;
const float EMPTY_FILLER = 1000;
const float NA_VALUE = -9999;

const char DELIMITER = ',';

void ugly_hardcoded_parse(std::istream& stream, size_t* id, std::vector<float>* result);
