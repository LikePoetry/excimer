#include "hzpch.h"
#include "Font.h"

#include "MSDFData.h"
#include "excimer/core/os/FileSystem.h"
#include "excimer/core/Buffer.h"
#include "excimer/graphics/rhi/Texture.h"
#include "excimer/core/VFS.h"
#include "excimer/core/Application.h"

#if __has_include(<filesystem>)
#include <filesystem>
#endif // __has_include(<filesystem>)


#include <stb/stb.h>

using namespace msdf_atlas;