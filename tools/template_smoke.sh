#!/usr/bin/env bash
# Compile-check every template in templates/ against the in-repo library.
#
# Renders each template the way `mcpp new` does ({{project.name}} /
# {{self.name}} / {{self.version}}), then swaps the generated version
# dependency for a path dependency on this checkout so templates are
# verified BEFORE a release exists in the index.
#
# Usage: bash tools/template_smoke.sh   (requires mcpp on PATH, or $MCPP)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MCPP_BIN="${MCPP:-$(command -v mcpp || true)}"
if [[ -z "$MCPP_BIN" || ! -x "$MCPP_BIN" ]]; then
    echo "FATAL: set MCPP=/path/to/mcpp or put mcpp on PATH" >&2
    exit 1
fi

SELF_NAME="llmapi"
SELF_VERSION="$(sed -n 's/^version *= *"\([^"]*\)".*/\1/p' "$ROOT/mcpp.toml" | head -1)"

# Stay inside the repo so the workspace mcpp pin (.xlings.json) still
# resolves the `mcpp` shim; target/ is gitignored.
TMP="$ROOT/target/template-smoke"
rm -rf "$TMP"
mkdir -p "$TMP"
trap 'rm -rf "$TMP"' EXIT

fail=0
for tdir in "$ROOT"/templates/*/; do
    tname="$(basename "$tdir")"
    proj="$TMP/smoke_$tname"
    mkdir -p "$proj"

    # render: strip .in, expand the placeholder vocabulary
    while IFS= read -r -d '' f; do
        rel="${f#"$tdir"}"
        case "$rel" in template.toml) continue ;; esac
        dest="$proj/${rel%.in}"
        mkdir -p "$(dirname "$dest")"
        if [[ "$f" == *.in ]]; then
            sed -e "s/{{project\.name}}/smoke_$tname/g" \
                -e "s/{{self\.name}}/$SELF_NAME/g" \
                -e "s/{{self\.version}}/$SELF_VERSION/g" "$f" > "$dest"
        else
            cp "$f" "$dest"
        fi
    done < <(find "$tdir" -type f -print0)

    # build against this checkout, not the (possibly unreleased) index version
    python3 - "$proj/mcpp.toml" "$ROOT" <<'EOF'
import sys, re, pathlib
p, root = pathlib.Path(sys.argv[1]), sys.argv[2]
t = p.read_text()
t = re.sub(r'llmapi *= *"[^"]*"', f'llmapi = {{ path = "{root}" }}', t)
p.write_text(t)
EOF

    echo "=== template: $tname ==="
    if ! (cd "$proj" && "$MCPP_BIN" build); then
        echo "FAIL: template '$tname' does not compile" >&2
        fail=1
    fi
done

[[ $fail -eq 0 ]] && echo "All templates compile."
exit $fail
