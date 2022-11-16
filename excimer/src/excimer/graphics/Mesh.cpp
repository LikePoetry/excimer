#include "hzpch.h"
#include "Mesh.h"

#include "excimer/graphics/rhi/Renderer.h"

#include <ModelLoaders/meshoptimizer/src/meshoptimizer.h>


namespace Excimer
{
	namespace Graphics
	{
		Mesh::Mesh()
			:m_VertexBuffer(nullptr)
			, m_IndexBuffer(nullptr)
			, m_BoundingBox(nullptr)
			, m_Indices()
			, m_Vertices()
		{
		}

		Mesh::Mesh(const Mesh& mesh)
			: m_VertexBuffer(mesh.m_VertexBuffer)
			, m_IndexBuffer(mesh.m_IndexBuffer)
			, m_BoundingBox(mesh.m_BoundingBox)
			, m_Name(mesh.m_Name)
			, m_Material(mesh.m_Material)
			, m_Indices(mesh.m_Indices)
			, m_Vertices(mesh.m_Vertices)
		{
		}

		Mesh::Mesh(SharedPtr<VertexBuffer>& vertexBuffer, SharedPtr<IndexBuffer>& indexBuffer, const SharedPtr<Maths::BoundingBox>& boundingBox)
			: m_VertexBuffer(vertexBuffer)
			, m_IndexBuffer(indexBuffer)
			, m_BoundingBox(boundingBox)
			, m_Material(nullptr)
		{
		}

		Mesh::Mesh(const std::vector<uint32_t>& indices, const std::vector<Vertex>& vertices, float optimiseThreshold)
		{
			m_Indices = indices;
			m_Vertices = vertices;

			// int lod = 2;
			// float threshold = powf(0.7f, float(lod));

			size_t indexCount = indices.size();
			size_t target_index_count = size_t(indices.size() * optimiseThreshold);

			float target_error = 1e-3f;
			float* resultError = nullptr;

			auto newIndexCount = meshopt_simplify(m_Indices.data(), m_Indices.data(), m_Indices.size(), (const float*)(&m_Vertices[0]), m_Vertices.size(), sizeof(Graphics::Vertex), target_index_count, target_error, resultError);

			auto newVertexCount = meshopt_optimizeVertexFetch( // return vertices (not vertex attribute values)
				(m_Vertices.data()),
				(unsigned int*)(m_Indices.data()),
				newIndexCount, // total new indices (not faces)
				(m_Vertices.data()),
				(size_t)m_Vertices.size(), // total vertices (not vertex attribute values)
				sizeof(Graphics::Vertex)   // vertex stride
			);

			m_BoundingBox = CreateSharedPtr<Maths::BoundingBox>();

			for (auto& vertex : m_Vertices)
			{
				m_BoundingBox->Merge(vertex.Position);
			}

			m_IndexBuffer = SharedPtr<Graphics::IndexBuffer>(Graphics::IndexBuffer::Create(m_Indices.data(), (uint32_t)newIndexCount));

			m_VertexBuffer = SharedPtr<VertexBuffer>(VertexBuffer::Create(BufferUsage::STATIC));
			m_VertexBuffer->SetData((uint32_t)(sizeof(Graphics::Vertex) * newVertexCount), m_Vertices.data());
		}

		Mesh::~Mesh()
		{
		}

        void Mesh::GenerateNormals(Vertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount)
        {
            glm::vec3* normals = new glm::vec3[vertexCount];

            for (uint32_t i = 0; i < vertexCount; ++i)
            {
                normals[i] = glm::vec3();
            }

            if (indices)
            {
                for (uint32_t i = 0; i < indexCount; i += 3)
                {
                    const int a = indices[i];
                    const int b = indices[i + 1];
                    const int c = indices[i + 2];

                    const glm::vec3 _normal = glm::cross((vertices[b].Position - vertices[a].Position), (vertices[c].Position - vertices[a].Position));

                    normals[a] += _normal;
                    normals[b] += _normal;
                    normals[c] += _normal;
                }
            }
            else
            {
                // It's just a list of triangles, so generate face normals
                for (uint32_t i = 0; i < vertexCount; i += 3)
                {
                    glm::vec3& a = vertices[i].Position;
                    glm::vec3& b = vertices[i + 1].Position;
                    glm::vec3& c = vertices[i + 2].Position;

                    const glm::vec3 _normal = glm::cross(b - a, c - a);

                    normals[i] = _normal;
                    normals[i + 1] = _normal;
                    normals[i + 2] = _normal;
                }
            }

            for (uint32_t i = 0; i < vertexCount; ++i)
            {
                glm::normalize(normals[i]);
                vertices[i].Normal = normals[i];
            }

            delete[] normals;
        }

        void Mesh::GenerateTangents(Vertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t numIndices)
        {
            glm::vec3* tangents = new glm::vec3[vertexCount];

            for (uint32_t i = 0; i < vertexCount; ++i)
            {
                tangents[i] = glm::vec3();
            }

            if (indices)
            {
                for (uint32_t i = 0; i < numIndices; i += 3)
                {
                    int a = indices[i];
                    int b = indices[i + 1];
                    int c = indices[i + 2];

                    const glm::vec3 tangent = GenerateTangent(vertices[a].Position, vertices[b].Position, vertices[c].Position, vertices[a].TexCoords, vertices[b].TexCoords, vertices[c].TexCoords);

                    tangents[a] += tangent;
                    tangents[b] += tangent;
                    tangents[c] += tangent;
                }
            }
            else
            {
                for (uint32_t i = 0; i < vertexCount; i += 3)
                {
                    const glm::vec3 tangent = GenerateTangent(vertices[i].Position, vertices[i + 1].Position, vertices[i + 2].Position, vertices[i].TexCoords, vertices[i + 1].TexCoords,
                        vertices[i + 2].TexCoords);

                    tangents[i] += tangent;
                    tangents[i + 1] += tangent;
                    tangents[i + 2] += tangent;
                }
            }
            for (uint32_t i = 0; i < vertexCount; ++i)
            {
                glm::normalize(tangents[i]);
                vertices[i].Tangent = tangents[i];
            }

            delete[] tangents;
        }

        glm::vec3 Mesh::GenerateTangent(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec2& ta, const glm::vec2& tb, const glm::vec2& tc)
        {
            const glm::vec2 coord1 = tb - ta;
            const glm::vec2 coord2 = tc - ta;

            const glm::vec3 vertex1 = b - a;
            const glm::vec3 vertex2 = c - a;

            const glm::vec3 axis = glm::vec3(vertex1 * coord2.y - vertex2 * coord1.y);

            const float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

            return axis * factor;
        }

        glm::vec3* Mesh::GenerateNormals(uint32_t numVertices, glm::vec3* vertices, uint32_t* indices, uint32_t numIndices)
        {
            glm::vec3* normals = new glm::vec3[numVertices];

            for (uint32_t i = 0; i < numVertices; ++i)
            {
                normals[i] = glm::vec3();
            }

            if (indices)
            {
                for (uint32_t i = 0; i < numIndices; i += 3)
                {
                    const int a = indices[i];
                    const int b = indices[i + 1];
                    const int c = indices[i + 2];

                    const glm::vec3 _normal = glm::cross((vertices[b] - vertices[a]), (vertices[c] - vertices[a]));

                    normals[a] += _normal;
                    normals[b] += _normal;
                    normals[c] += _normal;
                }
            }
            else
            {
                // It's just a list of triangles, so generate face normals
                for (uint32_t i = 0; i < numVertices; i += 3)
                {
                    glm::vec3& a = vertices[i];
                    glm::vec3& b = vertices[i + 1];
                    glm::vec3& c = vertices[i + 2];

                    const glm::vec3 _normal = glm::cross(b - a, c - a);

                    normals[i] = _normal;
                    normals[i + 1] = _normal;
                    normals[i + 2] = _normal;
                }
            }

            for (uint32_t i = 0; i < numVertices; ++i)
            {
                glm::normalize(normals[i]);
            }

            return normals;
        }

        glm::vec3* Mesh::GenerateTangents(uint32_t numVertices, glm::vec3* vertices, uint32_t* indices, uint32_t numIndices, glm::vec2* texCoords)
        {
            if (!texCoords)
            {
                return nullptr;
            }

            glm::vec3* tangents = new glm::vec3[numVertices];

            for (uint32_t i = 0; i < numVertices; ++i)
            {
                tangents[i] = glm::vec3();
            }

            if (indices)
            {
                for (uint32_t i = 0; i < numIndices; i += 3)
                {
                    int a = indices[i];
                    int b = indices[i + 1];
                    int c = indices[i + 2];

                    const glm::vec3 tangent = GenerateTangent(vertices[a], vertices[b], vertices[c], texCoords[a], texCoords[b], texCoords[c]);

                    tangents[a] += tangent;
                    tangents[b] += tangent;
                    tangents[c] += tangent;
                }
            }
            else
            {
                for (uint32_t i = 0; i < numVertices; i += 3)
                {
                    const glm::vec3 tangent = GenerateTangent(vertices[i], vertices[i + 1], vertices[i + 2], texCoords[i], texCoords[i + 1],
                        texCoords[i + 2]);

                    tangents[i] += tangent;
                    tangents[i + 1] += tangent;
                    tangents[i + 2] += tangent;
                }
            }
            for (uint32_t i = 0; i < numVertices; ++i)
            {
                glm::normalize(tangents[i]);
            }

            return tangents;
        }
	}
}