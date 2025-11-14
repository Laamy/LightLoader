#pragma once

#include <d2d1.h>
#include <d2d1_2.h>
#include <dwrite.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include <dxgi1_4.h>

#pragma comment(lib, "dxgi.lib")

#include "../../../API/Extensions/imgui/imgui.h"
#include "../../../API/Extensions/imgui/imgui_impl_dx11.h"
#include "../../../API/Extensions/imgui/imgui_impl_dx12.h"
#include "../../../API/Extensions/imgui/imgui_impl_win32.h"

struct GAMEEVENTS_API D3DRenderEvent : public BaseEvent {
    EventID id() const override { return EventID::D3DRender; }

	ID3D12Device* d3d12Device = nullptr;
	ID3D11Device* d3d11Device = nullptr;
};
