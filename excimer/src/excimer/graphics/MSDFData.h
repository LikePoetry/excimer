#pragma once
#undef INFINITE
#include <msdf-atlas-gen.h>
#include <vector>

namespace Excimer
{
	struct MSDFData
	{
		msdf_atlas::FontGeometry FontGeometry;
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
	};
}