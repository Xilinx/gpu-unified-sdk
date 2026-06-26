/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <yaml-cpp/emitter.h>
#include <yaml-cpp/node/node.h>

#include <glm/glm.hpp>




namespace yaml {


template <glm::length_t L, typename T, glm::qualifier Q>
static void dumpVec(YAML::Emitter & yaml, const glm::vec<L, T, Q> & vec)
{
	yaml << YAML::Flow;
	yaml << YAML::BeginSeq;
	for (int i = 0; i < L; ++i) yaml << vec[i];
	yaml << YAML::EndSeq;
}


template <glm::length_t L, typename T, glm::qualifier Q>
static glm::vec<L, T, Q> loadVec(const YAML::Node & node)
{
	glm::vec<L, T, Q> vec;
	for (int i = 0; i < L; ++i) {
		vec[i] = node[i].as<float>();
	}
	return vec;
}


template <typename V>
static V loadVec(const YAML::Node & node)
{
	V vec;
	for (int i = 0; i < V::length(); ++i) {
		vec[i] = node[i].as<float>();
	}
	return vec;
}


template <glm::length_t L, typename T, glm::qualifier Q>
static void loadVec(const YAML::Node & node, glm::vec<L, T, Q> & vec)
{
	vec = loadVec<L, T, Q>(node);
}


} // yaml
