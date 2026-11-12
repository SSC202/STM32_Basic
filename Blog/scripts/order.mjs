import path from 'node:path';

const collator = new Intl.Collator('zh-CN', { numeric: true });

function fileOrder(relativePath) {
  const filename = path.basename(relativePath, path.extname(relativePath));
  const prefix = filename.match(/^((?:[A-Z]|\d+)(?:_\d+)*)_(?=\D|$)/i)?.[1];
  return prefix?.split('_').map(part => /^\d+$/.test(part) ? Number(part) : part.toUpperCase());
}

function directoryNumber(directory) {
  const prefix = path.basename(directory).match(/^(\d+)(?:[_\s-]|$)/)?.[1];
  return prefix === undefined ? undefined : Number(prefix);
}

export function sortArticlesByNumber(articles) {
  const groupPrefixes = new Map();
  for (const article of articles) {
    const order = fileOrder(article.relativePath);
    const directory = path.dirname(article.relativePath);
    if (order?.length > 1 && order.at(-1) === directoryNumber(directory)) {
      const group = path.dirname(directory);
      const prefix = order.slice(0, -1);
      if (!groupPrefixes.has(group)) groupPrefixes.set(group, prefix);
      else if (groupPrefixes.get(group) && compareOrder(groupPrefixes.get(group), prefix) !== 0) groupPrefixes.set(group, false);
    }
  }

  const orderFor = article => {
    const explicit = fileOrder(article.relativePath);
    if (explicit) return explicit;
    const directory = path.dirname(article.relativePath);
    const number = directoryNumber(directory);
    const prefix = groupPrefixes.get(path.dirname(directory));
    return prefix && number !== undefined ? [...prefix, number] : null;
  };

  return [...articles].sort((a, b) => {
    const left = orderFor(a);
    const right = orderFor(b);
    if (left && right) return compareOrder(left, right) || collator.compare(a.relativePath, b.relativePath);
    if (left || right) return left ? -1 : 1;
    return collator.compare(a.relativePath, b.relativePath);
  });
}

function compareOrder(left, right) {
  for (let index = 0; index < Math.min(left.length, right.length); index += 1) {
    const a = left[index];
    const b = right[index];
    const result = typeof a === 'number' && typeof b === 'number' ? a - b : collator.compare(String(a), String(b));
    if (result) return result;
  }
  return left.length - right.length;
}
