#include "hzpch.h"
#include "excimer/maths/Frustum.h"
#include "excimer/maths/BoundingBox.h"
#include "excimer/maths/BoundingSphere.h"
#include "excimer/maths/Rect.h"
#include "excimer/maths/Ray.h"

#include <glm/gtc/matrix_transform.hpp>

#include "excimer/core/Profiler.h"

namespace Excimer
{
    namespace Maths
    {
        Frustum::Frustum()
        {
            Define(glm::mat4(1.0f));
        }

        Frustum::Frustum(const glm::mat4& transform)
        {
            Define(transform);
        }

        Frustum::Frustum(const glm::mat4& projection, const glm::mat4& view)
        {
            glm::mat4 m = projection * view;
            Define(m);
        }

        Frustum::~Frustum()
        {
        }

        void Frustum::Define(const glm::mat4& projection, const glm::mat4& view)
        {
            EXCIMER_PROFILE_FUNCTION();
            glm::mat4 m = projection * view;
            Define(m);
        }

        void Frustum::Transform(const glm::mat4& transform)
        {
            EXCIMER_PROFILE_FUNCTION();
            for(int i = 0; i < 6; i++)
            {
                m_Planes[i].Transform(transform);
            }

            for(int i = 0; i < 6; i++)
            {
                m_Planes[i].Normalise();
            }

            CalculateVertices(transform);
        }

        void Frustum::Define(const glm::mat4& transform)
        {
            EXCIMER_PROFILE_FUNCTION();
            auto& m               = transform;
            m_Planes[PLANE_LEFT]  = Plane(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0], m[3][3] + m[3][0]);
            m_Planes[PLANE_RIGHT] = Plane(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0], m[3][3] - m[3][0]);
            m_Planes[PLANE_DOWN]  = Plane(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1], m[3][3] + m[3][1]);
            m_Planes[PLANE_UP]    = Plane(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1], m[3][3] - m[3][1]);
            m_Planes[PLANE_NEAR]  = Plane(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2], m[3][3] + m[3][2]);
            m_Planes[PLANE_FAR]   = Plane(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2], m[3][3] - m[3][2]);

            for(int i = 0; i < 6; i++)
            {
                m_Planes[i].Normalise();
            }

            CalculateVertices(transform);
        }

        void Frustum::DefineOrtho(float scale, float aspectRatio, float near, float far, const glm::mat4& viewMatrix)
        {
            EXCIMER_PROFILE_FUNCTION();
            glm::mat4 m = glm::ortho(-scale * aspectRatio, scale * aspectRatio, -scale, scale, near, far);
            m           = m * viewMatrix;
            Define(m);
        }
        void Frustum::Define(float fov, float aspectRatio, float near, float far, const glm::mat4& viewMatrix)
        {
            EXCIMER_PROFILE_FUNCTION();
            float tangent = tan(fov * 0.5f);
            float height  = near * tangent;
            float width   = height * aspectRatio;

            glm::mat4 m = glm::frustum(-width, width, -height, height, near, far);
            m           = m * viewMatrix;
            Define(m);
        }

        bool Frustum::IsInside(const glm::vec3& point) const
        {
            EXCIMER_PROFILE_FUNCTION();
            for(int i = 0; i < 6; i++)
            {
                if(m_Planes[i].Distance(point) < 0.0f)
                {
                    return false;
                }
            }

            return true;
        }

        bool Frustum::IsInside(const BoundingSphere& sphere) const
        {
            EXCIMER_PROFILE_FUNCTION();
            for(int i = 0; i < 6; i++)
            {
                if(m_Planes[i].Distance(sphere.GetCenter()) < -sphere.GetRadius())
                {
                    return false;
                }
            }

            return true;
        }

        bool Frustum::IsInside(const BoundingBox& box) const
        {
            EXCIMER_PROFILE_FUNCTION();
            for(int i = 0; i < 6; i++)
            {
                glm::vec3 p = box.Min(), n = box.Max();
                glm::vec3 N = m_Planes[i].Normal();
                if(N.x >= 0)
                {
                    p.x = box.Max().x;
                    n.x = box.Min().x;
                }
                if(N.y >= 0)
                {
                    p.y = box.Max().y;
                    n.y = box.Min().y;
                }
                if(N.z >= 0)
                {
                    p.z = box.Max().z;
                    n.z = box.Min().z;
                }

                if(m_Planes[i].Distance(p) < 0)
                {
                    return false;
                }
            }
            return true;
        }

        bool Frustum::IsInside(const Rect& rect) const
        {
            EXCIMER_PROFILE_FUNCTION();
            for(int i = 0; i < 6; i++)
            {
                glm::vec3 N = m_Planes[i].Normal();
                if(N.x >= 0)
                {
                    if(m_Planes[i].Distance(glm::vec3(rect.GetPosition(), 0)) < 0)
                    {
                        return false;
                    }
                }
                else
                {
                    if(m_Planes[i].Distance(glm::vec3(rect.GetPosition().x + rect.GetSize().x, rect.GetPosition().y, 0)) < 0)
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        bool Frustum::IsInside(const Ray& ray) const
        {
            EXCIMER_PROFILE_FUNCTION();
            for(int i = 0; i < 6; i++)
            {
                if(m_Planes[i].Distance(ray.Origin) < 0.0f)
                {
                    return false;
                }
            }

            return true;
        }

        bool Frustum::IsInside(const Plane& plane) const
        {
            EXCIMER_PROFILE_FUNCTION();
            for(int i = 0; i < 6; i++)
            {
                if(m_Planes[i].Distance(plane.Normal()) < 0.0f)
                {
                    return false;
                }
            }

            return true;
        }

        const Plane& Frustum::GetPlane(FrustumPlane plane) const
        {
            EXCIMER_PROFILE_FUNCTION();
            return m_Planes[plane];
        }

        glm::vec3* Frustum::GetVerticies()
        {
            EXCIMER_PROFILE_FUNCTION();
            return m_Verticies;
        }

        void Frustum::CalculateVertices(const glm::mat4& transform)
        {
            EXCIMER_PROFILE_FUNCTION();
            bool zerotoOne         = false;
            bool leftHand          = true;
            glm::mat4 transformInv = glm::inverse(transform);
            m_Verticies[0]         = glm::vec4(-1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
            m_Verticies[1]         = glm::vec4(1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
            m_Verticies[2]         = glm::vec4(1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
            m_Verticies[3]         = glm::vec4(-1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);

            m_Verticies[4] = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
            m_Verticies[5] = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
            m_Verticies[6] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            m_Verticies[7] = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);

            glm::vec4 temp;
            for(int i = 0; i < 8; i++)
            {
                temp           = transformInv * glm::vec4(m_Verticies[i], 1.0f);
                m_Verticies[i] = temp / temp.w;
            }
        }
    }
}
