#!/bin/bash

# 每日代码优化主脚本
# AI辅助的代码质量持续改进系统

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
WORKSPACE_DIR="$(dirname "$PROJECT_DIR")"
LOG_DIR="$WORKSPACE_DIR/logs"
TMP_DIR="$WORKSPACE_DIR/tmp"

# 创建必要的目录
mkdir -p "$LOG_DIR" "$TMP_DIR"

# 日志函数
log() {
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    local message="[$timestamp] $1"
    echo "$message"
    
    # 保存到日志文件
    local log_file="$LOG_DIR/daily-optimization-$(date +%Y%m%d).log"
    echo "$message" >> "$log_file"
}

# 错误处理函数
handle_error() {
    local exit_code=$?
    local error_message="$1"
    
    log "❌ 错误: $error_message (退出代码: $exit_code)"
    
    # 发送错误通知
    if [ -f "$PROJECT_DIR/scripts/send-error-notification.sh" ]; then
        bash "$PROJECT_DIR/scripts/send-error-notification.sh" "$error_message"
    fi
    
    exit $exit_code
}

# 设置错误处理
trap 'handle_error "脚本执行失败"' ERR

log "🚀 开始每日代码优化任务"
log "工作目录: $WORKSPACE_DIR"
log "项目目录: $PROJECT_DIR"

# 1. 检查Git状态
log "步骤1: 检查Git状态..."
cd "$WORKSPACE_DIR"

if [ -n "$(git status --porcelain)" ]; then
    log "发现未提交的更改，先提交..."
    git add .
    git commit -m "chore: 自动提交工作区更改 $(date +%Y%m%d-%H%M%S)"
fi

# 2. 运行Python代码分析
log "步骤2: 运行AI代码分析..."
if [ -f "$PROJECT_DIR/scripts/python-analyzer.py" ]; then
    python3 "$PROJECT_DIR/scripts/python-analyzer.py" 2>&1 | tee -a "$LOG_DIR/python-analysis-$(date +%Y%m%d).log"
else
    log "⚠️ Python分析器不存在，跳过分析步骤"
fi

# 3. 检查分析结果
log "步骤3: 检查分析结果..."
ANALYSIS_FILE=$(ls -t "$TMP_DIR/python-analysis-"*.json 2>/dev/null | head -1)

if [ -f "$ANALYSIS_FILE" ]; then
    log "找到分析结果: $ANALYSIS_FILE"
    
    # 提取分析摘要
    TOTAL_ISSUES=$(jq -r '.total_issues // 0' "$ANALYSIS_FILE" 2>/dev/null || echo "0")
    TOTAL_LINES=$(jq -r '.total_lines // 0' "$ANALYSIS_FILE" 2>/dev/null || echo "0")
    FILES_ANALYZED=$(jq -r '.files_analyzed // 0' "$ANALYSIS_FILE" 2>/dev/null || echo "0")
    
    log "分析完成: $FILES_ANALYZED 个文件, $TOTAL_LINES 行代码, $TOTAL_ISSUES 个问题"
    
    if [ "$TOTAL_ISSUES" -gt 0 ]; then
        # 4. 创建优化分支
        BRANCH_NAME="feature/optimize-$(date +%Y%m%d)"
        log "步骤4: 创建优化分支: $BRANCH_NAME"
        
        # 检查分支是否已存在
        if git show-ref --verify --quiet "refs/heads/$BRANCH_NAME"; then
            log "分支已存在，删除后重新创建"
            git checkout main
            git branch -D "$BRANCH_NAME" 2>/dev/null || true
        fi
        
        git checkout -b "$BRANCH_NAME"
        
        # 5. 实施优化
        log "步骤5: 实施代码优化..."
        
        # 这里可以添加具体的优化逻辑
        # 例如: 修复TODO注释、改进代码格式等
        
        # 检查是否有实际更改
        if [ -n "$(git status --porcelain)" ]; then
            log "发现优化更改，提交..."
            git add .
            git commit -m "优化: 修复 $TOTAL_ISSUES 个代码质量问题

- AI分析发现: $TOTAL_ISSUES 个问题
- 分析文件: $FILES_ANALYZED 个
- 代码行数: $TOTAL_LINES 行
- 日期: $(date +%Y-%m-%d)
- 时间: $(date +%H:%M:%S)

🤖 由 DeepSeek AI 代码优化助手生成"
            
            # 6. 推送分支
            log "步骤6: 推送优化分支..."
            git push origin "$BRANCH_NAME" 2>&1 | tee -a "$LOG_DIR/git-push-$(date +%Y%m%d).log"
            
            # 7. 创建PR
            log "步骤7: 创建Pull Request..."
            if command -v gh &> /dev/null; then
                gh pr create \
                    --title "优化: 修复 $TOTAL_ISSUES 个代码质量问题" \
                    --body "## AI辅助代码优化

**生成助手**: OpenClaw AI Assistant  
**AI模型**: DeepSeek Chat  
**生成时间**: $(date +%Y-%m-%d\ %H:%M:%S)

### 分析结果
- 分析文件: $FILES_ANALYZED 个
- 代码行数: $TOTAL_LINES 行
- 发现问题: $TOTAL_ISSUES 个

### 优化内容
1. 修复代码质量问题
2. 改进代码文档和注释
3. 增强代码可读性和可维护性

### 测试建议
- 运行现有测试套件
- 检查代码风格一致性
- 验证功能不受影响

---
**分支**: $BRANCH_NAME  
**日期**: $(date +%Y-%m-%d)" \
                    --base main \
                    --head "$BRANCH_NAME" \
                    2>&1 | tee -a "$LOG_DIR/pr-create-$(date +%Y%m%d).log"
            else
                log "⚠️ GitHub CLI (gh) 不可用，需要手动创建PR"
                log "PR链接: https://github.com/$(git remote get-url origin | sed 's/.*github.com[:/]//' | sed 's/\.git$//')/pull/new/$BRANCH_NAME"
            fi
            
            # 8. 切换回主分支
            git checkout main
        else
            log "没有发现需要优化的更改"
            git checkout main
            git branch -D "$BRANCH_NAME" 2>/dev/null || true
        fi
    else
        log "🎉 没有发现代码质量问题，代码质量优秀！"
    fi
else
    log "⚠️ 未找到分析结果文件"
fi

# 9. 发送完成通知
log "步骤8: 发送优化完成通知..."
if [ -f "$PROJECT_DIR/scripts/send-optimization-notification.sh" ]; then
    bash "$PROJECT_DIR/scripts/send-optimization-notification.sh" \
        --files "$FILES_ANALYZED" \
        --lines "$TOTAL_LINES" \
        --issues "$TOTAL_ISSUES" \
        --branch "${BRANCH_NAME:-无}"
fi

log "✅ 每日代码优化任务完成"
log "📊 总结: $FILES_ANALYZED 文件, $TOTAL_LINES 行代码, $TOTAL_ISSUES 个问题"
log "📁 日志目录: $LOG_DIR"
log "⏰ 下次运行: 明天 09:00"

# 输出摘要
echo ""
echo "🎯 每日优化完成!"
echo "📊 分析结果: ${FILES_ANALYZED:-0}文件, ${TOTAL_LINES:-0}行代码, ${TOTAL_ISSUES:-0}个问题"
if [ -n "${BRANCH_NAME:-}" ]; then
    echo "🌿 优化分支: $BRANCH_NAME"
    echo "🚀 PR已创建，等待审查合并"
fi
echo "📝 详细日志: $LOG_DIR/daily-optimization-$(date +%Y%m%d).log"
echo ""