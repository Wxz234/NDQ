@echo off

IF NOT EXIST "nuget" (
    mkdir nuget
    curl https://dist.nuget.org/win-x86-commandline/latest/nuget.exe -o nuget\nuget.exe
) ELSE (
    IF NOT EXIST "nuget\nuget.exe" (
        curl https://dist.nuget.org/win-x86-commandline/latest/nuget.exe -o nuget\nuget.exe
    )
)

nuget\nuget.exe install Microsoft.Direct3D.DXC -ExcludeVersion -OutputDirectory nuget\packages
nuget\nuget.exe install Microsoft.Direct3D.D3D12 -ExcludeVersion -OutputDirectory nuget\packages