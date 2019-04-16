#include "Pch.h"
#include "EngineCore.h"
#include "ParticleShader.h"
#include "ParticleSystem.h"
#include "Texture.h"
#include "CameraBase.h"
#include "Render.h"
#include "DirectX.h"

struct ParticleVertex
{
	Vec3 pos;
	Vec2 tex;
	Vec4 color;
};

ParticleShader::ParticleShader(Render* render) : render(render), device(render->GetDevice()), effect(nullptr), vbParticle(nullptr)
{
}

void ParticleShader::OnInitOnce()
{
	const D3DVERTEXELEMENT9 Particle[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{0, 12, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},
		{0, 20, D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0},
		D3DDECL_END()
	};
	V(device->CreateVertexDeclaration(Particle, &vd_particle));
}

void ParticleShader::OnInit()
{
	effect = render->CompileShader("particle.fx");

	tech_particle = effect->GetTechniqueByName("particle");
	tech_trail = effect->GetTechniqueByName("trail");
	assert(tech_particle && tech_trail);

	h_mat = effect->GetParameterByName(nullptr, "matCombined");
	h_tex = effect->GetParameterByName(nullptr, "tex0");
	assert(h_mat && h_tex);
}

void ParticleShader::OnReset()
{
	if(effect)
		effect->OnLostDevice();
	SafeRelease(vbParticle);
}

void ParticleShader::OnReload()
{
	if(effect)
		effect->OnResetDevice();
}

void ParticleShader::OnRelease()
{
	SafeRelease(effect);
	SafeRelease(vbParticle);
}

void ParticleShader::SetParameters(const CameraBase& camera)
{
	mat_view_proj = camera.matViewProj;
	mat_view_inv = camera.matViewInv;
}

void ParticleShader::Draw(const vector<ParticleEmitter*>& pes, const vector<TrailParticleEmitter*>& tpes)
{
	render->SetAlphaTest(false);
	render->SetAlphaBlend(true);
	render->SetNoCulling(true);
	render->SetNoZWrite(true);

	DrawTrailParticles(tpes);
	DrawParticles(pes);
}

void ParticleShader::DrawParticles(const vector<ParticleEmitter*>& pes)
{
	V(device->SetVertexDeclaration(vd_particle));

	uint passes;
	V(effect->SetTechnique(tech_particle));
	V(effect->SetMatrix(h_mat, (D3DXMATRIX*)&mat_view_proj));
	V(effect->Begin(&passes, 0));
	V(effect->BeginPass(0));

	for(vector<ParticleEmitter*>::const_iterator it = pes.begin(), end = pes.end(); it != end; ++it)
	{
		const ParticleEmitter& pe = **it;

		// stwórz vertex buffer na cz¹steczki jeœli nie ma wystarczaj¹co du¿ego
		if(!vbParticle || particle_count < pe.alive)
		{
			if(vbParticle)
				vbParticle->Release();
			V(device->CreateVertexBuffer(sizeof(ParticleVertex)*pe.alive * 6, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vbParticle, nullptr));
			particle_count = pe.alive;
		}
		V(device->SetStreamSource(0, vbParticle, 0, sizeof(ParticleVertex)));

		// wype³nij vertex buffer
		ParticleVertex* v;
		V(vbParticle->Lock(0, sizeof(ParticleVertex)*pe.alive * 6, (void**)&v, D3DLOCK_DISCARD));
		int idx = 0;
		for(vector<Particle>::const_iterator it2 = pe.particles.begin(), end2 = pe.particles.end(); it2 != end2; ++it2)
		{
			const Particle& p = *it2;
			if(!p.exists)
				continue;

			mat_view_inv._41 = p.pos.x;
			mat_view_inv._42 = p.pos.y;
			mat_view_inv._43 = p.pos.z;
			Matrix m1 = Matrix::Scale(pe.GetScale(p)) * mat_view_inv;

			const Vec4 color(1.f, 1.f, 1.f, pe.GetAlpha(p));

			v[idx].pos = Vec3::Transform(Vec3(-1, -1, 0), m1);
			v[idx].tex = Vec2(0, 0);
			v[idx].color = color;

			v[idx + 1].pos = Vec3::Transform(Vec3(-1, 1, 0), m1);
			v[idx + 1].tex = Vec2(0, 1);
			v[idx + 1].color = color;

			v[idx + 2].pos = Vec3::Transform(Vec3(1, -1, 0), m1);
			v[idx + 2].tex = Vec2(1, 0);
			v[idx + 2].color = color;

			v[idx + 3] = v[idx + 1];

			v[idx + 4].pos = Vec3::Transform(Vec3(1, 1, 0), m1);
			v[idx + 4].tex = Vec2(1, 1);
			v[idx + 4].color = color;

			v[idx + 5] = v[idx + 2];

			idx += 6;
		}

		V(vbParticle->Unlock());

		switch(pe.mode)
		{
		case 0:
			V(device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD));
			V(device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
			V(device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
			break;
		case 1:
			V(device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD));
			V(device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
			V(device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE));
			break;
		case 2:
			V(device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT));
			V(device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
			V(device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE));
			break;
		default:
			assert(0);
			break;
		}

		V(effect->SetTexture(h_tex, pe.tex->tex));
		V(effect->CommitChanges());

		V(device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, pe.alive * 2));
	}

	V(effect->EndPass());
	V(effect->End());

	V(device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD));
	V(device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
	V(device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
}

void ParticleShader::DrawTrailParticles(const vector<TrailParticleEmitter*>& tpes)
{
	V(device->SetVertexDeclaration(vertex_decl[VDI_COLOR]));

	uint passes;
	V(effect->SetTechnique(tech_trail));
	V(effect->SetMatrix(hParticleCombined, (D3DXMATRIX*)&cam.matViewProj));
	V(effect->Begin(&passes, 0));
	V(effect->BeginPass(0));

	VColor v[4];

	for(vector<TrailParticleEmitter*>::const_iterator it = tpes.begin(), end = tpes.end(); it != end; ++it)
	{
		const TrailParticleEmitter& tp = **it;

		if(tp.alive < 2)
			continue;

		int id = tp.first;
		const TrailParticle* prev = &tp.parts[id];
		id = prev->next;

		if(id < 0 || id >= (int)tp.parts.size() || !tp.parts[id].exists)
		{
			ReportError(6, Format("Trail particle emitter error, id = %d!", id));
			const_cast<TrailParticleEmitter&>(tp).alive = false;
			continue;
		}

		while(id != -1)
		{
			const TrailParticle& p = tp.parts[id];

			v[0].pos = prev->pt1;
			v[1].pos = prev->pt2;
			v[2].pos = p.pt1;
			v[3].pos = p.pt2;

			v[0].color = Vec4::Lerp(tp.color1, tp.color2, 1.f - prev->t / tp.fade);
			v[1].color = v[0].color;
			v[2].color = Vec4::Lerp(tp.color1, tp.color2, 1.f - p.t / tp.fade);
			v[3].color = v[2].color;

			V(device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(VColor)));

			prev = &p;
			id = prev->next;
		}
	}

	V(effect->EndPass());
	V(effect->End());
}
