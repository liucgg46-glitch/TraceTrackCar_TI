param(
    [string]$ProjectPath = ""
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($ProjectPath)) {
    $ProjectPath = Join-Path $repoRoot "keil\TraceTrackCar_TI.uvprojx"
}

$ProjectPath = [System.IO.Path]::GetFullPath($ProjectPath)
$projectText = [System.IO.File]::ReadAllText($ProjectPath)

function ConvertTo-XmlText {
    param([string]$Value)

    return [System.Security.SecurityElement]::Escape($Value)
}

function New-KeilFileNode {
    param(
        [string]$Name,
        [string]$Path,
        [int]$Type,
        [bool]$IncludeInBuild = $true
    )

    $nameXml = ConvertTo-XmlText $Name
    $pathXml = ConvertTo-XmlText $Path
    $lines = @(
        "            <File>",
        "              <FileName>$nameXml</FileName>",
        "              <FileType>$Type</FileType>",
        "              <FilePath>$pathXml</FilePath>"
    )

    if (-not $IncludeInBuild) {
        $lines += @(
            "              <FileOption>",
            "                <CommonProperty>",
            "                  <UseCPPCompiler>0</UseCPPCompiler>",
            "                  <RVCTCodeConst>0</RVCTCodeConst>",
            "                  <RVCTZI>0</RVCTZI>",
            "                  <RVCTOtherData>0</RVCTOtherData>",
            "                  <ModuleSelection>0</ModuleSelection>",
            "                  <IncludeInBuild>0</IncludeInBuild>",
            "                  <AlwaysBuild>0</AlwaysBuild>",
            "                  <GenerateAssemblyFile>0</GenerateAssemblyFile>",
            "                  <AssembleAssemblyFile>0</AssembleAssemblyFile>",
            "                  <PublicsOnly>0</PublicsOnly>",
            "                  <StopOnExitCode>3</StopOnExitCode>",
            "                  <CustomArgument></CustomArgument>",
            "                  <IncludeLibraryModules></IncludeLibraryModules>",
            "                  <ComprImg>1</ComprImg>",
            "                </CommonProperty>",
            "              </FileOption>"
        )
    }

    $lines += "            </File>"
    return $lines -join "`r`n"
}

function New-KeilGroup {
    param(
        [string]$Name,
        [array]$Files
    )

    $nodes = foreach ($file in $Files) {
        New-KeilFileNode `
            -Name $file.Name `
            -Path $file.Path `
            -Type $file.Type `
            -IncludeInBuild $file.IncludeInBuild
    }

    return @(
        "        <Group>",
        "          <GroupName>$(ConvertTo-XmlText $Name)</GroupName>",
        "          <Files>",
        ($nodes -join "`r`n"),
        "          </Files>",
        "        </Group>"
    ) -join "`r`n"
}

function Get-LayerFiles {
    param([string]$Layer)

    $directory = Join-Path $repoRoot "User\$Layer"
    $files = Get-ChildItem -LiteralPath $directory -File |
        Where-Object { $_.Extension -in ".c", ".h" } |
        Sort-Object BaseName, @{
            Expression = {
                if ($_.Extension -eq ".c") { 0 } else { 1 }
            }
        }

    return @(
        foreach ($file in $files) {
            [pscustomobject]@{
                Name           = $file.Name
                Path           = "../User/$Layer/$($file.Name)"
                Type           = if ($file.Extension -eq ".c") { 1 } else { 5 }
                IncludeInBuild = ($Layer -ne "Test") -or
                    ($file.Extension -ne ".c")
            }
        }
    )
}

$groups = @()
$groups += New-KeilGroup -Name "User" -Files @(
    [pscustomobject]@{
        Name = "main.c"
        Path = "../User/Core/main.c"
        Type = 1
        IncludeInBuild = $true
    },
    [pscustomobject]@{
        Name = "empty_mspm0g3519.syscfg"
        Path = "../User/Config/empty_mspm0g3519.syscfg"
        Type = 5
        IncludeInBuild = $true
    },
    [pscustomobject]@{
        Name = "startup_mspm0g351x_uvision.s"
        Path = "startup_mspm0g351x_uvision.s"
        Type = 2
        IncludeInBuild = $true
    },
    [pscustomobject]@{
        Name = "ti_msp_dl_config.h"
        Path = "../User/Config/ti_msp_dl_config.h"
        Type = 5
        IncludeInBuild = $true
    },
    [pscustomobject]@{
        Name = "ti_msp_dl_config.c"
        Path = "../User/Config/ti_msp_dl_config.c"
        Type = 1
        IncludeInBuild = $true
    }
)

$layerOrder = @(
    "Common",
    "BSP",
    "Algorithm",
    "Driver",
    "Test",
    "APP",
    "Route",
    "VL53L1_core",
    "VL53L1_platform"
)

foreach ($layer in $layerOrder) {
    $groups += New-KeilGroup -Name $layer -Files (Get-LayerFiles $layer)
}

$driverlibMatch = [regex]::Match(
    $projectText,
    '(?s)\s*<Group>\s*<GroupName>Driverlib</GroupName>.*?</Group>'
)
if (-not $driverlibMatch.Success) {
    throw "Driverlib group was not found in the Keil project."
}
$groups += $driverlibMatch.Value.Trim()

$newGroups = "      <Groups>`r`n" +
    ($groups -join "`r`n") +
    "`r`n      </Groups>"

$updated = [regex]::Replace(
    $projectText,
    '(?s)      <Groups>.*?      </Groups>',
    [System.Text.RegularExpressions.MatchEvaluator]{
        param($match)
        return $newGroups
    },
    1
)

if ($updated -eq $projectText) {
    throw "The Groups section was not updated."
}

[System.IO.File]::WriteAllText(
    $ProjectPath,
    $updated,
    [System.Text.UTF8Encoding]::new($false)
)

Write-Output "Keil project groups synchronized: $ProjectPath"
