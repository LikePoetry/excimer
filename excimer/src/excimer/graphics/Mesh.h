#pragma once
#include "excimer/graphics/rhi/IndexBuffer.h"
#include "excimer/graphics/rhi/VertexBuffer.h"
#include "excimer/graphics/rhi/CommandBuffer.h"
#include "excimer/graphics/rhi/DescriptorSet.h"

#include "excimer/maths/Maths.h"
#include "excimer/graphics/Material.h"


#include <glm/gtx/hash.hpp>
#include <array>

namespace Excimer
{
	namespace Graphics
	{
		class Texture2D;

		struct EXCIMER_EXPORT BasicVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoords;
		};

		struct EXCIMER_EXPORT Vertex
		{
			Vertex()
				:Position(glm::vec3(0.0f))
				, Colours(glm::vec4(0.0f))
				, TexCoords(glm::vec2(0.0f))
				, Normal(glm::vec3(0.0f))
				, Tangent(glm::vec3(0.0f))
			{

			}

			glm::vec3 Position;
			glm::vec4 Colours;
			glm::vec2 TexCoords;
			glm::vec3 Normal;
			glm::vec3 Tangent;

			bool operator==(const Vertex& other) const
			{
				return Position == other.Position && TexCoords == other.TexCoords && Colours == other.Colours && Normal == other.Normal && Tangent == other.Tangent;
			}
		};

		class EXCIMER_EXPORT Mesh
		{
		public:
			Mesh();
			Mesh(const Mesh& mesh);
			Mesh(const std::vector<uint32_t>& indices, const std::vector<Vertex>& vertices, float optimiseThreshold = 0.95f);
			Mesh(SharedPtr<VertexBuffer>& vertexBuffer, SharedPtr<IndexBuffer>& indexBuffer, const SharedPtr<Maths::BoundingBox>& boundingBox);

			virtual ~Mesh();

			const SharedPtr<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }
			const SharedPtr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
			const SharedPtr<Material>& GetMaterial() const { return m_Material; }
			const SharedPtr<Maths::BoundingBox>& GetBoundingBox() const { return m_BoundingBox; }

			void SetMaterial(const SharedPtr<Material>& material) { m_Material = material; }
			bool& GetActive() { return m_Active; }
			void SetName(const std::string& name) { m_Name = name; }

			static void GenerateNormals(Vertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount);
			static void GenerateTangents(Vertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount);

		protected:
			static glm::vec3 GenerateTangent(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec2& ta, const glm::vec2& tb, const glm::vec2& tc);
			static glm::vec3* GenerateNormals(uint32_t numVertices, glm::vec3* vertices, uint32_t* indices, uint32_t numIndices);
			static glm::vec3* GenerateTangents(uint32_t numVertices, glm::vec3* vertices, uint32_t* indices, uint32_t numIndices, glm::vec2* texCoords);

			SharedPtr<VertexBuffer> m_VertexBuffer;
			SharedPtr<IndexBuffer> m_IndexBuffer;
			SharedPtr<Material> m_Material;
			SharedPtr<Maths::BoundingBox> m_BoundingBox;

			std::string m_Name;

			bool m_Active = true;
			std::vector<uint32_t> m_Indices;
			std::vector<Vertex> m_Vertices;
		};
	}
}

namespace std
{
	template <>
	struct hash<Excimer::Graphics::Vertex>
	{
		size_t operator()(Excimer::Graphics::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec2>()(vertex.TexCoords) << 1) ^ (hash<glm::vec4>()(vertex.Colours) << 1) ^ (hash<glm::vec3>()(vertex.Normal) << 1) ^ (hash<glm::vec3>()(vertex.Tangent) << 1)));
		}
	};
}

