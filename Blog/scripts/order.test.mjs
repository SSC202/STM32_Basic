import assert from 'node:assert/strict';
import path from 'node:path';
import test from 'node:test';
import { sortArticlesByNumber } from './order.mjs';

test('sorts numbered articles and infers an unnumbered sibling', () => {
  const paths = [
    ['signal', '2_crosstalk', 'crosstalk.md'],
    ['signal', '1_power', 'B_2_1_power.md'],
    ['emc', '2_filter', 'B_1_2_filter.md'],
    ['emc', '1_parts', 'B_1_1_parts.md'],
    ['signal', '0_basics', 'B_2_0_basics.md']
  ].map(parts => parts.join(path.sep));
  const result = sortArticlesByNumber(paths.map(relativePath => ({ relativePath })));
  assert.deepEqual(result.map(article => article.relativePath), [paths[3], paths[2], paths[4], paths[1], paths[0]]);
});

test('sorts numeric and nested numbers numerically', () => {
  const paths = ['5_10_CRC.md', '5_3_2_clock.md', '5_3_1_timer.md', '5_2_interrupt.md'];
  const result = sortArticlesByNumber(paths.map(relativePath => ({ relativePath })));
  assert.deepEqual(result.map(article => article.relativePath), [paths[3], paths[2], paths[1], paths[0]]);
});
