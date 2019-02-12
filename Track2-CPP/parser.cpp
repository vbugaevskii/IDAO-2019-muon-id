#include "./parser.h"

inline bool not_number(const char pen) {
    return !isdigit(pen) && !(pen == '.') && !(pen == '-');
}

void skip_to_number_pointer(const char *& pen) {
    while ((*pen) && not_number(*pen)) ++pen;
}

inline float square(const float x) {
    return x*x;
}

// https://stackoverflow.com/questions/5678932/fastest-way-to-read-numerical-values-from-text-file-in-c-double-in-this-case
template<class T>
T rip_uint_pointer(const char *&pen, T val = 0) {
    // Will return val if *pen is not a digit
    // WARNING: no overflow checks
    for (char c; (c = *pen ^ '0') <= 9; ++pen)
        val = val * 10 + c;
    return val;
}

template<class T>
T rip_float_pointer(const char *&pen) {
    static double const exp_lookup[]
        = {1, 0.1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9, 1e-10,
           1e-11, 1e-12, 1e-13, 1e-14, 1e-15, 1e-16, 1e-17};
    T sign = 1.;
    if (*pen == '-') {
        ++pen;
        sign = -1.;
    }
    uint64_t val = rip_uint_pointer<uint64_t>(pen);
    unsigned int neg_exp = 0;
    if (*pen == '.') {
        const char* const fracs = ++pen;
        val = rip_uint_pointer(pen, val);
        neg_exp  = pen - fracs;
    }
    return std::copysign(val*exp_lookup[neg_exp], sign);
}

// Warning: this is not a general-puropse parser, you have
// std::istream for that. As a rule, in the interest of speed, it
// doesn't check for input correctness and will have undefined
// behavior at incorrect input
class BufferedStream {
 public:
    explicit BufferedStream(std::istream& stream);
    // Discards data from the stream until ecountering a digit, "." or "-"
    void skip_to_number();
    // Reads a float from the stream, starting from the current character
    // and has undefined behaviour if there is no number at the
    // current position
    template<class T> T rip_float() {return rip_float_pointer<T>(pen);}
    // Reads an unsigned integer from stream, starting from the
    // current character and has undefined behaviour if there is no
    // number at the current position
    template<class T> T rip_uint() {return rip_uint_pointer<T>(pen);}
    // Reads a vector of floats of the given size from the stream,
    // skipping everything as needed
    template<class T>
    std::vector<T> fill_vector_float(const size_t size);
    // Reads a vector of unsigned ints of the given size from the stream,
    // skipping as needed. In principle, should be templated from
    // fill_vector_float, but if constexpr is C++17 :(
    template<class T>
    std::vector<T> fill_vector_uint(const size_t size);
    // Reads count floating point numbers and stores them into the
    // container pointed to by the iterator
    template<class IteratorType>
    void fill_iterator_float(const IteratorType& iterator, const size_t count);
    // Discards data from the stream until encountering the delimiter
    void skip_to_char(const char delimiter);
    // Discrads data from the stream until twice encountering the delimiter
    void skip_record(const char delimiter);

 private:
    void next_line();
    // Buffer size is measured to fit the longest line in the test dataset
    // but the code doesn't rely on it
    static const size_t BUFFER_SIZE = 1016;
    char buffer[BUFFER_SIZE];
    std::istream& stream;
    const char* pen;
};

void BufferedStream::next_line() {
    stream.getline(buffer, BUFFER_SIZE);
    pen = buffer;
}

BufferedStream::BufferedStream(std::istream& stream): stream(stream) {
    next_line();
}

void BufferedStream::skip_to_number() {
    skip_to_number_pointer(pen);
    while ((*pen) == 0) {
        next_line();
        skip_to_number_pointer(pen);
        // The skip stops either at 0-byte or
        // a number part
    }
}

template<class T>
std::vector<T> BufferedStream::fill_vector_float(const size_t size) {
    std::vector<T> result(size);
    fill_iterator_float<std::vector<float>::iterator>(result.begin(), size);
    return result;
}

template<class T>
std::vector<T> BufferedStream::fill_vector_uint(const size_t size) {
    std::vector<T> result(size);
    for (auto& value : result) {
        skip_to_number();
        value = rip_uint<T>();
    }
    return result;
}

void BufferedStream::skip_to_char(const char delimiter) {
    while ((*pen) != delimiter) {
        while ((*pen) && (*(++pen)) != delimiter) {}
        if (!(*pen)) next_line();
    }
}

void BufferedStream::skip_record(const char delimiter) {
    skip_to_char(delimiter);
    ++pen;
    skip_to_char(delimiter);
}

template<class IteratorType>
void BufferedStream::fill_iterator_float(const IteratorType& iterator, const size_t count) {
    for (IteratorType value = iterator; value != iterator + count; ++value) {
        skip_to_number();
        *value = rip_float<typename std::iterator_traits<IteratorType>::value_type>();
    }
}

void ugly_hardcoded_parse(std::istream& stream, size_t* id, std::vector<float>* result) {
    BufferedStream buffered_stream(stream);
    *id = buffered_stream.rip_uint<size_t>();
    buffered_stream.fill_iterator_float<std::vector<float>::iterator>(
        result->begin(), N_RAW_FEATURES - N_RAW_FEATURES_TAIL);

    // fillna X
    {
    size_t i_prev = MATCHED_HIT_X_INDEX + 1;
    size_t i_curr = MATCHED_HIT_X_INDEX + 2;
    size_t i_next = MATCHED_HIT_X_INDEX + 3;

    bool mask = ( (*result)[i_curr] < NA_VALUE + 1 && \
                  !( (*result)[i_next + 3] < NA_VALUE + 1 ) );
    if ( mask ) {
        (*result)[i_curr] = ( (*result)[i_prev] + (*result)[i_next] ) / 2.0;
    }
    }

    // fillna Y
    {
    size_t i_prev = MATCHED_HIT_Y_INDEX + 1;
    size_t i_curr = MATCHED_HIT_Y_INDEX + 2;
    size_t i_next = MATCHED_HIT_Y_INDEX + 3;

    bool mask = ( (*result)[i_curr] < NA_VALUE + 1 && \
                  !( (*result)[i_next + 3] < NA_VALUE + 1 ) );
    if ( mask ) {
        (*result)[i_curr] = ( (*result)[i_prev] + (*result)[i_next] ) / 2.0;
    }
    }

    // fillna Z
    {
    size_t i_prev = MATCHED_HIT_Z_INDEX + 1;
    size_t i_curr = MATCHED_HIT_Z_INDEX + 2;
    size_t i_next = MATCHED_HIT_Z_INDEX + 3;

    bool mask = ( (*result)[i_curr] < NA_VALUE + 1 && \
                  !( (*result)[i_next + 3] < NA_VALUE + 1 ) );
    if ( mask ) {
        (*result)[i_curr] = ( (*result)[i_prev] + (*result)[i_next] ) / 2.0;
    }
    }

    // No need to skip, fill_vector takes care of it
    const size_t FOI_hits_N = (*result)[FOI_HITS_N_INDEX];
    const std::vector<float> FOI_hits_X = buffered_stream.fill_vector_float<float>(FOI_hits_N);
    const std::vector<float> FOI_hits_Y = buffered_stream.fill_vector_float<float>(FOI_hits_N);
    const std::vector<float> FOI_hits_Z = buffered_stream.fill_vector_float<float>(FOI_hits_N);
    const std::vector<float> FOI_hits_DX = buffered_stream.fill_vector_float<float>(FOI_hits_N);
    const std::vector<float> FOI_hits_DY = buffered_stream.fill_vector_float<float>(FOI_hits_N);
    buffered_stream.skip_record(DELIMITER);
    const std::vector<float> FOI_hits_T = buffered_stream.fill_vector_float<float>(FOI_hits_N);
    buffered_stream.skip_record(DELIMITER);
    const std::vector<size_t> FOI_hits_S = \
        buffered_stream.fill_vector_uint<size_t>(FOI_hits_N);

    std::array<size_t, N_STATIONS> closest_hit_index;

    std::array<float, N_STATIONS> hit_distance_min;
    hit_distance_min.fill(std::numeric_limits<float>::infinity());

    std::array<float, N_STATIONS> hit_distance_max;
    hit_distance_max.fill(-1);

    std::array<double, N_STATIONS> hit_distance_mean;
    hit_distance_mean.fill(0);

    std::array<double, N_STATIONS> hit_distance_coord_x_mean;
    hit_distance_mean.fill(0);

    std::array<double, N_STATIONS> hit_distance_coord_y_mean;
    hit_distance_mean.fill(0);

    std::array<int, N_STATIONS> hit_distance_counts;
    hit_distance_counts.fill(0);

    for (size_t hit_index = 0; hit_index < FOI_hits_N; ++hit_index) {
        const size_t this_station = FOI_hits_S[hit_index];
        const float distance_x_2 = square(FOI_hits_X[hit_index] -
                                          (*result)[LEXTRA_X_INDEX + this_station]);
        const float distance_y_2 = square(FOI_hits_Y[hit_index] -
                                          (*result)[LEXTRA_Y_INDEX + this_station]);
        const float distance_2 = distance_x_2 + distance_y_2;
        if (distance_2 < hit_distance_min[this_station]) {
            hit_distance_min[this_station] = distance_2;
            closest_hit_index[this_station] = hit_index;

            (*result)[FOI_FEATURES_START + 0 * N_STATIONS + this_station] = distance_x_2;
            (*result)[FOI_FEATURES_START + 1 * N_STATIONS + this_station] = distance_y_2;
        }

        if (distance_2 > hit_distance_max[this_station]) {
            hit_distance_max[this_station] = distance_2;
        }

        hit_distance_counts[this_station]++;
        hit_distance_mean[this_station] += distance_2;

        hit_distance_coord_x_mean[this_station] += FOI_hits_X[hit_index];
        hit_distance_coord_y_mean[this_station] += FOI_hits_Y[hit_index];
    }

    for (size_t station = 0; station < N_STATIONS; ++station) {
        if (!hit_distance_counts[station])
            continue;

        hit_distance_mean[station] /= hit_distance_counts[station];
        hit_distance_coord_x_mean[station] /= hit_distance_counts[station];
        hit_distance_coord_y_mean[station] /= hit_distance_counts[station];
    }

    /* [closest_x_per_station, closest_y_per_station, closest_T_per_station,
       closest_z_per_station, closest_dx_per_station, closest_dy_per_station]) */
    for (size_t station = 0; station < N_STATIONS; ++station) {
        if (!hit_distance_counts[station]) {
            for (size_t feature_index = 0;
                 feature_index < FOI_FEATURES_PER_STATION;
                 ++feature_index) {
                (*result)[FOI_FEATURES_START + feature_index * N_STATIONS + station] = NA_VALUE;
            }
        } else {
            // first 2 fields are filled upper

            (*result)[FOI_FEATURES_START + 2 * N_STATIONS + station] = \
                FOI_hits_T[closest_hit_index[station]];
            (*result)[FOI_FEATURES_START + 3 * N_STATIONS + station] = \
                FOI_hits_Z[closest_hit_index[station]];
            (*result)[FOI_FEATURES_START + 4 * N_STATIONS + station] = \
                FOI_hits_DX[closest_hit_index[station]];
            (*result)[FOI_FEATURES_START + 5 * N_STATIONS + station] = \
                FOI_hits_DY[closest_hit_index[station]];

            (*result)[FOI_FEATURES_START + 6 * N_STATIONS + station] = \
                hit_distance_min[station];
            (*result)[FOI_FEATURES_START + 7 * N_STATIONS + station] = \
                hit_distance_max[station];
            (*result)[FOI_FEATURES_START + 8 * N_STATIONS + station] = \
                hit_distance_mean[station];

            (*result)[FOI_FEATURES_START + 9 * N_STATIONS + station] = \
                (*result)[LEXTRA_X_INDEX + station] - hit_distance_coord_x_mean[station];
            (*result)[FOI_FEATURES_START + 10 * N_STATIONS + station] = \
                (*result)[LEXTRA_Y_INDEX + station] - hit_distance_coord_y_mean[station];
        }
    }
    buffered_stream.fill_iterator_float<std::vector<float>::iterator>(
        result->begin() + N_RAW_FEATURES - N_RAW_FEATURES_TAIL, N_RAW_FEATURES_TAIL);

    // make_unbiased_z
    {
    for (size_t station = 0; station < N_STATIONS; ++station) {
        (*result)[MATCHED_HIT_Z_INDEX + station] -= MATCHED_HIT_Z_MEAN[station];
    }
    }

    // foi hits per station
    {
    size_t offset = N_FEATURES_BASELINE + FO_CUSTOM_00;

    for (size_t station = 0; station < N_STATIONS; ++station) {
        (*result)[offset + station] = hit_distance_counts[station];
    }
    }

    // compute_polar_distances_matched_hit
    {
    size_t offset = N_FEATURES_BASELINE + FO_CUSTOM_01;

    for (size_t station = 0; station < N_STATIONS; ++station) {
        bool mask = ( (*result)[MATCHED_HIT_X_INDEX + station] < NA_VALUE + 1 || \
                      (*result)[MATCHED_HIT_Y_INDEX + station] < NA_VALUE + 1 );
        if (mask) {
            (*result)[offset + station] = NA_VALUE;                 // radius
            (*result)[offset + N_STATIONS + station] = NA_VALUE;    // angle
            continue;
        }

        float r = pow( (*result)[MATCHED_HIT_X_INDEX + station], 2 ) + \
                  pow( (*result)[MATCHED_HIT_Y_INDEX + station], 2 );
        (*result)[offset + station] = std::log(1.0 + r);

        float a = atan2( (*result)[MATCHED_HIT_Y_INDEX + station],
                         (*result)[MATCHED_HIT_X_INDEX + station] );
        (*result)[offset + N_STATIONS + station] = a;
    }
    }

    // distance_dist_lextra_matched
    {
    size_t offset = N_FEATURES_BASELINE + FO_CUSTOM_02;

    int distance_counts = 0;
    float distance_se = 0, distance_ae = 0;

    for (size_t station = 0; station < N_STATIONS; ++station) {
        bool mask = ( (*result)[MATCHED_HIT_X_INDEX + station] < NA_VALUE + 1 || \
                      (*result)[MATCHED_HIT_Y_INDEX + station] < NA_VALUE + 1 );
        if (mask) {
            (*result)[offset + station] = NA_VALUE;
            continue;
        }

        float delta_x = (*result)[MATCHED_HIT_X_INDEX + station] - (*result)[LEXTRA_X_INDEX + station];
        float delta_y = (*result)[MATCHED_HIT_Y_INDEX + station] - (*result)[LEXTRA_Y_INDEX + station];

        float mse = pow(delta_x, 2) + pow(delta_y, 2);
        float mae = std::max(fabs(delta_x), fabs(delta_y));

        (*result)[offset + station] = std::log(1.0 + mse);

        distance_se += mse;
        distance_ae = std::max(distance_ae, mae);
        distance_counts++;
    }

    // previously filled N_STATIONS features
    (*result)[offset + N_STATIONS + 0] = distance_se / distance_counts;
    (*result)[offset + N_STATIONS + 1] = distance_ae;
    }

    // ratio_radius_matched_hit
    {
    size_t offset = N_FEATURES_BASELINE + FO_CUSTOM_03;

    for (size_t station_i = 0; station_i < N_STATIONS; ++station_i) {
        size_t station_j = 0;
        if (station_i < N_STATIONS - 1) {
            station_j = station_i + 1;
        } else {
            station_j = 0;
        }

        bool mask = ( (*result)[MATCHED_HIT_X_INDEX + station_i] < NA_VALUE + 1 || \
                      (*result)[MATCHED_HIT_Y_INDEX + station_i] < NA_VALUE + 1 || \
                      (*result)[MATCHED_HIT_X_INDEX + station_j] < NA_VALUE + 1 || \
                      (*result)[MATCHED_HIT_Y_INDEX + station_j] < NA_VALUE + 1 );
        if (mask) {
            (*result)[offset + 3 * station_i + 0] = NA_VALUE; // delta
            (*result)[offset + 3 * station_i + 1] = NA_VALUE; // delta abs
            (*result)[offset + 3 * station_i + 2] = NA_VALUE; // frac
            continue;
        }

        float delta = (*result)[MATCHED_HIT_RADIUS_INDEX + station_j] \
                    - (*result)[MATCHED_HIT_RADIUS_INDEX + station_i];
        (*result)[offset + 3 * station_i + 0] = delta;
        (*result)[offset + 3 * station_i + 1] = fabs(delta);

        float frac = (*result)[MATCHED_HIT_RADIUS_INDEX + station_j] \
                   / (*result)[MATCHED_HIT_RADIUS_INDEX + station_i];
        (*result)[offset + 3 * station_i + 2] = frac;
    }
    }

    // ratio_mextra_matched
    {
    size_t offset = N_FEATURES_BASELINE + FO_CUSTOM_04;

    for (size_t station = 0; station < N_STATIONS; ++station) {
        bool mask = ( (*result)[MATCHED_HIT_DX_INDEX + station] < NA_VALUE + 1 || \
                      (*result)[MATCHED_HIT_DY_INDEX + station] < NA_VALUE + 1 );
        if (mask) {
            (*result)[offset + station] = NA_VALUE;
            continue;
        }

        float frac = (*result)[MATCHED_HIT_DX_INDEX + station] / pow( (*result)[MEXTRA_DX_INDEX + station], 0.5 ) + \
                     (*result)[MATCHED_HIT_DY_INDEX + station] / pow( (*result)[MEXTRA_DY_INDEX + station], 0.5 );
        (*result)[offset + station] = frac;
    }
    }

    // momentum
    {
    size_t offset = N_FEATURES_BASELINE + FO_CUSTOM_05;

    bool mask = ( (*result)[P_INDEX] < NA_VALUE + 1 || (*result)[PT_INDEX] < NA_VALUE + 1 );
    if (mask) {
        (*result)[offset] = NA_VALUE;
    }

    float momentum = pow( (*result)[P_INDEX],  2.0 ) - pow( (*result)[PT_INDEX], 2.0 );
    (*result)[offset] = momentum;
    }

    // nshared_foihits
    {
    size_t offset = N_FEATURES_BASELINE + FO_CUSTOM_06;
    if ( !static_cast<int>( (*result)[FOI_HITS_N_INDEX] ) ) {
        (*result)[offset] = -1;
    } else {
        (*result)[offset] = static_cast<float>( (*result)[N_SHARED_INDEX] ) / (*result)[FOI_HITS_N_INDEX];
    }
    }

    // cluster_size
    {
    size_t offset = N_FEATURES_BASELINE + FO_CUSTOM_07;
    for ( size_t station = 0; station < N_STATIONS; ++station) {
        float cls_size = (*result)[NCL_INDEX + station] * (*result)[AVG_CS_INDEX + station];
        (*result)[offset + station] = cls_size;
    }
    }
}
