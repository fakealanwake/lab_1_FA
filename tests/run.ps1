param([switch]$NoBuild)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

function Find-GccBin {
    $cmd = Get-Command gcc -ErrorAction SilentlyContinue
    if ($cmd) {
        return Split-Path -Parent $cmd.Source
    }
    $pkg = Join-Path $env:LOCALAPPDATA "Microsoft\WinGet\Packages"
    if (-not (Test-Path $pkg)) {
        return $null
    }
    $hit = Get-ChildItem $pkg -Directory -Filter "BrechtSanders.WinLibs*" | Select-Object -First 1
    if (-not $hit) {
        return $null
    }
    $bin = Join-Path $hit.FullName "mingw64\bin"
    if (Test-Path (Join-Path $bin "gcc.exe")) {
        return $bin
    }
    return $null
}

$bin = Find-GccBin
if (-not $bin) {
    Write-Output "FAIL gcc not found"
    exit 1
}
$env:Path = "$bin;" + $env:Path

if (-not $NoBuild) {
    & mingw32-make -C $root all
    if ($LASTEXITCODE -ne 0) {
        Write-Output "FAIL build"
        exit 1
    }
}

$utf8 = New-Object System.Text.UTF8Encoding $false
$casesPath = Join-Path $PSScriptRoot "cases.json"
$raw = [IO.File]::ReadAllText($casesPath, $utf8)
if ($raw.Length -gt 0 -and [int]$raw[0] -eq 0xFEFF) {
    $raw = $raw.Substring(1)
}
$cases = $raw | ConvertFrom-Json

function Norm([string]$text) {
    if ($null -eq $text) {
        return ""
    }
    return $text.Replace("`r`n", "`n").Replace("`r", "`n")
}

function Write-TextFile([string]$path, [string]$text) {
    $dir = Split-Path -Parent $path
    if ($dir -and -not (Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
    }
    if ($null -eq $text) {
        $text = ""
    }
    [IO.File]::WriteAllText($path, $text, $utf8)
}

function Read-TextFile([string]$path) {
    if (-not (Test-Path $path)) {
        return $null
    }
    return Norm ([IO.File]::ReadAllText($path, $utf8))
}

function Invoke-Lab([string]$exe, [string]$argLine, [string]$stdinText) {
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = (Join-Path $root $exe)
    $psi.Arguments = $argLine
    $psi.WorkingDirectory = $root
    $psi.UseShellExecute = $false
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError = $true
    $psi.RedirectStandardInput = $true
    $psi.StandardOutputEncoding = $utf8
    $psi.StandardErrorEncoding = $utf8
    $psi.CreateNoWindow = $true
    $proc = [Diagnostics.Process]::Start($psi)
    if ($null -ne $stdinText) {
        $proc.StandardInput.Write((Norm $stdinText))
    }
    $proc.StandardInput.Close()
    $out = $proc.StandardOutput.ReadToEnd()
    $err = $proc.StandardError.ReadToEnd()
    $proc.WaitForExit()
    return @{ Code = $proc.ExitCode; Out = (Norm $out); Err = (Norm $err) }
}

function Get-Floats([string]$text) {
    $list = New-Object System.Collections.Generic.List[double]
    foreach ($m in [regex]::Matches($text, "-?\d+\.\d+")) {
        $list.Add([double]::Parse($m.Value, [Globalization.CultureInfo]::InvariantCulture))
    }
    return $list
}

$passed = 0
$failed = 0
$report = New-Object System.Collections.Generic.List[string]

foreach ($case in $cases) {
    $problems = New-Object System.Collections.Generic.List[string]
    if ($case.PSObject.Properties.Name -contains "prepare") {
        foreach ($file in @($case.prepare)) {
            Write-TextFile (Join-Path $root $file.path) $file.text
        }
    }
    $stdin = $null
    if ($case.PSObject.Properties.Name -contains "stdin") {
        $stdin = $case.stdin
    }
    $result = Invoke-Lab $case.exe $case.args $stdin
    if ($result.Code -ne [int]$case.exit) {
        $problems.Add("exit $($result.Code), expected $($case.exit)")
    }
    if ($case.PSObject.Properties.Name -contains "stdout" -and (Norm $result.Out) -ne (Norm $case.stdout)) {
        $problems.Add("stdout:`n$($result.Out)")
    }
    if ($case.PSObject.Properties.Name -contains "stderr" -and (Norm $result.Err) -ne (Norm $case.stderr)) {
        $problems.Add("stderr:`n$($result.Err)")
    }
    if ($case.PSObject.Properties.Name -contains "files") {
        foreach ($file in @($case.files)) {
            $actual = Read-TextFile (Join-Path $root $file.path)
            if ($null -eq $actual) {
                $problems.Add("missing $($file.path)")
            } elseif ($actual -ne (Norm $file.text)) {
                $problems.Add("file $($file.path): [$actual]")
            }
        }
    }
    if ($case.PSObject.Properties.Name -contains "floats") {
        $got = Get-Floats $result.Out
        $expectedFloats = @($case.floats)
        if ($got.Count -ne $expectedFloats.Count) {
            $problems.Add("float count $($got.Count), expected $($expectedFloats.Count)")
        } else {
            for ($i = 0; $i -lt $got.Count; $i++) {
                $exp = [double]$expectedFloats[$i].v
                $tol = [double]$expectedFloats[$i].t
                if ([math]::Abs($got[$i] - $exp) -gt $tol) {
                    $problems.Add("float[$i]=$($got[$i]) expected $exp tol $tol")
                }
            }
        }
    }
    if ($case.PSObject.Properties.Name -contains "contain") {
        $needle = $case.contain
        $count = ([regex]::Matches($result.Out, [regex]::Escape($needle))).Count
        $expectCount = 1
        if ($case.PSObject.Properties.Name -contains "containCount") {
            $expectCount = [int]$case.containCount
        }
        if ($count -ne $expectCount) {
            $problems.Add("contain '$needle' x$count, expected $expectCount")
        }
    }
    if ($case.PSObject.Properties.Name -contains "check" -and $case.check -eq "task9") {
        $lines = @($result.Out -split "`n" | Where-Object { $_ -ne "" })
        $rows = @()
        foreach ($line in $lines) {
            $colon = $line.LastIndexOf(":")
            if ($colon -lt 0) { continue }
            $rest = $line.Substring($colon + 1).Trim()
            if ($rest -match "^-?\d+(\s+-?\d+)*$") {
                $nums = @($rest -split "\s+" | ForEach-Object { [int]$_ })
                $rows += ,$nums
            }
        }
        if ($rows.Count -lt 2) {
            $problems.Add("task9: no before/after rows")
        } else {
            $before = $rows[0]
            $after = $rows[1]
            if ($before.Count -ne 20 -or $after.Count -ne 20) {
                $problems.Add("task9: fixed length $($before.Count)/$($after.Count)")
            } else {
                $min = [int]($before | Measure-Object -Minimum).Minimum
                $max = [int]($before | Measure-Object -Maximum).Maximum
                $imin = [array]::IndexOf($before, $min)
                $imax = [array]::IndexOf($before, $max)
                if ($after[$imin] -ne $max -or $after[$imax] -ne $min) {
                    $problems.Add("task9: min/max not swapped")
                }
                foreach ($n in $before) {
                    if ($n -lt 1 -or $n -gt 5) {
                        $problems.Add("task9: value $n outside 1..5")
                        break
                    }
                }
            }
        }
        $size = [regex]::Match($result.Out, "A: (\d+), .*B: (\d+)")
        if (-not $size.Success) {
            $problems.Add("task9: sizes not printed")
        } else {
            $aLen = [int]$size.Groups[1].Value
            $bLen = [int]$size.Groups[2].Value
            if ($aLen -lt 10 -or $aLen -gt 10000 -or $bLen -lt 10 -or $bLen -gt 10000) {
                $problems.Add("task9: bad lengths $aLen $bLen")
            }
            $cCount = 0
            foreach ($row in $rows) {
                if ($row.Count -eq $aLen) {
                    $cCount++
                }
            }
            $need = 1
            if ($aLen -eq 20) {
                $need = 3
            }
            if ($cCount -lt $need) {
                $problems.Add("task9: C length is not $aLen")
            }
        }
    }
    if ($problems.Count -eq 0) {
        $passed++
        Write-Output "OK $($case.id)"
        if ($case.PSObject.Properties.Name -contains "cleanup") {
            foreach ($path in @($case.cleanup)) {
                $full = Join-Path $root $path
                if (Test-Path $full) {
                    Remove-Item $full -Force
                }
            }
        }
    } else {
        $failed++
        Write-Output "FAIL $($case.id)"
        $report.Add("FAIL $($case.id)")
        foreach ($item in $problems) {
            $report.Add($item)
        }
        $report.Add("---")
    }
}

$summary = "passed $passed failed $failed"
Write-Output $summary
$report.Insert(0, $summary)
[IO.File]::WriteAllText((Join-Path $PSScriptRoot "report.txt"), ($report -join "`n"), $utf8)
if ($failed -gt 0) {
    exit 1
}
exit 0
