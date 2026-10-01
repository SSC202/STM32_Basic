const aside = document.querySelector('#article-aside');
const toggle = document.querySelector('#toc-toggle');
const close = document.querySelector('#toc-close');

function setToc(open) {
  aside.classList.toggle('open', open);
  toggle.setAttribute('aria-expanded', String(open));
}

toggle?.addEventListener('click', () => setToc(!aside.classList.contains('open')));
close?.addEventListener('click', () => setToc(false));
aside?.querySelectorAll('a').forEach(link => link.addEventListener('click', () => setToc(false)));

document.querySelectorAll('.copy-code').forEach(button => {
  button.addEventListener('click', async () => {
    const code = button.closest('pre')?.querySelector('code')?.textContent || '';
    await navigator.clipboard.writeText(code);
    button.textContent = '已复制';
    window.setTimeout(() => { button.textContent = '复制'; }, 1400);
  });
});

const headings = [...document.querySelectorAll('.markdown-body h2[id], .markdown-body h3[id]')];
const tocLinks = [...document.querySelectorAll('.toc a[href^="#"]')];
if (headings.length && 'IntersectionObserver' in window) {
  const observer = new IntersectionObserver(entries => {
    const current = entries.filter(entry => entry.isIntersecting).at(-1);
    if (!current) return;
    tocLinks.forEach(link => link.classList.toggle('active', link.hash === `#${current.target.id}`));
  }, { rootMargin: '-15% 0px -75%' });
  headings.forEach(heading => observer.observe(heading));
}
