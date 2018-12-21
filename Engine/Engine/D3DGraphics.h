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
	static ID3D11Buffer*			CreateBuffer(ID3D11Device* device, D3D11_USAGE usage, UINT byteWidth, UINT bindFlags, UINT cpuAccessFlags, UINT byteStride, UINT miscFlags, void* initData = nullptr)
	{
		ID3D11Buffer* buffer;
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = usage;
		bufferDesc.ByteWidth = byteWidth;
		bufferDesc.BindFlags = bindFlags;
		bufferDesc.CPUAccessFlags = cpuAccessFlags;
		bufferDesc.StructureByteStride = byteStride;
		bufferDesc.MiscFlags = miscFlags;
		if (initData)
		{
			D3D11_SUBRESOURCE_DATA instanceData;
			instanceData.pSysMem = initData;
			instanceData.SysMemPitch = 0;
			instanceData.SysMemSlicePitch = 0;
			device->CreateBuffer(&bufferDesc, &instanceData, &buffer);
		}
		else
		{
			device->CreateBuffer(&bufferDesc, 0, &buffer);
		}
		return buffer;
	}

	static ID3D11Texture2D*							CreateTexture2D(ID3D11Device* device, int width, int height)
	{
		ID3D11Texture2D* texture = nullptr;
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		device->CreateTexture2D(&textureDesc, 0, &texture);

		return texture;
	}

	static ID3D11ShaderResourceView*				CreateShaderResourceView(ID3D11Device* device, ID3D11Resource* resource)
	{
		ID3D11ShaderResourceView* srv = nullptr;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(resource, &srvDesc, &srv);
		return srv;
	}

	static ID3D11UnorderedAccessView*				CreateTextureUnorderedAccessView(ID3D11Device* device, ID3D11Resource* resource)
	{
		ID3D11UnorderedAccessView* uav = nullptr;
		D3D11_UNORDERED_ACCESS_VIEW_DESC descUAV;
		ZeroMemory(&descUAV, sizeof(descUAV));
		descUAV.Format = DXGI_FORMAT_UNKNOWN;
		descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		descUAV.Texture2D.MipSlice = 0;
		device->CreateUnorderedAccessView(resource, &descUAV, &uav);
		return uav;
	}

	static ID3D11UnorderedAccessView*				CreateBufferUnorderedAccessView(ID3D11Device* device, ID3D11Resource* resource, int instanceCount)
	{
		ID3D11UnorderedAccessView* uav = nullptr;
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = 0;
		uavDesc.Buffer.NumElements = instanceCount;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		device->CreateUnorderedAccessView(resource, &uavDesc, &uav);
		return uav;
	}

	static ID3D10Blob*								CompileShader(ID3D11Device* device, HWND hwnd, const wchar_t* filename, string shaderStage)
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
		buffer_ = D3DGraphics::CreateBuffer(device, D3D11_USAGE_DYNAMIC, sizeof(T), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0);
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