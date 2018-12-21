////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"
#include "rendertextureclass.h"
#include "inputclass.h"
#include "SkyShader.h"
#include "Particle.h"
#include "ParticleShader.h"
#include "PlaneMesh.h"
#include "TerrainShader.h"
#include "Window.h"
#include "EditField.h"
#include "CloudShader.h"
#include "CurvedPlaneMesh.h"
#include "OrthoMesh.h"
#include "DefferedShader.h"
#include <vector>
#include "PBRShader.h"
#include "NonLitShader.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
								GraphicsClass(InputClass& input);
								GraphicsClass(const GraphicsClass&);
								~GraphicsClass();

	bool						Initialize(int, int, Window* window);
	void						Shutdown();
	bool						Frame();
	void						OnCommand(WPARAM param);
	void						OnPaint(HDC hdc);
private:
	bool						Render();
	bool						RenderScene(D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix);
	bool						RenderSky(D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix);
	bool						RenderDefferedScene(D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix);
	void						AddEditorField(float * value, int x, int y, int width, int height, HWND parent, int id);
	bool						MessageBoxFailed(bool value, std::string name);

	D3DClass					m_D3D;
	CameraClass					m_Camera;
	ModelClass					m_SphereMesh, m_CubeMesh;
	InputClass&					m_Input;
	SkyShader					m_SkyShader;
	Particle					m_InstanceRenderer;
	ParticleShader				m_ParticleShader;
	PlaneMesh					m_PlaneMesh;
	TerrainShader				m_TerrainShader;
	Window*						m_GameWindow;

	CurvedPlaneMesh				m_CurvedPlane;
	CloudShader					m_CloudShader;
	float						m_TerrainScaler, m_Persistence, m_Octaves;
	D3DXVECTOR2 				m_CloudBrightness, m_CloudScales, m_CloudSpeeds;
	float						m_LightCount;

	std::vector<EditField*>		m_EditFields;
	bool						m_UpdateHeightmap;

	TextureClass				m_CloudTexture, m_PerturbTexture, m_SnowTexture;
	InstanceType				m_ParticleInstances;
	int							m_ParticleCount;
	OrthoMesh					m_OrthoMesh;
	DefferedShader				m_DefferedShader;
	PBRShader					m_PbrShader;
	NonLitShader				m_NonLitShader;
	TextureClass				m_SunTexture;
	float						m_TerrainSlopeHeight, m_SunRotation, m_Exposure, m_Srgb, m_TonemapValue;
	HFONT						m_StandardFont, m_BoldFont;
	D3DXVECTOR3					m_SunDirection;
};

#endif