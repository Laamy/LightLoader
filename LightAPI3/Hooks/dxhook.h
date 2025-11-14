#pragma once

#include "Libs/kiero/kiero.h"

#include <polyhook2/IHook.hpp>

typedef kiero::RenderType::Enum DeviceType;

//https://github.com/DrNseven/D3D12-Hook-ImGui/blob/master/main.cpp
namespace DxComps {
	void* __o__present;
	void* __o__resize;

	bool passWndProc = false;

	auto initCtx = new std::once_flag();

	// dx12 resources
	void* __o__executeCommandLists;
	ID3D12Device*				d3d12Device = nullptr;
	ID3D12DescriptorHeap*		d3d12DescriptorHeapBackBuffers;
	ID3D12DescriptorHeap*		d3d12DescriptorHeapImGuiRender;
	ID3D12GraphicsCommandList*	d3d12CommandList;
	ID3D12CommandQueue*			d3d12CommandQueue;

	struct _FrameContext {
		ID3D12CommandAllocator*		CommandAllocator;
		ID3D12Resource*				Resource;
		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle;
	};

	uint8_t			BuffersCounts = -1;
	_FrameContext*	FrameContext;
	
	// dx11 resourcees
	ID3D11Device*			d3d11Device = nullptr;
	ID3D11RenderTargetView* d3d11Target = nullptr;
	ID3D11Texture2D*		d3d11BackBuffer = nullptr;
	ID3D11DeviceContext*	d3d11Context = nullptr;

	// hacky
	static void ResetOnceFlag() { initCtx = new std::once_flag(); }
}

void SetupImGuiStyle()
{
	// Moonlight style by deathsu/madam-herta
	// https://github.com/Madam-Herta/Moonlight/
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	//style.DisabledAlpha = 1.0f;
	style.WindowPadding = ImVec2(12.0f, 12.0f);
	style.WindowRounding = 11.5f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(20.0f, 20.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(20.0f, 3.400000095367432f);
	style.FrameRounding = 11.89999961853027f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(4.300000190734863f, 5.5f);
	style.ItemInnerSpacing = ImVec2(7.099999904632568f, 1.799999952316284f);
	style.CellPadding = ImVec2(12.10000038146973f, 9.199999809265137f);
	style.IndentSpacing = 0.0f;
	style.ColumnsMinSpacing = 4.900000095367432f;
	style.ScrollbarSize = 11.60000038146973f;
	style.ScrollbarRounding = 15.89999961853027f;
	style.GrabMinSize = 3.700000047683716f;
	style.GrabRounding = 20.0f;
	style.TabRounding = 0.0f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.2745098173618317f, 0.3176470696926117f, 0.4509803950786591f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.09250493347644806f, 0.100297249853611f, 0.1158798336982727f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1120669096708298f, 0.1262156516313553f, 0.1545064449310303f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.9725490212440491f, 1.0f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.971993625164032f, 1.0f, 0.4980392456054688f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.7953379154205322f, 0.4980392456054688f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1821731775999069f, 0.1897992044687271f, 0.1974248886108398f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1545050293207169f, 0.1545048952102661f, 0.1545064449310303f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.1414651423692703f, 0.1629818230867386f, 0.2060086131095886f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1072951927781105f, 0.107295036315918f, 0.1072961091995239f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.1293079704046249f, 0.1479243338108063f, 0.1931330561637878f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1459212601184845f, 0.1459220051765442f, 0.1459227204322815f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.9725490212440491f, 1.0f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.999999463558197f, 1.0f, 0.9999899864196777f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1249424293637276f, 0.2735691666603088f, 0.5708154439926147f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8841201663017273f, 0.7941429018974304f, 0.5615870356559753f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.9570815563201904f, 0.9570719599723816f, 0.9570761322975159f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.9356134533882141f, 0.9356129765510559f, 0.9356223344802856f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.266094446182251f, 0.2890366911888123f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
}

void ImguiSetup() {
	ImGui::CreateContext();

	SetupImGuiStyle();
}

void testRender() {
	if (!ImGui::GetCurrentContext())
		return;

	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	D3DRenderEvent dispatch;
	dispatch.d3d12Device = DxComps::d3d12Device;
	dispatch.d3d11Device = DxComps::d3d11Device;
	GameEvents::dispatch(&dispatch);

	if (!lightConfig.GetOrDefault<bool>("debug", false))
		return;

	//ImGui::ShowDemoWindow();

	if (ImGui::BeginMainMenuBar()) {
		// NOTE: leave this here cuz i have no fucking clue how to use imgui (not off by heart)
		if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("Basic utils", NULL, false, false);
			//if (ImGui::MenuItem("New")) {}
			//if (ImGui::MenuItem("Open", "Ctrl+O")) {}
			//if (ImGui::MenuItem("Save", "Ctrl+S")) {}
			//if (ImGui::MenuItem("Save As..")) {}
			//
			//ImGui::Separator();
			//if (ImGui::BeginMenu("Options")) {
			//	static bool enabled = true;
			//	ImGui::MenuItem("Enabled", "", &enabled);
			//	ImGui::BeginChild("child", ImVec2(0, 60), true);
			//	for (int i = 0; i < 10; i++)
			//		ImGui::Text("Scrolling Text %d", i);
			//	ImGui::EndChild();
			//	static float f = 0.5f;
			//	static int n = 0;
			//	ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
			//	ImGui::InputFloat("Input", &f, 0.1f);
			//	ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
			//	ImGui::EndMenu();
			//}
			//
			//if (ImGui::BeginMenu("Colors")) {
			//	float sz = ImGui::GetTextLineHeight();
			//	for (int i = 0; i < ImGuiCol_COUNT; i++) {
			//		const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
			//		ImVec2 p = ImGui::GetCursorScreenPos();
			//		ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
			//		ImGui::Dummy(ImVec2(sz, sz));
			//		ImGui::SameLine();
			//		ImGui::MenuItem(name);
			//	}
			//	ImGui::EndMenu();
			//}
			//
			//if (ImGui::BeginMenu("Options"))
			//{
			//	static bool b = true;
			//	ImGui::Checkbox("SomeOption", &b);
			//	ImGui::EndMenu();
			//}

			if (ImGui::MenuItem("Quit", "Alt+F4"))
				PostMessage(GameConfig::getWindow(), WM_QUIT, 0, 0);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Mods")) {
			if (ModAPI::Mods.empty())
				ImGui::MenuItem(std::format("No mod(s) loaded").c_str(), NULL, false, false);
			else {
				for (auto mod : ModAPI::Mods) {
					if (ImGui::BeginMenu(mod->Path.c_str())) {
						ImGui::MenuItem(std::format("Loaded: {}", mod->IsLoaded ? "Yes" : "No").c_str(), NULL, false, false);
						ImGui::MenuItem(std::format("ModAPI: {}", mod->IsSupported ? "Yes" : "No").c_str(), NULL, false, false);
						ImGui::Text(std::format("Module Handle: 0x{:X}", reinterpret_cast<uintptr_t>(mod->Handle)).c_str());

						if (mod->IsSupported) {
							if (ImGui::BeginMenu("Mod Config")) {
								if (mod->Config->Settings.empty())
									ImGui::MenuItem(std::format("No mod configurations").c_str(), NULL, false, false);
								else {
									for (auto& [key, value] : mod->Config->Settings) {
										ImGui::Text(std::format("{}: {}", key, value).c_str());
									}
								}
								ImGui::EndMenu();
							}
						}
						ImGui::EndMenu();
					}
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void executeCommandListD3D12(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists) {
	if (!DxComps::d3d12CommandQueue)
		DxComps::d3d12CommandQueue = queue;

	PLH::FnCast(DxComps::__o__executeCommandLists, executeCommandListD3D12)(queue, NumCommandLists, ppCommandLists);
}

void DirectReset(IDXGISwapChain3* pSwapChain, DeviceType devType = DeviceType::None) {
	DeviceType deviceType = devType;

	std::print("Resizing d3d\n");

	if (deviceType == DeviceType::None) {
		if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&DxComps::d3d11Device))))
			deviceType = DeviceType::D3D11;

		if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&DxComps::d3d12Device))))
			deviceType = DeviceType::D3D12;
	}

	if (deviceType == DeviceType::D3D11) {
		// reinit allat bs
		DxComps::d3d11Device->GetImmediateContext(&DxComps::d3d11Context);

		pSwapChain->GetBuffer(0, IID_PPV_ARGS(&DxComps::d3d11BackBuffer));

		DxComps::d3d11Device->CreateRenderTargetView(DxComps::d3d11BackBuffer, nullptr, &DxComps::d3d11Target);
		DxComps::d3d11BackBuffer->Release();

		ImGui_ImplWin32_Init(GameConfig::getWindow());
		ImGui_ImplDX11_Init(DxComps::d3d11Device, DxComps::d3d11Context);
	}
	
	if (deviceType == DeviceType::D3D12)
	{
		std::print("Resizing d3d12\n");
		const auto RTVDescriptorSize = DxComps::d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = DxComps::d3d12DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();

		for (size_t i = 0; i < DxComps::BuffersCounts; i++) {
			ID3D12Resource* pBackBuffer = nullptr;
			DxComps::FrameContext[i].DescriptorHandle = RTVHandle;
			pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			DxComps::d3d12Device->CreateRenderTargetView(pBackBuffer, nullptr, RTVHandle);
			DxComps::FrameContext[i].Resource = pBackBuffer;
			RTVHandle.ptr += RTVDescriptorSize;
		}
	}
}

HRESULT PresentDetour(IDXGISwapChain3* pSwapChain, UINT syncInterval, UINT flags) {
#define CallPresent() PLH::FnCast(DxComps::__o__present, PresentDetour)(pSwapChain, syncInterval, flags);
	HWND window = GameConfig::getWindow();
	DeviceType deviceType = DeviceType::None;

	if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&DxComps::d3d11Device))))
		deviceType = DeviceType::D3D11;

	if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&DxComps::d3d12Device))))
		deviceType = DeviceType::D3D12;

	if (deviceType == DeviceType::D3D12)
	{
		if (!lightConfig.GetOrDefault<bool>("d3d12_experiment", false)) {
			static_cast<ID3D12Device5*>(DxComps::d3d12Device)->RemoveDevice();
			return CallPresent();
		}

		std::call_once(*DxComps::initCtx, [&]() {
			ImguiSetup();
			
			// add fonts or smth

			DXGI_SWAP_CHAIN_DESC Desc;
			pSwapChain->GetDesc(&Desc);
			Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			Desc.OutputWindow = window;
			Desc.Windowed = ((GetWindowLongPtr(window, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

			DxComps::BuffersCounts = Desc.BufferCount;
			DxComps::FrameContext = new DxComps::_FrameContext[DxComps::BuffersCounts];

			D3D12_DESCRIPTOR_HEAP_DESC DescriptorImGuiRender = {};
			DescriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			DescriptorImGuiRender.NumDescriptors = DxComps::BuffersCounts;
			DescriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			if (DxComps::d3d12Device->CreateDescriptorHeap(&DescriptorImGuiRender, IID_PPV_ARGS(&DxComps::d3d12DescriptorHeapImGuiRender)) != S_OK)
				throw std::runtime_error("Failed to create ImGui descriptor heap");

			ID3D12CommandAllocator* Allocator;
			if (DxComps::d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Allocator)) != S_OK)
				throw std::runtime_error("Failed to create command allocator");

			for (size_t i = 0; i < DxComps::BuffersCounts; i++)
				DxComps::FrameContext[i].CommandAllocator = Allocator;

			if (DxComps::d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, Allocator, NULL, IID_PPV_ARGS(&DxComps::d3d12CommandList)) != S_OK ||
				DxComps::d3d12CommandList->Close() != S_OK)
				throw std::runtime_error("Failed to create command list");

			D3D12_DESCRIPTOR_HEAP_DESC DescriptorBackBuffers;
			DescriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			DescriptorBackBuffers.NumDescriptors = DxComps::BuffersCounts;
			DescriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			DescriptorBackBuffers.NodeMask = 1;

			if (DxComps::d3d12Device->CreateDescriptorHeap(&DescriptorBackBuffers, IID_PPV_ARGS(&DxComps::d3d12DescriptorHeapBackBuffers)) != S_OK)
				throw std::runtime_error("Failed to create backbuffer descriptor heap");

			const auto RTVDescriptorSize = DxComps::d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = DxComps::d3d12DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();

			for (size_t i = 0; i < DxComps::BuffersCounts; i++) {
				ID3D12Resource* pBackBuffer = nullptr;
				DxComps::FrameContext[i].DescriptorHandle = RTVHandle;
				pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
				DxComps::d3d12Device->CreateRenderTargetView(pBackBuffer, nullptr, RTVHandle);
				DxComps::FrameContext[i].Resource = pBackBuffer;
				RTVHandle.ptr += RTVDescriptorSize;
			}

			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX12_Init(DxComps::d3d12Device, DxComps::BuffersCounts, DXGI_FORMAT_R8G8B8A8_UNORM, DxComps::d3d12DescriptorHeapImGuiRender,
				DxComps::d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart(), DxComps::d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart());
			ImGui_ImplDX12_CreateDeviceObjects();
			ImGui::GetIO().ImeWindowHandle = window;
			DxComps::passWndProc = true;
		});

		if (DxComps::d3d12CommandQueue == nullptr)
			return CallPresent();
		
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		
		testRender();
		
		ImGui::EndFrame();
		
		DxComps::_FrameContext& CurrentFrameContext = DxComps::FrameContext[pSwapChain->GetCurrentBackBufferIndex()];
		CurrentFrameContext.CommandAllocator->Reset();
		
		D3D12_RESOURCE_BARRIER Barrier;
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = CurrentFrameContext.Resource;
		Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		
		DxComps::d3d12CommandList->Reset(CurrentFrameContext.CommandAllocator, nullptr);
		DxComps::d3d12CommandList->ResourceBarrier(1, &Barrier);
		DxComps::d3d12CommandList->OMSetRenderTargets(1, &CurrentFrameContext.DescriptorHandle, FALSE, nullptr);
		DxComps::d3d12CommandList->SetDescriptorHeaps(1, &DxComps::d3d12DescriptorHeapImGuiRender);
		
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DxComps::d3d12CommandList);
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		DxComps::d3d12CommandList->ResourceBarrier(1, &Barrier);
		DxComps::d3d12CommandList->Close();
		DxComps::d3d12CommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&DxComps::d3d12CommandList));
	}
	else if (deviceType == DeviceType::D3D11)
	{
		std::call_once(*DxComps::initCtx, [&]() {
			ImguiSetup();

			// add fonts or smth

			DirectReset(pSwapChain);
		});

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// set estimated guidata or smth to give modders SOMETHING to use if they want to go sigless

		testRender();

		ImGui::EndFrame();
		ImGui::Render();

		DxComps::d3d11Context->OMSetRenderTargets(1, &DxComps::d3d11Target, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	return CallPresent();
}

HRESULT ResizeDetour(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	if (DxComps::d3d11Target) {
		DxComps::d3d11Context->OMSetRenderTargets(0, 0, 0);
		DxComps::d3d11Target->Release();
	}

	if (DxComps::FrameContext) {
		for (UINT i = 0; i < DxComps::BuffersCounts; ++i)
		{
			if (DxComps::FrameContext[i].Resource)
				DxComps::FrameContext[i].Resource->Release();
		}
	}

	HRESULT hr = PLH::FnCast(DxComps::__o__resize, ResizeDetour)(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	DirectReset((IDXGISwapChain3*)pSwapChain);

	return hr;
}

void* __o__CreateDXGIFactory = nullptr;
std::once_flag createFactoryFlag;
HRESULT WINAPI CreateDXGIFactoryDetour(REFIID riid, _COM_Outptr_ void** ppFactory) {	
	std::call_once(createFactoryFlag, []() {
		bool isRtx = kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success;
		if (!isRtx)
			kiero::init(kiero::RenderType::D3D11);

		std::print("Initialized kiero with {} support\n", isRtx ? "D3D12" : "D3D11");

		auto VTable = kiero::getMethodsTable();
		if (isRtx) // dx12 is stupid but here you go
		{
			NativeCore::hookFunction((uintptr_t)VTable[54], &executeCommandListD3D12, &DxComps::__o__executeCommandLists);
		}

		NativeCore::hookFunction((uintptr_t)VTable[isRtx ? 140 : 8], &PresentDetour, &DxComps::__o__present);
		NativeCore::hookFunction((uintptr_t)VTable[isRtx ? 145 : 13], &ResizeDetour, &DxComps::__o__resize);
	});

	return PLH::FnCast(__o__CreateDXGIFactory, CreateDXGIFactory)(riid, ppFactory);
}

// https://github.com/DrNseven/D3D12-Hook-ImGui/blob/master/ImGui/imgui_impl_win32.cpp#L323
static bool ImGui_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui::GetCurrentContext() == NULL)
		return false;

	ImGuiIO& io = ImGui::GetIO();

	switch (msg)
	{
	case WM_MOUSEMOVE:
			io.MousePos.x = (float)(short)LOWORD(lParam);
			io.MousePos.y = (float)(short)HIWORD(lParam);
		return io.WantCaptureMouse;
	case WM_KILLFOCUS:
		memset(io.KeysDown, 0, sizeof(io.KeysDown));
		return false;
	case WM_CHAR:
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacterUTF16((unsigned short)wParam);
		return false;
		//case WM_SETCURSOR:
		//	if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
		//		return 1;
		//return 0;
	}

	//if (io.WantCaptureMouse)
	{
		switch (msg)
		{
		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
		{
			int button = 0;
			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
			if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
			if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
			if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
			io.MouseDown[button] = true;
			break;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			int button = 0;
			if (msg == WM_LBUTTONUP) { button = 0; }
			if (msg == WM_RBUTTONUP) { button = 1; }
			if (msg == WM_MBUTTONUP) { button = 2; }
			if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
			io.MouseDown[button] = false;
			break;
		}
		case WM_MOUSEWHEEL:
			io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
			break;
		case WM_MOUSEHWHEEL:
			io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
			break;
		}
		return true;
	}

	//if (io.WantCaptureKeyboard)
	{
		switch (msg)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (wParam < 256)
				io.KeysDown[wParam] = 1;
			return true;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (wParam < 256)
				io.KeysDown[wParam] = 0;
			return true;
		}
	}

	return false;
}

// defer the hook for when dxgi is being setup in mc
// NOTE: THIS IS NOT AN ESSENTIAL STEP
void initDxHook() {
	NativeCore::hookFunction(reinterpret_cast<uintptr_t>(&CreateDXGIFactory1), &CreateDXGIFactoryDetour, &__o__CreateDXGIFactory);
}
