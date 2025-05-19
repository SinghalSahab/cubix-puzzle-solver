const { execSync } = require("child_process");
const path = require("path");
const os = require("os");
const fs = require("fs");

const isWindows = os.platform() === "win32";
const root = path.join(__dirname, "..");
const emsdkPath = path.join(root, "emsdk");

console.log("Setting up WebAssembly environment...");
function runCommand(command, cwd = root) {
  try {
    console.log(`> ${command}`);
    execSync(command, {
      cwd,
      stdio: "inherit",
      shell: isWindows ? "cmd.exe" : "/bin/bash",
    });
    return true;
  } catch (error) {
    console.error(`Command failed: ${command}`);
    console.error(error.message);
    return false;
  }
}

if (!fs.existsSync(emsdkPath)) {
  console.error(`Emscripten SDK directory not found at ${emsdkPath}`);
  process.exit(1);
}

console.log("\n---- Activating Emscripten SDK ----");
if (!isWindows) {
  try {
    fs.chmodSync(path.join(emsdkPath, "emsdk"), 0o755);
  } catch {}
}
const emsdkBin = isWindows
  ? path.join(emsdkPath, "emsdk.bat")
  : path.join(emsdkPath, "emsdk");
const activateEmSDK = isWindows
  ? `"${emsdkBin}" install latest && "${emsdkBin}" activate latest`
  : `bash "${emsdkBin}" install latest && bash "${emsdkBin}" activate latest`;

if (!runCommand(activateEmSDK)) {
  console.error(
    "Failed to activate Emscripten SDK. Please check the error messages above."
  );
  process.exit(1);
}

console.log("\n---- Setting up Emscripten environment ----");
const emsdkEnv = isWindows
  ? path.join(emsdkPath, "emsdk_env.bat")
  : path.join(emsdkPath, "emsdk_env.sh");
const setupEnv = isWindows ? `call "${emsdkEnv}"` : `source "${emsdkEnv}"`;

if (isWindows) {
  const tempBatchFile = path.join(os.tmpdir(), "setup-emscripten.bat");
  fs.writeFileSync(
    tempBatchFile,
    `
@echo off
cd "${root}"
${setupEnv}
node "${path.join(root, "scripts", "compile-wasm.js")}"
`
  );

  runCommand(`"${tempBatchFile}"`);

  fs.unlinkSync(tempBatchFile);
} else {
  const tempShellFile = path.join(os.tmpdir(), "setup-emscripten.sh");
  fs.writeFileSync(
    tempShellFile,
    `#!/bin/bash
cd "${root}"
${setupEnv}
node "${path.join(root, "scripts", "compile-wasm.js")}"
`
  );

  fs.chmodSync(tempShellFile, "755");
  runCommand(`"${tempShellFile}"`);
  fs.unlinkSync(tempShellFile);
}
console.log("\nSetup complete! WebAssembly modules should be ready to use.");
