#include "hzpch.h"
#include "Material.h"
#include "excimer/graphics/rhi/Shader.h"
#include "excimer/graphics/rhi/Texture.h"
#include "excimer/graphics/rhi/DescriptorSet.h"
#include "excimer/graphics/rhi/Pipeline.h"
#include "excimer/graphics/rhi/UniformBuffer.h"
#include "excimer/graphics/rhi/GraphicsContext.h"
#include "excimer/core/os/FileSystem.h"
#include "excimer/core/VFS.h"
#include "excimer/core/Application.h"

#include <imgui/imgui.h>

namespace Excimer::Graphics
{
	SharedPtr<Graphics::Texture2D> Material::s_DefaultTexture = nullptr;

	Material::Material(SharedPtr<Graphics::Shader>& shader, const MaterialProperties& properties, const PBRMataterialTextures& textures)
		: m_PBRMaterialTextures(textures)
		, m_Shader(shader)
	{
		EXCIMER_PROFILE_FUNCTION();

		m_Flags = 0;
		SetFlag(RenderFlags::DEPTHTEST);
		m_DescriptorSet = nullptr;
		m_MaterialProperties = new MaterialProperties();
		m_MaterialBufferSize = sizeof(MaterialProperties);

		m_DescriptorSet = nullptr;

		SetMaterialProperites(properties);
	}

	Material::Material()
		: m_Shader(nullptr)
	{
		EXCIMER_PROFILE_FUNCTION();

		m_Flags = 0;
		SetFlag(RenderFlags::DEPTHTEST);
		m_DescriptorSet = nullptr;
		m_MaterialProperties = new MaterialProperties();
		m_PBRMaterialTextures.albedo = nullptr;
		m_MaterialBufferSize = sizeof(MaterialProperties);
	}

	Material::~Material()
	{
		EXCIMER_PROFILE_FUNCTION();
		delete m_DescriptorSet;
		delete m_MaterialProperties;
	}

	void Material::SetTextures(const PBRMataterialTextures& textures)
	{
		EXCIMER_PROFILE_FUNCTION();
		m_PBRMaterialTextures.albedo = textures.albedo;
		m_PBRMaterialTextures.normal = textures.normal;
		m_PBRMaterialTextures.roughness = textures.roughness;
		m_PBRMaterialTextures.metallic = textures.metallic;
		m_PBRMaterialTextures.ao = textures.ao;
		m_PBRMaterialTextures.emissive = textures.emissive;
	}

	bool FileExists(const std::string& path)
	{
		EXCIMER_PROFILE_FUNCTION();

		std::string physicalPath;

		VFS::Get().ResolvePhysicalPath(path, physicalPath);
		return FileSystem::FileExists(physicalPath);
	}

    void Material::LoadPBRMaterial(const std::string& name, const std::string& path, const std::string& extension)
    {
        EXCIMER_PROFILE_FUNCTION();

        m_Name = name;
        m_PBRMaterialTextures = PBRMataterialTextures();
        auto params = Graphics::TextureDesc(Graphics::RHIFormat::R8G8B8A8_Unorm, Graphics::TextureFilter::LINEAR, Graphics::TextureFilter::LINEAR, Graphics::TextureWrap::CLAMP_TO_EDGE);

        auto filePath = path + "/" + name + "/albedo" + extension;

        if (FileExists(filePath))
            m_PBRMaterialTextures.albedo = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/albedo" + extension, params));

        filePath = path + "/" + name + "/normal" + extension;

        if (FileExists(filePath))
            m_PBRMaterialTextures.normal = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/normal" + extension, params));

        filePath = path + "/" + name + "/roughness" + extension;

        if (FileExists(filePath))
            m_PBRMaterialTextures.roughness = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/roughness" + extension, params));

        filePath = path + "/" + name + "/metallic" + extension;

        if (FileExists(filePath))
            m_PBRMaterialTextures.metallic = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/metallic" + extension, params));

        filePath = path + "/" + name + "/ao" + extension;

        if (FileExists(filePath))
            m_PBRMaterialTextures.ao = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/ao" + extension, params));

        filePath = path + "/" + name + "/emissive" + extension;

        if (FileExists(filePath))
            m_PBRMaterialTextures.emissive = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/emissive" + extension, params));
    }

    void Material::LoadMaterial(const std::string& name, const std::string& path)
    {
        EXCIMER_PROFILE_FUNCTION();

        m_Name = name;
        m_PBRMaterialTextures = PBRMataterialTextures();
        m_PBRMaterialTextures.albedo = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path));
        m_PBRMaterialTextures.normal = nullptr;
        m_PBRMaterialTextures.roughness = nullptr;
        m_PBRMaterialTextures.metallic = nullptr;
        m_PBRMaterialTextures.ao = nullptr;
        m_PBRMaterialTextures.emissive = nullptr;
    }

    void Material::UpdateMaterialPropertiesData()
    {
        if (!m_DescriptorSet)
            return;

        m_DescriptorSet->SetUniformBufferData("UniformMaterialData", *&m_MaterialProperties);
        m_DescriptorSet->Update();
    }

    void Material::SetMaterialProperites(const MaterialProperties& properties)
    {
        EXCIMER_PROFILE_FUNCTION();

        m_MaterialProperties->albedoColour = properties.albedoColour;
        m_MaterialProperties->metallic = properties.metallic;
        m_MaterialProperties->roughness = properties.roughness;
        m_MaterialProperties->emissive = properties.emissive;
        m_MaterialProperties->albedoMapFactor = properties.albedoMapFactor;
        m_MaterialProperties->normalMapFactor = properties.normalMapFactor;
        m_MaterialProperties->metallicMapFactor = properties.metallicMapFactor;
        m_MaterialProperties->roughnessMapFactor = properties.roughnessMapFactor;
        m_MaterialProperties->occlusionMapFactor = properties.occlusionMapFactor;
        m_MaterialProperties->emissiveMapFactor = properties.emissiveMapFactor;
        m_MaterialProperties->alphaCutoff = properties.alphaCutoff;
        m_MaterialProperties->workflow = properties.workflow;

        UpdateMaterialPropertiesData();
    }

    void Material::UpdateDescriptorSet()
    {
        if (m_PBRMaterialTextures.albedo != nullptr)
        {
            m_DescriptorSet->SetTexture("u_AlbedoMap", m_PBRMaterialTextures.albedo.get());
        }
        else
        {
            m_DescriptorSet->SetTexture("u_AlbedoMap", s_DefaultTexture.get());
            m_MaterialProperties->albedoMapFactor = 0.0f;
        }

        // if(pbr)
        {
            m_DescriptorSet->SetTexture("u_MetallicMap", m_PBRMaterialTextures.metallic ? m_PBRMaterialTextures.metallic.get() : s_DefaultTexture.get());

            if (!m_PBRMaterialTextures.metallic)
                m_MaterialProperties->metallicMapFactor = 0.0f;

            m_DescriptorSet->SetTexture("u_RoughnessMap", m_PBRMaterialTextures.roughness ? m_PBRMaterialTextures.roughness.get() : s_DefaultTexture.get());

            if (!m_PBRMaterialTextures.roughness)
                m_MaterialProperties->roughnessMapFactor = 0.0f;

            if (m_PBRMaterialTextures.normal != nullptr)
            {
                m_DescriptorSet->SetTexture("u_NormalMap", m_PBRMaterialTextures.normal.get());
            }
            else
            {
                m_DescriptorSet->SetTexture("u_NormalMap", s_DefaultTexture.get());
                m_MaterialProperties->normalMapFactor = 0.0f;
            }

            if (m_PBRMaterialTextures.ao != nullptr)
            {
                m_DescriptorSet->SetTexture("u_AOMap", m_PBRMaterialTextures.ao.get());
            }
            else
            {
                m_DescriptorSet->SetTexture("u_AOMap", s_DefaultTexture.get());
                m_MaterialProperties->occlusionMapFactor = 0.0f;
            }

            if (m_PBRMaterialTextures.emissive != nullptr)
            {
                m_DescriptorSet->SetTexture("u_EmissiveMap", m_PBRMaterialTextures.emissive.get());
            }
            else
            {
                m_DescriptorSet->SetTexture("u_EmissiveMap", s_DefaultTexture.get());
                m_MaterialProperties->emissiveMapFactor = 0.0f;
            }

            UpdateMaterialPropertiesData();
        }

        m_DescriptorSet->Update();
    }

    void Material::CreateDescriptorSet(int layoutID, bool pbr)
    {
        EXCIMER_PROFILE_FUNCTION();

        if (m_DescriptorSet)
            delete m_DescriptorSet;

        if (!m_Shader)
        {
            // If no shader then set it to the default pbr shader
            // TODO default to forward
            m_Shader = Application::Get().GetShaderLibrary()->GetResource("ForwardPBR");
        }

        Graphics::DescriptorDesc descriptorDesc;
        descriptorDesc.layoutIndex = layoutID;
        descriptorDesc.shader = m_Shader.get();

        m_DescriptorSet = Graphics::DescriptorSet::Create(descriptorDesc);

        UpdateDescriptorSet();
    }

    void Material::Bind()
    {
        EXCIMER_PROFILE_FUNCTION();

        if (m_DescriptorSet == nullptr || GetTexturesUpdated())
        {
            CreateDescriptorSet(1);
            SetTexturesUpdated(false);
        }

        m_DescriptorSet->Update();

        // UpdateDescriptorSet();
    }

    void Material::SetShader(const std::string& filePath)
    {
        m_Shader = Application::Get().GetShaderLibrary()->GetResource(filePath);
    }

    void Material::InitDefaultTexture()
    {
        EXCIMER_PROFILE_FUNCTION();

        uint32_t whiteTextureData = 0xffffffff;
        s_DefaultTexture = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromSource(1, 1, &whiteTextureData));
    }

    void Material::ReleaseDefaultTexture()
    {
        EXCIMER_PROFILE_FUNCTION();

        s_DefaultTexture.reset();
    }

    void Material::SetAlbedoTexture(const std::string& path)
    {
        EXCIMER_PROFILE_FUNCTION();

        auto tex = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(path, path));
        if (tex)
        {
            m_PBRMaterialTextures.albedo = tex;
            m_TexturesUpdated = true;
        }
    }

    void Material::SetNormalTexture(const std::string& path)
    {
        EXCIMER_PROFILE_FUNCTION();

        auto tex = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(path, path));
        if (tex)
        {
            m_PBRMaterialTextures.normal = tex;
            m_TexturesUpdated = true;
        }
    }

    void Material::SetRoughnessTexture(const std::string& path)
    {
        EXCIMER_PROFILE_FUNCTION();

        auto tex = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(path, path));
        if (tex)
        {
            m_PBRMaterialTextures.roughness = tex;
            m_TexturesUpdated = true;
        }
    }

    void Material::SetMetallicTexture(const std::string& path)
    {
        EXCIMER_PROFILE_FUNCTION();

        auto tex = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(path, path));
        if (tex)
        {
            m_PBRMaterialTextures.metallic = tex;
            m_TexturesUpdated = true;
        }
    }

    void Material::SetAOTexture(const std::string& path)
    {
        EXCIMER_PROFILE_FUNCTION();

        auto tex = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(path, path));
        if (tex)
        {
            m_PBRMaterialTextures.ao = tex;
            m_TexturesUpdated = true;
        }
    }

    void Material::SetEmissiveTexture(const std::string& path)
    {
        EXCIMER_PROFILE_FUNCTION();

        auto tex = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(path, path));
        if (tex)
        {
            m_PBRMaterialTextures.emissive = tex;
            m_TexturesUpdated = true;
        }
    }
}