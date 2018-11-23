#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <string>
#include <fstream>
using namespace std;

#define DeleteNull(x) if(x) { delete x; x = nullptr; }
#define ReleaseNull(x) if(x) { x->Release(); x = nullptr; }

namespace D3DGraphics
{
	static ID3D11Buffer*			CreateConstantBuffer(ID3D11Device* device, unsigned int size)
	{
		ID3D11Buffer* buffer;
		D3D11_BUFFER_DESC bufferDesc;
		// Setup the description of the light dynamic constant buffer that is in the pixel shader.
		// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = size;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
		if (SUCCEEDED(device->CreateBuffer(&bufferDesc, NULL, &buffer)))
		{
			return buffer;
		}
		return nullptr;
	};

	static ID3D11Buffer*			CreateStructuredBuffer(ID3D11Device* device, UINT structedByteSize, UINT byteWidth, void* initData)
	{
		ID3D11Buffer* structuredBuffer = nullptr;
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = byteWidth;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = structedByteSize;

		if (initData)
		{
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = initData;
			device->CreateBuffer(&desc, &InitData, &structuredBuffer);
		}
		else
		{
			device->CreateBuffer(&desc, NULL, &structuredBuffer);
		}
		return structuredBuffer;
	}

	static ID3D11ShaderResourceView*				CreateBufferSRV(ID3D11Device* device, ID3D11Buffer* buffer, int numOfElements)
	{

		ID3D11ShaderResourceView* shaderResourceView = nullptr;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.BufferEx.Flags = 0;
		srvDesc.BufferEx.NumElements = numOfElements;

		device->CreateShaderResourceView(buffer, &srvDesc, &shaderResourceView);
		return shaderResourceView;
	}

	static ID3D11UnorderedAccessView*				CreateBufferUAV(ID3D11Device* device, ID3D11Buffer* buffer, int numElements)
	{
		ID3D11UnorderedAccessView* pStructuredBufferUAV;
		// Create the UAV for the structured buffer
		D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc;
		sbUAVDesc.Buffer.FirstElement = 0;
		sbUAVDesc.Buffer.Flags = 0;
		sbUAVDesc.Buffer.NumElements = numElements;
		sbUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		sbUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		device->CreateUnorderedAccessView(buffer, &sbUAVDesc, &pStructuredBufferUAV);
		return pStructuredBufferUAV;
	}

	static ID3D10Blob*				CompileShader(ID3D11Device* device, HWND hwnd, const wchar_t* filename, string shaderStage)
	{
		ID3D10Blob* errorMessage;
		ID3D10Blob* shaderBuffer;
		// Compile the vertex shader code.
		if (FAILED(D3DX11CompileFromFile(filename, NULL, NULL, "main", shaderStage.c_str(), D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
			&shaderBuffer, &errorMessage, NULL)))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage)
			{
				char* compileErrors;
				unsigned long bufferSize, i;
				ofstream fout;

				// Get a pointer to the error message text buffer.
				compileErrors = (char*)(errorMessage->GetBufferPointer());

				// Get the length of the message.
				bufferSize = errorMessage->GetBufferSize();

				// Open a file to write the error message to.
				fout.open("shader-error.txt");

				// Write out the error message.
				for (i = 0; i< bufferSize; i++)
				{
					fout << compileErrors[i];
				}

				// Close the file.
				fout.close();

				// Release the error message.
				errorMessage->Release();
				errorMessage = 0;

				// Pop a message up on the screen to notify the user to check the text file for compile errors.
				MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", filename, MB_OK);
			}
			// If there was nothing in the error message then it simply could not find the shader file itself.
			else
			{
				MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);
			}
			return nullptr;
		}
		return shaderBuffer;
	}
};

template <class T>
class ConstantBuffer
{
public:
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}
	void operator delete(void* p)
	{
		_mm_free(p);
	}

	ConstantBuffer()
	{
		buffer_ = nullptr;
	};

	~ConstantBuffer()
	{
		ReleaseNull(buffer_);
	};

	bool Initalise(ID3D11Device* device)
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(T);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		return SUCCEEDED(device->CreateBuffer(&bufferDesc, nullptr, &buffer_));
	}

	T*			getValuePtr() { return &value_; };

	bool		Map(ID3D11DeviceContext* deviceContext)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(deviceContext->Map(buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		*(T*)mappedResource.pData = value_;
		// Unlock the constant buffer.
		deviceContext->Unmap(buffer_, 0);
		return true;
	};
	ID3D11Buffer*		getBuffer() const { return buffer_; };
private:
	ID3D11Buffer*		buffer_;
	T					value_;
};