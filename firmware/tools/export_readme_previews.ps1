param(
    [string]$SourcePath = (Join-Path $PSScriptRoot '..\..\assets\hmi-ui-v1-vs-v2.png'),
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '..\..\assets\ui'),
    [switch]$Force
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
$previewSource = (Resolve-Path -LiteralPath $SourcePath).Path
$previewOutput = [System.IO.Path]::GetFullPath($OutputDirectory)
$previewNames = @('overview', 'measure', 'faults', 'session', 'device')

# Fixed geometry of the original comparison: 16px gutters, 40px heading,
# two 960x640 columns. Export only the revised (right-hand) column.
$previewBitmap = [System.Drawing.Bitmap]::new($previewSource)
try {
    if ($previewBitmap.Width -ne 1968 -or $previewBitmap.Height -ne 3336) {
        throw 'Expected a 1968x3336 comparison image; review crop geometry before exporting.'
    }
    foreach ($previewName in $previewNames) {
        $previewTarget = Join-Path $previewOutput "$previewName.png"
        if ((Test-Path -LiteralPath $previewTarget) -and -not $Force) {
            throw "Output already exists: $previewTarget. Use -Force to regenerate."
        }
    }
    [System.IO.Directory]::CreateDirectory($previewOutput) | Out-Null
    for ($previewIndex = 0; $previewIndex -lt $previewNames.Count; $previewIndex++) {
        $previewRect = [System.Drawing.Rectangle]::new(992, 56 + $previewIndex * 656, 960, 640)
        $previewCrop = $previewBitmap.Clone($previewRect, $previewBitmap.PixelFormat)
        try {
            $previewTarget = Join-Path $previewOutput "$($previewNames[$previewIndex]).png"
            $previewCrop.Save($previewTarget, [System.Drawing.Imaging.ImageFormat]::Png)
            Write-Output $previewTarget
        }
        finally {
            $previewCrop.Dispose()
        }
    }
}
finally {
    $previewBitmap.Dispose()
}
