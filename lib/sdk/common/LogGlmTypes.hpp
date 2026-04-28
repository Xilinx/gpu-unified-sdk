
#pragma once

#include <glm/glm.hpp>

#include "Log.hpp"




template <glm::length_t L, typename T, glm::qualifier Q>
struct Log::Write<glm::vec<L, T, Q>> {
	static void write(std::ostream & s, const glm::vec<L, T, Q> & v) noexcept
	{
		s << "vec" << L << "{";
		for (int i = 0; i < L; ++i) {
			if (i != 0) s << " ";
			s << v[i];
		}
		s << "}";
	}
};
