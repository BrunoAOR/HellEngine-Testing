#ifndef __H_MODULE_SCENE__
#define __H_MODULE_SCENE__

#include <vector>
#include "Brofiler/include/Brofiler.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "Module.h"
#include "SpaceQuadTree.h"
class ComponentCamera;
class GameObject;

class ModuleScene :
	public Module
{
public:

	ModuleScene();
	~ModuleScene();

	bool Init();
	bool CleanUp();
	UpdateStatus Update();

	void OnEditorHierarchy(float mainMenuBarHeight, bool* pOpen);
	void OnEditorInspector(float mainMenuBarHeight, bool* pOpen);

	void UnloadSceneFixedQuadTree();
	void GenerateSceneFixedQuadTree(float* minPoint, float* maxPoint);
	void GenerateSceneAdaptiveQuadTree();
	void ChangeStaticStatus(ComponentTransform* transform, bool isStatic);
	void TestCollisionChecks(float3 aabbMinPoint, float3 aabbMaxPoint, float3 spawnMinPoint, float3 spawnMaxPoint, int spawnedObjectsCount);
	void QuadTreeFrustumCulling(std::vector<GameObject*> &insideFrustum, Frustum frustum);
	bool UsingQuadTree();

	void SetActiveGameCamera(ComponentCamera* camera);
	ComponentCamera* GetActiveGameCamera() const;

	GameObject* CreateGameObject();
	void Destroy(GameObject* gameObject);
	std::vector<GameObject*> FindByName(const std::string& name, GameObject* gameObject = nullptr);

public:

	GameObject* root;
	struct
	{
		GameObject* selectedGameObject = nullptr;
	} editorInfo;

	static uint gameObjectsCount;

private:

	void FindAllSceneStaticGameObjects(std::vector<GameObject*>& staticGameObjects, GameObject* go = nullptr);
	template<typename T>
	void Intersects(std::vector<GameObject*>& intersectedGameObjects, const T& primitive);

private:

	ComponentCamera* activeGameCamera;
	SpaceQuadTree  quadTree;
	
};

template<typename T>
inline void ModuleScene::Intersects(std::vector<GameObject*>& intersectedGameObjects, const T& primitive)
{
	std::vector<GameObject*> staticGameObjects;
	FindAllSceneStaticGameObjects(staticGameObjects);
	std::vector<ComponentTransform*> staticTransforms;
	for (GameObject* go : staticGameObjects)
	{
		staticTransforms.push_back((ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM));
	}

	BROFILER_CATEGORY("Brute Force check start", Profiler::Color::Brown);
	int checksPerformed = 0;
	for (std::vector<ComponentTransform*>::iterator it = staticTransforms.begin(); it != staticTransforms.end(); ++it)
	{
		++checksPerformed;
		if (primitive.Intersects((*it)->GetBoundingBox()))
			intersectedGameObjects.push_back((*it)->gameObject);
	}
	BROFILER_CATEGORY("Brute Force check end", Profiler::Color::White);
	LOGGER("Brute force checks: %i", checksPerformed);
}

#endif // !__H_MODULE_SCENE__
