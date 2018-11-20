//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <SPIRV/GlslangToSpv.h>

namespace
{
	void init_resources(TBuiltInResource& resources)
	{
		resources.maxLights = 32;
		resources.maxClipPlanes = 6;
		resources.maxTextureUnits = 32;
		resources.maxTextureCoords = 32;
		resources.maxVertexAttribs = 64;
		resources.maxVertexUniformComponents = 4096;
		resources.maxVaryingFloats = 64;
		resources.maxVertexTextureImageUnits = 32;
		resources.maxCombinedTextureImageUnits = 80;
		resources.maxTextureImageUnits = 32;
		resources.maxFragmentUniformComponents = 4096;
		resources.maxDrawBuffers = 32;
		resources.maxVertexUniformVectors = 128;
		resources.maxVaryingVectors = 8;
		resources.maxFragmentUniformVectors = 16;
		resources.maxVertexOutputVectors = 16;
		resources.maxFragmentInputVectors = 15;
		resources.minProgramTexelOffset = -8;
		resources.maxProgramTexelOffset = 7;
		resources.maxClipDistances = 8;
		resources.maxComputeWorkGroupCountX = 65535;
		resources.maxComputeWorkGroupCountY = 65535;
		resources.maxComputeWorkGroupCountZ = 65535;
		resources.maxComputeWorkGroupSizeX = 1024;
		resources.maxComputeWorkGroupSizeY = 1024;
		resources.maxComputeWorkGroupSizeZ = 64;
		resources.maxComputeUniformComponents = 1024;
		resources.maxComputeTextureImageUnits = 16;
		resources.maxComputeImageUniforms = 8;
		resources.maxComputeAtomicCounters = 8;
		resources.maxComputeAtomicCounterBuffers = 1;
		resources.maxVaryingComponents = 60;
		resources.maxVertexOutputComponents = 64;
		resources.maxGeometryInputComponents = 64;
		resources.maxGeometryOutputComponents = 128;
		resources.maxFragmentInputComponents = 128;
		resources.maxImageUnits = 8;
		resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
		resources.maxCombinedShaderOutputResources = 8;
		resources.maxImageSamples = 0;
		resources.maxVertexImageUniforms = 0;
		resources.maxTessControlImageUniforms = 0;
		resources.maxTessEvaluationImageUniforms = 0;
		resources.maxGeometryImageUniforms = 0;
		resources.maxFragmentImageUniforms = 8;
		resources.maxCombinedImageUniforms = 8;
		resources.maxGeometryTextureImageUnits = 16;
		resources.maxGeometryOutputVertices = 256;
		resources.maxGeometryTotalOutputComponents = 1024;
		resources.maxGeometryUniformComponents = 1024;
		resources.maxGeometryVaryingComponents = 64;
		resources.maxTessControlInputComponents = 128;
		resources.maxTessControlOutputComponents = 128;
		resources.maxTessControlTextureImageUnits = 16;
		resources.maxTessControlUniformComponents = 1024;
		resources.maxTessControlTotalOutputComponents = 4096;
		resources.maxTessEvaluationInputComponents = 128;
		resources.maxTessEvaluationOutputComponents = 128;
		resources.maxTessEvaluationTextureImageUnits = 16;
		resources.maxTessEvaluationUniformComponents = 1024;
		resources.maxTessPatchComponents = 120;
		resources.maxPatchVertices = 32;
		resources.maxTessGenLevel = 64;
		resources.maxViewports = 16;
		resources.maxVertexAtomicCounters = 0;
		resources.maxTessControlAtomicCounters = 0;
		resources.maxTessEvaluationAtomicCounters = 0;
		resources.maxGeometryAtomicCounters = 0;
		resources.maxFragmentAtomicCounters = 8;
		resources.maxCombinedAtomicCounters = 8;
		resources.maxAtomicCounterBindings = 1;
		resources.maxVertexAtomicCounterBuffers = 0;
		resources.maxTessControlAtomicCounterBuffers = 0;
		resources.maxTessEvaluationAtomicCounterBuffers = 0;
		resources.maxGeometryAtomicCounterBuffers = 0;
		resources.maxFragmentAtomicCounterBuffers = 1;
		resources.maxCombinedAtomicCounterBuffers = 1;
		resources.maxAtomicCounterBufferSize = 16384;
		resources.maxTransformFeedbackBuffers = 4;
		resources.maxTransformFeedbackInterleavedComponents = 64;
		resources.maxCullDistances = 8;
		resources.maxCombinedClipAndCullDistances = 8;
		resources.maxSamples = 4;
		resources.limits.nonInductiveForLoops = 1;
		resources.limits.whileLoops = 1;
		resources.limits.doWhileLoops = 1;
		resources.limits.generalUniformIndexing = 1;
		resources.limits.generalAttributeMatrixVectorIndexing = 1;
		resources.limits.generalVaryingIndexing = 1;
		resources.limits.generalSamplerIndexing = 1;
		resources.limits.generalVariableIndexing = 1;
		resources.limits.generalConstantMatrixVectorIndexing = 1;
	}

	int print_usage()
	{
		std::cerr << "Usage:\n"
				  << "  yglslc --vertex INPUT OUTPUT\n"
				  << "  yglslc --fragment INPUT OUTPUT\n";
		return 1;
	}
}

int main(int argc, char** argv)
{
	if (argc != 4)
		return print_usage();

	EShLanguage language;
	if (!std::strcmp(argv[1], "--vertex"))
		language = EShLangVertex;
	else if (!std::strcmp(argv[1], "--fragment"))
		language = EShLangFragment;
	else
		return print_usage();

	std::string glsl;
	{
		std::ifstream input{ argv[2], std::ios::binary | std::ios::in };
		if (!input.is_open())
		{
			std::cerr << "ERROR: Unable to open \"" << argv[2] << "\"\n";
			return 1;
		}
		const auto input_size = input.seekg(0, std::ios::end).tellg();
		input.seekg(0, std::ios_base::beg);
		glsl.resize(static_cast<size_t>(input_size));
		if (!input.read(glsl.data(), input_size))
		{
			std::cerr << "ERROR: Unable to read \"" << argv[2] << "\"\n";
			return 1;
		}
	}

	std::vector<uint32_t> spirv;
	{
		glslang::InitializeProcess();

		std::array<const char*, 1> source{ glsl.data() };

		glslang::TShader shader{ language };
		shader.setStrings(source.data(), source.size());

		const auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);

		TBuiltInResource resources;
		::init_resources(resources);
		if (!shader.parse(&resources, 100, false, messages))
		{
			std::cerr << "ERROR: Unable to process \"" << argv[2] << "\"\n"
					  << shader.getInfoLog() << '\n';
			return 1;
		}

		glslang::TProgram program;
		program.addShader(&shader);
		if (!program.link(messages))
		{
			std::cerr << "ERROR: Unable to process \"" << argv[2] << "\"\n"
					  << shader.getInfoLog() << '\n';
			return 1;
		}

		glslang::GlslangToSpv(*program.getIntermediate(language), spirv);

		glslang::FinalizeProcess();
	}

	std::ofstream output{ argv[3], std::ios::out | std::ios::trunc };
	if (!output.is_open())
	{
		std::cerr << "ERROR: Unable to open \"" << argv[3] << "\"\n";
		return 1;
	}

	if (!spirv.empty())
	{
		output << spirv[0];
		for (size_t i = 1; i < spirv.size(); ++i)
			output << ',' << spirv[i];
	}
	output << '\n';
}
