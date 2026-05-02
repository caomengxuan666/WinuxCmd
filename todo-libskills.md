# LibSkills — What's Done & What's Next

## ✅ 已建成
- GitHub Organization: `LibSkills`（头像已设置）
- 仓库: `LibSkills/LibSkills`（README / SPEC / CONTRIBUTING / GOVERNANCE 已推送，全英文）
- Logo 已设置

## 📋 明天要做的事

### 仓库拆分（按GPT建议）
1. 建 `libskills-cli` — 从当前仓库抽出CLI骨架
2. 建 `libskills-registry` — 抽出 registry/ 目录
3. 建 `libskills-schema` — 抽出 SPEC.md 的schema部分
4. 建 `libskills-protocol` — 未来MCP/HTTP定义
5. 当前 `LibSkills/LibSkills` 转为根文档仓库（README + GOVERNANCE + 索引）

### CLI开发（草猛写Rust）
6. 最小MVP：search / get / info / update / cache / list
7. `libskills init` — 生成skill模板
8. `libskills doctor` — 安全检查

### 第一个Skill（草纸写）
9. spdlog 完整 skill（tier1，c++，main）
10. fmt 完整 skill
11. nlohmann/json 完整 skill

### 发布准备
12. CI注册表索引自动构建
13. Registry snapshot 机制
14. HN / Reddit / Rust社区 发布
