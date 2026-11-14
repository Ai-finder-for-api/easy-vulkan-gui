$ErrorActionPreference = 'Continue'
$root = 'C:\Users\Yash\evg-work\evg'
$vcvars = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
$cm = 'C:\Program Files\CMake\bin\cmake.exe'
$ct = 'C:\Program Files\CMake\bin\ctest.exe'
$vk = 'C:\Users\yash\vcpkg\installed\x64-windows'
$env:PATH = "$vk\bin;" + $env:PATH
cmd /c "`"$vcvars`" >nul && cd /d `"$root`" && `"$cm`" -S . -B build-noqt -G Ninja -DCMAKE_BUILD_TYPE=Release -DEVG_WITH_QT=OFF -DEVG_WITH_EXR=OFF > $root\cfg_noqt.log 2>&1"
"configure exit: $LASTEXITCODE"
cmd /c "`"$vcvars`" >nul && cd /d `"$root`" && `"$cm`" --build build-noqt --parallel 4 > $root\build_noqt.log 2>&1"
"build exit: $LASTEXITCODE"
Select-String -Path "$root\build_noqt.log" -Pattern 'error|undefined' | Select-Object -First 20 | ForEach-Object { $_.Line }
cmd /c "`"$vcvars`" >nul && cd /d `"$root\build-noqt`" && `"$ct`" --output-on-failure > $root\ctest_noqt.log 2>&1"
"ctest exit: $LASTEXITCODE"
Get-Content "$root\ctest_noqt.log" | Select-Object -Last 12
