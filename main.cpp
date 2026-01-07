#include <iostream>
#include <lapacke.h>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>
#include <fstream>
#include <complex>
#include <format>
#include <cstring>

constexpr size_t get_n(const unsigned char f) {
    return f < 2 ? 2u : 2u * get_n(f-1);
}

/* 11 */
constexpr size_t N = get_n(11);

constexpr double s0 = 0.0;
constexpr double s1 = 1.0;

constexpr size_t repetitions() {
    if constexpr (N < 8) {
        return 1e6;
    } else if constexpr (N < 32) {
        return 1e5;
    } else if constexpr (N < 128) {
        return 1e4;
    } else if constexpr (N < 512) {
        return 10;
    }
    return 3;
}

std::vector<double> generate_matrix(size_t n);
std::vector<std::complex<double>> calculate();
void store_result(const std::vector<std::complex<double>>& result, size_t n);

int main() {
    constexpr lapack_int n = N;
    constexpr size_t reps = repetitions();

    const std::vector<double> A0 = generate_matrix(N);
    std::vector<double> A(A0.size());

    std::vector<double> wr(n), wi(n);
    std::vector<std::complex<double>> eig(n);

    std::vector<std::vector<std::complex<double>>> res;
    if constexpr (repetitions() < 1e2) {
        res.reserve(reps);
    }

    std::memcpy(A.data(), A0.data(), A0.size() * sizeof(double));
    (void)LAPACKE_dgeev(LAPACK_COL_MAJOR, 'N', 'N', n,
                        A.data(), n,
                        wr.data(), wi.data(),
                        nullptr, n, nullptr, n);

    const auto t0 = std::chrono::steady_clock::now();

    for (size_t rep = 0; rep < reps; ++rep) {
        std::memcpy(A.data(), A0.data(), A0.size() * sizeof(double));

        const lapack_int info = LAPACKE_dgeev(
            LAPACK_COL_MAJOR,
            'N', 'N',
            n,
            A.data(), n,
            wr.data(), wi.data(),
            nullptr, n,
            nullptr, n
        );

        if (info != 0) {
            std::cerr << "LAPACKE_dgeev failed, info=" << info << "\n";
            break;
        }

        for (lapack_int k = 0; k < n; ++k) {
            eig[static_cast<size_t>(k)] = {wr[static_cast<size_t>(k)], wi[static_cast<size_t>(k)]};
        }

        if constexpr (repetitions() < 1e2) {
            res.push_back(eig);
        }
    }

    const auto t1 = std::chrono::steady_clock::now();

    const auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    const double ms_total = static_cast<double>(us) / 1000.0;
    const double ms_avg = ms_total / static_cast<double>(reps);

    std::cout << "delta t: " << ms_total << " ms\n";
    std::cout << "delta t / repetitions: " << ms_avg << " ms\n";

    if constexpr (repetitions() < 1e2) {
        for (size_t rep = 0; rep < res.size(); ++rep) {
            store_result(res[rep], rep);
        }
    }

    return 0;
}

std::vector<double> generate_matrix(const size_t n) {
    std::vector<double> A(n * n);

    thread_local std::mt19937_64 gen(std::random_device{}());
    std::normal_distribution dist(s0, s1);

    for (size_t j = 0; j < n; ++j) {
        for (size_t i = 0; i < n; ++i) {
            A[i + j * n] = dist(gen);
        }
    }

    return A;
}

std::vector<std::complex<double>> calculate() {
    constexpr lapack_int n = N;

    /* np.random.normal */
    std::vector<double> A = generate_matrix(N);

    std::vector<double> wr(n), wi(n);

    const lapack_int info = LAPACKE_dgeev(
        LAPACK_COL_MAJOR,
        'N', 'N',
        n,
        A.data(), n,
        wr.data(), wi.data(),
        nullptr, n,
        nullptr, n
    );

    if (info != 0) {
        std::cerr << "LAPACKE_dgeev failed, info=" << info << "\n";
        std::vector<std::complex<double>> fallback(n);
        for (lapack_int k = 0; k < n; ++k) {
            fallback[static_cast<size_t>(k)] = {wr[static_cast<size_t>(k)], 0.0};
        }
        return fallback;
    }

    std::vector<std::complex<double>> eig(n);
    for (lapack_int k = 0; k < n; ++k) {
        eig[static_cast<size_t>(k)] = {wr[static_cast<size_t>(k)], wi[static_cast<size_t>(k)]};
    }

    return eig;
}

void store_result(const std::vector<std::complex<double>>& result, size_t n) {
    std::ofstream out(std::format("result-{}.txt", n), std::ios::trunc);
    if (!out) {
        std::cerr << "Could not open result.txt\n";
        return;
    }

    out << std::setprecision(17);
    for (const auto& z : result) {
        out << "[" << z.real() << " | " << z.imag() << "]" << '\n';
    }
}