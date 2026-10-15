import { createServer } from 'node:http';
import { existsSync, createReadStream } from 'node:fs';
import { stat } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { spawn } from 'node:child_process';

const scriptDir = path.dirname(fileURLToPath(import.meta.url));
const blogDir = path.resolve(scriptDir, '..');
const distDir = path.join(blogDir, 'dist');
const shouldBuild = !process.argv.includes('--no-build');
const port = Number(process.env.PORT || 4173);

if (shouldBuild) {
  await new Promise((resolve, reject) => {
    const child = spawn(process.execPath, [path.join(scriptDir, 'build.mjs')], { stdio: 'inherit' });
    child.on('exit', code => code === 0 ? resolve() : reject(new Error(`构建失败，退出码 ${code}`)));
  });
}

const mimeTypes = {
  '.html': 'text/html; charset=utf-8',
  '.css': 'text/css; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.svg': 'image/svg+xml',
  '.png': 'image/png',
  '.jpg': 'image/jpeg',
  '.jpeg': 'image/jpeg',
  '.gif': 'image/gif',
  '.webp': 'image/webp',
  '.xml': 'application/xml; charset=utf-8'
};

const server = createServer(async (request, response) => {
  let requestPath = decodeURIComponent((request.url || '/').split('?')[0]);
  if (requestPath === '/') requestPath = '/index.html';
  const filePath = path.resolve(distDir, `.${requestPath}`);
  if (!filePath.startsWith(distDir) || !existsSync(filePath) || !(await stat(filePath)).isFile()) {
    response.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
    response.end('404 - 页面不存在');
    return;
  }
  response.writeHead(200, { 'Content-Type': mimeTypes[path.extname(filePath).toLowerCase()] || 'application/octet-stream' });
  createReadStream(filePath).pipe(response);
});

server.listen(port, '127.0.0.1', () => {
  console.log(`本地预览：http://localhost:${port}`);
});
