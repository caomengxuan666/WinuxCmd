# Performance

**Performance characteristics:**
- **Draw call count**: Typically 10-100 draw calls per frame
- **Vertex count**: ~50-200 vertices per visible widget
- **CPU time**: <0.5ms for typical debug UIs
- **Memory**: ~1-5MB for context, grows with visible widgets

**Optimization tip 1: Minimize Begin/End pairs**
```cpp
// BAD: Many nested windows
for (int i = 0; i < 100; i++) {
    ImGui::Begin(("Window " + std::to_string(i)).c_str());
    ImGui::Text("Content");
    ImGui::End();
}

// GOOD: Use child regions instead
ImGui::Begin("Main Window");
for (int i = 0; i < 100; i++) {
    ImGui::BeginChild(("Child" + std::to_string(i)).c_str(), 
                      ImVec2(200, 50), true);
    ImGui::Text("Content %d", i);
    ImGui::EndChild();
}
ImGui::End();
```

**Optimization tip 2: Use ImGuiListClipper for large lists**
```cpp
// BAD: Rendering all items
for (int i = 0; i < 100000; i++) {
    ImGui::Text("Item %d", i);
}

// GOOD: Only render visible items
ImGuiListClipper clipper;
clipper.Begin(100000);
while (clipper.Step()) {
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
        ImGui::Text("Item %d", i);
    }
}
```

**Optimization tip 3: Cache expensive computations**
```cpp
// BAD: Computing every frame
ImGui::Text("Result: %f", ExpensiveComputation());

// GOOD: Cache result
static float cached_result = 0.0f;
static bool needs_update = true;
if (needs_update) {
    cached_result = ExpensiveComputation();
    needs_update = false;
}
ImGui::Text("Result: %f", cached_result);
```