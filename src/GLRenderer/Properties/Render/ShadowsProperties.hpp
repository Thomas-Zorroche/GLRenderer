#pragma once

#include <stdint.h>
#include <memory>

#include "ImBridge/Bridge.hpp"

namespace glrenderer {


	class ShadowsProperties
	{
	public:
		ShadowsProperties();
		~ShadowsProperties();

		// PCSS Settings
		uint32_t getBlockerSearchSamples() const;
		void setBlockerSearchSamples(uint32_t samples);
		unsigned int getBlockerSearchDistribution() const;

		uint32_t getPCFSamples() const;
		void setPCFSamples(uint32_t samples);
		unsigned int getPCFFilteringDistribution() const;

	public:
		const std::shared_ptr<ImBridge::Bridge>& getBridge() { return _bridge; }
		void onBlockerSearchSamplesUpdate(unsigned int id);

	private:
		void updateDistributionTexture(uint32_t samples, unsigned int  texture);
		
	private:
		std::shared_ptr<ImBridge::Bridge> _bridge;

		// PCSS Settings
		unsigned int _blockerSearchDistribution = 0; // Textures
		unsigned int  _PCFFilteringDistribution = 0;
		uint32_t _blockerSearchSamples = 16;		 // Samples
		uint32_t _PCFFilteringSamples = 16;
	};
}