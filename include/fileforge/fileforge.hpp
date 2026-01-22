#pragma once

/**
 * @file fileforge.hpp
 * @brief Main include header for FileForge library
 *
 * FileForge - Forging Order from Chaos
 * A high-performance, cross-platform file organization utility
 *
 * @author CrAvila
 * @copyright MIT License
 */

#include "types.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "scanner.hpp"
#include "categorizer.hpp"
#include "renamer.hpp"
#include "organizer.hpp"

namespace fileforge {

/**
 * @brief Get the FileForge version string
 * @return Version string in format "major.minor.patch"
 */
constexpr const char* version() noexcept {
    return "1.0.0";
}

/**
 * @brief Get the FileForge version as integers
 */
constexpr int versionMajor() noexcept { return 1; }
constexpr int versionMinor() noexcept { return 0; }
constexpr int versionPatch() noexcept { return 0; }

} // namespace fileforge
