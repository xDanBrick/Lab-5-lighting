////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass(InputClass& input) : input_(input)
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
	particleShader_ = 0;
	instanceRenderer_ = 0;
	terrainScaler_ = 4.0f;
	persistence_ = 5.0f;
	octaves_ = 2.0f;
	updateHeightmap_ = true;
	cloudBrightness_ = { 0.5f, 0.75f };
	cloudScales_ = { 0.5f, 0.7f };
	cloudSpeeds_ = { 0.00005f, 0.0001f };
	lightCount_ = 1.0f;
	terrainSlopeHeight_ = 0.0f;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other) : input_(other.input_)
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

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, Window* window)
{
	standardFont_ = CreateWindowFont(L"Arial", -12L, FW_REGULAR);
	boldFont_ = CreateWindowFont(L"Arial", -12L, FW_BOLD);
	bool result;
	gameWindow_ = window;
	HWND hwnd = gameWindow_->getHWND();

	editFields_.push_back(new EditField());
	editFields_.back()->Initalise(&terrainScaler_, 120, 40, 100, 22, window->getParent(), SCALER_ID);

	editFields_.push_back(new EditField());
	editFields_.back()->Initalise(&persistence_, 120, 70, 100, 22, window->getParent(), PERSISTENCE_ID);

	editFields_.push_back(new EditField());
	editFields_.back()->Initalise(&octaves_, 120, 100, 100, 22, window->getParent(), OCTAVES_ID);

	editFields_.push_back(new EditField());
	editFields_.back()->Initalise(&cloudBrightness_.x, 120, 240, 100, 22, window->getParent(), CLOUD_BRIGHTNESS1);

	editFields_.push_back(new EditField());
	editFields_.back()->Initalise(&cloudBrightness_.y, 120, 270, 100, 22, window->getParent(), CLOUD_BRIGHTNESS2);

	editFields_.push_back(new EditField());
	editFields_.back()->Initalise(&cloudScales_.x, 120, 300, 100, 22, window->getParent(), CLOUD_SCALER1);

	editFields_.push_back(new EditField());
	editFields_.back()->Initalise(&cloudScales_.x, 120, 330, 100, 22, window->getParent(), CLOUD_SCALER2);

	editFields_.push_back(new EditField());
	editFields_.back()->Initalise(&cloudSpeeds_.x, 120, 360, 100, 22, window->getParent(), CLOUD_SPEED1);

	editFields_.push_back(new EditField());
	editFields_.back()->Initalise(&cloudSpeeds_.y, 120, 390, 100, 22, window->getParent(), CLOUD_SPEED2);

	editFields_.push_back(new EditField());
	editFields_.back()->Initalise(&lightCount_, 120, 420, 100, 22, window->getParent(), LIGHT_COUNT);

	

	RedrawWindow(window->getParent(), nullptr, 0, RDW_INVALIDATE | RDW_ERASE);
	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	//NonHDR DXGI_FORMAT_R8G8B8A8_UNORM // HDR DXGI_FORMAT_R10G10B10A2_UNORM

	//Non HDR DXGI_FORMAT_D24_UNORM_S8_UINT // HDR DXGI_FORMAT_D32_FLOAT

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR, DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_D32_FLOAT, 2);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	if (!sunTexture_.Initialize(m_D3D->GetDevice(), L"../Engine/data/sun.png"))
	{
		return false;
	}

	planeMesh_ = new PlaneMesh(m_D3D->GetDevice());
	curvedPlane_ = new CurvedPlaneMesh(m_D3D->GetDevice());

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 10.0f, -10.0f);
	
	// Create the model object.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/sphere.txt", L"../Engine/data/dirt.png");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	} 

	if (!pbrShader_.Initialize(m_D3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the PBR Shader.", L"Error", MB_OK);
		return false;
	}

	cloudTexture_ = new TextureClass();
	result = cloudTexture_->Initialize(m_D3D->GetDevice(), L"../Engine/data/cloud.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cloud texture.", L"Error", MB_OK);
		return false;
	}
	

	if (!postShader_.Initialize(m_D3D->GetDevice(), hwnd, screenWidth, screenHeight))
	{
		MessageBox(hwnd, L"Could not initialize the Post Shader.", L"Error", MB_OK);
		return false;
	}

	if (!nonLitShader_.Initialize(m_D3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the NonLit Shader Shader.", L"Error", MB_OK);
		return false;
	}

	perturbTexture_ = new TextureClass();
	result = perturbTexture_->Initialize(m_D3D->GetDevice(), L"../Engine/data/perturb.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the perturb Texture.", L"Error", MB_OK);
		return false;
	}

	instanceRenderer_ = new Particle;
	if (!instanceRenderer_->Initalise(m_D3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the Instanced model object.", L"Error", MB_OK);
		return false;
	}

	
	if (!orthoMesh_.Initalise(m_D3D->GetDevice(), screenWidth, screenHeight))
	{
		MessageBox(hwnd, L"Could not initialize the Ortho Mesh.", L"Error", MB_OK);
		return false;
	}

	
	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	terrainShader_ = new TerrainShader();
	result = terrainShader_->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Terrain Shader.", L"Error", MB_OK);
		return false;
	}

	particleShader_ = new ParticleShader();
	result = particleShader_->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Partcle Shader.", L"Error", MB_OK);
		return false;
	}

	skyShader_ = new SkyShader();
	result = skyShader_->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Sky Shader.", L"Error", MB_OK);
		return false;
	}

	cloudShader_ = new CloudShader();
	result = cloudShader_->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Cloud Shader.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 0.0f);

	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

bool GraphicsClass::Frame()
{
	bool result;
	static float rotation = 0.0f;
	static float delta =0.0f;

	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.01f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Update the delta variable each frame. (keep this between 0 and 1)
	delta += 0.001;
	if(delta > 1.0f)
	{
		delta -= 1.0f;
	}
	
	if (terrainSlopeHeight_ < 1.0f)
	{
		terrainSlopeHeight_ += 0.001f;
	}

	//Render the scene
	return Render(rotation, delta);
}

D3DXVECTOR4 apexColor(0.0f, 0.1f, 0.5f, 1.0f);
D3DXVECTOR4 centreColor(0.7f, 0.4f, 0.25f, 1.0f);

bool GraphicsClass::Render(float rotation, float deltavalue)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoProjMatrix;
	
	postShader_.setDefferedRendering(m_D3D->GetDeviceContext(), m_D3D->getDepthStencilView());
	// Clear the buffers to begin the scene.

	//Update Camera Input
	UpdateCameraInput(deltavalue);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoProjMatrix);
	m_D3D->GetBaseViewMatrix(baseViewMatrix);

	m_D3D->CullFront();
	m_D3D->setDepthState(false);
	m_D3D->EnableOpagueBlending();

	auto position = m_Camera->GetPosition();
	D3DXMatrixTranslation(&worldMatrix, position.x, position.y, position.z);

	// Render the Skydom
	if (!skyShader_->Render(m_D3D->GetDeviceContext(), m_Model, worldMatrix, viewMatrix, projectionMatrix,
		apexColor, centreColor))
	{
		return false;
	}

	m_D3D->CullBack();
	
	D3DXMATRIX translationMatrix, scaleMatrix, rotationMatrix, rotationMatrixYPR, m5;

	D3DXMatrixTranslation(&translationMatrix, 0.0f, 30.0f, 200.0f);
	D3DXMatrixRotationYawPitchRoll(&rotationMatrixYPR, D3DXToRadian(rotation * 5.0f), D3DXToRadian(-30.0f), 0.0f);
	D3DXMatrixRotationY(&rotationMatrix, D3DXToRadian(rotation * 5.0f));
	D3DXMatrixScaling(&scaleMatrix, 10.0f, 10.0f, 10.0f);
	
	worldMatrix = scaleMatrix * translationMatrix * rotationMatrix;
	m5 = scaleMatrix * translationMatrix * rotationMatrixYPR;
	D3DXVECTOR3 scale, trans, lightDirection, normLightDirection;
	D3DXQUATERNION qua;

	D3DXMatrixDecompose(&scale, &qua, &trans, &m5);

	lightDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f) - trans;
	D3DXVec3Normalize(&normLightDirection, &lightDirection);
	if (!nonLitShader_.Render(m_D3D->GetDeviceContext(), m_Model, worldMatrix, viewMatrix, projectionMatrix, sunTexture_.GetTexture()))
	{
		return false;
	}

	m_D3D->EnableAdditiveBlending();
	D3DXMatrixTranslation(&worldMatrix, 0.0f, -5.0f, 0.0f);

	if (!cloudShader_->Render(m_D3D->GetDeviceContext(), curvedPlane_, worldMatrix, viewMatrix, projectionMatrix, cloudTexture_->GetTexture(), perturbTexture_->GetTexture(), cloudScales_, cloudBrightness_, cloudSpeeds_))
	{
		return false;
	}

	//// Render the model using the light shader.
	m_D3D->EnableOpagueBlending();
	m_D3D->setDepthState(true);

	D3DXMatrixIdentity(&worldMatrix);

	if (!terrainShader_->Render(m_D3D->GetDeviceContext(), planeMesh_, worldMatrix, viewMatrix, projectionMatrix, m_Model->getTexture(), terrainScaler_, persistence_, octaves_, terrainSlopeHeight_, updateHeightmap_))
	{
		return false;
	}
	
	/*for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			D3DXMatrixTranslation(&worldMatrix, i * 2.0f, j * 2.0f, 0.0f);
			if (!pbrShader_.Render(m_D3D->GetDeviceContext(), m_Model, worldMatrix, viewMatrix, projectionMatrix, { 1.0f, 0.0f, 0.0f }, (float)i * 0.1f, (float)j * 0.1f))
			{
				return false;
			}
		}
	}*/

	D3DXMatrixIdentity(&worldMatrix);

	m_D3D->CullNone();
	m_D3D->EnableAdditiveBlending();
	if (!particleShader_->Render(m_D3D->GetDeviceContext(), instanceRenderer_, worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}
	m_D3D->CullBack();
	
	m_D3D->EnableOpagueBlending();
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_D3D->setBackBufferRenderTarget();
	postShader_.Render(m_D3D->GetDeviceContext(), &orthoMesh_, worldMatrix, baseViewMatrix, orthoProjMatrix, m_Camera->GetPosition(), (int)lightCount_, TonemapType::filmic, 1.0f, false, normLightDirection);

	D3DXMatrixIdentity(&worldMatrix);

	

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

void GraphicsClass::UpdateCameraInput(float delta)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	auto cameraRot = m_Camera->GetRotation();

	//Camera Translation in the x
	if (input_.IsKeyDown('A'))
	{
		float radians = cameraRot.y * 0.0174532925f;
		m_Camera->Move(cosf(radians) * -CAM_MOVE_SPEED, 0.0f, -sinf(radians) * -CAM_MOVE_SPEED);
		
	}
	else if (input_.IsKeyDown('D'))
	{
		float radians = cameraRot.y * 0.0174532925f;
		m_Camera->Move(cosf(radians) * CAM_MOVE_SPEED, 0.0f, -sinf(radians) * CAM_MOVE_SPEED);
	}

	//Camera Translation in the Y
	if (input_.IsKeyDown('Q'))
	{
		m_Camera->Move(0.0f, delta * CAM_MOVE_SPEED * 2.0f, 0.0f);
	}
	else if (input_.IsKeyDown('E'))
	{
		m_Camera->Move(0.0f, -delta * CAM_MOVE_SPEED * 2.0f, 0.0f);
	}

	//Camera Translation in the z
	if (input_.IsKeyDown('W'))
	{
		float radiansX = cameraRot.x * 0.0174532925f;
		float radians = cameraRot.y * 0.0174532925f;
		m_Camera->Move(sinf(radians) * CAM_MOVE_SPEED, -sinf(radiansX) * CAM_MOVE_SPEED, cosf(radians) * CAM_MOVE_SPEED);
	}
	else if (input_.IsKeyDown('S'))
	{
		float radiansX = cameraRot.x * 0.0174532925f;
		float radians = cameraRot.y * 0.0174532925f;
		m_Camera->Move(sinf(radians) * -CAM_MOVE_SPEED, -sinf(radiansX) * -CAM_MOVE_SPEED, cosf(radians) * -CAM_MOVE_SPEED);
	}

	//Reset translation ammounts to 0
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	//Camera Rotation in the x
	if (input_.IsKeyDown(VK_UP))
	{
		x = -delta * CAM_MOVE_SPEED * 2.0f;
	}
	else if (input_.IsKeyDown(VK_DOWN))
	{
		x = delta * CAM_MOVE_SPEED * 2.0f;
	}

	//Camera Rotation in the Y
	if (input_.IsKeyDown(VK_RIGHT))
	{
		y = delta * 5.0f;
	}
	else if (input_.IsKeyDown(VK_LEFT))
	{
		y = -delta * 5.0f;
	}

	m_Camera->Rotate(x, y, z);
}

static const wchar_t perlinScaleText[] = L"Perlin Scaler";
static const wchar_t persistanceText[] = L"Persistance";
static const wchar_t octavesText[] = L"Octaves";
static const wchar_t terrainTitleText[] = L"Terrain";

static const wchar_t cloudSpeedText1[] = L"Cloud Speed 1";
static const wchar_t cloudSpeedText2[] = L"Cloud Speed 2";
static const wchar_t cloudBrightnessText1[] = L"Cloud Brightness 1";
static const wchar_t cloudBrightnessText2[] = L"Cloud Brightness 2";
static const wchar_t cloudScaleText1[] = L"Cloud Scale 1";
static const wchar_t cloudScaleText2[] = L"Cloud Scale 2";

static const wchar_t lightCountText[] = L"Light Count";
static const wchar_t cloudTitleText[] = L"Clouds";

void GraphicsClass::OnPaint(HDC hdc)
{
	SelectObject(hdc, boldFont_);
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, 5, 20, terrainTitleText, ARRAYSIZE(terrainTitleText));
	TextOut(hdc, 5, 220, cloudTitleText, ARRAYSIZE(cloudTitleText));

	SelectObject(hdc, standardFont_);
	TextOut(hdc, 5, 40, perlinScaleText, ARRAYSIZE(perlinScaleText));
	TextOut(hdc, 5, 70, persistanceText, ARRAYSIZE(persistanceText));
	TextOut(hdc, 5, 100, octavesText, ARRAYSIZE(octavesText));
	TextOut(hdc, 5, 240, cloudBrightnessText1, ARRAYSIZE(cloudBrightnessText1));
	TextOut(hdc, 5, 270, cloudBrightnessText2, ARRAYSIZE(cloudBrightnessText2));
	TextOut(hdc, 5, 300, cloudScaleText1, ARRAYSIZE(cloudScaleText1));
	TextOut(hdc, 5, 330, cloudScaleText2, ARRAYSIZE(cloudScaleText2));
	TextOut(hdc, 5, 360, cloudSpeedText1, ARRAYSIZE(cloudSpeedText1));
	TextOut(hdc, 5, 390, cloudSpeedText2, ARRAYSIZE(cloudSpeedText2));
	TextOut(hdc, 5, 420, lightCountText, ARRAYSIZE(lightCountText));
}

void GraphicsClass::OnCommand(WPARAM wParam)
{
	for (auto fieldItt = editFields_.begin(); fieldItt != editFields_.end(); ++fieldItt)
	{
		(*fieldItt)->OnCommand(wParam);
		updateHeightmap_ = true;
	}
}

