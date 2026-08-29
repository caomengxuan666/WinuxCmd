#!/bin/bash
mkdir -p /tmp/winuxcmd-wrappers
for exe in /mnt/d/repo/unixwin-winuxcmd/build-vs/usr/bin/*.exe; do
  name=$(basename "$exe" .exe)
  printf '#!/bin/bash\nexec "%s" "$@"\n' "$exe" > "/tmp/winuxcmd-wrappers/$name"
  chmod +x "/tmp/winuxcmd-wrappers/$name"
done
echo "Created $(ls /tmp/winuxcmd-wrappers/ | wc -l) wrappers"
