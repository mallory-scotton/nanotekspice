const fs = require('fs');
const path = require('path');
const ffmpeg = require('fluent-ffmpeg');
const { createCanvas, loadImage } = require('canvas');

// Configuration
const WIDTH = 64;
const HEIGHT = 64;
const FPS = 24;
var OUTPUT_FILE = 'animation.bin';
var AUDIO_FILE = 'audio.mp3';

// Command types (1 byte each)
const CMD_SET = 0x01;
const CMD_WAIT = 0x02;
const CMD_PLAY = 0x03;

/**
 * Converts a video file to a series of binary commands for a 64x64 simulator
 * @param {string} videoPath - Path to the video file
 * @param {number} targetFps - Target frames per second for the output
 */
async function convertVideoToCommands(videoPath, targetFps = FPS) {
  if (!fs.existsSync(videoPath)) {
    console.error(`Error: Video file not found at ${videoPath}`);
    return;
  }

  // Create a temporary directory for frame extraction
  const tempDir = path.join(__dirname, 'temp_frames');
  if (!fs.existsSync(tempDir)) {
    fs.mkdirSync(tempDir);
  }

  // Create binary file for commands
  const fd = fs.openSync(OUTPUT_FILE, 'w');
  let filePosition = 0;

  console.log('Extracting audio from video...');

  // Extract audio from video
  await new Promise((resolve, reject) => {
    ffmpeg(videoPath)
      .output(AUDIO_FILE)
      .audioCodec('libmp3lame')
      .audioBitrate('128k')
      .on('end', resolve)
      .on('error', reject)
      .run();
  }).catch((err) => {
    console.error('Error extracting audio:', err);
  });

  // Write initial PLAY command to binary file
  const audioFileBuf = Buffer.from(AUDIO_FILE);
  const playCommandBuffer = Buffer.alloc(2 + audioFileBuf.length);
  playCommandBuffer.writeUInt8(CMD_PLAY, 0);
  playCommandBuffer.writeUInt8(audioFileBuf.length, 1);
  audioFileBuf.copy(playCommandBuffer, 2);

  fs.writeSync(fd, playCommandBuffer, 0, playCommandBuffer.length, filePosition);
  filePosition += playCommandBuffer.length;

  console.log('Extracting frames from video...');

  // Extract frames from video
  await new Promise((resolve, reject) => {
    ffmpeg(videoPath)
      .outputOptions([`-vf fps=${targetFps},scale=${WIDTH}:${HEIGHT}:flags=lanczos,transpose=2,format=rgba`])
      .output(path.join(tempDir, 'frame-%04d.png'))
      .on('end', resolve)
      .on('error', reject)
      .run();
  }).catch((err) => {
    console.error('Error extracting frames:', err);
    process.exit(1);
  });

  console.log('Processing frames...');

  // Get all frame files
  const frameFiles = fs
    .readdirSync(tempDir)
    .filter((file) => file.startsWith('frame-'))
    .sort((a, b) => {
      const numA = parseInt(a.match(/frame-(\d+)/)[1]);
      const numB = parseInt(b.match(/frame-(\d+)/)[1]);
      return numA - numB;
    });

  // Process each frame
  let previousPixels = null;

  for (let i = 0; i < frameFiles.length; i++) {
    const frameFile = frameFiles[i];
    const framePath = path.join(tempDir, frameFile);

    // Process the frame and write to binary file
    filePosition = await processFrameBinary(framePath, fd, filePosition, previousPixels);

    // Add wait command after each frame (1 byte for command type)
    const waitBuffer = Buffer.alloc(1);
    waitBuffer.writeUInt8(CMD_WAIT, 0);
    fs.writeSync(fd, waitBuffer, 0, waitBuffer.length, filePosition);
    filePosition += waitBuffer.length;

    // Get pixels for difference calculation in next frame
    previousPixels = await getFramePixels(framePath);

    // Show progress
    if (i % 10 === 0) {
      console.log(`Processed ${i}/${frameFiles.length} frames`);
    }
  }

  fs.closeSync(fd);
  console.log(`Done! Binary commands written to ${OUTPUT_FILE}`);

  // Clean up temp directory
  frameFiles.forEach((file) => {
    fs.unlinkSync(path.join(tempDir, file));
  });
  fs.rmdirSync(tempDir);
}

/**
 * Get pixel data from a frame
 * @param {string} framePath - Path to the frame image
 * @returns {Array} Array of pixel data [x, y, r, g, b, a]
 */
async function getFramePixels(framePath) {
  const image = await loadImage(framePath);
  const canvas = createCanvas(WIDTH, HEIGHT);
  const ctx = canvas.getContext('2d');

  ctx.drawImage(image, 0, 0, WIDTH, HEIGHT);
  const imageData = ctx.getImageData(0, 0, WIDTH, HEIGHT);
  const pixels = [];

  for (let y = 0; y < HEIGHT; y++) {
    for (let x = 0; x < WIDTH; x++) {
      const idx = (y * WIDTH + x) * 4;
      pixels.push([
        x,
        y,
        imageData.data[idx], // R
        imageData.data[idx + 1], // G
        imageData.data[idx + 2], // B
        imageData.data[idx + 3] // A
      ]);
    }
  }

  return pixels;
}

/**
 * Process a frame and write binary commands to the output file
 * @param {string} framePath - Path to the frame image
 * @param {number} fd - File descriptor
 * @param {number} position - Current file position
 * @param {Array} previousPixels - Previous frame's pixels for optimization
 * @returns {number} New file position
 */
async function processFrameBinary(framePath, fd, position, previousPixels) {
  const image = await loadImage(framePath);
  const canvas = createCanvas(WIDTH, HEIGHT);
  const ctx = canvas.getContext('2d');

  ctx.drawImage(image, 0, 0, WIDTH, HEIGHT);
  const imageData = ctx.getImageData(0, 0, WIDTH, HEIGHT);

  // Process each pixel
  let currentPosition = position;

  for (let y = 0; y < HEIGHT; y++) {
    for (let x = 0; x < WIDTH; x++) {
      const idx = (y * WIDTH + x) * 4;
      const r = imageData.data[idx];
      const g = imageData.data[idx + 1];
      const b = imageData.data[idx + 2];
      const a = imageData.data[idx + 3];

      // Skip if pixel is identical to previous frame (optimization)
      if (previousPixels) {
        const prevPixel = previousPixels[y * WIDTH + x];
        if (
          prevPixel &&
          prevPixel[2] === r &&
          prevPixel[3] === g &&
          prevPixel[4] === b &&
          prevPixel[5] === a
        ) {
          continue;
        }
      }

      // Binary format:
      // 1 byte: command type (SET = 0x01)
      // 1 byte: x coordinate (0-63)
      // 1 byte: y coordinate (0-63)
      // 1 byte: R value (0-255)
      // 1 byte: G value (0-255)
      // 1 byte: B value (0-255)
      // 1 byte: A value (0-255)
      // Total: 7 bytes per pixel

      const pixelBuffer = Buffer.alloc(7);
      pixelBuffer.writeUInt8(CMD_SET, 0);
      pixelBuffer.writeUInt8(x, 1);
      pixelBuffer.writeUInt8(y, 2);
      pixelBuffer.writeUInt8(r, 3);
      pixelBuffer.writeUInt8(g, 4);
      pixelBuffer.writeUInt8(b, 5);
      pixelBuffer.writeUInt8(a, 6);

      fs.writeSync(fd, pixelBuffer, 0, pixelBuffer.length, currentPosition);
      currentPosition += pixelBuffer.length;
    }
  }

  return currentPosition;
}

/**
 * Main function to run the converter
 */
async function main() {
  if (process.argv.length < 3) {
    console.log('Usage: node video-converter.js <video_path> [fps]');
    process.exit(1);
  }

  const videoPath = process.argv[2];

  AUDIO_FILE = videoPath.replace(path.extname(videoPath), '') + '.mp3';
  OUTPUT_FILE = videoPath.replace(path.extname(videoPath), '') + '.bin';

  const fps = process.argv[3] ? parseInt(process.argv[3]) : FPS;

  console.log(`Converting video: ${videoPath}`);
  console.log(`Target resolution: ${WIDTH}x${HEIGHT}`);
  console.log(`Target FPS: ${fps}`);
  console.log(`Audio will be extracted to: ${AUDIO_FILE}`);

  await convertVideoToCommands(videoPath, fps);
}

main().catch(console.error);
