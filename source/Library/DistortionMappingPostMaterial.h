#pragma once

#include "Common.h"
#include "Material.h"
#include "VertexDeclarations.h"

namespace Library
{
	class DistortionMappingPostMaterial : public Material
	{
		RTTI_DECLARATIONS(Material, DistortionMappingPostMaterial)

			MATERIAL_VARIABLE_DECLARATION(SceneTexture)
			MATERIAL_VARIABLE_DECLARATION(DistortionMap)
			MATERIAL_VARIABLE_DECLARATION(DisplacementScale)

	public:
		DistortionMappingPostMaterial();

		virtual void Initialize(Effect& effect) override;
		virtual void CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const override;
		void CreateVertexBuffer(ID3D11Device* device, VertexPositionTexture* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;
		virtual UINT VertexSize() const override;
	};
}

