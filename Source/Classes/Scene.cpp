#include "precomp.h"
#include "Scene.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Ellipsoid.h"
#include "PointLight.h"

namespace rtc
{
    void SphereFlake(int a_Recursion, std::vector<BaseObject*>& a_List, Sphere* a_Parent)
    {
        Material mat = Material(Util::color(0.0f, 0.2f, 0.7f), MaterialType::LAMBERTIAN);
        Util::vec3 center = a_Parent->m_Center;
        float radius = a_Parent->m_Radius;
        float childRadius = radius / 2.8f;
        if (a_Recursion > 0)
        {
            a_List.push_back(new Sphere(Util::point3(center.x, center.y + radius + childRadius, center.z), childRadius, mat));
            SphereFlake(a_Recursion - 1, a_List, (Sphere*)a_List[a_List.size() - 1]);

            a_List.push_back(new Sphere(Util::point3(center.x, center.y - (radius + childRadius), center.z), childRadius, mat));
            SphereFlake(a_Recursion - 1, a_List, (Sphere*)a_List[a_List.size() - 1]);

            a_List.push_back(new Sphere(Util::point3(center.x + radius + childRadius, center.y, center.z), childRadius, mat));
            SphereFlake(a_Recursion - 1, a_List, (Sphere*)a_List[a_List.size() - 1]);

            a_List.push_back(new Sphere(Util::point3(center.x - (radius + childRadius), center.y, center.z), childRadius, mat));
            SphereFlake(a_Recursion - 1, a_List, (Sphere*)a_List[a_List.size() - 1]);

            a_List.push_back(new Sphere(Util::point3(center.x, center.y, center.z + radius + childRadius), childRadius, mat));
            SphereFlake(a_Recursion - 1, a_List, (Sphere*)a_List[a_List.size() - 1]);

            a_List.push_back(new Sphere(Util::point3(center.x, center.y, center.z - (radius + childRadius)), childRadius, mat));
            SphereFlake(a_Recursion - 1, a_List, (Sphere*)a_List[a_List.size() - 1]);
        }
    }

	void Scene::LoadScene(int a_SceneId)
	{
        m_LightList.clear();
        m_SceneList.clear();
        
        Material material_ground = Material(Util::color(0.5f, 0.5f, 0.5f), MaterialType::LAMBERTIAN, 0.f, rtc::TextureType::CHECKER);
        Material material_test = Material(Util::color(0.0f, 0.2f, 0.7f), MaterialType::LAMBERTIAN);

        Material material_left = Material(Util::color(0.8f, 0.8f, 0.8f), MaterialType::METAL, 0);
        Material material_right = Material(Util::color(0.8f, 0.6f, 0.2f), MaterialType::METAL, 0.5f);

        switch (a_SceneId)
        {
        case 0:
        {
            m_LightList.push_back(std::make_unique<PointLight>(Util::vec3(0, 2, -5), Util::color(1, 1, 1), 1.f));
            m_SceneList.push_back(new Sphere(Util::point3(0.f, 0.f, 0.f), 1.0f, Material(Util::color(1.f, 0.807f, 0.705f), MaterialType::LAMBERTIAN)));
            m_SceneList.push_back(new Sphere(Util::point3(2.f, 0.f, 0.f), 1.0f, Material(Util::color(1.f, 0.807f, 0.705f), MaterialType::LAMBERTIAN)));
            m_Camera = Camera(50, (0.f, 0.f, 0.f), (0.f, 0.f, 5.f), 10.f, 0.1f);
            break;
        }
        case 1: // 80000 spheres
            for (int x = -100; x < 183; x++)
                for (int z = -100; z < 183; z++)
                    m_SceneList.push_back(new Sphere(Util::point3((float)x, 0, (float)z), 0.5f, material_test));

            m_SceneList.push_back(new Sphere(Util::point3(0.f, -1000.5f, 0), 1000.f, material_ground));
            m_LightList.push_back(std::make_unique<PointLight>(Util::vec3(10, 5, 10), Util::color(1, 1, 1), 1.f));
            m_Camera = Camera(90, Util::point3(0, 0, 1), Util::point3(13, 13, 7), 10, 0.1f);
            break;
        case 2: // Elipsoid 
            m_SceneList.push_back(new Ellipsoid(Util::point3(2.f, 1.f, -1), Util::vec3(.3f, .6f, .3f), Material(Util::color(1.f, 1.f, 1.f), MaterialType::DIELECTRIC, 1.5f)));
            m_SceneList.push_back(new Sphere(Util::point3(0, 1.f, -1), 1.f, Material(Util::color(1.f, 1.f, 1.f), MaterialType::DIELECTRIC, 1.5f)));
            m_SceneList.push_back(new Sphere(Util::point3(0, 1.f, -20), 1.f, Material(Util::color(1.f, 1.f, 1.f), MaterialType::LAMBERTIAN)));

            m_SceneList.push_back(new Sphere(Util::point3(0.f, -1000.5f, 0), 1000.f, material_ground));

            m_LightList.push_back(std::make_unique<PointLight>(Util::vec3(10, 5, 10), Util::color(1, 1, 1), 2.f));
            m_Camera = Camera(50, Util::point3(0, 0, -1), Util::point3(0, 1, 3), 10, 0.1f);
            break;
        case 3:
        {
            Material mat = Material(Util::color(1.f, 1.f, 1.f), MaterialType::DIELECTRIC, 1.5f);
            Material mat1 = Material(Util::color(1.f, 1.f, 1.f), MaterialType::METAL, 0);

            m_SceneList.push_back(new Sphere(Util::point3(0.f, 0.f, 0.f),0.5f, mat));
            m_SceneList.push_back(new Sphere(Util::point3(-1.f, 0.f, -1.f),0.5f, mat1));
            m_SceneList.push_back(new Sphere(Util::point3(1.f, 1.f, -2.f),0.5f, material_right));
            m_SceneList.push_back(new Ellipsoid(Util::point3(1.f, 0.f, -1.f), Util::vec3(.3f, .6f, .3f), material_right));
            m_SceneList.push_back(new Sphere(Util::point3(0.f, -100.5f, 0.f),100.f, mat1));


            m_SceneList.push_back(new Sphere(Util::point3(0.f, 0.f, -3.f),0.5f, material_test));
            m_SceneList.push_back(new Sphere(Util::point3(2.f, 0.f, -3.f),0.5f, material_test));

            m_LightList.push_back(std::make_unique<PointLight>(Util::vec3(0, 5, 0), Util::color(1, 1, 1), 1.f));
            m_Camera = Camera(50, Util::point3(0, 0, -1), Util::point3(3, 3, 5), 10.f, 0.01f);
            break;
        }
        case 4:
        {
            Material mat = Material(Util::color(1.f, 1.f, 1.f), MaterialType::DIELECTRIC, 1.5f);
            Material mat1 = Material(Util::color(1.f, 1.f, 1.f), MaterialType::METAL, 0);
            Sphere* s = new Sphere(Util::point3(0.f, 0.f, -3.f), 0.5f, material_test);
            m_SceneList.push_back(s);
            SphereFlake(5, m_SceneList, s);

            m_LightList.push_back(std::make_unique<PointLight>(Util::vec3(0, 5, 0), Util::color(1, 1, 1), 1.f));
            m_Camera = Camera(50, Util::point3(0, 0, -3), Util::point3(3, 3, 5), 10.f, 0.01f);
            break;
        }
        case 5:
        {
            Material mat = Material(Util::color(1.f, 1.f, 1.f), MaterialType::DIELECTRIC, 1.5f);
            Material mat1 = Material(Util::color(1.f, 1.f, 1.f), MaterialType::METAL, 0);
            for (int i = 0; i > -1000; i--)
            {
                Sphere* s = new Sphere(Util::point3(0.f, 0.f, static_cast<float>(i)), 0.5f, material_test);
                m_SceneList.push_back(s);
            }
            m_LightList.push_back(std::make_unique<PointLight>(Util::vec3(0, 5, 0), Util::color(1, 1, 1), 1.f));
            m_Camera = Camera(50, Util::point3(0, 0, -1), Util::point3(0, 0, 3), 10.f, 0.01f);
            break;
        }
        case 6:
        {
            for (int x = 0; x < 128; x++)
                for (int z = 0; z < 128; z++)
                    m_SceneList.push_back(new Ellipsoid(Util::point3((float)x, 0, (float)z), 0.5f, material_test));

            m_LightList.push_back(std::make_unique<PointLight>(Util::vec3(10, 5, 10), Util::color(1, 1, 1), 1.f));
            m_Camera = Camera(90, Util::point3(64, 0, 64), Util::point3(13, 13, 7), 10, 0.1f);
            break;
        }
        case 7:
        {
            for (int z = 0; z < 256; z++)
                m_SceneList.push_back(new Sphere(Util::point3((float)0, (float)0, (float)z), 0.5f, material_test));

            m_LightList.push_back(std::make_unique<PointLight>(Util::vec3(0, 0, -3), Util::color(1, 1, 1), 1.f));
            m_Camera = Camera(90, Util::point3(0, 0, 1), Util::point3(0, 0, -3), 10, 0.1f);
            break;
        }
        default:
            m_SceneID = 0;
            m_LightList.push_back(std::make_unique<PointLight>(Util::vec3(0, 2, -5), Util::color(1, 1, 1), 1.f));
            m_SceneList.push_back(new Sphere(Util::point3(0.f, 0.f, 0.f), 1.0f, Material(Util::color(1.f, 0.807f, 0.705f), MaterialType::LAMBERTIAN)));
            m_SceneList.push_back(new Sphere(Util::point3(2.f, 0.f, 0.f), 1.0f, Material(Util::color(1.f, 0.807f, 0.705f), MaterialType::LAMBERTIAN)));
            m_Camera = Camera(50, (0.f, 0.f, 0.f), (0.f, 0.f, 5.f), 10.f, 0.1f);
            break;
        }
        m_TreeNode = Util::BVHNode(m_SceneList, (uint32_t)0, (uint32_t)m_SceneList.size(), 1.f);
	}
    bool Scene::Hit(const Util::Ray& r, float t_min, float t_max, HitData& rec)
    {
        HitData hitData;
        bool hitObject = false;
        auto closest = t_max;

        for (const auto& object : m_SceneList) {
            if (object->HitObject(r, t_min, closest, hitData)) {
                hitObject = true;
                closest = hitData.t;
                rec = hitData;
            }
        }

        return hitObject;
    }
}
