# update-winget-version.ps1 <version>
param(
    [Parameter(Mandatory=$true)]
    [string]$version
)

# Function to get the latest release
function Get-LatestGitHubRelease {
    param (
        [string]$Owner,
        [string]$Repo
    )

    $apiUrl = "https://api.github.com/repos/$Owner/$Repo/releases/latest"
    
    try {
        $response = Invoke-RestMethod -Uri $apiUrl -Method Get -Headers @{
            "Accept" = "application/vnd.github.v3+json"
        }

        return @{
            Version = $response.tag_name
            Url = $response.assets[1].browser_download_url
        }
    }
    catch {
        Write-Error "Failed to fetch the latest release: $_"
        return $null
    }
}

# Check if the version matches the format *.*.*
if ($version -notmatch '^\d+\.\d+\.\d+$') {
    Write-Error "Invalid version format. Please use the format x.y.z (e.g., 1.2.3)"
    exit 1
}

$owner = "LeoKlaus"
$repo = "HeadsetControl-GUI"

$latestRelease = Get-LatestGitHubRelease -Owner $owner -Repo $repo

if (-not $latestRelease) {
    Write-Error "Failed to fetch the latest release information."
    exit 1
}

$latestVersion = $latestRelease.Version.TrimStart('v')  # Remove 'v' prefix if present
$downloadUrl = $latestRelease.Url

if ($version -ne $latestVersion) {
    Write-Error "The vesrsion you gave as parameter($version) is diffferent from the latest released($latestVersion)"
    exit 1
}

# Get User Home Path
$userPath = $env:USERPROFILE
$manifestPath = "$userPath\manifests\l\LeoKlaus\HeadsetControl-GUI\$version"

# Get Latest Manifest
wingetcreate update --urls $downloadUrl --version $version LeoKlaus.HeadsetControl-GUI
# Validate New Manifest
winget validate --manifest $manifestPath
# Test Packet Installation
winget install --manifest $manifestPath
# Test Packet Uninstallation
winget uninstall --manifest $manifestPath
# Send PR with newer Manifest
wingetcreate submit $manifestPath