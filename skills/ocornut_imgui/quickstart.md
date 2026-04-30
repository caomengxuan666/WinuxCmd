# Quickstart

```cpp
// 1. Basic window with text and button
ImGui::Begin("My Window");
ImGui::Text("Hello, world!");
if (ImGui::Button("Click Me")) {
    // Action on click
}
ImGui::End();

// 2. Input text widget
char buf[256] = "";
ImGui::InputText("Label", buf, sizeof(buf));

// 3. Slider for float values
float value = 0.5f;
ImGui::SliderFloat("Slider", &value, 0.0f, 1.0f);

// 4. Combo box
const char* items[] = {"Option A", "Option B", "Option C"};
int current_item = 0;
ImGui::Combo("Combo", &current_item, items, IM_ARRAYSIZE(items));

// 5. Checkbox
bool enabled = true;
ImGui::Checkbox("Enable Feature", &enabled);

// 6. Color picker
float color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
ImGui::ColorEdit4("Color", color);

// 7. Tree node with children
if (ImGui::TreeNode("Settings")) {
    ImGui::Text("Inside tree node");
    ImGui::TreePop();
}

// 8. Progress bar
float progress = 0.5f;
ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
```