module;

#include "version.hpp"

export module version;

export namespace WinuxCmd {
  // Re-export all symbols from version.hpp
  using namespace WinuxCmd;
}