/** Dependencies */
const fs = require('fs');
const path = require('path');
const ffmpeg = require('fluent-ffmpeg');
const { createCanvas, loadImage } = require('canvas');

/** Constants */
const SIZE = 64;
const DEFAULT_FPS = 12;

const CMD_SET = 0x01;
const CMD_WAIT = 0x02;
const CMD_PLAY = 0x03;

/** Processing */
let SCREEN = Array(SIZE * SIZE * 4).fill(-255);
let FD = null;
let POSITION = 0;
let TRESHOLD = 3;

/**
 * @brief Convert a position (x, y) to an idx in the pixels
 * @param x The x coordinate
 * @param y The y coordinate
 * @retrun The current id at position
 */
const posToId = (x, y) => {
  return (y * SIZE + x) * 4;
};

/**
 * @brief Check if two values are almost the same
 * @param x The first value
 * @param y The second value
 * @param treshold The treshold
 * @return Is x almost y
 */
const almost = (x, y) => {
  return Math.abs(x - y) > TRESHOLD;
};

/**
 * @brief Process a specific frame from a filepath
 * @param filepath The path of the frame
 */
const processFrame = async (filepath) => {
  const image = await loadImage(filepath);
  const canvas = createCanvas(SIZE, SIZE);
  const context = canvas.getContext('2d');

  context.drawImage(image, 0, 0, SIZE, SIZE);
  const data = context.getImageData(0, 0, SIZE, SIZE).data;
  const pixels = [];

  for (let y = 0; y < SIZE; y++) {
    for (let x = 0; x < SIZE; x++) {
      const idx = posToId(x, y);
      const r = data[idx],
        g = data[idx + 1],
        b = data[idx + 2];

      if (almost(r, SCREEN[idx]) || almost(g, SCREEN[idx + 1]) || almost(b, SCREEN[idx + 2])) {
        pixels.push({ x, y, r, g, b });
        SCREEN[idx] = r;
        SCREEN[idx + 1] = g;
        SCREEN[idx + 2] = b;
      }
    }
  }

  if (pixels.length == 0) {
    return;
  }

  const buffer = Buffer.alloc(pixels.length * 6);
  let offset = 0;

  for (const pixel of pixels) {
    buffer.writeUint8(CMD_SET, offset++);
    buffer.writeUint8(pixel.x, offset++);
    buffer.writeUint8(pixel.y, offset++);
    buffer.writeUint8(pixel.r, offset++);
    buffer.writeUint8(pixel.g, offset++);
    buffer.writeUint8(pixel.b, offset++);
  }

  await new Promise((resolve, reject) => {
    fs.write(FD, buffer, 0, buffer.length, POSITION, (err) => {
      if (err) reject(err);
      else {
        POSITION += buffer.length;
        resolve();
      }
    });
  });
};

/**
 * @brief Convert a video to commands
 * @param filepath The path of the video
 * @param targetFps The target fps
 */
const convertVideoToCommands = async (filepath, targetFps = DEFAULT_FPS) => {
  if (!fs.existsSync(filepath)) {
    console.error(`Cannot find ${filepath}.`);
    return;
  }

  const dir = path.join(__dirname, 'temp');
  if (!fs.existsSync(dir)) {
    fs.mkdirSync(dir);
  }

  const outputPath = filepath.replace(path.extname(filepath), '') + '.anim';
  const audioPath = filepath.replace(path.extname(filepath), '') + '.mp3';

  FD = fs.openSync(outputPath, 'w');
  POSITION = 0;

  console.log('Extracting audio from video...');

  await new Promise((resolve, reject) => {
    ffmpeg(filepath)
      .output(audioPath)
      .audioCodec('libmp3lame')
      .audioBitrate('128k')
      .on('end', resolve)
      .on('error', reject)
      .run();
  }).catch((err) => {
    console.error('Error extracting audio:', err);
  });

  const audioFileBuf = Buffer.from(audioPath);
  const playCommandBuffer = Buffer.alloc(2 + audioFileBuf.length);
  playCommandBuffer.writeUInt8(CMD_PLAY, 0);
  playCommandBuffer.writeUInt8(audioFileBuf.length, 1);
  audioFileBuf.copy(playCommandBuffer, 2);

  fs.writeSync(FD, playCommandBuffer, 0, playCommandBuffer.length, POSITION);
  POSITION += playCommandBuffer.length;

  console.log('Extracting frames from video...');

  await new Promise((resolve, reject) => {
    ffmpeg(filepath)
      .outputOptions([
        `-vf fps=${targetFps},scale=${SIZE}:${SIZE}:flags=lanczos,transpose=2,format=rgba`
      ])
      .output(path.join(dir, 'frame-%04d.png'))
      .on('end', resolve)
      .on('error', reject)
      .run();
  }).catch((err) => {
    console.error('Error extracting frames:', err);
    process.exit(1);
  });

  console.log('Processing frames...');

  const frames = fs
    .readdirSync(dir)
    .filter((file) => file.startsWith('frame-'))
    .sort((a, b) => {
      const numA = parseInt(a.match(/frame-(\d+)/)[1]);
      const numB = parseInt(b.match(/frame-(\d+)/)[1]);
      return numA - numB;
    });

  for (let i = 0; i < frames.length; i++) {
    await processFrame(path.join(dir, frames[i]));

    const percentage = ((i + 1) / frames.length) * 100;
    console.log(`Processing frames: ${percentage.toFixed(2)}% completed`);

    const waitBuffer = Buffer.alloc(1);
    waitBuffer.writeUint8(CMD_WAIT, 0);
    fs.writeSync(FD, waitBuffer, 0, waitBuffer.length, POSITION);
    POSITION++;
  }

  fs.closeSync(FD);
  fs.rmSync(dir, { recursive: true, force: true });
};

const main = async () => {
  if (process.argv.length < 3) {
    console.log('Usage: node video-converter.js <video_path> [fps]');
    process.exit(1);
  }

  const fps = process.argv[3] ? parseInt(process.argv[3]) : DEFAULT_FPS;
  TRESHOLD = process.argv[4] ? parseInt(process.argv[4]) : TRESHOLD;

  console.log(`Converting video: ${process.argv[2]}`);
  console.log(`Target resolution: ${SIZE}x${SIZE}`);
  console.log(`Target FPS: ${fps}`);
  console.log(`Target Treshold: ${TRESHOLD}`);

  await convertVideoToCommands(process.argv[2], fps);
};

main().catch(console.error);
