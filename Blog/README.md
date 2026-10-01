# STM32 Notes Blog

这个目录是仓库笔记的独立静态博客。构建脚本会读取上一级目录中的 `Note/`，不会修改原始笔记。

## 本地使用

```powershell
cd Blog
npm install
npm run dev
```

浏览器访问 `http://localhost:4173`。修改笔记或网页源码后，重新运行 `npm run build` 即可刷新 `dist/`。

使用 `npm run check` 可以检查生成页面中的本地链接和图片是否完整。

## 静态部署

```powershell
npm run build
```

将生成的 `dist/` 目录部署到 GitHub Pages、Netlify、Cloudflare Pages 或任意静态文件服务器。构建过程会生成文章页、搜索索引、站点地图，并复制笔记引用的本地图片。
