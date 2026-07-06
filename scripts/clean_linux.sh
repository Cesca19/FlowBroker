#!/usr/bin/env bash

# ==============================
#  Project Cleanup Script
#  Deletes build artifacts safely
#  Directories: build, bin
# ==============================

set -e

# --- Colors ---
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
RED="\033[0;31m"
BLUE="\033[1;34m"
RESET="\033[0m"

VIRTUAL_ENV_NAME=".conan_venv"

# --- Logging functions ---
log_info()    { echo -e "${BLUE}[INFO]${RESET} $1"; }
log_success() { echo -e "${GREEN}[OK]${RESET} $1"; }
log_warn()    { echo -e "${YELLOW}[WARN]${RESET} $1"; }
log_error()   { echo -e "${RED}[ERROR]${RESET} $1"; }

# --- Delete folder function ---
delete_folder() {
    local folder="$1"

    if [ -z "$folder" ]; then
        log_error "No folder specified for deletion."
        exit 1
    fi

    if [ -d "$folder" ]; then
        log_warn "Deleting folder: $folder ..."
        rm -rf "$folder"
        if [ $? -eq 0 ]; then
            log_success "Successfully deleted: $folder"
        else
            log_error "Failed to delete: $folder"
            exit 1
        fi
    else
        log_info "Folder not found: $folder (nothing to delete)"
    fi
}

# --- Main ---
log_info "Starting cleanup process..."

delete_folder "build"
delete_folder "bin"
#delete_folder ${VIRTUAL_ENV_NAME}

log_success "Cleanup completed successfully."
