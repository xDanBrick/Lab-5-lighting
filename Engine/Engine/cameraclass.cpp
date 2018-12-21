////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"

const float CAM_MOVE_SPEED = 0.2f;

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}


D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}


D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}


void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw   = m_rotationY * 0.0174532925f;
	roll  = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

	return;
}


void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void CameraClass::Move(float x, float y, float z)
{
	m_positionX += x;
	m_positionY += y;
	m_positionZ += z;
}

void CameraClass::Rotate(float x, float y, float z)
{
	m_rotationX += x;
	m_rotationY += y;
	m_rotationZ += z;
	
}

void CameraClass::Update(InputClass& input)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	//Camera Translation in the x
	if (input.IsKeyDown('A'))
	{
		float radians = m_rotationY * 0.0174532925f;
		Move(cosf(radians) * -CAM_MOVE_SPEED, 0.0f, -sinf(radians) * -CAM_MOVE_SPEED);

	}
	else if (input.IsKeyDown('D'))
	{
		float radians = m_rotationY * 0.0174532925f;
		Move(cosf(radians) * CAM_MOVE_SPEED, 0.0f, -sinf(radians) * CAM_MOVE_SPEED);
	}

	//Camera Translation in the Y
	if (input.IsKeyDown('Q'))
	{
		Move(0.0f, CAM_MOVE_SPEED * 2.0f, 0.0f);
	}
	else if (input.IsKeyDown('E'))
	{
		Move(0.0f, -CAM_MOVE_SPEED * 2.0f, 0.0f);
	}

	//Camera Translation in the z
	if (input.IsKeyDown('W'))
	{
		Move(sinf(D3DXToRadian(m_rotationY)) * CAM_MOVE_SPEED, -sinf(D3DXToRadian(m_rotationX)) * CAM_MOVE_SPEED, cosf(D3DXToRadian(m_rotationY)) * CAM_MOVE_SPEED);
	}
	else if (input.IsKeyDown('S'))
	{
		Move(sinf(D3DXToRadian(m_rotationY)) * -CAM_MOVE_SPEED, -sinf(D3DXToRadian(m_rotationX)) * -CAM_MOVE_SPEED, cosf(D3DXToRadian(m_rotationY)) * -CAM_MOVE_SPEED);
	}

	//Reset translation ammounts to 0
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	//Camera Rotation in the x
	if (input.IsKeyDown(VK_UP))
	{
		x = -CAM_MOVE_SPEED * 2.0f;
	}
	else if (input.IsKeyDown(VK_DOWN))
	{
		x = CAM_MOVE_SPEED * 2.0f;
	}

	//Camera Rotation in the Y
	if (input.IsKeyDown(VK_RIGHT))
	{
		y = 5.0f;
	}
	else if (input.IsKeyDown(VK_LEFT))
	{
		y = -5.0f;
	}

	Rotate(x, y, z);

	Render();
}
