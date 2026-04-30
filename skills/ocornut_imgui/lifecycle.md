# Lifecycle

**Construction: Initialize ImGui once at application startup**
```cpp
// Setup Dear ImGui context
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO();

// Initialize backends
ImGui_ImplGlfw_InitForOpenGL(window, true);
ImGui_ImplOpenGL3_Init("#version 130");
```

**Frame lifecycle: NewFrame → UI code → Render**
```cpp
// Each frame
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplGlfw_NewFrame();
ImGui::NewFrame();

// Your UI code here
ImGui::Begin("My Window");
ImGui::Text("Frame %d", frame_count);
ImGui::End();

// End frame and render
ImGui::Render();
ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
```

**Destruction: Clean up in reverse order**
```cpp
void ShutdownImGui() {
    // Destroy backends first
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    
    // Destroy ImGui context
    ImGui::DestroyContext();
}
```

**Resource management: Fonts and textures**
```cpp
// Load fonts (must be done before first NewFrame)
ImGuiIO& io = ImGui::GetIO();
ImFont* font = io.Fonts->AddFontFromFileTTF("arial.ttf", 18.0f);

// Create font texture (after adding all fonts)
unsigned char* pixels;
int width, height;
io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
// Upload to GPU texture
```