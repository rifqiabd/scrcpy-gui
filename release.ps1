param (
    [Parameter(Mandatory=$true)]
    [string]$Version,
    
    [string]$Message = "Release $Version"
)

# Validate version format (vX.Y.Z)
if ($Version -notmatch "^v\d+\.\d+\.\d+$") {
    Write-Error "Version format must be vX.Y.Z (e.g., v1.0.4)"
    exit 1
}

$RawVersion = $Version.Substring(1) # Remove 'v' prefix

Write-Host "🚀 Preparing release $Version..." -ForegroundColor Cyan

# 1. Update version in CMakeLists.txt
$cmakeFile = "CMakeLists.txt"
$cmakeContent = Get-Content $cmakeFile
$cmakeContent = $cmakeContent -replace "project\(scrcpy-gui VERSION \d+\.\d+\.\d+ LANGUAGES CXX\)", "project(scrcpy-gui VERSION $RawVersion LANGUAGES CXX)"
Set-Content $cmakeFile $cmakeContent
Write-Host "✔ Updated CMakeLists.txt" -ForegroundColor Green

# 2. Update version in main.cpp
$mainCppFile = "src/main.cpp"
$mainCppContent = Get-Content $mainCppFile
$mainCppContent = $mainCppContent -replace 'QApplication::setApplicationVersion\("\d+\.\d+\.\d+"\);', "QApplication::setApplicationVersion(""$RawVersion"");"
Set-Content $mainCppFile $mainCppContent
Write-Host "✔ Updated src/main.cpp" -ForegroundColor Green

# 3. Commit changes
git add CMakeLists.txt src/main.cpp
git commit -m "Bump version to $Version"
Write-Host "✔ Committed version bump" -ForegroundColor Green

# 4. Create Tag
git tag -a $Version -m "$Message"
Write-Host "✔ Created tag $Version" -ForegroundColor Green

# 5. Push Commit and Tag
Write-Host "Pushing to GitHub..." -ForegroundColor Cyan
git push --atomic origin main $Version

if ($?) {
    Write-Host "✅ Release $Version pushed successfully!" -ForegroundColor Green
    Write-Host "Check status at: https://github.com/rifqiabd/scrcpy-gui/actions" -ForegroundColor Yellow
} else {
    Write-Error "Failed to push to GitHub"
}
