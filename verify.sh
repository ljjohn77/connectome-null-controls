#!/usr/bin/env bash
# verify.sh — build and run the self-test. No external dependencies.
#
# License: MIT
set -euo pipefail

cd "$(dirname "$0")"

CXX="${CXX:-g++}"
echo "=== building staging_probe ==="
"$CXX" -std=c++17 -O2 -Wall -Wextra tools/staging_probe.cpp -o staging_probe

echo
echo "=== running self-test ==="
./staging_probe

echo
echo "=== expected ==="
cat <<'EOF'
  1. "separable"                must PASS  (accuracy near 1.0, p < 0.05)
  2. "pure noise (must FAIL)"   must FAIL  (proves the criterion can fail)
  3. "random projection"        must PASS  -- and that is the point:
                                a structure-free linear map also reaches ~1.0,
                                so any success claim must beat THIS control.

If line 2 prints PASS, the criterion is broken (false positive).
If line 3 prints FAIL, the task is too easy to need a structure claim.
EOF
