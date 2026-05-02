#!/usr/bin/env bash

# bsub -W 23:0 -R'select[ubuntu24_llvm]' -q normal /prj/llvm-arm/home/common/tools/hexframe/bin/hf_run.pl --config-file=/prj/qct/llvm/devops/aether/arm/config/nightly/new_hf_configs/cpullvm-220/comm-profile-generation.ini --top-dir=/prj/qct/llvm/devops/aether/tmp/hexframe/arm/hexbuild --tools-dir=/prj/llvm-arm/hexbuild_home/nightly/install/cpu_pravanv_toolchain/install/install

set -euo pipefail

PR_NUMBER="${PR_NUMBER:-manual}"
RUN_ID="${RUN_ID:-local}"

HF_BIN="/prj/llvm-arm/home/common/tools/hexframe/bin/hf_run.pl"
HF_CONFIG="/prj/qct/llvm/devops/aether/arm/config/nightly/new_hf_configs/cpullvm-220/ubuntu24/llvm-arm-precheckin-correct.ini"
HF_TOOLS="/prj/llvm-arm/hexbuild_home/nightly/install/cpu_pravanv_toolchain/install/install"

# Use a unique top-dir per run to avoid collisions
HF_TOP="/prj/qct/llvm/devops/aether/tmp/hexframe/arm/hexbuild/pr-${PR_NUMBER}-${RUN_ID}"

echo "PR_NUMBER=${PR_NUMBER}"
echo "RUN_ID=${RUN_ID}"
echo "HF_TOP=${HF_TOP}"

test -x "${HF_BIN}"
test -f "${HF_CONFIG}"
test -d "${HF_TOOLS}"

CMD=(
  bsub
  -K
  -W 23:0
  -R "select[ubuntu24_llvm]"
  -q normal
  "${HF_BIN}"
  --config-file="${HF_CONFIG}"
  --top-dir="${HF_TOP}"
  --tools-dir="${HF_TOOLS}"
)

echo "Command to run:"
printf ' %q' "${CMD[@]}"
echo

"${CMD[@]}"
