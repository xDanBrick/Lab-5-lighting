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
#include "lightshaderclass.h"
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
#include "PostShader.h"
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
const float CAM_MOVE_SPEED = 0.2f;

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass(InputClass& input);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool						Initialize(int, int, Window* hwnd);
	void						Shutdown();
	bool						Frame();
	void						OnCommand(WPARAM param);
	void						OnPaint(HDC hdc);
private:
	bool						Render(float, float);
	void						UpdateCameraInput(float delta);
	
private:
	D3DClass*					m_D3D;
	CameraClass*				m_Camera;
	ModelClass*					m_Model;
	LightShaderClass*			m_LightShader;
	LightClass*					m_Light;
	InputClass&					input_;
	SkyShader*					skyShader_;
	Particle*					instanceRenderer_;
	ParticleShader*				particleShader_;
	PlaneMesh*					planeMesh_;
	TerrainShader*				terrainShader_;
	Window*						gameWindow_;

	CurvedPlaneMesh*			curvedPlane_;
	CloudShader*				cloudShader_;
	float						terrainScaler_, persistence_, octaves_;
	D3DXVECTOR2 				cloudBrightness_, cloudScales_, cloudSpeeds_;
	float						lightCount_;

	std::vector<EditField*>		editFields_;
	bool						updateHeightmap_;

	TextureClass*				cloudTexture_, *perturbTexture_;
	InstanceType*				particleInstances_;
	int							particleCount_;
	OrthoMesh					orthoMesh_;
	PostShader					postShader_;
	PBRShader					pbrShader_;
	NonLitShader				nonLitShader_;
	TextureClass				sunTexture_;
	float						terrainSlopeHeight_;
	HFONT						standardFont_, boldFont_;
};

#endif