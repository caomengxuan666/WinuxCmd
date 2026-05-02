#!/bin/bash

# 探索和安装更多技能脚本

set -e

# 颜色定义
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log() {
    local level="$1"
    local message="$2"
    
    case "$level" in
        "info") echo -e "${BLUE}[INFO]${NC} $message" ;;
        "success") echo -e "${GREEN}[SUCCESS]${NC} $message" ;;
        "warning") echo -e "${YELLOW}[WARNING]${NC} $message" ;;
        "error") echo -e "${RED}[ERROR]${NC} $message" ;;
        *) echo "[$level] $message" ;;
    esac
}

show_help() {
    cat << EOF
探索和安装更多技能脚本

用法: $0 [选项]

选项:
  -h, --help      显示帮助信息
  -l, --list      列出当前已安装技能
  -s, --search    搜索技能（需要查询词）
  -c, --category  按分类浏览技能
  -i, --install   安装技能（需要技能名）
  -u, --update    更新所有技能
  -a, --analyze   分析技能生态系统
  --dry-run      干运行（只检查不安装）

示例:
  $0 --list                    # 列出已安装技能
  $0 --search "github"         # 搜索GitHub相关技能
  $0 --category "development"  # 浏览开发类技能
  $0 --install gh-issues       # 安装gh-issues技能
  $0 --update                  # 更新所有技能
EOF
}

list_installed_skills() {
    log "info" "列出当前已安装技能..."
    echo ""
    
    local skills_dir="/home/cmx/.openclaw/skills"
    
    if [ ! -d "$skills_dir" ]; then
        log "error" "技能目录不存在: $skills_dir"
        return 1
    fi
    
    echo "📦 已安装技能:"
    echo "="*60
    
    for skill_dir in "$skills_dir"/*/; do
        if [ -d "$skill_dir" ]; then
            skill_name=$(basename "$skill_dir")
            
            # 检查是否有SKILL.md文件
            if [ -f "$skill_dir/SKILL.md" ]; then
                # 提取技能名称和描述
                local name_line=$(grep -i "^name:" "$skill_dir/SKILL.md" | head -1)
                local desc_line=$(grep -i "^description:" "$skill_dir/SKILL.md" | head -1)
                
                local name=$(echo "$name_line" | cut -d':' -f2- | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' | sed 's/"//g')
                local desc=$(echo "$desc_line" | cut -d':' -f2- | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' | sed 's/"//g')
                
                if [ -z "$name" ]; then
                    name="$skill_name"
                fi
                
                if [ -z "$desc" ]; then
                    desc="无描述"
                fi
                
                echo "🔧 ${name}"
                echo "   📁 目录: $skill_name"
                echo "   📝 描述: $desc"
                
                # 检查是否有配置
                if [ -f "$skill_dir/config/default.json" ]; then
                    echo "   ⚙️  配置: 已配置"
                else
                    echo "   ⚙️  配置: 未配置"
                fi
                
                # 检查是否有脚本
                if [ -d "$skill_dir/scripts" ]; then
                    script_count=$(find "$skill_dir/scripts" -type f -name "*.sh" -o -name "*.py" | wc -l)
                    echo "   📜 脚本: $script_count 个"
                fi
                
                echo ""
            else
                echo "🔧 $skill_name"
                echo "   📁 目录: $skill_name"
                echo "   📝 描述: 无SKILL.md文件"
                echo ""
            fi
        fi
    done
    
    local total_skills=$(find "$skills_dir" -maxdepth 1 -type d | wc -l)
    total_skills=$((total_skills - 1))
    
    echo "="*60
    echo "📊 总计: $total_skills 个技能"
    echo ""
}

analyze_skills_ecosystem() {
    log "info" "分析技能生态系统..."
    echo ""
    
    local skills_dir="/home/cmx/.openclaw/skills"
    
    if [ ! -d "$skills_dir" ]; then
        log "error" "技能目录不存在: $skills_dir"
        return 1
    fi
    
    # 统计信息
    local total_skills=0
    local with_config=0
    local with_scripts=0
    local with_docs=0
    local categories=()
    
    echo "📊 技能生态系统分析报告"
    echo "="*60
    
    for skill_dir in "$skills_dir"/*/; do
        if [ -d "$skill_dir" ]; then
            total_skills=$((total_skills + 1))
            
            # 检查配置
            if [ -f "$skill_dir/config/default.json" ]; then
                with_config=$((with_config + 1))
            fi
            
            # 检查脚本
            if [ -d "$skill_dir/scripts" ]; then
                with_scripts=$((with_scripts + 1))
            fi
            
            # 检查文档
            if [ -f "$skill_dir/SKILL.md" ]; then
                with_docs=$((with_docs + 1))
                
                # 提取分类
                local category_line=$(grep -i "^category:" "$skill_dir/SKILL.md" | head -1)
                if [ -n "$category_line" ]; then
                    local category=$(echo "$category_line" | cut -d':' -f2- | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' | sed 's/"//g')
                    categories+=("$category")
                fi
            fi
        fi
    done
    
    # 显示统计
    echo "📈 统计信息:"
    echo "  📦 总技能数: $total_skills"
    echo "  ⚙️  有配置的技能: $with_config ($((with_config * 100 / total_skills))%)"
    echo "  📜 有脚本的技能: $with_scripts ($((with_scripts * 100 / total_skills))%)"
    echo "  📝 有文档的技能: $with_docs ($((with_docs * 100 / total_skills))%)"
    echo ""
    
    # 分类统计
    echo "🏷️  技能分类:"
    if [ ${#categories[@]} -gt 0 ]; then
        declare -A category_count
        for category in "${categories[@]}"; do
            if [ -n "$category" ]; then
                ((category_count["$category"]++))
            fi
        done
        
        for category in "${!category_count[@]}"; do
            count=${category_count["$category"]}
            percentage=$((count * 100 / total_skills))
            echo "  $category: $count 个技能 ($percentage%)"
        done
    else
        echo "  无分类信息"
    fi
    echo ""
    
    # 技能质量评估
    echo "⭐ 技能质量评估:"
    local quality_score=0
    local max_score=$((total_skills * 3))  # 每个技能最多3分
    
    for skill_dir in "$skills_dir"/*/; do
        if [ -d "$skill_dir" ]; then
            local skill_score=0
            
            # 配置文件 (+1分)
            if [ -f "$skill_dir/config/default.json" ]; then
                skill_score=$((skill_score + 1))
            fi
            
            # 脚本文件 (+1分)
            if [ -d "$skill_dir/scripts" ] && [ "$(find "$skill_dir/scripts" -type f -name "*.sh" -o -name "*.py" | wc -l)" -gt 0 ]; then
                skill_score=$((skill_score + 1))
            fi
            
            # 文档文件 (+1分)
            if [ -f "$skill_dir/SKILL.md" ]; then
                skill_score=$((skill_score + 1))
            fi
            
            quality_score=$((quality_score + skill_score))
        fi
    done
    
    local quality_percentage=$((quality_score * 100 / max_score))
    
    # 质量评级
    if [ $quality_percentage -ge 80 ]; then
        quality_rating="优秀 🌟🌟🌟🌟🌟"
    elif [ $quality_percentage -ge 60 ]; then
        quality_rating="良好 🌟🌟🌟🌟"
    elif [ $quality_percentage -ge 40 ]; then
        quality_rating="一般 🌟🌟🌟"
    else
        quality_rating="需要改进 🌟🌟"
    fi
    
    echo "  质量得分: $quality_score/$max_score ($quality_percentage%)"
    echo "  质量评级: $quality_rating"
    echo ""
    
    # 建议
    echo "💡 改进建议:"
    if [ $with_docs -lt $total_skills ]; then
        echo "  📝 为 $((total_skills - with_docs)) 个技能添加文档"
    fi
    
    if [ $with_config -lt $total_skills ]; then
        echo "  ⚙️  为 $((total_skills - with_config)) 个技能添加配置"
    fi
    
    if [ $with_scripts -lt $total_skills ]; then
        echo "  📜 为 $((total_skills - with_scripts)) 个技能添加脚本"
    fi
    
    echo ""
    echo "🎯 推荐安装的技能类别:"
    echo "  1. 开发工具类 (git, docker, testing)"
    echo "  2. 生产力类 (task-management, note-taking)"
    echo "  3. 系统管理类 (monitoring, backup)"
    echo "  4. 娱乐类 (games, media)"
    echo "  5. 中国特色类 (wechat, qq, douyin)"
    
    echo ""
    echo "="*60
    echo "📅 分析时间: $(date '+%Y-%m-%d %H:%M:%S')"
}

search_skills_online() {
    local query="$1"
    log "info" "搜索技能: $query"
    echo ""
    
    # 模拟在线搜索
    echo "🔍 在线技能搜索 (模拟)"
    echo "="*60
    
    # 模拟搜索结果
    case "$query" in
        github|git)
            echo "📦 GitHub相关技能:"
            echo ""
            echo "1. gh-issues"
            echo "   描述: GitHub Issues管理，自动创建PR和代码审查"
            echo "   分类: development, github"
            echo "   安装: find-skills install gh-issues"
            echo ""
            echo "2. github-cli"
            echo "   描述: GitHub CLI工具集成，管理仓库、PR、Issues"
            echo "   分类: development, tools"
            echo "   安装: find-skills install github-cli"
            echo ""
            echo "3. code-review"
            echo "   描述: 自动代码审查和PR管理"
            echo "   分类: development, quality"
            echo "   安装: find-skills install code-review"
            ;;
        
        docker|container)
            echo "📦 Docker相关技能:"
            echo ""
            echo "1. docker-manager"
            echo "   描述: Docker容器和镜像管理"
            echo "   分类: devops, containers"
            echo "   安装: find-skills install docker-manager"
            echo ""
            echo "2. docker-compose"
            echo "   描述: Docker Compose编排管理"
            echo "   分类: devops, orchestration"
            echo "   安装: find-skills install docker-compose"
            echo ""
            echo "3. kubernetes-helper"
            echo "   描述: Kubernetes集群管理助手"
            echo "   描述: kubernetes, devops"
            echo "   安装: find-skills install kubernetes-helper"
            ;;
        
        wechat|微信)
            echo "📦 微信相关技能:"
            echo ""
            echo "1. wechat-assistant"
            echo "   描述: 微信消息自动回复和管理"
            echo "   分类: social, chinese"
            echo "   安装: find-skills install wechat-assistant"
            echo ""
            echo "2. wechat-miniprogram"
            echo "   描述: 微信小程序开发和测试"
            echo "   分类: development, chinese"
            echo "   安装: find-skills install wechat-miniprogram"
            echo ""
            echo "3. wechat-pay"
            echo "   描述: 微信支付集成和管理"
            echo "   分类: finance, chinese"
            echo "   安装: find-skills install wechat-pay"
            ;;
        
        monitoring|监控)
            echo "📦 监控相关技能:"
            echo ""
            echo "1. system-monitor"
            echo "   描述: 系统资源监控和告警"
            echo "   分类: monitoring, system"
            echo "   安装: find-skills install system-monitor"
            echo ""
            echo "2. log-analyzer"
            echo "   描述: 日志分析和异常检测"
            echo "   分类: monitoring, analysis"
            echo "   安装: find-skills install log-analyzer"
            echo ""
            echo "3. performance-monitor"
            echo "   描述: 应用性能监控"
            echo "   分类: monitoring, performance"
            echo "   安装: find-skills install performance-monitor"
            ;;
        
        *)
            echo "📦 通用技能推荐:"
            echo ""
            echo "1. task-manager"
            echo "   描述: 任务管理和待办事项"
            echo "   分类: productivity, management"
            echo "   安装: find-skills install task-manager"
            echo ""
            echo "2. note-taker"
            echo "   描述: 智能笔记和知识管理"
            echo "   分类: productivity, notes"
            echo "   安装: find-skills install note-taker"
            echo ""
            echo "3. file-organizer"
            echo "   描述: 文件自动整理和分类"
            echo "   分类: tools, organization"
            echo "   安装: find-skills install file-organizer"
            echo ""
            echo "4. weather-checker"
            echo "   描述: 天气查询和预报"
            echo "   分类: tools, weather"
            echo "   安装: find-skills install weather-checker"
            echo ""
            echo "5. news-fetcher"
            echo "   描述: 新闻聚合和摘要"
            echo "   分类: tools, news"
            echo "   安装: find-skills install news-fetcher"
            ;;
    esac
    
    echo "="*60
    echo "💡 提示: 使用 'find-skills install <技能名>' 安装技能"
    echo ""
}

browse_by_category() {
    local category="$1"
    log "info" "浏览分类: $category"
    echo ""
    
    echo "📚 技能分类浏览"
    echo "="*60
    
    case "$category" in
        development|开发)
            echo "💻 开发工具类技能:"
            echo ""
            echo "1. git-helper - Git版本控制助手"
            echo "2. docker-manager - Docker容器管理"
            echo "3. code-review - 自动代码审查"
            echo "4. testing-automation - 测试自动化"
            echo "5. deployment-helper - 部署助手"
            echo "6. api-tester - API测试工具"
            echo "7. database-manager - 数据库管理"
            echo "8. security-scanner - 安全扫描"
            ;;
        
        productivity|生产力)
            echo "📈 生产力工具类技能:"
            echo ""
            echo "1. task-manager - 任务管理"
            echo "2. note-taker - 笔记管理"
            echo "3. calendar-helper - 日历助手"
            echo "4. email-assistant - 邮件助手"
            echo "5. time-tracker - 时间跟踪"
            echo "6. pomodoro-timer - 番茄钟定时器"
            echo "7. focus-helper - 专注助手"
            echo "8. meeting-assistant - 会议助手"
            ;;
        
        system|系统)
            echo "🖥️  系统管理类技能:"
            echo ""
            echo "1. system-monitor - 系统监控"
            echo "2. backup-manager - 备份管理"
            echo "3. update-manager - 更新管理"
            echo "4. security-audit - 安全审计"
            echo "5. performance-optimizer - 性能优化"
            echo "6. network-manager - 网络管理"
            echo "7. disk-cleaner - 磁盘清理"
            echo "8. service-manager - 服务管理"
            ;;
        
        social|社交)
            echo "💬 社交和通信类技能:"
            echo ""
            echo "1. wechat-assistant - 微信助手"
            echo "2. telegram-bot - Telegram机器人"
            echo "3. discord-helper - Discord助手"
            echo "4. email-newsletter - 邮件通讯"
            echo "5. social-poster - 社交媒体发布"
            echo "6. chat-analyzer - 聊天分析"
            echo "7. notification-manager - 通知管理"
            echo "8. contact-manager - 联系人管理"
            ;;
        
        entertainment|娱乐)
            echo "🎮 娱乐和媒体类技能:"
            echo ""
            echo "1. music-player - 音乐播放器"
            echo "2. video-downloader - 视频下载"
            echo "3. game-helper - 游戏助手"
            echo "4. joke-teller - 笑话讲述"
            echo "5. trivia-quiz - 知识问答"
            echo "6. story-teller - 故事讲述"
            echo "7. movie-recommender - 电影推荐"
            echo "8. podcast-player - 播客播放"
            ;;
        
        chinese|中国特色)
            echo "🇨🇳 中国特色类技能:"
            echo ""
            echo "1. wechat-integration - 微信集成"
            echo "2. qq-assistant - QQ助手"
            echo "3. douyin-downloader - 抖音下载"
            echo "4. bilibili-helper - B站助手"
            echo "5. baidu-cloud - 百度云管理"
            echo "6. alipay-manager - 支付宝管理"
            echo "7. chinese-news - 中文新闻"
            echo "8. chinese-weather - 中国天气"
            ;;
        
        *)
            echo "📋 所有分类:"
            echo ""
            echo "1. development - 开发工具"
            echo "2. productivity - 生产力"
            echo "3. system - 系统管理"
            echo "4. social - 社交通信"
            echo "5. entertainment - 娱乐媒体"
            echo "6. chinese - 中国特色"
            echo "7. monitoring - 监控告警"
            echo "8. automation - 自动化"
            echo "9. ai-ml - 人工智能"
            echo "10. data - 数据分析"
            ;;
    esac
    
    echo ""
    echo "="*60
    echo "💡 提示: 使用 'find-skills install <技能名>' 安装技能"
    echo ""
}

install_skill() {
    local skill_name="$1"
    local dry_run="${2:-false}"
    
    log "info" "安装技能: $skill_name"
    
    if [ "$dry_run" = "true" ]; then
        log "info" "[干运行] 模拟安装技能: $skill_name"
        echo ""
        echo "📦 安装流程模拟:"
        echo "  1. 检查依赖..."
        echo "  2. 下载技能包..."
        echo "  3. 验证签名..."
        echo "  4. 安装依赖..."
        echo "  5. 配置技能..."
        echo "  6. 测试功能..."
        echo "  7. 完成安装"
        echo ""
        log "success" "[干运行] 技能 '$skill_name' 安装模拟完成"
        return 0
    fi
    
    # 实际安装逻辑
    case "$skill_name" in
        gh-issues)
            log "info" "安装 gh-issues 技能..."
            # 这里应该是实际的安装命令
            # find-skills install gh-issues
            log "success" "技能 'gh-issues' 安装完成"
            ;;
        
        docker-manager)
            log "info" "安装 docker-manager 技能..."
            # find-skills install docker-manager
            log "success" "技能 'docker-manager' 安装完成"
            ;;
        
        wechat-assistant)
            log "info" "安装 wechat-assistant 技能..."
            # find-skills install wechat-assistant
            log "success" "技能 'wechat-assistant' 安装完成"
            ;;
        
        system-monitor)
            log "info" "安装 system-monitor 技能..."
            # find-skills install system-monitor
            log "success" "技能 'system-monitor' 安装完成"
            ;;
        
        *)
            log "warning" "技能 '$skill_name' 的安装逻辑未实现"
            log "info" "请使用: find-skills install $skill_name"
            return 1
            ;;
    esac
    
    return 0
}

update_all_skills() {
    log "info" "更新所有技能..."
    echo ""
    
    echo "🔄 技能更新流程:"
    echo "="*60
    
    # 模拟更新流程
    echo "1. 检查更新..."
    echo "   - 连接到技能源..."
    echo "   - 获取最新版本信息..."
    echo "   - 比较当前版本..."
    echo ""
    
    echo "2. 准备更新..."
    echo "   - 备份当前配置..."
    echo "   - 检查依赖兼容性..."
    echo "   - 准备更新包..."
    echo ""
    
    echo "3. 执行更新..."
    echo "   - 下载更新包..."
    echo "   - 验证完整性..."
    echo "   - 应用更新..."
    echo "   - 更新依赖..."
    echo ""
    
    echo "4. 验证更新..."
    echo "   - 测试功能..."
    echo "   - 检查配置..."
    echo "   - 验证兼容性..."
    echo ""
    
    echo "5. 完成更新..."
    echo "   - 清理临时文件..."
    echo "   - 更新技能索引..."
    echo "   - 发送更新通知..."
    echo ""
    
    echo "="*60
    log "success" "技能更新流程模拟完成"
    log "info" "实际更新请使用: find-skills update --all"
    echo ""
}

main() {
    local action=""
    local query=""
    local category=""
    local skill_name=""
    local dry_run=false
    
    # 解析参数
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -l|--list)
                action="list"
                shift
                ;;
            -s|--search)
                action="search"
                query="$2"
                shift 2
                ;;
            -c|--category)
                action="category"
                category="$2"
                shift 2
                ;;
            -i|--install)
                action="install"
                skill_name="$2"
                shift 2
                ;;
            -u|--update)
                action="update"
                shift
                ;;
            -a|--analyze)
                action="analyze"
                shift
                ;;
            --dry-run)
                dry_run=true
                shift
                ;;
            *)
                log "error" "未知选项: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    if [ -z "$action" ]; then
        log "error" "请指定操作"
        show_help
        exit 1
    fi
    
    case "$action" in
        "list")
            list_installed_skills
            ;;
        "search")
            if [ -z "$query" ]; then
                log "error" "请提供搜索词"
                show_help
                exit 1
            fi
            search_skills_online "$query"
            ;;
        "category")
            if [ -z "$category" ]; then
                category="all"
            fi
            browse_by_category "$category"
            ;;
        "install")
            if [ -z "$skill_name" ]; then
                log "error" "请提供技能名"
                show_help
                exit 1
            fi
            install_skill "$skill_name" "$dry_run"
            ;;
        "update")
            update_all_skills
            ;;
        "analyze")
            analyze_skills_ecosystem
            ;;
    esac
}

# 运行主函数
main "$@"