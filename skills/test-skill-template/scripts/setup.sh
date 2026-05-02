#!/bin/bash

# 技能安装脚本
# 用于安装和配置技能

set -e

VERSION="1.0.0"
SKILL_NAME="技能名称"
INSTALL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CONFIG_DIR="${INSTALL_DIR}/config"
LOG_DIR="/tmp/${SKILL_NAME// /-}"

# 颜色定义
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

log_info() {
    echo -e "[INFO] $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

show_help() {
    cat << EOF
${SKILL_NAME} 安装脚本 v${VERSION}

用法: $0 [选项]

选项:
  -h, --help      显示帮助信息
  -c, --config    交互式配置
  -d, --dry-run   干运行（只显示不执行）
  -v, --verbose   详细输出
  --skip-deps     跳过依赖检查
  --force         强制安装（覆盖现有配置）

示例:
  $0               # 默认安装
  $0 --config      # 交互式配置安装
  $0 --dry-run     # 检查安装但不执行
EOF
}

check_dependencies() {
    log_info "检查系统依赖..."
    
    local missing_deps=()
    
    # 检查基本命令
    for cmd in bash curl wget; do
        if ! command -v "$cmd" > /dev/null 2>&1; then
            missing_deps+=("$cmd")
        fi
    done
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        log_error "缺少依赖: ${missing_deps[*]}"
        return 1
    fi
    
    log_success "所有依赖已安装"
    return 0
}

check_permissions() {
    log_info "检查权限..."
    
    # 检查安装目录权限
    if [ ! -w "$INSTALL_DIR" ]; then
        log_error "安装目录不可写: $INSTALL_DIR"
        return 1
    fi
    
    # 检查配置文件权限
    if [ -f "${CONFIG_DIR}/default.json" ] && [ ! -w "${CONFIG_DIR}/default.json" ]; then
        log_error "配置文件不可写: ${CONFIG_DIR}/default.json"
        return 1
    fi
    
    log_success "权限检查通过"
    return 0
}

create_directories() {
    log_info "创建目录结构..."
    
    local dirs=(
        "$LOG_DIR"
        "${INSTALL_DIR}/data"
        "${INSTALL_DIR}/cache"
        "${INSTALL_DIR}/backups"
    )
    
    for dir in "${dirs[@]}"; do
        if [ ! -d "$dir" ]; then
            mkdir -p "$dir"
            log_info "创建目录: $dir"
        fi
    done
    
    log_success "目录结构创建完成"
}

setup_configuration() {
    log_info "设置配置..."
    
    # 检查配置文件是否存在
    if [ ! -f "${CONFIG_DIR}/default.json" ]; then
        log_warning "默认配置文件不存在，创建中..."
        cp "${CONFIG_DIR}/default.json.example" "${CONFIG_DIR}/default.json" 2>/dev/null || true
    fi
    
    # 创建用户配置（如果不存在）
    if [ ! -f "${CONFIG_DIR}/user.json" ]; then
        cat > "${CONFIG_DIR}/user.json" << EOF
{
  "user": {
    "name": "$(whoami)",
    "email": "",
    "preferences": {}
  },
  "custom": {
    "note": "用户自定义配置，会覆盖default.json中的设置"
  }
}
EOF
        log_info "创建用户配置: ${CONFIG_DIR}/user.json"
    fi
    
    log_success "配置设置完成"
}

setup_environment() {
    log_info "设置环境..."
    
    # 创建环境文件
    local env_file="${INSTALL_DIR}/.env"
    if [ ! -f "$env_file" ]; then
        cat > "$env_file" << EOF
# ${SKILL_NAME} 环境配置
SKILL_NAME="${SKILL_NAME}"
SKILL_VERSION="${VERSION}"
SKILL_INSTALL_DIR="${INSTALL_DIR}"
SKILL_LOG_DIR="${LOG_DIR}"

# 性能配置
SKILL_TIMEOUT=30000
SKILL_RETRY=3
SKILL_CACHE=true

# 调试配置
SKILL_DEBUG=false
SKILL_VERBOSE=false

# 注意：修改此文件后需要重启技能
EOF
        log_info "创建环境文件: $env_file"
    fi
    
    # 设置文件权限
    chmod 600 "$env_file"
    
    log_success "环境设置完成"
}

create_service_file() {
    log_info "创建服务文件..."
    
    local service_file="/etc/systemd/system/${SKILL_NAME// /-}.service"
    
    if [ ! -f "$service_file" ]; then
        cat > "/tmp/${SKILL_NAME// /-}.service" << EOF
[Unit]
Description=${SKILL_NAME} Service
After=network.target

[Service]
Type=simple
User=$(whoami)
WorkingDirectory=${INSTALL_DIR}
EnvironmentFile=${INSTALL_DIR}/.env
ExecStart=/bin/bash ${INSTALL_DIR}/scripts/start.sh
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF
        
        log_info "服务文件已创建: /tmp/${SKILL_NAME// /-}.service"
        log_warning "需要手动复制到系统目录: sudo cp /tmp/${SKILL_NAME// /-}.service $service_file"
    else
        log_info "服务文件已存在: $service_file"
    fi
    
    log_success "服务文件创建完成"
}

run_tests() {
    log_info "运行安装测试..."
    
    # 测试配置文件
    if [ -f "${CONFIG_DIR}/default.json" ]; then
        if python3 -m json.tool "${CONFIG_DIR}/default.json" > /dev/null 2>&1; then
            log_success "配置文件JSON格式正确"
        else
            log_error "配置文件JSON格式错误"
            return 1
        fi
    fi
    
    # 测试脚本权限
    local scripts=("setup.sh" "start.sh" "test.sh")
    for script in "${scripts[@]}"; do
        if [ -f "${INSTALL_DIR}/scripts/${script}" ]; then
            if [ -x "${INSTALL_DIR}/scripts/${script}" ]; then
                log_success "脚本可执行: ${script}"
            else
                log_warning "脚本不可执行: ${script}"
                chmod +x "${INSTALL_DIR}/scripts/${script}"
            fi
        fi
    done
    
    log_success "安装测试通过"
    return 0
}

show_summary() {
    echo ""
    echo "="*60
    echo "${SKILL_NAME} 安装完成!"
    echo "="*60
    echo ""
    echo "📁 安装目录: $INSTALL_DIR"
    echo "📄 配置文件: ${CONFIG_DIR}/"
    echo "📝 日志目录: $LOG_DIR"
    echo "🔧 环境文件: ${INSTALL_DIR}/.env"
    echo ""
    echo "🚀 下一步操作:"
    echo "  1. 编辑配置: vim ${CONFIG_DIR}/user.json"
    echo "  2. 测试运行: bash ${INSTALL_DIR}/scripts/test.sh"
    echo "  3. 启动服务: bash ${INSTALL_DIR}/scripts/start.sh"
    echo "  4. 查看日志: tail -f $LOG_DIR/skill.log"
    echo ""
    echo "💡 提示:"
    echo "  - 修改配置后需要重启技能"
    echo "  - 查看帮助: bash ${INSTALL_DIR}/scripts/help.sh"
    echo "  - 卸载技能: bash ${INSTALL_DIR}/scripts/uninstall.sh"
    echo ""
    echo "🎉 安装完成! 开始使用 ${SKILL_NAME} 吧!"
}

# 主函数
main() {
    local skip_deps=false
    local dry_run=false
    local verbose=false
    local interactive=false
    local force=false
    
    # 解析参数
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -c|--config)
                interactive=true
                shift
                ;;
            -d|--dry-run)
                dry_run=true
                shift
                ;;
            -v|--verbose)
                verbose=true
                shift
                ;;
            --skip-deps)
                skip_deps=true
                shift
                ;;
            --force)
                force=true
                shift
                ;;
            *)
                log_error "未知选项: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    echo ""
    log_info "开始安装 ${SKILL_NAME} v${VERSION}"
    log_info "安装目录: $INSTALL_DIR"
    echo ""
    
    if [ "$dry_run" = true ]; then
        log_info "干运行模式，只检查不执行"
        echo ""
    fi
    
    # 执行安装步骤
    local steps=(
        "check_dependencies"
        "check_permissions"
        "create_directories"
        "setup_configuration"
        "setup_environment"
        "create_service_file"
        "run_tests"
    )
    
    for step in "${steps[@]}"; do
        if [ "$dry_run" = true ]; then
            log_info "[干运行] 执行步骤: $step"
        else
            if ! $step; then
                log_error "步骤失败: $step"
                exit 1
            fi
        fi
    done
    
    if [ "$dry_run" = false ]; then
        show_summary
    else
        log_success "干运行完成，所有检查通过"
    fi
}

# 运行主函数
main "$@"