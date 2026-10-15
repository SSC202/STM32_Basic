import { createHash } from 'node:crypto';
import { copyFile, cp, mkdir, readFile, readdir, rm, stat, writeFile } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { marked } from 'marked';
import hljs from 'highlight.js';
import katex from 'katex';
import markedKatex from 'marked-katex-extension';

const scriptDir = path.dirname(fileURLToPath(import.meta.url));
const blogDir = path.resolve(scriptDir, '..');
const repoDir = path.resolve(blogDir, '..');
const notesDir = path.join(repoDir, 'Note');
const srcDir = path.join(blogDir, 'src');
const distDir = path.join(blogDir, 'dist');

marked.use(markedKatex({ throwOnError: false, nonStandard: true, strict: false }));

const htmlEscape = (value = '') => String(value)
  .replaceAll('&', '&amp;')
  .replaceAll('<', '&lt;')
  .replaceAll('>', '&gt;')
  .replaceAll('"', '&quot;')
  .replaceAll("'", '&#039;');

const trimOrder = (value) => value.replace(/^(?:\d+|[A-Z])[_\s-]+/i, '').trim();
const slugFor = (relativePath) => createHash('sha1')
  .update(relativePath.replaceAll('\\', '/'))
  .digest('hex')
  .slice(0, 12);

async function walk(directory, extension) {
  const entries = await readdir(directory, { withFileTypes: true });
  const found = [];
  for (const entry of entries) {
    const fullPath = path.join(directory, entry.name);
    if (entry.isDirectory()) found.push(...await walk(fullPath, extension));
    if (entry.isFile() && entry.name.toLowerCase().endsWith(extension)) found.push(fullPath);
  }
  return found;
}

function stripMarkdown(value) {
  return value
    .replace(/<[^>]+>/g, ' ')
    .replace(/!\[[^\]]*]\([^)]*\)/g, ' ')
    .replace(/\[([^\]]+)]\([^)]*\)/g, '$1')
    .replace(/[`*_>#|~-]/g, ' ')
    .replace(/\s+/g, ' ')
    .trim();
}

function extractTitle(markdown, fallback) {
  const heading = markdown.match(/^#\s+(.+)$/m)?.[1];
  return stripMarkdown(heading || trimOrder(fallback.replace(/\.md$/i, '')));
}

function extractExcerpt(markdown) {
  const withoutCode = markdown.replace(/```[\s\S]*?```/g, ' ');
  const paragraphs = withoutCode.split(/\r?\n\s*\r?\n/);
  for (const paragraph of paragraphs) {
    if (/^\s*(#|!\[|\||>|[-*+]\s)/.test(paragraph)) continue;
    const text = stripMarkdown(paragraph);
    if (text.length >= 18) return `${text.slice(0, 105)}${text.length > 105 ? '…' : ''}`;
  }
  return 'STM32 学习笔记与工程实践记录。';
}

function readingTime(markdown) {
  const text = stripMarkdown(markdown.replace(/```[\s\S]*?```/g, ' '));
  const code = [...markdown.matchAll(/```[\s\S]*?```/g)].reduce((sum, item) => sum + item[0].length, 0);
  return Math.max(1, Math.ceil((text.length + code / 3) / 500));
}

function categoryFor(relativePath) {
  const first = relativePath.split(path.sep)[0] || '其他';
  return trimOrder(first);
}

function queueLocalImage(article, imageJobs, href) {
  if (!href || /^(https?:|data:|\/|#)/i.test(href)) return href;
  let decodedHref = href;
  try { decodedHref = decodeURIComponent(href); } catch {}
  const source = path.resolve(path.dirname(article.absolutePath), decodedHref);
  const extension = path.extname(source) || '.jpg';
  const imageName = `${imageJobs.length + 1}${extension.toLowerCase()}`;
  const output = path.join(distDir, 'media', article.slug, imageName);
  imageJobs.push({ source, output });
  return `../media/${article.slug}/${imageName}`;
}

function makeRenderer(article, imageJobs) {
  const renderer = new marked.Renderer();
  const originalImage = renderer.image.bind(renderer);
  renderer.image = ({ href, title, text, ...rest }) => {
    return originalImage({
      href: queueLocalImage(article, imageJobs, href),
      title,
      text: text === 'NULL' ? '' : text,
      ...rest
    });
  };
  return renderer;
}

function extractDisplayMath(markdown) {
  const lines = markdown.split(/\r?\n/);
  const blocks = [];
  const output = [];
  const delimiter = /^(?:\s*>\s*)*\s*\$\$\s*$/;
  const stripContainerPrefix = (line) => line.replace(/^\s*(?:>\s*)*/, '');

  for (let index = 0; index < lines.length; index += 1) {
    const line = lines[index];
    if (!delimiter.test(line)) {
      output.push(line);
      continue;
    }

    const content = [];
    let closingIndex = index + 1;
    while (closingIndex < lines.length && !delimiter.test(lines[closingIndex])) {
      content.push(stripContainerPrefix(lines[closingIndex]));
      closingIndex += 1;
    }

    if (closingIndex >= lines.length) {
      output.push(line);
      continue;
    }

    const quotePrefix = line.match(/^\s*((?:>\s*)+)/)?.[1] || '';
    const indentPrefix = quotePrefix ? '' : line.match(/^\s*/)?.[0] || '';
    const placeholder = `STM32BLOGMATHBLOCK${blocks.length}PLACEHOLDER`;
    blocks.push({
      placeholder,
      html: katex.renderToString(content.join('\n').trim(), {
        displayMode: true,
        throwOnError: false,
        strict: false
      })
    });
    output.push(`${indentPrefix}${quotePrefix}${placeholder}`);
    index = closingIndex;
  }

  return { markdown: output.join('\n'), blocks };
}

function renderMarkdown(article, imageJobs) {
  const renderer = makeRenderer(article, imageJobs);
  renderer.code = ({ text, lang }) => {
    const language = lang && hljs.getLanguage(lang) ? lang : 'plaintext';
    const highlighted = hljs.highlight(text, { language }).value;
    return `<pre><div class="code-head"><span>${htmlEscape(lang || 'text')}</span><button class="copy-code" type="button" aria-label="复制代码">复制</button></div><code class="hljs language-${htmlEscape(language)}">${highlighted}</code></pre>`;
  };
  const markdownWithImages = article.markdown.replace(
    /(<img\b[^>]*\bsrc\s*=\s*)(["'])([^"']+)\2/gi,
    (match, prefix, quote, href) => `${prefix}${quote}${queueLocalImage(article, imageJobs, href)}${quote}`
  );
  const displayMath = extractDisplayMath(markdownWithImages);
  let html = marked.parse(displayMath.markdown, {
    renderer,
    gfm: true,
    breaks: false
  });
  for (const block of displayMath.blocks) {
    html = html.replaceAll(block.placeholder, block.html);
  }
  return html;
}

function icon(name) {
  const icons = {
    search: '<svg viewBox="0 0 24 24" aria-hidden="true"><circle cx="11" cy="11" r="7"></circle><path d="m20 20-3.5-3.5"></path></svg>',
    arrow: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="m9 18 6-6-6-6"></path></svg>',
    back: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="m15 18-6-6 6-6"></path></svg>',
    github: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M15 22v-4a4.8 4.8 0 0 0-1-3.5c3.3-.4 6.8-1.6 6.8-7A5.4 5.4 0 0 0 19.3 4 5 5 0 0 0 19.2.5S18 0 15 1.9a13.4 13.4 0 0 0-7 0C5 .1 3.8.5 3.8.5A5 5 0 0 0 3.7 4a5.4 5.4 0 0 0-1.5 3.7c0 5.4 3.5 6.6 6.8 7A4.8 4.8 0 0 0 8 18v4"></path><path d="M8 19c-3 .9-3-1.5-4.2-2"></path></svg>',
    file: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8Z"></path><path d="M14 2v6h6M8 13h8M8 17h5"></path></svg>',
    folder: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M3 6a2 2 0 0 1 2-2h5l2 3h7a2 2 0 0 1 2 2v9a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2Z"></path></svg>',
    book: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M4 19.5A2.5 2.5 0 0 1 6.5 17H20"></path><path d="M6.5 2H20v20H6.5A2.5 2.5 0 0 1 4 19.5v-15A2.5 2.5 0 0 1 6.5 2Z"></path></svg>',
    terminal: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="m4 17 6-6-6-6M12 19h8"></path></svg>',
    cpu: '<svg viewBox="0 0 24 24" aria-hidden="true"><rect x="6" y="6" width="12" height="12" rx="1"></rect><path d="M9 1v3M15 1v3M9 20v3M15 20v3M20 9h3M20 14h3M1 9h3M1 14h3M9 9h6v6H9z"></path></svg>',
    layers: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="m12 2 9 5-9 5-9-5 9-5Z"></path><path d="m3 12 9 5 9-5M3 17l9 5 9-5"></path></svg>',
    network: '<svg viewBox="0 0 24 24" aria-hidden="true"><rect x="9" y="2" width="6" height="6" rx="1"></rect><rect x="2" y="16" width="6" height="6" rx="1"></rect><rect x="16" y="16" width="6" height="6" rx="1"></rect><path d="M12 8v4M5 16v-2h14v2"></path></svg>',
    code: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="m8 9-4 3 4 3M16 9l4 3-4 3M14 5l-4 14"></path></svg>',
    tool: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M14.7 6.3a4 4 0 0 0-5-5L7 4l3 3 2.7-2.7a4 4 0 0 0 2 2Z"></path><path d="m5 21 8.4-8.4M3 17l4 4"></path></svg>',
    monitor: '<svg viewBox="0 0 24 24" aria-hidden="true"><rect x="2" y="3" width="20" height="14" rx="2"></rect><path d="M8 21h8M12 17v4"></path></svg>',
    menu: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M4 7h16M4 12h16M4 17h16"></path></svg>',
    close: '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="m6 6 12 12M18 6 6 18"></path></svg>'
  };
  return icons[name];
}

function shell({ title, description, body, root = '.', pageClass = '', script = '' }) {
  return `<!doctype html>
<html lang="zh-CN">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta name="theme-color" content="#101311">
  <meta name="description" content="${htmlEscape(description)}">
  <title>${htmlEscape(title)}</title>
  <link rel="icon" href="${root}/favicon.svg" type="image/svg+xml">
  <link rel="stylesheet" href="${root}/katex.min.css">
  <link rel="stylesheet" href="${root}/styles.css">
</head>
<body class="${pageClass}">
${body}
${script ? `<script type="module" src="${root}/${script}"></script>` : ''}
</body>
</html>`;
}

function siteHeader(root = '.') {
  return `<header class="site-header">
  <a class="brand" href="${root}/index.html" aria-label="STM32 开发笔记首页"><span class="brand-mark">S</span><span>STM32 开发笔记</span></a>
  <nav class="top-nav" aria-label="主导航">
    <a href="${root}/index.html#paths">学习路径</a>
    <a class="icon-link" href="https://github.com/SSC202/STM32_Basic" target="_blank" rel="noreferrer" aria-label="GitHub 仓库">${icon('github')}</a>
  </nav>
</header>`;
}

function homePage(articles, categories) {
  const starter = articles.find(article => article.category.includes('基本概念')) || articles.at(-1);
  const pathIcons = ['book', 'terminal', 'cpu', 'layers', 'cpu', 'tool', 'layers', 'monitor', 'network', 'network', 'code', 'tool', 'cpu'];
  const pathCards = categories.map(({ name, count, slug }, index) => `<a class="path-card tone-${index % 4}" href="topics/${slug}.html">
    <span class="path-icon">${icon(pathIcons[index] || 'book')}</span>
    <span class="path-order">${String(index + 1).padStart(2, '0')}</span>
    <h3>${htmlEscape(name)}</h3>
    <p>${htmlEscape(categoryDescription(name))}</p>
    <span class="path-link">${count} 篇笔记 ${icon('arrow')}</span>
  </a>`).join('');

  const body = `<div class="page-shell">
    ${siteHeader('.')}
    <main>
      <section class="home-hero" aria-labelledby="intro-title">
        <div class="hero-copy">
          <div class="eyebrow"><span></span>系统化 STM32 学习教程</div>
          <h1 id="intro-title"><span>STM32 开发笔记</span><small>Embedded Systems Notes</small></h1>
          <p>从 Cortex-M 架构和基本外设出发，逐步进入 FreeRTOS、图形界面、网络协议与工程实践。</p>
          <div class="hero-actions">
            <a class="primary-button" href="articles/${starter.slug}.html">从基础开始 ${icon('arrow')}</a>
            <a class="secondary-button" href="#paths">查看学习路径</a>
          </div>
          <div class="hero-stats"><span><b>${articles.length}</b> 篇笔记</span><span><b>${categories.length}</b> 个专题</span><span><b>4</b> 个实践方向</span></div>
        </div>
        <div class="hero-console" aria-label="STM32 示例代码">
          <div class="console-head"><i></i><i></i><i></i><span>main.c · arm-none-eabi</span></div>
          <pre><code><span class="code-dim">#include</span> <span class="code-string">"stm32h7xx_hal.h"</span>

<span class="code-keyword">int</span> main(<span class="code-keyword">void</span>) {
  HAL_Init();
  SystemClock_Config();

  <span class="code-keyword">while</span> (<span class="code-number">1</span>) {
    osThreadYield();
  }
}</code></pre>
          <div class="console-status"><span></span>Build finished · 0 errors</div>
        </div>
      </section>

      <section class="paths-section" id="paths" aria-labelledby="paths-title">
        <div class="section-heading"><div><span>Learning paths</span><h2 id="paths-title">学习路径</h2></div><p>从基础到工程实践，按专题循序阅读</p></div>
        <div class="path-grid">
          ${pathCards}
        </div>
      </section>

    </main>
    <footer><span>STM32 Notes</span><p>持续记录，保持具体。</p><a href="#top">回到顶部 ↑</a></footer>
  </div>`;

  return shell({
    title: 'STM32 Notes · 嵌入式开发笔记',
    description: 'STM32、FreeRTOS、网络协议与嵌入式工程实践笔记。',
    body
  });
}

function categoryDescription(name) {
  const descriptions = [
    ['基本概念', '认识嵌入式系统的组成、约束与开发方式。'],
    ['开发环境', '搭建工具链、调试环境与工程基础配置。'],
    ['总体架构', '理解 Cortex-M 内核、总线、存储与缓存。'],
    ['HAL', '掌握 HAL 库结构与常用驱动接口。'],
    ['基本外设', '覆盖 GPIO、定时器、通信与模拟外设。'],
    ['进阶使用', '连接存储器、传感器、显示与文件系统。'],
    ['FreeRTOS', '学习任务调度、同步通信与内存管理。'],
    ['LVGL', '构建嵌入式图形界面与交互组件。'],
    ['LwIP', '从网络基础进入 TCP/IP 协议栈实践。'],
    ['工业总线', '梳理 CANopen、EtherCAT 与编码器协议。'],
    ['C++', '将现代 C++ 特性应用到嵌入式开发。'],
    ['工程经验', '沉淀工程框架、启动流程与调试方法。'],
    ['硬件设计', '整理原理图设计、EMC 与硬件可靠性经验。']
  ];
  return descriptions.find(([keyword]) => name.includes(keyword))?.[1] || 'STM32 学习笔记与工程实践记录。';
}

function topicPage(category, categories) {
  const index = categories.findIndex(item => item.slug === category.slug);
  const pathIcons = ['book', 'terminal', 'cpu', 'layers', 'cpu', 'tool', 'layers', 'monitor', 'network', 'network', 'code', 'tool', 'cpu'];
  const articleRows = category.articles.map((article, articleIndex) => `<a class="topic-article-row" href="../articles/${article.slug}.html">
    <span class="topic-article-index">${String(articleIndex + 1).padStart(2, '0')}</span>
    <span class="topic-article-copy"><b>${htmlEscape(article.title)}</b><small>${htmlEscape(article.excerpt)}</small></span>
    <span class="topic-article-time">${article.minutes} 分钟</span>
    <span class="topic-article-open">${icon('arrow')}</span>
  </a>`).join('');
  const previous = categories[index - 1];
  const next = categories[index + 1];
  const previousLink = previous
    ? `<a href="${previous.slug}.html"><small>上一专题</small><b>${htmlEscape(previous.name)}</b></a>` : '<span></span>';
  const nextLink = next
    ? `<a class="next" href="${next.slug}.html"><small>下一专题</small><b>${htmlEscape(next.name)}</b></a>` : '<span></span>';

  const body = `<div class="page-shell">
    ${siteHeader('..')}
    <main class="topic-main">
      <a class="topic-breadcrumb" href="../index.html#paths">${icon('back')} 全部学习路径</a>
      <header class="topic-hero tone-${index % 4}">
        <span class="topic-hero-icon">${icon(pathIcons[index] || 'book')}</span>
        <div><span class="topic-kicker">学习路径 ${String(index + 1).padStart(2, '0')}</span><h1>${htmlEscape(category.name)}</h1><p>${htmlEscape(categoryDescription(category.name))}</p></div>
        <span class="topic-count"><b>${category.count}</b> 篇笔记</span>
      </header>
      <section class="topic-articles" aria-labelledby="topic-articles-title">
        <div class="section-heading"><div><span>Contents</span><h2 id="topic-articles-title">专题内容</h2></div><p>建议按顺序阅读</p></div>
        <div class="topic-article-list">${articleRows}</div>
      </section>
      <nav class="topic-pagination" aria-label="相邻专题">${previousLink}${nextLink}</nav>
    </main>
    <footer><span>STM32 Notes</span><p>持续记录，保持具体。</p><a href="#top">回到顶部 ↑</a></footer>
  </div>`;

  return shell({
    title: `${category.name} · STM32 Notes`,
    description: categoryDescription(category.name),
    body,
    root: '..',
    pageClass: 'topic-page'
  });
}

function extractToc(html) {
  const headings = [];
  let index = 0;
  const content = html.replace(/<(h[23])>([\s\S]*?)<\/h[23]>/g, (_, tag, inner) => {
    index += 1;
    const label = inner.replace(/<[^>]+>/g, '').trim();
    const id = `section-${index}`;
    headings.push({ tag, label, id });
    return `<${tag} id="${id}"><a class="heading-anchor" href="#${id}">${inner}</a></${tag}>`;
  });
  return { content, headings };
}

function articlePage(article, previous, next, imageJobs) {
  const rendered = renderMarkdown(article, imageJobs);
  const { content, headings } = extractToc(rendered);
  const toc = headings.length
    ? headings.map(item => `<a class="toc-${item.tag}" href="#${item.id}">${htmlEscape(item.label)}</a>`).join('')
    : '<span class="toc-empty">本文暂无章节目录</span>';
  const previousLink = previous
    ? `<a href="${previous.slug}.html"><small>上一篇</small><span>${htmlEscape(previous.title)}</span></a>` : '<span></span>';
  const nextLink = next
    ? `<a class="next" href="${next.slug}.html"><small>下一篇</small><span>${htmlEscape(next.title)}</span></a>` : '<span></span>';

  const body = `<div class="reading-shell">
    ${siteHeader('..')}
    <button class="toc-toggle" id="toc-toggle" type="button" aria-label="打开文章目录" aria-expanded="false">${icon('menu')}</button>
    <aside class="article-aside" id="article-aside">
      <div class="toc-head"><span>本文目录</span><button id="toc-close" type="button" aria-label="关闭文章目录">${icon('close')}</button></div>
      <nav class="toc" aria-label="文章目录">${toc}</nav>
      <a class="back-link" href="../topics/${article.categorySlug}.html">${icon('back')} 返回专题</a>
    </aside>
    <main class="article-main">
      <header class="article-header">
        <a class="article-category" href="../topics/${article.categorySlug}.html">${htmlEscape(article.category)}</a>
        <h1>${htmlEscape(article.title)}</h1>
        <div class="article-info"><span>${article.minutes} 分钟阅读</span><span>${article.wordCount.toLocaleString('zh-CN')} 字</span><span>持续更新</span></div>
      </header>
      <article class="markdown-body">${content}</article>
      <nav class="article-pagination" aria-label="相邻文章">${previousLink}${nextLink}</nav>
    </main>
  </div>`;

  return shell({
    title: `${article.title} · STM32 Notes`,
    description: article.excerpt,
    body,
    root: '..',
    pageClass: 'article-page',
    script: 'article.js'
  });
}

async function copyImage(job) {
  try {
    await stat(job.source);
    await mkdir(path.dirname(job.output), { recursive: true });
    await copyFile(job.source, job.output);
    return true;
  } catch {
    console.warn(`缺少图片: ${path.relative(repoDir, job.source)}`);
    return false;
  }
}

async function build() {
  await rm(distDir, { recursive: true, force: true });
  await mkdir(path.join(distDir, 'articles'), { recursive: true });
  await mkdir(path.join(distDir, 'topics'), { recursive: true });

  const files = await walk(notesDir, '.md');
  const articles = await Promise.all(files.map(async absolutePath => {
    const relativePath = path.relative(notesDir, absolutePath);
    const markdown = await readFile(absolutePath, 'utf8');
    const info = await stat(absolutePath);
    return {
      absolutePath,
      relativePath,
      markdown,
      slug: slugFor(relativePath),
      title: extractTitle(markdown, path.basename(absolutePath)),
      excerpt: extractExcerpt(markdown),
      category: categoryFor(relativePath),
      minutes: readingTime(markdown),
      wordCount: stripMarkdown(markdown).length,
      modified: info.mtimeMs
    };
  }));

  articles.sort((a, b) => b.modified - a.modified || a.relativePath.localeCompare(b.relativePath, 'zh-CN'));
  const counts = new Map();
  for (const article of articles) counts.set(article.category, (counts.get(article.category) || 0) + 1);
  const categoryOrder = [
    '嵌入式系统的基本概念', 'STM32的开发环境配置', 'STM32的总体架构', 'HAL库简介',
    'STM32基本外设', 'STM32进阶使用', 'FreeRTOS', 'LVGL图形库',
    'LwIP网络编程', '工业总线协议', '嵌入式C++', '工程经验', '硬件设计相关'
  ];
  const categoryRank = (name) => {
    if (name === '硬件设计相关') return Number.MAX_SAFE_INTEGER;
    const index = categoryOrder.indexOf(name);
    return index === -1 ? categoryOrder.length : index;
  };
  const categories = [...counts]
    .map(([name, count]) => ({ name, count, slug: slugFor(`topic:${name}`) }))
    .sort((a, b) => categoryRank(a.name) - categoryRank(b.name));
  for (const category of categories) {
    category.articles = articles
      .filter(article => article.category === category.name)
      .sort((a, b) => a.relativePath.localeCompare(b.relativePath, 'zh-CN', { numeric: true }));
    category.articles.forEach((article, index) => {
      article.categorySlug = category.slug;
      article.previous = category.articles[index - 1] || null;
      article.next = category.articles[index + 1] || null;
    });
  }
  const imageJobs = [];

  await Promise.all(articles.map(article => writeFile(
    path.join(distDir, 'articles', `${article.slug}.html`),
    articlePage(article, article.previous, article.next, imageJobs),
    'utf8'
  )));
  await Promise.all(categories.map(category => writeFile(
    path.join(distDir, 'topics', `${category.slug}.html`),
    topicPage(category, categories),
    'utf8'
  )));
  await Promise.all(imageJobs.map(copyImage));
  await writeFile(path.join(distDir, 'index.html'), homePage(articles, categories), 'utf8');
  await Promise.all(['styles.css', 'article.js', 'favicon.svg'].map(file => copyFile(path.join(srcDir, file), path.join(distDir, file))));
  await copyFile(path.join(blogDir, 'node_modules', 'katex', 'dist', 'katex.min.css'), path.join(distDir, 'katex.min.css'));
  await cp(path.join(blogDir, 'node_modules', 'katex', 'dist', 'fonts'), path.join(distDir, 'fonts'), { recursive: true });

  const sitemap = [
    ...categories.map(category => `<url><loc>topics/${category.slug}.html</loc></url>`),
    ...articles.map(article => `<url><loc>articles/${article.slug}.html</loc></url>`)
  ].join('');
  await writeFile(path.join(distDir, 'sitemap.xml'), `<?xml version="1.0" encoding="UTF-8"?><urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9"><url><loc>index.html</loc></url>${sitemap}</urlset>`, 'utf8');
  console.log(`构建完成：${articles.length} 篇文章，${categories.length} 个专题，${imageJobs.length} 个图片引用。`);
}

await build();
