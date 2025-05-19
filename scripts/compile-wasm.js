const { execSync } = require('child_process');
const fs = require('fs');
const path = require('path');
const os = require('os');

const cppDir = path.join(__dirname, '..', 'cpp');
const wasmOutputDir = path.join(__dirname, '..', 'public', 'wasm');
const isWindows = os.platform() === 'win32';

function findEmcc() {
  const localEmcc = isWindows
    ? path.join(__dirname, '..', 'emsdk', 'upstream', 'emscripten', 'emcc.bat')
    : path.join(__dirname, '..', 'emsdk', 'upstream', 'emscripten', 'emcc');
  
  if (fs.existsSync(localEmcc)) {
    return localEmcc;
  }
  
  return isWindows ? 'emcc.bat' : 'emcc';
}

const emcc = findEmcc();
console.log(`Using Emscripten compiler: ${emcc}`);

if (!fs.existsSync(wasmOutputDir)) {
  fs.mkdirSync(wasmOutputDir, { recursive: true });
}

function compileToWasm(inputFile, outputName) {
  console.log(`Compiling ${inputFile} to WebAssembly...`);
  
  try {
    const command = `"${emcc}" "${path.join(cppDir, inputFile)}" -o "${path.join(wasmOutputDir, outputName)}.js" -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] -s EXPORT_ES6=1 -s MODULARIZE=1 -s "EXPORTED_FUNCTIONS=['_malloc','_free']" -s NO_EXIT_RUNTIME=1 -lembind -O3 -std=c++17`;
    
    execSync(command, { stdio: 'inherit' });
    console.log(`Successfully compiled ${inputFile} to WebAssembly`);
  } catch (error) {
    console.error(`Failed to compile ${inputFile}:`, error.message);
    process.exit(1);
  }
}

compileToWasm('sudoku_wasm.cpp', 'sudoku');
compileToWasm('number_slider_wasm.cpp', 'number_slider');
compileToWasm('rubix_wasm.cpp', 'rubix');

console.log('WebAssembly compilation complete!');