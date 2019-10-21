#include "pch.h"
#include "Visuals.h"
#include "Icetrix/Menu/ImGuiDesign.h"
#include "imgui.h"
#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx11.h"
#include "Icetrix/Platform/DirectX/Helper.h"
#include "d3d11.h"
#pragma comment(lib, "d3d11.lib")

static bool bShow = false;
LONG_PTR OriginalWndProc = NULL;
WNDPROC WndProcHandler = NULL;
HWND hwnd = NULL;
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Visuals::OnAttach()
{
	auto& modules = Icetrix::Process::GetInstance()->modules();
	/*
	void* SwapChain[18];

	Sleep(3000);
	__try
	{
		if (Icetrix::Platform::DirectX::GetD3D11SwapchainDeviceContext(SwapChain, sizeof(SwapChain), nullptr, 0, nullptr, 0))
		{
	*/
			f_Present present = reinterpret_cast<f_Present>(modules.GetModule(L"dxgi.dll")->baseAddress + 0x4300); // IDXGISwapChainPresent
			//f_Present present = reinterpret_cast<f_Present>(SwapChain[8]); // IDXGISwapChainPresent
			
			if (d_Present.Hook(present, &Visuals::h_Present, blackbone::HookType::HWBP))
				LOG_INFO("Hooked 'Present'");
			else
				LOG_ERROR("[!] Failed to Hook: Present");
	/*
		}
		else
		{
			LOG_ERROR("[!] Failed to find: SwapChain/Device/Context");
		}
	}
	__finally
	{
		LOG_ERROR("[!] Failed to find: SwapChain/Device/Context, exception thrown");
	}
	*/
}

void Visuals::OnDetach()
{
	if (d_Present.Restore())
	{
		std::cout << "[+] Restored 'Present'" << std::endl;
	}
	else
	{
		std::cout << "[!] Failed to restore Present" << std::endl;
	}

	SetWindowLongPtrA(hwnd, GWLP_WNDPROC, OriginalWndProc);

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	//ImGui::DestroyContext();
}

void __fastcall Visuals::h_Present(IDXGISwapChain* &pChain, UINT &SyncInterval, UINT &Flags)
{
	static bool init = false;
	static ID3D11Device* g_pd3dDevice = NULL;
	static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	static ID3D11RenderTargetView* ppRTView = NULL;

	if (!init)
	{
		std::cout << "[+] Present::Init" << std::endl;
		HRESULT ret = pChain->GetDevice(__uuidof(ID3D11Device), (PVOID*)&g_pd3dDevice);

		if (SUCCEEDED(ret))
			(*&g_pd3dDevice)->GetImmediateContext(&g_pd3dDeviceContext);

		// get window
		DXGI_SWAP_CHAIN_DESC pDesc;
		pChain->GetDesc(&pDesc);
		hwnd = pDesc.OutputWindow;

		if (!OriginalWndProc)
			OriginalWndProc = GetWindowLongPtr(hwnd, GWLP_WNDPROC);

		if (!WndProcHandler)
			WndProcHandler = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)&Visuals::hWndProc);

		// setup imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
		Icetrix::Menu::ApplyIcetrixDesign();

		ID3D11Texture2D* pBackBuffer;
		pChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		
		if (pBackBuffer)
		{
			g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &ppRTView);
			pBackBuffer->Release();
		}

		init = true;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	auto features = Icetrix::Features::GetInstance();
	if (bShow)
	{
		if (ImGui::Begin("Icetrix vagreany unpx", &bShow, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (ImGui::Button("Activate"))
			{
				// TODO: Replace with Features class method
				for (auto feature : features->All())
				{
					feature->enabled = true;
				}
			}

			if (ImGui::Button("Deactivate"))
			{
				for (auto feature : features->All())
				{
					feature->enabled = false;
				}
			}

			for (auto feature : features->All())
			{
				ImGui::Checkbox(feature->label, &feature->enabled);
			}
		}
		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();

	g_pd3dDeviceContext->OMSetRenderTargets(1, &ppRTView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK Visuals::hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	POINT mPos;
	GetCursorPos(&mPos);
	ScreenToClient(hWnd, &mPos);
	io.MousePos.x = static_cast<float>(mPos.x);
	io.MousePos.y = static_cast<float>(mPos.y);

	if (uMsg == WM_KEYUP)
	{
		if (wParam == VK_INSERT)
		{
			bShow = !bShow;
		}
	}

	if (bShow)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		io.MouseDrawCursor = true;
		return true;
	}

	io.MouseDrawCursor = false;
	return CallWindowProc(WndProcHandler, hWnd, uMsg, wParam, lParam);
}

