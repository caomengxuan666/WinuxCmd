# Pitfalls

**Pitfall 1: Forgetting to call End() after Begin()**
```cpp
// BAD: Missing End() call
ImGui::Begin("My Window");
ImGui::Text("Hello");

// GOOD: Always pair Begin/End
ImGui::Begin("My Window");
ImGui::Text("Hello");
ImGui::End();
```

**Pitfall 2: Using static buffers for dynamic text**
```cpp
// BAD: Fixed buffer may overflow
char buffer[16];
snprintf(buffer, sizeof(buffer), "Value: %d", large_number);
ImGui::Text("%s", buffer);

// GOOD: Use format string directly
ImGui::Text("Value: %d", large_number);
```

**Pitfall 3: Modifying state during item rendering**
```cpp
// BAD: State change affects current item
ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255,0,0,255));
if (ImGui::Button("Click")) { /* ... */ }
ImGui::PopStyleColor();

// GOOD: Style changes before item
ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255,0,0,255));
ImGui::Button("Click");
ImGui::PopStyleColor();
```

**Pitfall 4: Incorrect ID management with same-label items**
```cpp
// BAD: Duplicate labels cause ID conflicts
for (int i = 0; i < 5; i++)
    ImGui::Button("Click"); // All have same ID

// GOOD: Use unique IDs
for (int i = 0; i < 5; i++) {
    ImGui::PushID(i);
    ImGui::Button("Click");
    ImGui::PopID();
}
```

**Pitfall 5: Not checking return values**
```cpp
// BAD: No check for button press
ImGui::Button("Save");
SaveData(); // Always called

// GOOD: Check return value
if (ImGui::Button("Save")) {
    SaveData(); // Only called on click
}
```

**Pitfall 6: Mixing immediate and retained state**
```cpp
// BAD: Trying to store UI state manually
static bool window_open = true;
if (window_open) {
    ImGui::Begin("Window", &window_open);
    ImGui::End();
}

// GOOD: Let ImGui manage visibility
ImGui::Begin("Window", &window_open);
ImGui::End();
// window_open is updated automatically when user closes
```