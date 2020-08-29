#pragma once

#include "../Core/Color.h"
#include "../Core/Math.h"
#include "../Core/Vector.h"
#include "../Core/Matrix.h"

static class Shader
{
private:
	//------------------------------------------------------------------------------
	// BRDF
	//------------------------------------------------------------------------------

	// Walter et al. 2007, "Microfacet Models for Refraction through Rough Surfaces"
	inline static float D_GGX(float linearRoughness, float NoH) {
		float oneMinusNoHSquared = 1.0 - NoH * NoH;
		float a = NoH * linearRoughness;
		float k = linearRoughness / (oneMinusNoHSquared + a * a);
		float d = k * k * Math::INV_PI;
		return d;
	}

	// Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
	inline static float V_SmithGGXCorrelated(float linearRoughness, float NoV, float NoL) {
		float a2 = linearRoughness * linearRoughness;
		float GGXV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
		float GGXL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
		return 0.5 / (GGXV + GGXL);
	}

	// Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
	inline static Vector3 F_Schlick(const Vector3 f0, float VoH) { return f0 + (Vectors::one_v3 - f0) * Math::pow5(1.0 - VoH); }

	inline static float F_Schlick(float f0, float f90, float VoH) { return f0 + (f90 - f0) * Math::pow5(1.0 - VoH); }

	// Burley 2012, "Physically-Based Shading at Disney"
	inline static float Fd_Burley(float linearRoughness, float NoV, float NoL, float LoH) {
		float f90 = 0.5 + 2.0 * linearRoughness * LoH * LoH;
		float lightScatter = F_Schlick(1.0, f90, NoL);
		float viewScatter = F_Schlick(1.0, f90, NoV);
		return lightScatter * viewScatter * Math::INV_PI;
	}

public:
	Shader() {}
	~Shader() {}

	inline static void PhysicallyBasedShading(RGBColor& outColor, float roughness, float metallic,
		Vector3 n, Vector3 l, Vector3 v, float NoL) {
		auto beseColor = Vector3(outColor.r, outColor.g, outColor.b);
		auto h = (v + l).normalize();
		float NoV = abs(n.dot(v)) + 1e-5;
		float NoH = Math::clamp(n.dot(h));
		float LoH = Math::clamp(l.dot(h));

		float linearRoughness = roughness * roughness;
		Vector3 diffuseColor = (1.0 - metallic) * beseColor;
		Vector3 f0 = beseColor * metallic + 0.04 * (1.0 - metallic);

		// specular BRDF
		float D = D_GGX(linearRoughness, NoH);
		float V = V_SmithGGXCorrelated(linearRoughness, NoV, NoL);
		Vector3  F = F_Schlick(f0, LoH);
		Vector3 Fr = (D * V) * F;

		// diffuse BRDF
		Vector3 Fd = diffuseColor * Fd_Burley(linearRoughness, NoV, NoL, LoH);

		//auto c = Vector3(n.dot(v));
		auto c = Fd + Fr;
		outColor.r = c.x;
		outColor.g = c.y;
		outColor.b = c.z;
	}
};