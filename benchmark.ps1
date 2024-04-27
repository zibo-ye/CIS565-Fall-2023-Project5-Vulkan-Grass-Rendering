

# Navigate to the script's directory
Push-Location -Path $PSScriptRoot

# Define the working directory where the executable is located
$workingDirectory = Join-Path -Path $PSScriptRoot -ChildPath "build\src"

# Define the log file path relative to the script's location
$logFile = Join-Path -Path $PSScriptRoot -ChildPath "raw_data.log"

# Function to run a process with timeout and log the output
function RunProcess($executable, $arguments, $timeout) {
    $fullCommand = Join-Path -Path $workingDirectory -ChildPath $executable
    $dateStamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    Add-Content -Path $logFile -Value "`n$dateStamp - Running command: $fullCommand $arguments"
    
    $processInfo = New-Object System.Diagnostics.ProcessStartInfo
    $processInfo.FileName = $fullCommand
    $processInfo.Arguments = $arguments
    $processInfo.WorkingDirectory = $workingDirectory
    $processInfo.RedirectStandardOutput = $true
    $processInfo.RedirectStandardError = $true
    $processInfo.UseShellExecute = $false
    $processInfo.CreateNoWindow = $true
    
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $processInfo
    
    if ($process.Start()) {
        $process.WaitForExit($timeout * 1000)
        if (!$process.HasExited) {
            $process.Kill()
        }
        # Log output and errors
        $output = $process.StandardOutput.ReadToEnd()
        $error = $process.StandardError.ReadToEnd()
        Add-Content -Path $logFile -Value "Output: $output"
        Add-Content -Path $logFile -Value "Error: $error"
    } else {
        Add-Content -Path $logFile -Value "Failed to start the process."
    }
}

# Run the vulkan_grass_rendering.exe with different configurations and a 20-second timeout
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode tess --bladeNumExponent 10" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 10" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 10 --enableMeshShaderCompute" 20

RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode tess --bladeNumExponent 12" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 12" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 12 --enableMeshShaderCompute" 20

RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode tess --bladeNumExponent 14" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 14" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 14 --enableMeshShaderCompute" 20

RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode tess --bladeNumExponent 16" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 16" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 16 --enableMeshShaderCompute" 20

RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode tess --bladeNumExponent 18" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 18" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 18 --enableMeshShaderCompute" 20

RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode tess --bladeNumExponent 20" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 20" 20
RunProcess "..\..\bin\Release\vulkan_grass_rendering.exe" "--drawing-size 1920 1080 --mode mesh --bladeNumExponent 20 --enableMeshShaderCompute" 20

# Return to the original directory
cd ..\..\
Pop-Location
