#include "Mesh.h"
#include "Model.h"
#include "Material.h"
#include "Bone.h"
#include "Game.h"
#include "GameException.h"
#include "scene.h"

namespace Library
{
	Mesh::Mesh(Model& model, aiMesh& mesh)
		: mModel(model), mMaterial(nullptr), mName(mesh.mName.C_Str()), mVertices(), mNormals(), mTangents(), mBiNormals(), mTextureCoordinates(), mVertexColors(),
		mFaceCount(0), mIndices(), mBoneWeights(), mVertexBuffer(), mIndexBuffer()
	{
		mMaterial = mModel.Materials().at(mesh.mMaterialIndex);

		// Vertices
		mVertices.reserve(mesh.mNumVertices);
		for (UINT i = 0; i < mesh.mNumVertices; i++)
		{
			mVertices.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mVertices[i])));
		}

		// Normals
		if (mesh.HasNormals())
		{
			mNormals.reserve(mesh.mNumVertices);
			for (UINT i = 0; i < mesh.mNumVertices; i++)
			{
				mNormals.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mNormals[i])));
			}
		}

		// Tangents and Binormals
		if (mesh.HasTangentsAndBitangents())
		{
			mTangents.reserve(mesh.mNumVertices);
			mBiNormals.reserve(mesh.mNumVertices);
			for (UINT i = 0; i < mesh.mNumVertices; i++)
			{
				mTangents.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mTangents[i])));
				mBiNormals.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mBitangents[i])));
			}
		}

		// Texture Coordinates
		UINT uvChannelCount = mesh.GetNumUVChannels();
		for (UINT i = 0; i < uvChannelCount; i++)
		{
			std::vector<XMFLOAT3>* textureCoordinates = new std::vector<XMFLOAT3>();
			textureCoordinates->reserve(mesh.mNumVertices);
			mTextureCoordinates.push_back(textureCoordinates);

			aiVector3D* aiTextureCoordinates = mesh.mTextureCoords[i];
			for (UINT j = 0; j < mesh.mNumVertices; j++)
			{
				textureCoordinates->push_back(XMFLOAT3(reinterpret_cast<const float*>(&aiTextureCoordinates[j])));
			}
		}

		// Vertex Colors
		UINT colorChannelCount = mesh.GetNumColorChannels();
		for (UINT i = 0; i < colorChannelCount; i++)
		{
			std::vector<XMFLOAT4>* vertexColors = new std::vector<XMFLOAT4>();
			vertexColors->reserve(mesh.mNumVertices);
			mVertexColors.push_back(vertexColors);

			aiColor4D* aiVertexColors = mesh.mColors[i];
			for (UINT j = 0; j < mesh.mNumVertices; j++)
			{
				vertexColors->push_back(XMFLOAT4(reinterpret_cast<const float*>(&aiVertexColors[j])));
			}
		}

		// Faces (note: could pre-reserve if we limit primitive types)
		if (mesh.HasFaces())
		{
			mFaceCount = mesh.mNumFaces;
			for (UINT i = 0; i < mFaceCount; i++)
			{
				aiFace* face = &mesh.mFaces[i];

				for (UINT j = 0; j < face->mNumIndices; j++)
				{
					mIndices.push_back(face->mIndices[j]);
				}
			}
		}

		// Bones
		if (mesh.HasBones())
		{
			mBoneWeights.resize(mesh.mNumVertices);

			for (UINT i = 0; i < mesh.mNumBones; i++)
			{
				aiBone* meshBone = mesh.mBones[i];

				// Look up the bone in the model's hierarchy, or add it if not found.
				UINT boneIndex = 0U;
				std::string boneName = meshBone->mName.C_Str();
				auto boneMappingIterator = mModel.mBoneIndexMapping.find(boneName);
				if (boneMappingIterator != mModel.mBoneIndexMapping.end())
				{
					boneIndex = boneMappingIterator->second;
				}
				else
				{
					boneIndex = mModel.mBones.size();
					XMMATRIX offsetMatrix = XMLoadFloat4x4(&(XMFLOAT4X4(reinterpret_cast<const float*>(meshBone->mOffsetMatrix[0]))));
					XMFLOAT4X4 offset;
					XMStoreFloat4x4(&offset, XMMatrixTranspose(offsetMatrix));

					Bone* modelBone = new Bone(boneName, boneIndex, offset);
					mModel.mBones.push_back(modelBone);
					mModel.mBoneIndexMapping[boneName] = boneIndex;
				}

				for (UINT i = 0; i < meshBone->mNumWeights; i++)
				{
					aiVertexWeight vertexWeight = meshBone->mWeights[i];
					mBoneWeights[vertexWeight.mVertexId].AddWeight(vertexWeight.mWeight, boneIndex);
				}
			}
		}
	}

	Mesh::~Mesh()
	{
		for (std::vector<XMFLOAT3>* textureCoordinates : mTextureCoordinates)
		{
			delete textureCoordinates;
		}

		for (std::vector<XMFLOAT4>* vertexColors : mVertexColors)
		{
			delete vertexColors;
		}

		mVertexBuffer.ReleaseBuffer();
		mIndexBuffer.ReleaseBuffer();
	}

	Model& Mesh::GetModel()
	{
		return mModel;
	}

	ModelMaterial* Mesh::GetMaterial()
	{
		return mMaterial;
	}

	const std::string& Mesh::Name() const
	{
		return mName;
	}

	const std::vector<XMFLOAT3>& Mesh::Vertices() const
	{
		return mVertices;
	}

	const std::vector<XMFLOAT3>& Mesh::Normals() const
	{
		return mNormals;
	}

	const std::vector<XMFLOAT3>& Mesh::Tangents() const
	{
		return mTangents;
	}

	const std::vector<XMFLOAT3>& Mesh::BiNormals() const
	{
		return mBiNormals;
	}

	const std::vector<std::vector<XMFLOAT3>*>& Mesh::TextureCoordinates() const
	{
		return mTextureCoordinates;
	}

	const std::vector<std::vector<XMFLOAT4>*>& Mesh::VertexColors() const
	{
		return mVertexColors;
	}

	UINT Mesh::FaceCount() const
	{
		return mFaceCount;
	}

	const std::vector<UINT>& Mesh::Indices() const
	{
		return mIndices;
	}

	const std::vector<BoneVertexWeights>& Mesh::BoneWeights() const
	{
		return mBoneWeights;
	}

	BufferContainer& Mesh::VertexBuffer()
	{
		return mVertexBuffer;
	}

	BufferContainer& Mesh::IndexBuffer()
	{
		return mIndexBuffer;
	}

	bool Mesh::HasCachedVertexBuffer() const
	{
		Mesh* mesh = const_cast<Mesh*>(this);

		return mesh->mVertexBuffer.Buffer() != nullptr;
	}

	bool Mesh::HasCachedIndexBuffer() const
	{
		Mesh* mesh = const_cast<Mesh*>(this);

		return mesh->mIndexBuffer.Buffer() != nullptr;
	}

	void Mesh::CreateIndexBuffer(ID3D11Buffer** indexBuffer)
	{
		assert(indexBuffer != nullptr);

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.ByteWidth = sizeof(UINT) * mIndices.size();
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubResourceData;
		ZeroMemory(&indexSubResourceData, sizeof(indexSubResourceData));
		indexSubResourceData.pSysMem = &mIndices[0];
		if (FAILED(mModel.GetGame().Direct3DDevice()->CreateBuffer(&indexBufferDesc, &indexSubResourceData, indexBuffer)))
		{
			throw GameException("ID3D11Device::CreateBuffer() failed.");
		}
	}

	void Mesh::CreateCachedVertexAndIndexBuffers(ID3D11Device& device, const Material& material)
	{
		mVertexBuffer.ReleaseBuffer();
		mIndexBuffer.ReleaseBuffer();

		ID3D11Buffer* buffer = nullptr;
		material.CreateVertexBuffer(&device, *this, &buffer);
		mVertexBuffer.SetBuffer(buffer);
		mVertexBuffer.SetElementCount(mVertices.size());

		buffer = nullptr;
		CreateIndexBuffer(&buffer);
		mIndexBuffer.SetBuffer(buffer);
		mIndexBuffer.SetElementCount(mIndices.size());
	}
}