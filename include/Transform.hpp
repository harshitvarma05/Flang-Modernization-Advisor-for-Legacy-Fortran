#pragma once

#include <filesystem>
#include <vector>

std::vector<std::filesystem::path> applySafeTransformations(const std::filesystem::path &source,
                                                            const std::filesystem::path &outputDir);
