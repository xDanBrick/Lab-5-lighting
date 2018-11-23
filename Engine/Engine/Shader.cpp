////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Shader.h"

D3D11_INPUT_ELEMENT_DESC CreateInputElement(string name, DXGI_FORMAT format, UINT alignedByteOffset)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout;
	polygonLayout.SemanticName = name.c_str();
	polygonLayout.SemanticIndex = 0;
	polygonLayout.Format = format;
	polygonLayout.InputSlot = 0;
	polygonLayout.AlignedByteOffset = alignedByteOffset;
	polygonLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout.InstanceDataStepRate = 0;
	return polygonLayout;
}

Shader::Shader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
}


Shader::Shader(const Shader& other)
{
}


Shader::~Shader()
{
}

void Shader::Shutdown()
{
	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	return;
}

bool Shader::Render(ID3D11DeviceContext* deviceContext, Mesh* mesh)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	DrawMesh(deviceContext, mesh);

	return true;
}

bool Shader::InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilename)
{
	HRESULT result;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
    D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC variableBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;


	// Initialize the pointers this function will use to null.
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // Compile the vertex shader code.
	vertexShaderBuffer = D3DGraphics::CompileShader(device, hwnd, vsFilename, "vs_5_0");
	if(!vertexShaderBuffer)
	{
		return false;
	}

   pixelShaderBuffer = D3DGraphics::CompileShader(device, hwnd, psFilename, "ps_5_0");
	if(!pixelShaderBuffer)
	{
		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if(FAILED(result))
	{
		return false;
	}

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if(FAILED(result))
	{
		return false;
	}

	if (!CreateInputLayout(device, vertexShaderBuffer))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create a texture sampler state description.
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(result))
	{
		return false;
	}

	//Create the matrix buffer
	if(!matrixBuffer_.Initalise(device))
	{
		return false;
	}
	return true;
}

bool Shader::SetBaseParameters(ID3D11DeviceContext* deviceContext, Mesh* mesh, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	unsigned int bufferNumber;
	VariableBufferType* dataPtr3;

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	matrixBuffer_.getValuePtr()->world = worldMatrix;
	matrixBuffer_.getValuePtr()->view = viewMatrix;
	matrixBuffer_.getValuePtr()->projection = projectionMatrix;
	matrixBuffer_.Map(deviceContext);
	
	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	auto buffer = matrixBuffer_.getBuffer();
	// Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &buffer);

	mesh->Render(deviceContext);
	Render(deviceContext, mesh);
	return true;
}

bool Shader::CreateInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBlob)
{
	return SUCCEEDED(device->CreateInputLayout(VertexPosTexNorm::getInputElements(), VertexPosTexNorm::getInputElementCount(), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(),
		&m_layout));
}

void Shader::DrawMesh(ID3D11DeviceContext * context, Mesh* mesh)
{
	// Render the triangle.
	context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
}
