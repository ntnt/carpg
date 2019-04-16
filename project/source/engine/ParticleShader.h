#pragma once

#include "ShaderHandler.h"

class ParticleShader : public ShaderHandler
{
public:
	ParticleShader(Render* render);
	void OnInitOnce() override;
	void OnInit() override;
	void OnReset() override;
	void OnReload() override;
	void OnRelease() override;
	void SetParameters(const CameraBase& camera);
	void Draw(const vector<ParticleEmitter*>& pes, const vector<TrailParticleEmitter*>& tpes);

private:
	void DrawParticles(const vector<ParticleEmitter*>& pes);
	void DrawTrailParticles(const vector<TrailParticleEmitter*>& tpes);

	Render* render;
	IDirect3DDevice9* device;
	VB vbParticle;
	IDirect3DVertexDeclaration9* vd_particle;
	ID3DXEffect* effect;
	D3DXHANDLE tech_particle, tech_trail;
	D3DXHANDLE h_mat, h_tex;
	int particle_count;
	Matrix mat_view_proj, mat_view_inv;
};
