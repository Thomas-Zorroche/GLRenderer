#include "ShadowsProperties.hpp"

#include "../../Math/PoissonGenerator.hpp"

#include <glad/glad.h>
#include <iostream>
#include <memory>
#include <vector>

namespace glrenderer {

	ShadowsProperties::ShadowsProperties()
	{
		glGenTextures(1, &_blockerSearchDistribution);
		glGenTextures(1, &_PCFFilteringDistribution);

		updateDistributionTexture(_blockerSearchSamples, _blockerSearchDistribution);
		updateDistributionTexture(_PCFFilteringSamples, _PCFFilteringDistribution);

		_bridge = std::make_shared<ImBridge::Bridge>();
		_bridge->addCombo(
			"Blocker Search Samples", 
			"16\0 32\0 64\0 128\0\0", 
			4, 
			[this](unsigned int id) {this->onBlockerSearchSamplesUpdate(id); }, 
			"Adjust Blocker Search Samples (Shadow)");
	}

	ShadowsProperties::~ShadowsProperties()
	{
		glDeleteTextures(1, &_blockerSearchDistribution);
		glDeleteTextures(1, &_PCFFilteringDistribution);
	}

	// ImBridge Callbacks
	void ShadowsProperties::onBlockerSearchSamplesUpdate(unsigned int id)
	{	
		switch (id)
		{
		case 0: updateDistributionTexture(16, _blockerSearchDistribution); break;
		case 1: updateDistributionTexture(32, _blockerSearchDistribution); break;
		case 2: updateDistributionTexture(64, _blockerSearchDistribution); break;
		case 3: updateDistributionTexture(128, _blockerSearchDistribution); break;
		}
	}


	// PCSS Settings
	uint32_t ShadowsProperties::getBlockerSearchSamples() const 
	{ 
		return _blockerSearchSamples; 
	}
	void ShadowsProperties::setBlockerSearchSamples(uint32_t samples) 
	{
		_blockerSearchSamples = samples;
		updateDistributionTexture(samples, _blockerSearchDistribution);
	}
	unsigned int ShadowsProperties::getBlockerSearchDistribution() const 
	{ 
		return _blockerSearchDistribution; 
	}

	uint32_t ShadowsProperties::getPCFSamples() const 
	{ 
		return _PCFFilteringSamples; 
	}
	void ShadowsProperties::setPCFSamples(uint32_t samples) 
	{
		_PCFFilteringSamples = samples;
		updateDistributionTexture(samples, _PCFFilteringDistribution);
	}
	unsigned int ShadowsProperties::getPCFFilteringDistribution() const 
	{ 
		return _PCFFilteringDistribution; 
	}

	void ShadowsProperties::updateDistributionTexture(uint32_t samples, unsigned int texture)
	{
		auto points = PoissonGenerator::generatePoissonPoints(samples, PoissonGenerator::DefaultPRNG());
		size_t attempts = 0;
		while (points.size() < samples && attempts < 10)
		{
			PoissonGenerator::generatePoissonPoints(samples, PoissonGenerator::DefaultPRNG());
			attempts++;
		}

		if (attempts == 10)
		{
			std::cout << "Could not generate poisson distribution with " << samples << " samples." << std::endl;
			return;
		}

		std::cout << "Number of points: " << points.size() << std::endl;
		std::cout << "Attempts: " << attempts << std::endl;

		std::vector<float> data(samples * 2);
		for (auto i = 0, j = 0; i < samples; i++, j += 2)
		{
			auto& point = points[i];
			data[j] = point.x;
			data[j + 1] = point.y;
		}

		//int idTex = texture == _blockerSearchDistribution ? 1 : 2;
		//glActiveTexture(GL_TEXTURE0 + idTex);
		glBindTexture(GL_TEXTURE_1D, texture);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RG, samples, 0, GL_RG, GL_FLOAT, &data[0]);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

}