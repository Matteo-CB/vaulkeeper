#include "hash/phash.hpp"

#include <array>
#include <bit>
#include <cmath>
#include <numbers>

namespace vk::core {

namespace {

constexpr std::uint32_t DctSize = 32;
constexpr std::uint32_t HashSize = 8;

PerceptualHash computeDHash(const std::uint8_t* pixels, std::uint32_t width, std::uint32_t height) {
    if (width < 9 || height < 8) { return 0; }
    PerceptualHash hash { 0 };
    for (std::uint32_t y = 0; y < 8; ++y) {
        for (std::uint32_t x = 0; x < 8; ++x) {
            const auto ia = pixels[(y * width) + x];
            const auto ib = pixels[(y * width) + x + 1];
            if (ib > ia) {
                hash |= (1ULL << ((y * 8) + x));
            }
        }
    }
    return hash;
}

PerceptualHash computeAHash(const std::uint8_t* pixels, std::uint32_t width, std::uint32_t height) {
    if (width < 8 || height < 8) { return 0; }
    std::uint64_t total = 0;
    for (std::uint32_t y = 0; y < 8; ++y) {
        for (std::uint32_t x = 0; x < 8; ++x) {
            total += pixels[(y * width) + x];
        }
    }
    const auto mean = total / 64u;
    PerceptualHash hash { 0 };
    for (std::uint32_t y = 0; y < 8; ++y) {
        for (std::uint32_t x = 0; x < 8; ++x) {
            if (pixels[(y * width) + x] > mean) {
                hash |= (1ULL << ((y * 8) + x));
            }
        }
    }
    return hash;
}

PerceptualHash computePHash(const std::uint8_t* pixels, std::uint32_t width, std::uint32_t height) {
    if (width < DctSize || height < DctSize) { return computeAHash(pixels, width, height); }

    std::array<double, DctSize * DctSize> matrix {};
    for (std::uint32_t y = 0; y < DctSize; ++y) {
        for (std::uint32_t x = 0; x < DctSize; ++x) {
            matrix[(y * DctSize) + x] = pixels[(y * width) + x];
        }
    }

    std::array<double, DctSize * DctSize> out {};
    const auto norm = std::numbers::pi / static_cast<double>(DctSize);
    for (std::uint32_t u = 0; u < HashSize; ++u) {
        for (std::uint32_t v = 0; v < HashSize; ++v) {
            double sum = 0.0;
            for (std::uint32_t y = 0; y < DctSize; ++y) {
                for (std::uint32_t x = 0; x < DctSize; ++x) {
                    sum += matrix[(y * DctSize) + x]
                           * std::cos(norm * (x + 0.5) * u)
                           * std::cos(norm * (y + 0.5) * v);
                }
            }
            out[(u * HashSize) + v] = sum;
        }
    }

    std::array<double, HashSize * HashSize> low {};
    for (std::uint32_t i = 0; i < HashSize * HashSize; ++i) {
        low[i] = out[i];
    }
    double sum = 0.0;
    for (std::uint32_t i = 1; i < low.size(); ++i) {
        sum += low[i];
    }
    const auto mean = sum / static_cast<double>(low.size() - 1);

    PerceptualHash hash { 0 };
    for (std::uint32_t i = 0; i < HashSize * HashSize; ++i) {
        if (low[i] > mean) {
            hash |= (1ULL << i);
        }
    }
    return hash;
}

}

std::uint32_t hammingDistance(PerceptualHash a, PerceptualHash b) noexcept {
    return static_cast<std::uint32_t>(std::popcount(a ^ b));
}

PerceptualHashes perceptualHashFromGrayscale(std::span<const std::uint8_t> pixels, std::uint32_t width, std::uint32_t height) noexcept {
    if (pixels.size() < static_cast<std::size_t>(width) * height) { return {}; }
    return PerceptualHashes {
        .phash = computePHash(pixels.data(), width, height),
        .dhash = computeDHash(pixels.data(), width, height),
        .whash = computeAHash(pixels.data(), width, height)
    };
}

Result<PerceptualHashes> perceptualHashImage(const std::filesystem::path&) {
    return fail(ErrorCode::NotSupported, "image decoding pipeline not yet wired");
}

}
