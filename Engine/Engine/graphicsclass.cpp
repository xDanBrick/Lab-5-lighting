////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

const int CUBE_COUNT = 5;
D3DXVECTOR3	cubePositions[CUBE_COUNT][CUBE_COUNT];
float	cubeScalesY[CUBE_COUNT][CUBE_COUNT];

bool GraphicsClass::MessageBoxFailed(bool value, std::string name)
{
	if (!value)
	{
		MessageBoxA(m_GameWindow->getHWND(), std::string("Could not initialize " + name).c_str(), "Error", MB_OK);
	}
	return value;
}

GraphicsClass::GraphicsClass(InputClass& input) : m_Input(input)
{
	m_TerrainScaler = 4.0f;
	m_Persistence = 5.0f;
	m_Octaves = 2.0f;
	m_UpdateHeightmap = true;
	m_CloudBrightness = { 0.5f, 0.75f };
	m_CloudScales = { 0.5f, 0.7f };
	m_CloudSpeeds = { 0.00005f, 0.0001f };
	m_LightCount = 1.0f;
	m_TerrainSlopeHeight = 0.0f;
	m_Exposure = 1.0f;
	m_TonemapValue = 3.0f;
	m_Srgb = 1.0f;
	m_SunRotation = 0.0f;

	//Init all the cube positions and sizes
	for (int i = 0; i < CUBE_COUNT; ++i)
	{
		for (int j = 0; j < CUBE_COUNT; ++j)
		{
			cubePositions[i][j] = { (float)(rand() % 256) - 128.0f, 3.0f, (float)(rand() % 256) - 128.0f };
			cubeScalesY[i][j] = (float)(rand() % 10) + 5.0f;
		}
	}

	// Set the initial position of the camera.
	m_Camera.SetPosition(0.0f, 10.0f, -10.0f);
}


GraphicsClass::GraphicsClass(const GraphicsClass& other) : m_Input(other.m_Input)
{
}


GraphicsClass::~GraphicsClass()
{
}

#define SCALER_ID 2000
#define PERSISTENCE_ID 2001
#define OCTAVES_ID 2002
#define CLOUD_SCALER1 2003
#define CLOUD_SCALER2 2002
#define CLOUD_BRIGHTNESS1 2005
#define CLOUD_BRIGHTNESS2 2006
#define CLOUD_SPEED1 2007
#define CLOUD_SPEED2 2008
#define LIGHT_COUNT 2009
#define TONEMAP 2010
#define EXPOSURE 2011
#define SRGB 2012

void GraphicsClass::AddEditorField(float* value, int x, int y, int width, int height, HWND parent, int id)
{
	m_EditFields.push_back(new EditField());
	m_EditFields.back()->Initalise(value, x, y, width, height, parent, id);
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, Window* window)
{
	//Create window fonts
	m_StandardFont = CreateWindowFont(L"Arial", -12L, FW_REGULAR);
	m_BoldFont = CreateWindowFont(L"Arial", -12L, FW_BOLD);

	m_GameWindow = window;
	HWND hwnd = m_GameWindow->getHWND();

	//Add Editor fields for variables
	AddEditorField(&m_TerrainScaler, 120, 40, 100, 22, window->getParent(), SCALER_ID);
	AddEditorField(&m_Persistence, 120, 70, 100, 22, window->getParent(), PERSISTENCE_ID);
	AddEditorField(&m_Octaves, 120, 100, 100, 22, window->getParent(), OCTAVES_ID);

	AddEditorField(&m_CloudBrightness.x, 120, 190, 100, 22, window->getParent(), CLOUD_BRIGHTNESS1);
	AddEditorField(&m_CloudBrightness.y, 120, 220, 100, 22, window->getParent(), CLOUD_BRIGHTNESS2);
	AddEditorField(&m_CloudScales.x, 120, 250, 100, 22, window->getParent(), CLOUD_SCALER1);
	AddEditorField(&m_CloudScales.x, 120, 280, 100, 22, window->getParent(), CLOUD_SCALER2);
	AddEditorField(&m_CloudSpeeds.x, 120, 310, 100, 22, window->getParent(), CLOUD_SPEED1);
	AddEditorField(&m_CloudSpeeds.y, 120, 340, 100, 22, window->getParent(), CLOUD_SPEED2);

	AddEditorField(&m_TonemapValue, 120, 430, 100, 22, window->getParent(), TONEMAP);
	AddEditorField(&m_Exposure, 120, 460, 100, 22, window->getParent(), EXPOSURE);
	AddEditorField(&m_Srgb, 120, 490, 100, 22, window->getParent(), SRGB);

	AddEditorField(&m_LightCount, 120, 550, 100, 22, window->getParent(), LIGHT_COUNT);

	// Initialize the Direct3D object.
	if(!MessageBoxFailed(m_D3D.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR, DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_D32_FLOAT, 2), "Direct3D"))
	{
		return false;
	}

	//Initalise Textures
	if (!MessageBoxFailed(m_SunTexture.Initialize(m_D3D.GetDevice(), L"../Engine/data/sun.png"), "Sun Texture"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_PerturbTexture.Initialize(m_D3D.GetDevice(), L"../Engine/data/perturb.dds"), "Perturb Texture"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_CloudTexture.Initialize(m_D3D.GetDevice(), L"../Engine/data/cloud.dds"), "Cloud Texture"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_SnowTexture.Initialize(m_D3D.GetDevice(), L"../Engine/data/snowflake.png"), "Snowflake Texture"))
	{
		return false;
	}

	//Initalise Meshes
	if (!MessageBoxFailed(m_PlaneMesh.Initalise(m_D3D.GetDevice()), "Plane Mesh"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_CurvedPlane.Initalise(m_D3D.GetDevice()), "Curved Plane"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_InstanceRenderer.Initalise(m_D3D.GetDevice(), hwnd), "Instance Renderer"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_OrthoMesh.Initalise(m_D3D.GetDevice(), screenWidth, screenHeight), "Ortho Mesh"))
	{
		return false;
	}

	// Initialize the model objects
	if(!MessageBoxFailed(m_SphereMesh.Initialize(m_D3D.GetDevice(), "../Engine/data/sphere.txt", L"../Engine/data/dirt.png"), "Sphere Mesh"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_CubeMesh.Initialize(m_D3D.GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/BuildingTexture.png"), "Cube Mesh"))
	{
		return false;
	}

	//Initialise Shaders
	if (!MessageBoxFailed(m_PbrShader.Initialize(m_D3D.GetDevice(), hwnd), "PBR Shader"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_DefferedShader.Initialize(m_D3D.GetDevice(), hwnd, screenWidth, screenHeight), "Deffered Shader"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_NonLitShader.Initialize(m_D3D.GetDevice(), hwnd), "Non Lit Shader"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_TerrainShader.Initialize(m_D3D.GetDevice(), hwnd), "Terrain Shader"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_ParticleShader.Initialize(m_D3D.GetDevice(), hwnd), "Particle Shader"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_SkyShader.Initialize(m_D3D.GetDevice(), hwnd), "Sky Shader"))
	{
		return false;
	}
	if (!MessageBoxFailed(m_CloudShader.Initialize(m_D3D.GetDevice(), hwnd), "Cloud Shader"))
	{
		return false;
	}
	return true;
}

void GraphicsClass::Shutdown()
{
}

bool GraphicsClass::Frame()
{
	// Update the rotation of the sun
	m_SunRotation += (float)D3DX_PI * 0.01f;
	if(m_SunRotation > 360.0f)
	{
		m_SunRotation -= 360.0f;
	}

	//Update the slope snow on the terrain
	if (m_TerrainSlopeHeight < 1.0f)
	{
		m_TerrainSlopeHeight += 0.0001f;
	}

	//Render the scene
	return Render();
}

//colors for the skybox
const D3DXVECTOR4 apexColor(0.0f, 0.1f, 0.5f, 1.0f);
const D3DXVECTOR4 centreColor(0.7f, 0.4f, 0.25f, 1.0f);

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	
	//Set the render targets to the deffered rendering targets
	m_DefferedShader.setDefferedRendering(m_D3D.GetDeviceContext(), m_D3D.getDepthStencilView());

	//Update Camera Input
	m_Camera.Update(m_Input);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera.GetViewMatrix(viewMatrix);
	m_D3D.GetWorldMatrix(worldMatrix);
	m_D3D.GetProjectionMatrix(projectionMatrix);

	//Render the sky, clouds and sun
	if (!RenderSky(worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	//Render the scene Objects
	if (!RenderScene(worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	//Render the final scene image using the deffered textures
	if (!RenderDefferedScene(worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D.EndScene();

	return true;
}

bool GraphicsClass::RenderScene(D3DXMATRIX & worldMatrix, D3DXMATRIX & viewMatrix, D3DXMATRIX & projectionMatrix)
{
	D3DXMATRIX translationMatrix, scaleMatrix;
	D3DXMatrixIdentity(&worldMatrix);

	if (!m_TerrainShader.Render(m_D3D.GetDeviceContext(), &m_PlaneMesh, worldMatrix, viewMatrix, projectionMatrix, m_SphereMesh.getTexture(), m_TerrainScaler, m_Persistence, m_Octaves, m_TerrainSlopeHeight, m_UpdateHeightmap))
	{
		return false;
	}

	for (int i = 0; i < CUBE_COUNT; ++i)
	{
		for (int j = 0; j < CUBE_COUNT; ++j)
		{
			D3DXMatrixScaling(&scaleMatrix, 5.0f, cubeScalesY[i][j], 5.0f);
			D3DXMatrixTranslation(&translationMatrix, cubePositions[i][j].x, cubePositions[i][j].y, cubePositions[i][j].z);

			if (!m_PbrShader.Render(m_D3D.GetDeviceContext(), &m_CubeMesh, m_CubeMesh.getTexture(), scaleMatrix * translationMatrix, viewMatrix, projectionMatrix, { 1.0f, 1.0f, 1.0f }, (float)i * 0.1f, (float)j * 0.1f))
			{
				return false;
			}
		}
	}

	m_D3D.CullNone();
	m_D3D.EnableAdditiveBlending();
	if (!m_ParticleShader.Render(m_D3D.GetDeviceContext(), &m_InstanceRenderer, m_SnowTexture.GetTexture(), worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}
	m_D3D.CullBack();
	m_D3D.EnableOpagueBlending();
	return true;
}

bool GraphicsClass::RenderSky(D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix)
{
	m_D3D.CullFront();
	m_D3D.setDepthState(false);
	m_D3D.EnableOpagueBlending();

	auto position = m_Camera.GetPosition();
	D3DXMatrixTranslation(&worldMatrix, position.x, position.y, position.z);

	// Render the Skydome
	if (!m_SkyShader.Render(m_D3D.GetDeviceContext(), &m_SphereMesh, worldMatrix, viewMatrix, projectionMatrix, apexColor, centreColor))
	{
		return false;
	}

	m_D3D.CullBack();

	D3DXMATRIX translationMatrix, scaleMatrix, rotationMatrix, rotationMatrixYPR;
	
	//Calculate the position of the Sun
	D3DXMatrixTranslation(&translationMatrix, 0.0f, 30.0f, 200.0f);
	D3DXMatrixRotationYawPitchRoll(&rotationMatrixYPR, (float)D3DXToRadian(m_SunRotation), D3DXToRadian(-30.0f), 0.0f);
	D3DXMatrixRotationY(&rotationMatrix, D3DXToRadian(m_SunRotation));
	D3DXMatrixScaling(&scaleMatrix, 10.0f, 10.0f, 10.0f);

	worldMatrix = scaleMatrix * translationMatrix * rotationMatrix;

	//Render the Sun
	if (!m_NonLitShader.Render(m_D3D.GetDeviceContext(), &m_SphereMesh, worldMatrix, viewMatrix, projectionMatrix, m_SunTexture.GetTexture()))
	{
		return false;
	}

	worldMatrix = scaleMatrix * translationMatrix * rotationMatrixYPR;
	D3DXVECTOR3 scale, trans, lightDirection;
	D3DXQUATERNION qua;

	D3DXMatrixDecompose(&scale, &qua, &trans, &worldMatrix);

	lightDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f) - trans;
	D3DXVec3Normalize(&m_SunDirection, &lightDirection);

	m_D3D.EnableAdditiveBlending();

	//Move the clouds to position
	D3DXMatrixTranslation(&worldMatrix, 0.0f, -5.0f, 0.0f);

	//Render the clouds
	if (!m_CloudShader.Render(m_D3D.GetDeviceContext(), &m_CurvedPlane, worldMatrix, viewMatrix, projectionMatrix, m_CloudTexture.GetTexture(), m_PerturbTexture.GetTexture(), m_CloudScales, m_CloudBrightness, m_CloudSpeeds))
	{
		return false;
	}

	m_D3D.EnableOpagueBlending();
	m_D3D.setDepthState(true);

	//Reset World matrix
	D3DXMatrixIdentity(&worldMatrix);
	
	return true;
}

bool GraphicsClass::RenderDefferedScene(D3DXMATRIX & worldMatrix, D3DXMATRIX & viewMatrix, D3DXMATRIX & projectionMatrix)
{
	D3DXMATRIX baseViewMatrix, orthoProjMatrix;
	m_D3D.GetOrthoMatrix(orthoProjMatrix);
	m_D3D.GetBaseViewMatrix(baseViewMatrix);

	//Clear the back buffer to black
	m_D3D.BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_D3D.setBackBufferRenderTarget();
	
	//Render the scene texture onto the orthographic mesh
	m_DefferedShader.Render(m_D3D.GetDeviceContext(), &m_OrthoMesh, worldMatrix, baseViewMatrix, orthoProjMatrix, m_Camera.GetPosition(), (int)m_LightCount, (int)m_TonemapValue, m_Exposure, m_Srgb == 0.0f ? false : true , m_SunDirection);
	return true;
}

static const wchar_t terrainTitleText[] = L"Terrain";
static const wchar_t perlinScaleText[] = L"Noise Scaler";
static const wchar_t persistanceText[] = L"Persistance";
static const wchar_t octavesText[] = L"Octaves";


static const wchar_t cloudTitleText[] = L"Clouds";
static const wchar_t cloudSpeedText1[] = L"Cloud Speed 1";
static const wchar_t cloudSpeedText2[] = L"Cloud Speed 2";
static const wchar_t cloudBrightnessText1[] = L"Cloud Brightness 1";
static const wchar_t cloudBrightnessText2[] = L"Cloud Brightness 2";
static const wchar_t cloudScaleText1[] = L"Cloud Scale 1";
static const wchar_t cloudScaleText2[] = L"Cloud Scale 2";

static const wchar_t lightCountText[] = L"Light Count";

static const wchar_t cameraTitleText[] = L"Camera";
static const wchar_t tonemapText[] = L"Tonemap Value";
static const wchar_t exposureText[] = L"Exposure";
static const wchar_t srgbText[] = L"SRGB";

void GraphicsClass::OnPaint(HDC hdc)
{
	SelectObject(hdc, m_BoldFont);
	SetBkMode(hdc, TRANSPARENT);
	//Add titles
	TextOut(hdc, 5, 20, terrainTitleText, ARRAYSIZE(terrainTitleText));
	TextOut(hdc, 5, 160, cloudTitleText, ARRAYSIZE(cloudTitleText));
	TextOut(hdc, 5, 400, cameraTitleText, ARRAYSIZE(cameraTitleText));

	SelectObject(hdc, m_StandardFont);
	//Add terrain text
	TextOut(hdc, 5, 40, perlinScaleText, ARRAYSIZE(perlinScaleText));
	TextOut(hdc, 5, 70, persistanceText, ARRAYSIZE(persistanceText));
	TextOut(hdc, 5, 100, octavesText, ARRAYSIZE(octavesText));

	//Add cloud text
	TextOut(hdc, 5, 190, cloudBrightnessText1, ARRAYSIZE(cloudBrightnessText1));
	TextOut(hdc, 5, 220, cloudBrightnessText2, ARRAYSIZE(cloudBrightnessText2));
	TextOut(hdc, 5, 250, cloudScaleText1, ARRAYSIZE(cloudScaleText1));
	TextOut(hdc, 5, 280, cloudScaleText2, ARRAYSIZE(cloudScaleText2));
	TextOut(hdc, 5, 310, cloudSpeedText1, ARRAYSIZE(cloudSpeedText1));
	TextOut(hdc, 5, 340, cloudSpeedText2, ARRAYSIZE(cloudSpeedText2));

	//Add Camera text
	TextOut(hdc, 5, 430, tonemapText, ARRAYSIZE(tonemapText));
	TextOut(hdc, 5, 460, exposureText, ARRAYSIZE(exposureText));
	TextOut(hdc, 5, 490, srgbText, ARRAYSIZE(srgbText));

	//Add lightcount text
	TextOut(hdc, 5, 550, lightCountText, ARRAYSIZE(lightCountText));
}

void GraphicsClass::OnCommand(WPARAM wParam)
{
	//For all the edit fields
	for (auto fieldItt = m_EditFields.begin(); fieldItt != m_EditFields.end(); ++fieldItt)
	{
		(*fieldItt)->OnCommand(wParam);
		m_UpdateHeightmap = true;
	}
}

