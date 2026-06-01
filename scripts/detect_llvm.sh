#!/usr/bin/env bash
set -euo pipefail

mode="${1:---human}"

is_root() {
  local root="$1"
  [[ -n "$root" ]] || return 1
  [[ -f "$root/include/flang/Parser/parsing.h" ]] || return 1
  [[ -f "$root/include/llvm/Support/raw_ostream.h" ]] || return 1
  compgen -G "$root/lib/libFortranParser.*" >/dev/null || compgen -G "$root/lib64/libFortranParser.*" >/dev/null || return 1
}

has_flang_include() {
  local root="$1"
  [[ -f "$root/include/flang/Parser/parsing.h" ]]
}

has_llvm_include() {
  local root="$1"
  [[ -f "$root/include/llvm/Support/raw_ostream.h" ]]
}

candidates=()
add_candidate() {
  local value="${1:-}"
  if [[ -n "$value" ]]; then
    candidates+=("$value")
  fi
  return 0
}

add_candidate "${FLANG_ROOT:-}"
add_candidate "${LLVM_ROOT:-}"

if command -v brew >/dev/null 2>&1; then
  add_candidate "$(brew --prefix flang 2>/dev/null || true)"
  add_candidate "$(brew --prefix llvm 2>/dev/null || true)"
fi

if command -v llvm-config >/dev/null 2>&1; then
  add_candidate "$(llvm-config --prefix 2>/dev/null || true)"
fi

for version in 22 21 20 19 18 17 16 15 14; do
  add_candidate "/usr/lib/llvm-${version}"
  add_candidate "/usr/local/llvm-${version}"
done
add_candidate "/opt/homebrew/opt/flang"
add_candidate "/opt/homebrew/opt/llvm"
add_candidate "/usr/local/opt/flang"
add_candidate "/usr/local/opt/llvm"
add_candidate "/opt/llvm"
add_candidate "/usr/local"
add_candidate "/usr"

flang_root=""
llvm_root=""
for candidate in "${candidates[@]}"; do
  [[ -d "$candidate" ]] || continue
  if [[ -z "$flang_root" ]] && has_flang_include "$candidate"; then
    flang_root="$candidate"
  fi
  if [[ -z "$llvm_root" ]] && has_llvm_include "$candidate"; then
    llvm_root="$candidate"
  fi
  if [[ -n "$flang_root" && -n "$llvm_root" ]]; then
    break
  fi
done

if [[ -z "$flang_root" || -z "$llvm_root" ]]; then
  echo "Could not find Flang/LLVM development headers." >&2
  echo "Set FLANG_ROOT and LLVM_ROOT, or install LLVM/Flang development packages." >&2
  exit 1
fi

if [[ "$mode" == "--cmake" ]]; then
  printf -- '-DFLANG_ROOT=%q -DLLVM_ROOT=%q\n' "$flang_root" "$llvm_root"
else
  echo "Detected Flang root: $flang_root"
  echo "Detected LLVM root:  $llvm_root"
  echo "CMake args: -DFLANG_ROOT=$flang_root -DLLVM_ROOT=$llvm_root"
fi
