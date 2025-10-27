#pragma once

#include <spdlog/spdlog.h>

#define LOG_TRACE(message, ...) spdlog::trace(message, __VA_ARGS__)
#define LOG_DEBUG(message, ...) spdlog::debug(message, __VA_ARGS__)
#define LOG_INFO(message, ...) spdlog::info(message, __VA_ARGS__)
#define LOG_WARN(message, ...) spdlog::warn(message, __VA_ARGS__)
#define LOG_ERROR(message, ...) spdlog::error(message, __VA_ARGS__)
#define LOG_CRITICAL(message, ...) spdlog::critical(message, __VA_ARGS__)

#if defined(__GNUC__) || defined(__clang__)
#define LOG_TRACE_CALLED() LOG_TRACE("{} called", __PRETTY_FUNCTION__)
#elif defined(_MSC_VER)
#define LOG_TRACE_CALLED() LOG_TRACE("{} called", __FUNCSIG__)
#else
#define LOG_TRACE_CALLED() LOG_TRACE("{} called", __func__)
#endif
