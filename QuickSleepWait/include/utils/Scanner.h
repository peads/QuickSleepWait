#pragma once
#include <Windows.h>
#include <Psapi.h>

 #ifdef USE_AVX
struct Pattern {
	static constexpr auto width = 32ui32;

	std::vector<__m256i> m_vec;
	std::vector<uint32_t> m_mask;
	__m256i m_lookahead_vec;

	std::string m_pattern;
	size_t m_first_offset;

	bool m_isReloff = false;
	size_t m_reloffStart = 0;
	size_t m_reloffLength = 0;

	// ReSharper disable once CppNonExplicitConvertingConstructor
	Pattern(const char* pattern) {
		m_pattern = pattern;

		uint8_t buf[width]{};
		size_t bufIdx{};
		uint32_t bufMask{ 0 };

		bool setFirst = false;

		const auto toByte = [](const char c) -> uint8_t {
			if (c >= '0' && c <= '9') {
				return c - '0';
			}

			if (c >= 'A' && c <= 'F') {
				return c - 'A' + 10;
			}

			return 0;
			};

		for (size_t i = 0; i < m_pattern.size(); i++) {
			if (pattern[i] == ' ') {
				continue;
			}

			// Not a wildcard
			if (pattern[i] != '?') {
				bufMask |= 1ui64 << bufIdx;

				if (i < m_pattern.size() - 1) {
					const auto byte = (toByte(pattern[i]) << 4) | toByte(pattern[i + 1]);

					if (!setFirst) {
						m_lookahead_vec = _mm256_set1_epi8(static_cast<int8_t>(byte));
						m_first_offset = bufIdx;
						setFirst = true;
					}

					buf[bufIdx] = byte;
					i++;
				}
			}

			// Flush
			if (++bufIdx == width) {
				this->m_vec.emplace_back(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(&buf[0])));
				this->m_mask.push_back(bufMask);

				memset(&buf[0], 0, width);
				bufMask = {};
				bufIdx = 0;
			}
		}

		// Flush
		if (bufIdx != 0) {
			this->m_vec.emplace_back(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(&buf[0])));
			this->m_mask.push_back(bufMask);
		}
	}

	Pattern(const char* pattern, const size_t reloffStart, const size_t reloffLen) : Pattern(pattern) {
		m_isReloff = true;
		m_reloffStart = reloffStart;
		m_reloffLength = reloffLen;
	}

	// Returns pointer if found + minimum number of bytes until pattern should be scanned again
	std::tuple<uint8_t*, size_t> check(const void* _addr) const {
		const auto addr = const_cast<uint8_t*>(static_cast<const uint8_t*>(_addr));

		// Default to skip `width` bytes
		// If starting byte is found in scan, this will be reduced to start there
		size_t nextStart = width;

		for (size_t i = 0; i < m_mask.size(); i++) {
			const auto offset = (i * width);
			const auto ptr = reinterpret_cast<const __m256i*>(addr + offset);
			const auto mem = _mm256_loadu_si256(ptr);

			// Get a bitmask of bytes that don't match
			const auto cmp = _mm256_cmpeq_epi8(m_vec[i], mem);
			const auto res = (_mm256_movemask_epi8(cmp) ^ UINT32_MAX) & m_mask[i];

			if (i == 0) {
				// Scan bytes for any matching first byte in pattern
				// Count starting from LSB to first '1' bit
				// Will be 32 if not found, otherwise next start value
				constexpr auto mask = (UINT32_MAX - 1);
				const auto lookahead_cmp = _mm256_cmpeq_epi8(m_lookahead_vec, mem);
				const auto lookahead_result = _mm256_movemask_epi8(lookahead_cmp) & mask;

				if (lookahead_result > 0) {
					const auto bit = _tzcnt_u32(lookahead_result);

					// < 32 means it was found
					// In case of wildcards in first bytes of pattern,
					// need to adjust next starting pos based on first actual byte
					// * but only if nextStart > first real byte offset
					// Pattern: ?? ?? AA
					// Mem: xx xx xx xx xx xx AA
					// Next start would be 6, but pattern starts with two wildcards
					// Adjust nextStart to 4
					if (bit < width && bit > m_first_offset) {
						nextStart = bit - m_first_offset;
					}
				}
			}

			// Found a mismatch
			if (res > 0) {
				return { nullptr, nextStart };
			}
		}

		// Pattern is a for a RIP relative offset
		// AB CD ? ? ? ? EF GH
		// Where ? ? ? ? points to an RIP relative address
		if (m_isReloff) {
			const auto relOff = *reinterpret_cast<int32_t*>(addr + m_reloffStart);
			const auto rip = (addr + m_reloffStart + m_reloffLength);
			const auto ptr = rip + relOff;
			return { ptr, nextStart };
		}

		// Matches at start address
		return { addr , nextStart };
	}
};
 #else
 struct Pattern {
 	std::vector<uint8_t> bytes{};
 	std::vector<bool> mask{};

 	std::string m_pattern;

 	size_t m_first{};
 	size_t m_first_offset;

 	bool m_isReloff = false;
 	size_t m_reloffStart = 0;
 	size_t m_reloffLength = 0;

 	// ReSharper disable once CppNonExplicitConvertingConstructor
 	Pattern(const char* pattern) {
 		m_pattern = pattern;

 		bool setFirst = false;
 		const auto toByte = [](const char c) -> uint8_t {
 			if (c >= '0' && c <= '9') {
 				return c - '0';
 			}

 			if (c >= 'A' && c <= 'F') {
 				return c - 'A' + 10;
 			}

 			return 0;
 			};

 		size_t bufIdx = 0;
 		for (size_t i = 0; i < m_pattern.size(); i++) {
 			if (pattern[i] == ' ') {
 				continue;
 			}

 			// Not a wildcard
 			if (pattern[i] != '?') {
 				if (i < m_pattern.size() - 1) {
 					const auto byte = (toByte(pattern[i]) << 4) | toByte(pattern[i + 1]);

 					if (!setFirst) {
 						m_first_offset = bufIdx;
 						m_first = byte;
 						setFirst = true;
 					}

 					bytes.push_back(byte);
 					mask.push_back(false);

 					i++;
 				}
 			}
 			else {
 				bytes.push_back(0);
 				mask.push_back(true);
 			}

 			bufIdx++;
 		}
 	}

 	Pattern(const char* pattern, const size_t reloffStart, const size_t reloffLen) : Pattern(pattern) {
 		m_isReloff = true;
 		m_reloffStart = reloffStart;
 		m_reloffLength = reloffLen;
 	}

 	// Returns pointer if found + minimum number of bytes until pattern should be scanned again
 	std::tuple<uint8_t*, size_t> search(const void* _addr) const {
 		const auto addr = const_cast<uint8_t*>(static_cast<const uint8_t*>(_addr));

 		// Default to skip `width` bytes
 		// If starting byte is found in scan, this will be reduced to start there
 		bool setNextStart = false;
 		size_t nextStart = 1;

 		for (size_t i = 0; i < bytes.size(); i++) {
 			const auto mem = addr[i];

 			if (mem == m_first) {
 				if (i > 0) {
 				}
 			}

 			// Found next potential pattern starting point
 			if (!setNextStart && mem == m_first && i > 0 && i > m_first_offset) {
 				nextStart = i - m_first_offset;
 				setNextStart = true;
 			}

 			if (!mask[i] && mem != bytes[i]) {
 				return { nullptr, nextStart };
 			}
 		}

 		// Pattern is a for a RIP relative offset
 		// AB CD ? ? ? ? EF GH
 		// Where ? ? ? ? points to an RIP relative address
 		if (m_isReloff) {
 			const auto relOff = *reinterpret_cast<int32_t*>(addr + m_reloffStart);
 			const auto rip = (addr + m_reloffStart + m_reloffLength);
 			const auto ptr = rip + relOff;
 			return { ptr, nextStart };
 		}

 		// Matches at start address
 		return { addr, nextStart };
 	}
 };
 #endif

namespace Scanner {
	using handler_t = std::function<void(uint8_t*)>;
	using pattern_info_t = std::tuple<Pattern, handler_t>;
	inline std::vector<pattern_info_t> handlers{};

	template <typename T = uint8_t*>
	void Add(const Pattern& pat, T* ptr) {
		handlers.push_back({
			pat, [=](uint8_t* addr) {
				*ptr = reinterpret_cast<T>(addr);
			}
		});
	}

	template <typename T = uint8_t*>
	void Add(const Pattern& pat, const handler_t& callback) {
		handlers.emplace_back(pat, callback);
	}

	inline void Scan() {
		constexpr auto module = "OblivionRemastered-WinGDK-Shipping.exe";
		const auto hmod = GetModuleHandleA(module);
		if (!hmod) {
			constexpr auto emsg = "GetModuleHandleA returned nullptr.\n";
			puts(emsg);
			throw std::runtime_error(std::string(emsg));
		}

		MODULEINFO info{};
		if (!GetModuleInformation(GetCurrentProcess(), hmod, &info, sizeof(info))) {
			constexpr auto emsg = "GetModuleInformation returned FALSE.\n";
			puts(emsg);
			throw std::runtime_error(std::string(emsg));
		}

		std::vector<std::tuple<size_t, pattern_info_t*>> nextIndices{};
		for (auto& handler : handlers) {
			nextIndices.emplace_back(0, &handler);
		}

		const auto base = static_cast<uint8_t*>(info.lpBaseOfDll);
		const auto size = static_cast<size_t>(info.SizeOfImage);

		for (auto curAddr = base; curAddr <= (base + size - 64); curAddr++) {
			for (auto& [next, ph] : nextIndices) {
				// Already found
				if (next == UINT64_MAX) {
					continue;
				}

				if (next == 0) {
					const auto& [pattern, callback] = *ph;
					const auto [addr, skip] = pattern.search(curAddr);

					if (addr) {
						// Skip in future
						next = UINT64_MAX;
						callback(addr);
					}
					else {
						next = skip - 1;
					}
				}
				else {
					next--;
				}
			}
		}

		for (auto& [next, ph] : nextIndices) {
			// Already found
			if (next != UINT64_MAX) {
				const auto& [pattern, callback] = *ph;
			}
		}
	}
};
