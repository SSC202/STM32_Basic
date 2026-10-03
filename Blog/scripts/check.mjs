import { access, readFile, readdir } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const scriptDir = path.dirname(fileURLToPath(import.meta.url));
const distDir = path.resolve(scriptDir, '..', 'dist');

async function walk(directory) {
  const entries = await readdir(directory, { withFileTypes: true });
  const files = [];
  for (const entry of entries) {
    const fullPath = path.join(directory, entry.name);
    if (entry.isDirectory()) files.push(...await walk(fullPath));
    if (entry.isFile()) files.push(fullPath);
  }
  return files;
}

const files = await walk(distDir);
const htmlFiles = files.filter(file => file.endsWith('.html'));
const failures = [];
const sequenceFailures = [];
const rawMathFailures = [];
let references = 0;

for (const htmlFile of htmlFiles) {
  const html = await readFile(htmlFile, 'utf8');
  if (html.includes('$$')) {
    rawMathFailures.push(path.relative(distDir, htmlFile));
  }
  const attributes = html.matchAll(/\b(?:href|src)\s*=\s*["']([^"']+)["']/gi);
  for (const [, reference] of attributes) {
    if (/^(?:https?:|data:|mailto:|#)/i.test(reference)) continue;
    const cleanReference = reference.split(/[?#]/)[0];
    if (!cleanReference) continue;
    references += 1;
    const target = path.resolve(path.dirname(htmlFile), decodeURIComponent(cleanReference));
    try {
      await access(target);
    } catch {
      failures.push(`${path.relative(distDir, htmlFile)} -> ${reference}`);
    }
  }
}

const topicFiles = htmlFiles.filter(file => path.dirname(file) === path.join(distDir, 'topics'));
for (const topicFile of topicFiles) {
  const topicHtml = await readFile(topicFile, 'utf8');
  const articleSlugs = [...topicHtml.matchAll(/class="topic-article-row" href="\.\.\/articles\/([a-f0-9]+)\.html"/g)]
    .map(match => match[1]);

  for (let index = 0; index < articleSlugs.length; index += 1) {
    const slug = articleSlugs[index];
    const articleHtml = await readFile(path.join(distDir, 'articles', `${slug}.html`), 'utf8');
    const pagination = articleHtml.match(/<nav class="article-pagination"[^>]*>([\s\S]*?)<\/nav>/)?.[1] || '';
    const actualLinks = [...pagination.matchAll(/href="([a-f0-9]+\.html)"/g)].map(match => match[1]);
    const expectedLinks = [articleSlugs[index - 1], articleSlugs[index + 1]]
      .filter(Boolean)
      .map(item => `${item}.html`);

    if (actualLinks.join('|') !== expectedLinks.join('|')) {
      sequenceFailures.push(`${path.basename(topicFile)} / ${slug}: 期望 ${expectedLinks.join(', ') || '无'}，实际 ${actualLinks.join(', ') || '无'}`);
    }
  }
}

if (failures.length || sequenceFailures.length || rawMathFailures.length) {
  console.error(`发现 ${failures.length} 个失效的本地引用：`);
  failures.slice(0, 30).forEach(failure => console.error(`- ${failure}`));
  if (sequenceFailures.length) {
    console.error(`发现 ${sequenceFailures.length} 个文章顺序错误：`);
    sequenceFailures.slice(0, 30).forEach(failure => console.error(`- ${failure}`));
  }
  if (rawMathFailures.length) {
    console.error(`发现 ${rawMathFailures.length} 个仍含原始 $$ 的页面：`);
    rawMathFailures.slice(0, 30).forEach(failure => console.error(`- ${failure}`));
  }
  process.exitCode = 1;
} else {
  console.log(`检查通过：${htmlFiles.length} 个页面，${references} 个本地引用，${topicFiles.length} 个专题的文章顺序。`);
}
