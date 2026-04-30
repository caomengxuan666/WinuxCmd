# Best Practices

**Pattern 1: Consistent frame structure**
```cpp
void RenderFrame() {
    ImGui::NewFrame();
    
    // All UI code here
    RenderMainMenu();
    RenderToolWindows();
    RenderDebugOverlay();
    
    ImGui::Render();
    // Submit ImDrawData to renderer
}
```

**Pattern 2: Use PushID for dynamic content**
```cpp
void RenderItems(const std::vector<Item>& items) {
    for (size_t i = 0; i < items.size(); i++) {
        ImGui::PushID(static_cast<int>(i));
        if (ImGui::Button("Delete")) {
            DeleteItem(i);
        }
        ImGui::SameLine();
        ImGui::Text("%s", items[i].name.c_str());
        ImGui::PopID();
    }
}
```

**Pattern 3: Group related widgets in collapsible sections**
```cpp
void RenderSettings() {
    if (ImGui::CollapsingHeader("Graphics")) {
        ImGui::SliderInt("FPS Limit", &fps_limit, 30, 240);
        ImGui::Checkbox("VSync", &vsync_enabled);
    }
    if (ImGui::CollapsingHeader("Audio")) {
        ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f);
    }
}
```

**Pattern 4: Use ImGui::GetIO() for configuration**
```cpp
void SetupImGuiStyle() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("path/to/font.ttf", 16.0f);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    ImGui::GetStyle().FrameRounding = 4.0f;
}
```