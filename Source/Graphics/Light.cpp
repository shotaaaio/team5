#include	"Light.h"

Light::Light(LightType lightType)
	: lightType(lightType)
{
}

void Light::PushRenderContext(LightRenderContext& rc) const
{
	// ìoò^Ç≥ÇÍÇƒÇ¢ÇÈåıåπÇÃèÓïÒÇê›íË
	switch (lightType)
	{
	case	LightType::Directional:
	{
		rc.directionalLightData.direction.x = direction.x;
		rc.directionalLightData.direction.y = direction.y;
		rc.directionalLightData.direction.z = direction.z;
		rc.directionalLightData.direction.w = 0.0f;
		rc.directionalLightData.color = color;
		break;
	}
	case	LightType::Point:
	{
		if (rc.pointLightCount >= PointLightMax)
			break;
		rc.pointLightData[rc.pointLightCount].position.x = position.x;
		rc.pointLightData[rc.pointLightCount].position.y = position.y;
		rc.pointLightData[rc.pointLightCount].position.z = position.z;
		rc.pointLightData[rc.pointLightCount].position.w = 1.0f;
		rc.pointLightData[rc.pointLightCount].color = color;
		rc.pointLightData[rc.pointLightCount].range = range;
		++rc.pointLightCount;
		break;
	}
	case	LightType::Spot:
	{
		if (rc.spotLightCount >= SpotLightMax)
			break;

		rc.spotLightData[rc.spotLightCount].position.x = position.x;
		rc.spotLightData[rc.spotLightCount].position.y = position.y;
		rc.spotLightData[rc.spotLightCount].position.z = position.z;
		rc.spotLightData[rc.spotLightCount].position.w = 1.0f;
		rc.spotLightData[rc.spotLightCount].direction.x = direction.x;
		rc.spotLightData[rc.spotLightCount].direction.y = direction.y;
		rc.spotLightData[rc.spotLightCount].direction.z = direction.z;
		rc.spotLightData[rc.spotLightCount].direction.w = 0.0f;
		rc.spotLightData[rc.spotLightCount].color = color;
		rc.spotLightData[rc.spotLightCount].range = range;
		rc.spotLightData[rc.spotLightCount].innerCorn = innerCorn;
		rc.spotLightData[rc.spotLightCount].outerCorn = outerCorn;
		++rc.spotLightCount;
		break;
	}
	}
}

void Light::DrawDebugGUI()
{
}

void Light::DrawDebugPrimitive()
{
	switch (lightType)
	{
	case LightType::Directional:
	{
		//	ïΩçsåıåπÇÕï\é¶ÇµÇ»Ç¢ÅB
		break;
	}

	}
}
