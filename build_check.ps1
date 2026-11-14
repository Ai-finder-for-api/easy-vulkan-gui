$ErrorActionPreference = 'Continue'
$root = 'C:\Users\Yash\evg-work\evg'
$env_bat = 'C:\Users\Yash\evg-work\env_build.bat'
$cm = 'C:\Program Files\CMake\bin\cmake.exe'
$ct = 'C:\Program Files\CMake\bin\ctest.exe'
$vk = 'C:\Users\yash\vcpkg\installed\x64-windows'
$qt = 'C:\Qt\6.8.3\msvc2022_64'
if (-not (Test-Path "$root\build-1\CMakeCache.txt")) {
  cmd /c "`"$env_bat`" && cd /d `"$root`" && `"$cm`" -S . -B build-1 -G Ninja -DCMAKE_BUILD_TYPE=Release -DEVG_WITH_QT=ON -DEVG_WITH_EXR=ON -DCMAKE_PREFIX_PATH=`"$vk;$qt`" > $root\configure.log 2>&1"
  "configure exit: $LASTEXITCODE"
}
cmd /c "`"$env_bat`" && cd /d `"$root`" && `"$cm`" --build build-1 --parallel 2 > $root\build1.log 2>&1"
"build exit: $LASTEXITCODE"
Select-String -Path "$root\build1.log" -Pattern 'error C|fatal|LNK' | Select-Object -First 20 | ForEach-Object { $_.Line }
cmd /c "`"$env_bat`" && cd /d `"$root\build-1`" && `"$ct`" --output-on-failure > $root\ctest1.log 2>&1"
"ctest exit: $LASTEXITCODE"
Get-Content "$root\ctest1.log" | Select-Object -Last 30
