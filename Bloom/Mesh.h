/*!
 * \file Mesh.h
 *
 * \author LYG
 * \date ���� 2018
 *
 * 
 */
#pragma once
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Common.h"
#include "GameDef.h"
#include "Material.h"
#include "Renderable.h"
#include <map>
class MeshManager;
class Mesh : public Renderable
{
	friend class MeshManager;
	friend class RenderGroupManager;
	friend class UIElement;
public:
	Material* GetMaterial() const;
	void SetMaterial(Material* Mat);

	virtual void Render(Matrix4& WorldTransform) override;
protected:
	Mesh(std::string Name);
	~Mesh();

	bool Initialise(void* VertexBuffer, int VertexElementSize, int VertexCount, void* IndexBuffer, int IndexCount, D3D11_PRIMITIVE_TOPOLOGY Primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void RenderMesh(Matrix4& WorldTransform, bool IsPerspective = true) const;
	void RenderMesh(XMMATRIX& XMTransform) const;
private:
	Material* mMaterial;
	DataBuffer* mVertexBuffer;
	DataBuffer* mIndexBuffer;
	UINT mVertexCount;
	UINT mVertexElementSize;
	UINT mIndexCount;
	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveType;
	DXGI_FORMAT mIndexFormat;
};

class MeshManager
{
	friend class Scene;
public:
	Mesh* CreateMesh(std::string Name, void* VertexBuffer, int VertexElementSize, int VertexCount, void* IndexBuffer, int IndexCount);
	Mesh* CreateQuad(std::string Name, Vector3* Vertex = nullptr, Vector2* TexCoord = nullptr);
	Mesh* CreateLine(std::string Name, Vector3* Vertex);
	Mesh* CreateLineList(std::string Name, Vector3* Vertex, int VertexCount);
	Mesh* CreateSphere(std::string Name, int Col, int Row, float Radius);
	Mesh* CreateBox(std::string Name, const Vector3& V);

	Mesh* CreateMesh(void* VertexBuffer, int VertexElementSize, int VertexCount, void* IndexBuffer, int IndexCount);
	Mesh* CreateQuad(Vector3* Vertex = nullptr, Vector2* TexCoord = nullptr);
	Mesh* CreateLine(Vector3* Vertex);
	Mesh* CreateLineList(Vector3* Vertex, int VertexCount);
	Mesh* CreateSphere(int Col, int Row, float Radius);
	Mesh* CreateBox(const Vector3& V);

	Mesh* GetMeshByName(std::string Name);

	bool DestroyMesh(Mesh* M);
	bool DestroyMesh(std::string Name);
protected:
	MeshManager();
	~MeshManager();

private:
	std::map<std::string, Mesh*> mMeshArray;

	GET_AUTO_NAME("Mesh");
	CLEAR_MAP(mMeshArray);
};
