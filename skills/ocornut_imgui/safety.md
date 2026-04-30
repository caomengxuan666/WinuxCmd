# Safety

**Condition 1: NEVER call ImGui functions before ImGui::NewFrame()**
```cpp
// BAD: Calling before frame setup
ImGui::Text("Hello"); // Crash or undefined behavior

// GOOD: Proper frame sequence
ImGui::NewFrame();
ImGui::Text("Hello");
ImGui::Render();
```

**Condition 2: NEVER mismatch Push/Pop calls**
```cpp
// BAD: Unbalanced push/pop
ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
ImGui::Text("Red text");
// Missing PopStyleColor()

// GOOD: Always pop what you push
ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
ImGui::Text("Red text");
ImGui::PopStyleColor();
```

**Condition 3: NEVER access ImDrawData after ImGui::EndFrame()**
```cpp
// BAD: Using draw data after frame end
ImGui::Render();
ImDrawData* draw_data = ImGui::GetDrawData();
// draw_data is invalid after Render()

// GOOD: Use draw data before next frame
ImGui::Render();
ImDrawData* draw_data = ImGui::GetDrawData();
// Process draw_data immediately
```

**Condition 4: NEVER modify ImGui state from multiple threads**
```cpp
// BAD: Thread safety violation
std::thread t([]() {
    ImGui::Text("From thread"); // Unsafe!
});
t.join();

// GOOD: All ImGui calls from main thread
ImGui::Text("From main thread");
```

**Condition 5: NEVER use ImGui after Shutdown()**
```cpp
// BAD: Using after shutdown
ImGui::Shutdown();
ImGui::Text("After shutdown"); // Crash

// GOOD: Stop using after shutdown
ImGui::Shutdown();
// No more ImGui calls
```