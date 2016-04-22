#include "main.h"

#include "Graphics.h"

#include "ResourceManager.h"


namespace games
{
	Graphics::Graphics()
	{
	}

	Graphics::~Graphics()
	{
		SafeRelease(&m_pDirect2dFactory);
		SafeRelease(&m_pDWriteFactory);
		SafeRelease(&m_pRenderTarget);
		//DestroyDeviceResource();
		DestroyRenderCallback();
	}

	HRESULT Graphics::initialize(HWND hwnd, bool changeDPI)
	{
		HRESULT hr;

		// initialize device-indpendent resources, such
		// as the Direct2D factory.
		hr = CreateDeviceIndependentResources();

		if (SUCCEEDED(hr))
		{
			if (hwnd != NULL)
			{
				hwnd_ = hwnd;

				RECT rect;
				GetWindowRect(hwnd, &rect);
				UINT height_ = rect.bottom - rect.top;
				UINT width_ = rect.right - rect.left;

				if (changeDPI)
				{
					// Because the CreateWindow function takes its size in pixels,
					// obtain the system DPI and use it to scale the window size.
					FLOAT dpiX, dpiY;

					// The factory returns the current system DPI. This is also the value it will use
					// to create its own windows.
					m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

					height_ = static_cast<UINT>(ceil(height_ * dpiY / 96.f));
					width_ = static_cast<UINT>(ceil(width_ * dpiX / 96.0f));
				}

				RECT rt;
				HWND hDesk = GetDesktopWindow();
				GetClientRect(hDesk, &rt);

				SetWindowPos(
					hwnd,
					NULL,
					(rt.right - rt.left - width_) / 2,
					(rt.bottom - rt.top - height_) / 2,
					width_,
					height_,
					SWP_NOZORDER
					);
				
			}
			else
				hr = S_FALSE;
		}

		return hr;
	}

	Graphics & Graphics::Instance()
	{
		static Graphics grahpics;
		return grahpics;
	}

	HRESULT Graphics::CreateDeviceIndependentResources()
	{
		HRESULT hr = S_OK;

		// Create a Direct2D factory.
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
		if (SUCCEEDED(hr))
		{
			hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory),
				reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
				);
		}
	
		return hr;
	}

	HRESULT Graphics::CreateDeviceResources()
	{
		HRESULT hr = S_OK;

		if (!m_pRenderTarget)
		{
			RECT rc;
			GetClientRect(hwnd_, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top
				);

			// Create a Direct2D render target.
			hr = m_pDirect2dFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(hwnd_, size),
				&m_pRenderTarget
				);
		}

		return hr;
	}

	void Graphics::DiscardDeviceResources()
	{
		DestroyDeviceResource();
		SafeRelease(&m_pRenderTarget);
	}

	bool Graphics::CheckBitmapResource(const wstring & str)
	{
		if (bitmapList.find(str) != bitmapList.end())
			return true;
		else
			return false;
	}

	bool Graphics::CheckFoontResource(FontHandle handle)
	{
		if (!textFormatList.find(handle))
			return false;
		else
			return true;
	}

	HRESULT Graphics::OnRender()
	{
		HRESULT hr = S_OK;

		hr = CreateDeviceResources();
		if (SUCCEEDED(hr))
		{
			m_pRenderTarget->BeginDraw();
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			renderState = true;

			//D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

			//int width_ = static_cast<int>(rtSize.width_);
			//int height_ = static_cast<int>(rtSize.height_);

			for (auto &i : firstRenderCallbackList)
				i();

			for (auto &i : secondRenderCallbackList)
				i();

			for (auto &i : thirdRenderCallbackList)
				i();

			hr = m_pRenderTarget->EndDraw();
			renderState = false;
		}
		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}
		return hr;
	}


	void Graphics::OnResize(UINT width_, UINT height_)
	{
		if (m_pRenderTarget)
		{
			// Note: This method can fail, but it's okay to ignore the
			// error here, because the error will be returned again
			// the next time EndDraw is called.
			m_pRenderTarget->Resize(D2D1::SizeU(width_, height_));
		}
	}

	bool Graphics::GetBitmapSize(const wstring & path, D2D1_SIZE_F &rect)
	{
		ID2D1Bitmap *pBitmap = LoadBitmap(path);
		if (pBitmap == nullptr)
			return false;
		rect = pBitmap->GetSize();
		return true;
	}

	void Graphics::DestroyDeviceResource()
	{
		for (auto &i : bitmapList)
		{
			ID2D1Bitmap *pBitmap = reinterpret_cast<ID2D1Bitmap*>(i.second);
			SafeRelease(&pBitmap);
		}
		bitmapList.clear();
	}

	void Graphics::DestroyRenderCallback()
	{
		firstRenderCallbackList.clear();
		secondRenderCallbackList.clear();
		thirdRenderCallbackList.clear();
	}

	void Graphics::RegisterRenderCallback(RenderLevel level, std::function<void()> callback)
	{
		switch (level)
		{
		case RenderLevel::FIRST:
			firstRenderCallbackList.push_back(std::move(callback));
			break;
		case RenderLevel::SECOND:
			secondRenderCallbackList.push_back(std::move(callback));
			break;
		case RenderLevel::THIRED:
			thirdRenderCallbackList.push_back(std::move(callback));
			break;
		default:
			break;
		}
	}

	FontHandle Graphics::CreateFontObject(const wchar_t *font_name, float size, const wchar_t *local)
	{
		assert(font_name != nullptr && local != nullptr);

		if (!m_pDWriteFactory)
			return NULL;

		IDWriteTextFormat *pFont = nullptr;
		HRESULT hr = m_pDWriteFactory->CreateTextFormat(
				font_name, //L"Gabriola",                   // Font family name
				NULL,                          // Font collection(NULL sets it to the system font collection)
				DWRITE_FONT_WEIGHT_REGULAR,    // Weight
				DWRITE_FONT_STYLE_NORMAL,      // Style
				DWRITE_FONT_STRETCH_NORMAL,    // Stretch
				size,                         // Size    
				local,                      // Local
				&pFont						   // Pointer to recieve the created object
				);
		if (SUCCEEDED(hr))
		{
			return textFormatList.insert(pFont);
		}
		else
			return NULL;
	}

	void Graphics::FontCenter(FontHandle handle)
	{
		if (CheckFoontResource(handle))
		{
			IDWriteTextFormat *text = textFormatList[handle];
			text->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);           //水平居中
			text->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); //段落居中
		}
	}

	bool Graphics::DestroyFontObject(FontHandle handle)
	{
		IDWriteTextFormat *pFont = nullptr;
		if (textFormatList.find(handle))
		{
			textFormatList.load(handle, pFont);
			SafeRelease(&pFont);
			return textFormatList.remove(handle);
		}
		else
		{
			return false;
		}
	}

	void Graphics::DestroyAllFontObject()
	{
		for (auto &i : textFormatList)
		{
			IDWriteTextFormat *pFont = reinterpret_cast<IDWriteTextFormat*>(i.second);
			SafeRelease(&pFont);
		}
		textFormatList.clear();
	}

	bool Graphics::DrawLine(
		D2D1_POINT_2F from, 
		D2D1_POINT_2F to, 
		INT32 rgb,
		FLOAT alpha,
		FLOAT strokeWidth
		)
	{
		if (!m_pRenderTarget || !renderState)
			return false;

		HRESULT hr = S_FALSE;
		ID2D1SolidColorBrush *pBrush = nullptr;
		if (m_pRenderTarget)
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(rgb, alpha),
				&pBrush
				);
		}

        if (SUCCEEDED(hr))
        {
            m_pRenderTarget->DrawLine(from, to, pBrush, strokeWidth);
            pBrush->Release();
            return true;
        }
        return false;
	}

	bool Graphics::DrawRectangle(
		D2D1_RECT_F & rect, 
		INT32 rgb, 
		FLOAT alpha,
		FLOAT strokeWidth
		)
	{
		if (!m_pRenderTarget || !renderState)
			return false;

		HRESULT hr = S_FALSE;
		ID2D1SolidColorBrush *pBrush = nullptr;
		if (m_pRenderTarget)
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(rgb, alpha),
				&pBrush
				);
		}

        if (SUCCEEDED(hr))
        {
            m_pRenderTarget->DrawRectangle(rect, pBrush, strokeWidth);
            pBrush->Release();
            return true;
        }
        return false;
	}

	bool Graphics::FillRectangle(D2D1_RECT_F & rect, INT32 rgb, FLOAT alpha)
	{
		HRESULT hr = S_FALSE;
		ID2D1SolidColorBrush *pBrush = nullptr;
		if (m_pRenderTarget)
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(rgb, alpha),
				&pBrush
				);
		}

        if (SUCCEEDED(hr))
        {
            m_pRenderTarget->FillRectangle(rect, pBrush);
            pBrush->Release();
            return true;
        }
        return false;
	}

	bool Graphics::FillRoundedRectangle(
		const D2D1_ROUNDED_RECT & roundedRect, 
		INT32 rgb, 
		FLOAT alpha
		)
	{
		if (!m_pRenderTarget || !renderState)
			return false;

		HRESULT hr = S_FALSE;
		ID2D1SolidColorBrush *pBrush = nullptr;
		if (m_pRenderTarget)
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(rgb, alpha),
				&pBrush
				);
		}

        if (SUCCEEDED(hr))
        {
            m_pRenderTarget->FillRoundedRectangle(roundedRect, pBrush);
            pBrush->Release();
            return true;
        }
        return false;
	}

	bool Graphics::DrawRoundedRectangle(
		const D2D1_ROUNDED_RECT & roundedRect,
		INT32 rgb,
		FLOAT alpha,
		FLOAT strokeWidth
		)
	{
		if (!m_pRenderTarget || !renderState)
			return false;

		HRESULT hr = S_FALSE;
		ID2D1SolidColorBrush *pBrush = nullptr;
		if (m_pRenderTarget)
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(rgb, alpha),
				&pBrush
				);
		}

        if (SUCCEEDED(hr))
        {
            m_pRenderTarget->DrawRoundedRectangle(roundedRect, pBrush, strokeWidth);
            pBrush->Release();
            return true;
        }
        return false;
	}

	bool Graphics::DrawEllipse(
		const D2D1_ELLIPSE & ellipse, 
		INT32 rgb,
		FLOAT alpha,
		FLOAT strokeWidth
		)
	{
		if (!m_pRenderTarget || !renderState)
			return false;

		HRESULT hr = S_FALSE;
		ID2D1SolidColorBrush *pBrush = nullptr;
		if (m_pRenderTarget)
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(rgb, alpha),
				&pBrush
				);
		}

        if (SUCCEEDED(hr))
        {
            m_pRenderTarget->DrawEllipse(ellipse, pBrush, strokeWidth);
            pBrush->Release();
            return true;
        }
        return false;
	}

	bool Graphics::FillEllipse(
		const D2D1_ELLIPSE & ellipse,
		INT32 rgb,
		FLOAT alpha
		)
	{
		if (!m_pRenderTarget || !renderState)
			return false;

		HRESULT hr = S_FALSE;
		ID2D1SolidColorBrush *pBrush = nullptr;
		if (m_pRenderTarget)
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(rgb, alpha),
				&pBrush
				);
		}

        if (SUCCEEDED(hr))
        {
            m_pRenderTarget->FillEllipse(ellipse, pBrush);
            pBrush->Release();
            return true;
        }
        return false;
	}

	bool Graphics::DrawText(
		const wchar_t * wstring, 
		UINT wstringLength, 
		FontHandle textFormat, 
		D2D1_RECT_F &layoutRect, 
		INT32 rgb,
		FLOAT alpha
		)
	{
		if (!CheckFoontResource(textFormat) ||
			!m_pRenderTarget || !renderState)
			return false;

		HRESULT hr = S_FALSE;
		ID2D1SolidColorBrush *pBrush = nullptr;
		if (m_pRenderTarget)
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(rgb, alpha),
				&pBrush
				);
		}

        if (SUCCEEDED(hr))
        {
            IDWriteTextFormat *pFormat;
            textFormatList.load(textFormat, pFormat);

            m_pRenderTarget->DrawText(
                wstring,
                wstringLength,
                pFormat,
                layoutRect,
                pBrush
            );
            pBrush->Release();
            return true;
        }
        return false;
	}

	bool Graphics::DrawBitmap(
		const wstring & path, 
		D2D1_RECT_F &destinationRectangle,
		FLOAT opacity
		)
	{
		if (!m_pRenderTarget || !renderState)
			return false;

		ID2D1Bitmap *pBitmap = LoadBitmap(path);
		if (pBitmap == nullptr)
			return false;
		
		m_pRenderTarget->DrawBitmap(pBitmap, destinationRectangle, opacity);
		return true;
	}

	bool Graphics::DrawBitmap(
		const wstring & path,
		D2D1_RECT_F &destinationRectangle,
		D2D1_RECT_F &sourceRectangle,
		FLOAT opacity
		)
	{
		if (!m_pRenderTarget || !renderState)
			return false;

		ID2D1Bitmap *pBitmap = LoadBitmap(path);
		if (pBitmap == nullptr)
			return false;

		m_pRenderTarget->DrawBitmap(
			pBitmap, 
			destinationRectangle, 
			opacity, 
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, 
			sourceRectangle
			);
		return true;
	}
    
    bool Graphics::DrawRotationBitmap(
        const wstring & path,
        D2D1_RECT_F &destinationRectangle,
        FLOAT rotation,
        D2D1_POINT_2F point,
        FLOAT opacity
        )
    {
        if (!m_pRenderTarget || !renderState)
            return false;

        ID2D1Bitmap *pBitmap = LoadBitmap(path);
        if (pBitmap == nullptr)
            return false;

        // 对render target进行旋转变换
        m_pRenderTarget->SetTransform(
            D2D1::Matrix3x2F::Rotation(rotation, point)
            );

        m_pRenderTarget->DrawBitmap(pBitmap, destinationRectangle, opacity);

        m_pRenderTarget->SetTransform(
            D2D1::Matrix3x2F::Rotation(0.f, D2D1::Point2F(0.f, 0.f))
            );
        return true;
    }

    bool Graphics::DrawRotationBitmap(
        const wstring & path,
        D2D1_RECT_F &destinationRectangle,
        D2D1_RECT_F &sourceRectangle,
        FLOAT rotation,
        D2D1_POINT_2F point,
        FLOAT opacity
        )
    {
        if (!m_pRenderTarget || !renderState)
            return false;

        ID2D1Bitmap *pBitmap = LoadBitmap(path);
        if (pBitmap == nullptr)
            return false;

        // 对render target进行旋转变换
        m_pRenderTarget->SetTransform(
            D2D1::Matrix3x2F::Rotation( rotation, point)
            );

        m_pRenderTarget->DrawBitmap(
            pBitmap,
            destinationRectangle,
            opacity,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
            sourceRectangle
            );

        m_pRenderTarget->SetTransform(
            D2D1::Matrix3x2F::Rotation(0.f, D2D1::Point2F(0.f, 0.f))
            );
        return true;
    }

	HRESULT Graphics::LoadBitmapFromFile(
        PCWSTR uri,
		UINT destinationWidth,
		UINT destinationHeight,
		ID2D1Bitmap **ppBitmap
		)
	{
		HRESULT hr = S_OK;

		IWICBitmapDecoder *pDecoder = NULL;
		IWICBitmapFrameDecode *pSource = NULL;
		IWICStream *pStream = NULL;
		IWICFormatConverter *pConverter = NULL;
		IWICBitmapScaler *pScaler = NULL;

		IWICImagingFactory* m_pIWICFactory = nullptr;
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_pIWICFactory)
			);

		if (!m_pIWICFactory || !m_pRenderTarget)
			return S_FALSE;

		hr = m_pIWICFactory->CreateDecoderFromFilename(
			uri,
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
			);

		if (SUCCEEDED(hr))
		{
			// Create the initial frame_.
			hr = pDecoder->GetFrame(0, &pSource);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pIWICFactory->CreateFormatConverter(&pConverter);
		}

		// If a new width_ or height_ was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (SUCCEEDED(hr) && (destinationWidth != 0 || destinationHeight != 0))
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = m_pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
						);
				}
				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pScaler,
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						NULL,
						0.f,
						WICBitmapPaletteTypeMedianCut
						);
				}
			}
		}
		else if (SUCCEEDED(hr))
		{
			//initialize Converter
			hr = pConverter->Initialize(
				pSource,                          // Input bitmap to convert
				GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
				WICBitmapDitherTypeNone,         // Specified dither pattern
				NULL,                            // Specify a particular palette 
				0.f,                             // Alpha threshold
				WICBitmapPaletteTypeCustom       // Palette translation type
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a Direct2D bitmap from the WIC bitmap.
			hr = m_pRenderTarget->CreateBitmapFromWicBitmap(
				pConverter,
				NULL,
				ppBitmap
				);
		}

		SafeRelease(&m_pIWICFactory);
		SafeRelease(&pDecoder);
		SafeRelease(&pSource);
		SafeRelease(&pStream);
		SafeRelease(&pConverter);
		SafeRelease(&pScaler);

		return hr;
	}

	ID2D1Bitmap * Graphics::LoadBitmap(const wstring & path)
	{
		ID2D1Bitmap *pBitmap = nullptr;
		if (!CheckBitmapResource(path))
		{
			LoadBitmapFromFile(path.c_str(), 0, 0, &pBitmap);
			bitmapList[path] = pBitmap;
		}
		else
		{
			pBitmap = bitmapList.find(path)->second;
		}
		return pBitmap;
	}

}