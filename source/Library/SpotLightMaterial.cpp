#include "SpotLightMaterial.h"
#include "GameException.h"
#include "Mesh.h"

namespace Rendering
{
	RTTI_DEFINITIONS(SpotLightMaterial)

		SpotLightMaterial::SpotLightMaterial()
		: Material("main10"),
		MATERIAL_VARIABLE_INITIALIZATION(WorldViewProjection),
		MATERIAL_VARIABLE_INITIALIZATION(World),
		MATERIAL_VARIABLE_INITIALIZATION(SpecularColor),
		MATERIAL_VARIABLE_INITIALIZATION(SpecularPower),
		MATERIAL_VARIABLE_INITIALIZATION(AmbientColor),
		MATERIAL_VARIABLE_INITIALIZATION(LightColor),
		MATERIAL_VARIABLE_INITIALIZATION(LightPosition),
		MATERIAL_VARIABLE_INITIALIZATION(LightLookAt),
		MATERIAL_VARIABLE_INITIALIZATION(LightRadius),
		MATERIAL_VARIABLE_INITIALIZATION(SpotLightInnerAngle),
		MATERIAL_VARIABLE_INITIALIZATION(SpotLightOuterAngle),
		MATERIAL_VARIABLE_INITIALIZATION(CameraPosition),
		MATERIAL_VARIABLE_INITIALIZATION(ColorTexture)
	{
	}

	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, WorldViewProjection)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, World)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, SpecularColor)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, SpecularPower)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, AmbientColor)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, LightColor)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, LightPosition)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, LightLookAt)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, LightRadius)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, SpotLightInnerAngle)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, SpotLightOuterAngle)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, CameraPosition)
	MATERIAL_VARIABLE_DEFINITION(SpotLightMaterial, ColorTexture)

	void SpotLightMaterial::Initialize(Effect& effect)
	{
		Material::Initialize(effect);

		MATERIAL_VARIABLE_RETRIEVE(WorldViewProjection)
		MATERIAL_VARIABLE_RETRIEVE(World)
		MATERIAL_VARIABLE_RETRIEVE(SpecularColor)
		MATERIAL_VARIABLE_RETRIEVE(SpecularPower)
		MATERIAL_VARIABLE_RETRIEVE(AmbientColor)
		MATERIAL_VARIABLE_RETRIEVE(LightColor)
		MATERIAL_VARIABLE_RETRIEVE(LightPosition)
		MATERIAL_VARIABLE_RETRIEVE(LightLookAt)
		MATERIAL_VARIABLE_RETRIEVE(LightRadius)
		MATERIAL_VARIABLE_RETRIEVE(SpotLightInnerAngle)
		MATERIAL_VARIABLE_RETRIEVE(SpotLightOuterAngle)
		MATERIAL_VARIABLE_RETRIEVE(CameraPosition)
		MATERIAL_VARIABLE_RETRIEVE(ColorTexture)

			D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		CreateInputLayout("main10", "p0", inputElementDescriptions, ARRAYSIZE(inputElementDescriptions));
	}

	void SpotLightMaterial::CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const
	{
		const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		std::vector<XMFLOAT3>* textureCoordinates = mesh.TextureCoordinates().at(0);
		assert(textureCoordinates->size() == sourceVertices.size());
		const std::vector<XMFLOAT3>& normals = mesh.Normals();
		assert(textureCoordinates->size() == sourceVertices.size());

		std::vector<SpotLightMaterialVertex> vertices;
		vertices.reserve(sourceVertices.size());
		for (UINT i = 0; i < sourceVertices.size(); i++)
		{
			XMFLOAT3 position = sourceVertices.at(i);
			XMFLOAT3 uv = textureCoordinates->at(i);
			XMFLOAT3 normal = normals.at(i);
			vertices.push_back(SpotLightMaterialVertex(XMFLOAT4(position.x, position.y, position.z, 1.0f), XMFLOAT2(uv.x, uv.y), normal));
		}

		CreateVertexBuffer(device, &vertices[0], vertices.size(), vertexBuffer);
	}

	void SpotLightMaterial::CreateVertexBuffer(ID3D11Device* device, SpotLightMaterialVertex* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const
	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = VertexSize() * vertexCount;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData;
		ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
		vertexSubResourceData.pSysMem = vertices;
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, vertexBuffer)))
		{
			throw GameException("ID3D11Device::CreateBuffer() failed.");
		}
	}

	UINT SpotLightMaterial::VertexSize() const
	{
		return sizeof(SpotLightMaterialVertex);
	}
}