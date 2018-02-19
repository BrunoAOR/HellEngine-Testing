#include "ImGui/imgui.h"
#include "MathGeoLib/src/Math/TransformOps.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "globals.h"
#include "openGL.h"


ComponentTransform::ComponentTransform(GameObject* owner) : Component(owner)
{
	type = ComponentType::TRANSFORM;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	position = float3(0.0f, 0.0f, 0.0f);
	scale = float3(1.0f, 1.0f, 1.0f);
	rotation = Quat::FromEulerXYZ(0.0f, 0.0f, 0.0f);
	UpdateBoundingBox();
	LOGGER("Component of type '%s'", GetString(type));
}

ComponentTransform::~ComponentTransform()
{
	App->scene->ChangeStaticStatus(this, false);
	LOGGER("Deleting Component of type '%s'", GetString(type));
}

void ComponentTransform::Update()
{
	if (drawBoundingBox) {
		if (boundingBox.IsFinite())
		{
			float currentLineWidth = 0;
			glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
			glLineWidth(3.f);
			float currentColor[4];
			glGetFloatv(GL_CURRENT_COLOR, currentColor);

			glBegin(GL_LINES);
			glColor3f(0.f, 1.f, 0.f);
			vec points[8];
			boundingBox.GetCornerPoints(points);

			// LEFT SIDE
			glVertex3fv(&points[0][0]);
			glVertex3fv(&points[1][0]);

			glVertex3fv(&points[0][0]);
			glVertex3fv(&points[2][0]);

			glVertex3fv(&points[2][0]);
			glVertex3fv(&points[3][0]);

			glVertex3fv(&points[3][0]);
			glVertex3fv(&points[1][0]);

			// BACK SIDE
			glVertex3fv(&points[0][0]);
			glVertex3fv(&points[4][0]);

			glVertex3fv(&points[2][0]);
			glVertex3fv(&points[6][0]);

			glVertex3fv(&points[4][0]);
			glVertex3fv(&points[6][0]);

			// RIGHT SIDE
			glVertex3fv(&points[6][0]);
			glVertex3fv(&points[7][0]);

			glVertex3fv(&points[4][0]);
			glVertex3fv(&points[5][0]);

			glVertex3fv(&points[7][0]);
			glVertex3fv(&points[5][0]);

			// FRONT SIDE
			glVertex3fv(&points[1][0]);
			glVertex3fv(&points[5][0]);

			glVertex3fv(&points[3][0]);
			glVertex3fv(&points[7][0]);

			glEnd();

			glLineWidth(currentLineWidth);
			glColor4f(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);
		}
	}

}

bool ComponentTransform::GetIsStatic()
{
	return isStatic;
}

void ComponentTransform::SetIsStatic(bool isStatic)
{
	this->isStatic = isStatic;
	App->scene->ChangeStaticStatus(this, isStatic);
}

float3 ComponentTransform::GetPosition()
{
	return position;
}

float3 ComponentTransform::GetScale()
{
	return scale;
}

float3 ComponentTransform::GetRotationRad()
{
	return rotation.ToEulerXYZ();
}

float3 ComponentTransform::GetRotationDeg()
{
	float3 rotationRad = GetRotationRad();

	return float3(RadToDeg(rotationRad.x), RadToDeg(rotationRad.y), RadToDeg(rotationRad.z));
}

AABB ComponentTransform::GetBoundingBox()
{
	return boundingBox;
}

void ComponentTransform::SetPosition(float x, float y, float z)
{
	if (!isStatic)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}
}

void ComponentTransform::SetScale(float x, float y, float z)
{
	if (!isStatic)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}
}

void ComponentTransform::SetRotationRad(float x, float y, float z)
{
	if (!isStatic)
	{
		rotation = Quat::FromEulerXYZ(x, y, z);
	}
}

void ComponentTransform::SetRotationDeg(float x, float y, float z)
{
	if (!isStatic)
	{
		SetRotationRad(DegToRad(x), DegToRad(y), DegToRad(z));
	}
}

void ComponentTransform::UpdateBoundingBox(ComponentMesh* mesh)
{
	if (!mesh)
		mesh = (ComponentMesh*) gameObject->GetComponent(ComponentType::MESH);

	if (mesh)
	{
		std::vector<float3> *cubeMeshVertexes;
		float3 *pointerCubeMeshVertexes;
		int numVertexes;

		cubeMeshVertexes = mesh->GetBuildVectorCubePoints();
		pointerCubeMeshVertexes = &cubeMeshVertexes->at(0);
		numVertexes = cubeMeshVertexes->size();

		boundingBox.SetNegativeInfinity();
		boundingBox.Enclose(pointerCubeMeshVertexes, numVertexes);
		boundingBox.TransformBB(GetModelMatrix4x4().Transposed());
	}

	/* To make iterative */
	std::vector<GameObject*> children = gameObject->GetChildren();

	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
		GameObject* go = *it;
		ComponentTransform* t = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
		if (t != nullptr)
			t->UpdateBoundingBox();
	}
}

float* ComponentTransform::GetModelMatrix()
{
	return GetModelMatrix4x4().ptr();
}

void ComponentTransform::SetParent(ComponentTransform* newParent)
{
	/* Calculate the new local model matrix */
	float4x4 newParentWorldMatrix;
	if (newParent != nullptr)
		newParentWorldMatrix = newParent->GetModelMatrix4x4();
	else
		newParentWorldMatrix = float4x4::identity;

	float4x4 inverseNewParentWorldMatrix = newParentWorldMatrix;
	inverseNewParentWorldMatrix.Inverse();

	float4x4 currentWorldMatrix = GetModelMatrix4x4();
	float4x4 newLocalModelMatrix = currentWorldMatrix * inverseNewParentWorldMatrix;

	DecomposeMatrix(newLocalModelMatrix, position, rotation, scale);
	float3 rotEuler = rotation.ToEulerXYZ();
	rotationDeg[0] = RadToDeg(rotEuler.x);
	rotationDeg[1] = RadToDeg(rotEuler.y);
	rotationDeg[2] = RadToDeg(rotEuler.z);
}

void ComponentTransform::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		float positionFP[3] = { position.x, position.y, position.z };
		float scaleFP[3] = { scale.x, scale.y, scale.z };

		if (ImGui::DragFloat3("Position", positionFP, 0.03f))
		{
			if (!isStatic)
			{
				SetPosition(positionFP[0], positionFP[1], positionFP[2]);
				UpdateBoundingBox();
			}
		}
		if (ImGui::DragFloat3("Rotation", rotationDeg, 0.3f))
		{
			if (!isStatic)
			{
				SetRotationDeg(rotationDeg[0], rotationDeg[1], rotationDeg[2]);
				UpdateBoundingBox();
			}
		}
		if (ImGui::DragFloat3("Scale", scaleFP, 0.1f, 0.1f, 1000.0f))
		{
			if (!isStatic)
			{
				if (scaleFP[0] >= 0 && scaleFP[1] >= 0 && scaleFP[2] > 0)
				{
					SetScale(scaleFP[0], scaleFP[1], scaleFP[2]);
					UpdateBoundingBox();
				}
			}
		}
		
		if (DEBUG_MODE)
			ImGui::Checkbox("Draw Bounding Box", &drawBoundingBox);
		else if (drawBoundingBox)
			drawBoundingBox = false;

		if (ImGui::Checkbox("Static", &isStatic))
		{
			SetIsStatic(isStatic);
		}
	}
}

int ComponentTransform::MaxCountInGameObject()
{
	return 1;
}

float4x4& ComponentTransform::GetModelMatrix4x4()
{
	UpdateLocalModelMatrix();

	worldModelMatrix = localModelMatrix;

	GameObject* parent = gameObject->GetParent();
	while (parent)
	{
		ComponentTransform* parentTransform = (ComponentTransform*)parent->GetComponent(ComponentType::TRANSFORM);
		if (!parentTransform)
			break;

		worldModelMatrix = worldModelMatrix * parentTransform->GetModelMatrix4x4();
		parent = parent->GetParent();
	}

	return worldModelMatrix;
}

float4x4& ComponentTransform::UpdateLocalModelMatrix()
{
	float4x4 scaleMatrix = float4x4::Scale(scale.x, scale.y, scale.z);
	float4x4 rotationMatrix = float4x4::FromQuat(rotation);
	float4x4 translationMatrix = float4x4::Translate(position.x, position.y, position.z);
	memcpy_s(localModelMatrix.ptr(), sizeof(float) * 16, (translationMatrix * rotationMatrix * scaleMatrix).Transposed().ptr(), sizeof(float) * 16);
	return localModelMatrix;
}

void ComponentTransform::ApplyGuizmo(const float4x4& modelMatrix, const float3& newPosition, const float newRotation[], const float3& newScale) 
{
    if (!isStatic) 
    {
        localModelMatrix = modelMatrix;

        position = newPosition;

        rotationDeg[0] = newRotation[0];
        rotationDeg[1] = newRotation[1];
        rotationDeg[2] = newRotation[2];

        float3 radAngles = DegToRad((float3)newRotation);

        rotation = Quat::FromEulerXYZ(radAngles.x, radAngles.y, radAngles.z);

        scale = newScale;
    }
}