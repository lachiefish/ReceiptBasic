#pragma once

#include <Arduino.h>
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
  <title>Token Printer</title>
  <style>
    :root { --bg: #0f0f1a; --surface: #1a1a2e; --surface2: #16213e; --accent: #e94560;
            --text: #eee; --muted: #8888aa; --radius: 10px; }
    * { box-sizing: border-box; margin: 0; padding: 0; -webkit-tap-highlight-color: transparent; }
    html, body { height: 100%; overflow: hidden; }
    body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
           background: var(--bg); color: var(--text); display: flex; flex-direction: column; }

    /* ── Header area ── */
    .header { padding: 16px 16px 0; flex-shrink: 0; }
    .title-bar { display: flex; align-items: center; justify-content: center; position: relative;
                 margin-bottom: 14px; }
    h1 { font-size: 1.3em; letter-spacing: 0.5px;
         background: linear-gradient(135deg, var(--accent), #ff6b6b); -webkit-background-clip: text;
         -webkit-text-fill-color: transparent; }

    /* ── Hamburger menu ── */
    .menu-btn { position: absolute; right: 0; top: 50%; transform: translateY(-50%);
                background: none; border: none; color: var(--muted); font-size: 1.5em;
                cursor: pointer; padding: 4px 8px; line-height: 1; transition: color 0.2s; }
    .menu-btn:active, .menu-btn.open { color: var(--accent); }
    .menu-dropdown { display: none; position: absolute; right: 0; top: 100%; margin-top: 6px;
                     background: var(--surface); border: 1px solid var(--surface2);
                     border-radius: var(--radius); box-shadow: 0 8px 24px rgba(0,0,0,0.5);
                     min-width: 180px; z-index: 50; overflow: hidden; }
    .menu-dropdown.open { display: block; }
    .menu-dropdown a { display: block; padding: 13px 16px; color: var(--text); font-size: 0.92em;
                       text-decoration: none; transition: background 0.15s; font-weight: 500; }
    .menu-dropdown a:active { background: var(--accent); }
    @media(hover:hover) { .menu-dropdown a:hover { background: var(--surface2); } }
    .menu-dropdown a + a { border-top: 1px solid var(--surface2); }

    /* ── Tabs ── */
    .tabs { display: flex; gap: 8px; margin-bottom: 12px; }
    .tab { flex: 1; padding: 11px 0; text-align: center; border-radius: var(--radius);
           cursor: pointer; background: var(--surface2); font-weight: 600; font-size: 0.92em;
           transition: all 0.2s; border: 2px solid transparent; }
    .tab.active { background: var(--accent); border-color: var(--accent); }
    .tab:not(.active):active { background: #1e2a4a; }

    /* ── CMC bar ── */
    #cmc-bar { display: none; overflow-x: auto; white-space: nowrap; margin-bottom: 12px;
               padding: 2px 0; -webkit-overflow-scrolling: touch; scrollbar-width: none; }
    #cmc-bar::-webkit-scrollbar { display: none; }
    .cmc-btn { display: inline-block; min-width: 42px; padding: 9px 0; margin-right: 8px;
               text-align: center; border-radius: var(--radius); cursor: pointer;
               background: var(--surface2); font-weight: 600; font-size: 0.92em;
               transition: all 0.2s; border: 2px solid transparent; }
    .cmc-btn.active { background: var(--accent); border-color: var(--accent); }
    .cmc-btn:not(.active):active { background: #1e2a4a; }

    /* ── Search ── */
    .search-wrap { position: relative; margin-bottom: 2px; }
    #search { width: 100%; padding: 13px 40px 13px 14px; font-size: 1em; border-radius: var(--radius);
              border: 2px solid var(--surface2); background: var(--surface); color: var(--text); outline: none;
              transition: border-color 0.2s; }
    #search:focus { border-color: var(--accent); }
    #search::placeholder { color: var(--muted); }
    .clear-btn { position: absolute; right: 10px; top: 50%; transform: translateY(-50%);
                 background: none; border: none; color: var(--muted); font-size: 1.3em;
                 cursor: pointer; display: none; padding: 4px 8px; }
    .clear-btn.visible { display: block; }

    /* ── Results ── */
    .results-area { flex: 1; overflow: hidden; padding: 0 16px; display: flex; flex-direction: column; }
    #count { font-size: 0.8em; color: var(--muted); padding: 8px 2px 6px; flex-shrink: 0; }
    #results { flex: 1; overflow-y: auto; padding-bottom: 16px; -webkit-overflow-scrolling: touch; }
    .card { padding: 14px 16px; border-radius: var(--radius); margin-bottom: 8px;
            background: var(--surface2); cursor: pointer; transition: all 0.15s;
            font-size: 0.95em; border: 2px solid transparent; }
    .card:active { background: var(--accent); transform: scale(0.98); }
    @media(hover:hover) { .card:hover { border-color: var(--accent); background: #1e2a4a; } }

    /* ── Status toast ── */
    #status { position: fixed; bottom: 20px; left: 50%; transform: translateX(-50%);
              padding: 10px 24px; border-radius: 20px; font-weight: 600; font-size: 0.9em;
              pointer-events: none; opacity: 0; transition: opacity 0.3s;
              background: var(--surface); box-shadow: 0 4px 20px rgba(0,0,0,0.5); z-index: 10; }
    #status.show { opacity: 1; }
    #status.printing { border: 2px solid var(--accent); }
    #status.success { border: 2px solid #2ecc71; color: #2ecc71; }
    #status.error { border: 2px solid #e74c3c; color: #e74c3c; }

    .hint { text-align: center; padding: 30px 10px; color: var(--muted); }

    /* ── Spinner ── */
    .spinner { display: inline-block; width: 20px; height: 20px; border: 3px solid var(--surface2);
               border-top-color: var(--accent); border-radius: 50%;
               animation: spin 0.7s linear infinite; vertical-align: middle; margin-right: 8px; }
    @keyframes spin { to { transform: rotate(360deg); } }

    /* ── Preview modal ── */
    #modal-overlay { display: none; position: fixed; inset: 0; z-index: 100;
                     background: rgba(0,0,0,0.85); flex-direction: column; align-items: center;
                     justify-content: center; padding: 16px; }
    #modal-overlay.open { display: flex; }
    #modal-title { font-weight: 700; font-size: 1.1em; margin-bottom: 12px; text-align: center;
                   max-width: 100%; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }
    #preview-canvas { background: #fff; border-radius: 6px; max-width: 90vw; max-height: 55vh;
                      object-fit: contain; image-rendering: pixelated; }
    #preview-loading { color: var(--muted); padding: 40px; }
    .modal-btns { display: flex; gap: 12px; margin-top: 16px; width: 100%; max-width: 320px; }
    .modal-btns button { flex: 1; padding: 14px 0; border: none; border-radius: var(--radius);
                         font-size: 1em; font-weight: 700; cursor: pointer; transition: all 0.15s; }
    .btn-cancel { background: var(--surface2); color: var(--text); }
    .btn-cancel:active { background: #1e2a4a; }
    .btn-print { background: var(--accent); color: #fff; }
    .btn-print:active { background: #c73650; }
  </style>
</head>
<body>
  <div class="header">
    <div class="title-bar">
      <h1>Token Printer</h1>
      <button class="menu-btn" onclick="toggleMenu()" aria-label="Menu">&#9776;</button>
      <div class="menu-dropdown" id="menu">
        <a href="/status">System Status</a>
      </div>
    </div>
    <div class="tabs">
      <div class="tab active" id="tab-tokens" onclick="switchTab('tokens')">Tokens</div>
      <div class="tab" id="tab-creatures" onclick="switchTab('creatures')">Creatures</div>
    </div>
    <div id="cmc-bar"></div>
    <div class="search-wrap">
      <input id="search" type="search" placeholder="Search tokens..." autocomplete="off">
      <button class="clear-btn" id="clear-btn" onclick="clearSearch()">&times;</button>
    </div>
  </div>
  <div class="results-area">
    <div id="count"></div>
    <div id="results"><div class="hint"><div class="spinner"></div> Loading...</div></div>
  </div>
  <div id="status"></div>

  <!-- Preview modal -->
  <div id="modal-overlay" onclick="if(event.target===this)closePreview()">
    <div id="modal-title"></div>
    <div id="preview-loading"><div class="spinner"></div> Loading preview…</div>
    <canvas id="preview-canvas" style="display:none"></canvas>
    <div class="modal-btns">
      <button class="btn-cancel" onclick="closePreview()">Cancel</button>
      <button class="btn-print" id="btn-confirm-print" onclick="confirmPrint()">Print</button>
    </div>
  </div>
<script>
  let cards = [];
  let mode = 'tokens';
  let currentCmc = 0;
  let printing = false;
  const MAX_CMC = 16;

  /* ── Menu toggle ── */
  const menuEl = document.getElementById('menu');
  const menuBtn = document.querySelector('.menu-btn');
  function toggleMenu() {
    const open = menuEl.classList.toggle('open');
    menuBtn.classList.toggle('open', open);
  }
  document.addEventListener('click', e => {
    if (!menuEl.contains(e.target) && !menuBtn.contains(e.target)) {
      menuEl.classList.remove('open');
      menuBtn.classList.remove('open');
    }
  });

  const searchEl = document.getElementById('search');
  const clearBtn = document.getElementById('clear-btn');
  const resultsEl = document.getElementById('results');
  const countEl = document.getElementById('count');
  const statusEl = document.getElementById('status');

  // Build CMC buttons
  (function() {
    const bar = document.getElementById('cmc-bar');
    for (let i = 0; i <= MAX_CMC; i++) {
      const btn = document.createElement('div');
      btn.className = 'cmc-btn' + (i === 0 ? ' active' : '');
      btn.textContent = i;
      btn.id = 'cmc-' + i;
      btn.onclick = () => loadCmc(i);
      bar.appendChild(btn);
    }
  })();

  function switchTab(tab) {
    mode = tab;
    document.getElementById('tab-tokens').className = 'tab' + (tab === 'tokens' ? ' active' : '');
    document.getElementById('tab-creatures').className = 'tab' + (tab === 'creatures' ? ' active' : '');
    document.getElementById('cmc-bar').style.display = tab === 'creatures' ? 'block' : 'none';
    searchEl.value = '';
    clearBtn.className = 'clear-btn';
    if (tab === 'tokens') loadTokens();
    else loadCmc(currentCmc);
  }

  function parseIndex(text, baseDir) {
    const lines = text.trim().split('\n');
    lines.shift();
    return lines.filter(l => l.trim()).map(filename => {
      const name = filename.trim().replace(/\.bin$/i, '');
      return { name, path: baseDir + '/' + filename.trim() };
    }).sort((a, b) => a.name.localeCompare(b.name));
  }

  function showLoading() {
    resultsEl.innerHTML = '<div class="hint"><div class="spinner"></div> Loading...</div>';
    countEl.textContent = '';
  }

  function loadTokens() {
    showLoading();
    fetch('/api/cards')
      .then(r => r.text())
      .then(text => {
        cards = parseIndex(text, '/tokens');
        searchEl.placeholder = 'Search ' + cards.length + ' tokens\u2026';
        filterAndRender();
      })
      .catch(() => { resultsEl.innerHTML = '<div class="hint">Failed to load tokens.</div>'; });
  }

  function loadCmc(cmc) {
    currentCmc = cmc;
    document.querySelectorAll('.cmc-btn').forEach(b => b.className = 'cmc-btn');
    document.getElementById('cmc-' + cmc).className = 'cmc-btn active';
    showLoading();
    fetch('/api/cards?cmc=' + cmc)
      .then(r => r.text())
      .then(text => {
        cards = parseIndex(text, '/cards/' + cmc);
        searchEl.placeholder = 'Search ' + cards.length + ' creatures (CMC ' + cmc + ')\u2026';
        filterAndRender();
      })
      .catch(() => { resultsEl.innerHTML = '<div class="hint">Failed to load creatures.</div>'; });
  }

  function filterAndRender() {
    const q = searchEl.value.toLowerCase().trim();
    const filtered = q ? cards.filter(c => c.name.toLowerCase().includes(q)) : cards;
    countEl.textContent = q ? filtered.length + ' of ' + cards.length : cards.length + ' cards';
    renderResults(filtered.length > 100 ? filtered.slice(0, 100) : filtered);
  }

  searchEl.addEventListener('input', function() {
    clearBtn.className = 'clear-btn' + (this.value ? ' visible' : '');
    filterAndRender();
  });

  function clearSearch() {
    searchEl.value = '';
    clearBtn.className = 'clear-btn';
    searchEl.focus();
    filterAndRender();
  }

  function renderResults(list) {
    if (list.length === 0) {
      resultsEl.innerHTML = '<div class="hint">No results.</div>';
      return;
    }
    resultsEl.innerHTML = list.map(c =>
      '<div class="card" onclick="showPreview(\'' + c.path.replace(/\\/g, '\\\\').replace(/'/g, "\\'") + '\',\'' +
      escapeHtml(c.name).replace(/'/g, "\\'") + '\')">' +
      escapeHtml(c.name) + '</div>'
    ).join('');
  }

  function escapeHtml(s) {
    return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
  }

  function showStatus(msg, cls, duration) {
    statusEl.textContent = msg;
    statusEl.className = cls + ' show';
    if (duration) setTimeout(() => { statusEl.className = ''; }, duration);
  }

  let pendingPath = '';
  const overlay = document.getElementById('modal-overlay');
  const canvas = document.getElementById('preview-canvas');
  const previewLoading = document.getElementById('preview-loading');
  const modalTitle = document.getElementById('modal-title');
  const IMG_W = 384;

  function showPreview(path, name) {
    pendingPath = path;
    modalTitle.textContent = name;
    canvas.style.display = 'none';
    previewLoading.style.display = 'block';
    overlay.className = 'open';

    fetch('/api/preview?path=' + encodeURIComponent(path))
      .then(r => r.arrayBuffer())
      .then(buf => {
        const bytes = new Uint8Array(buf);
        const bytesPerRow = IMG_W / 8; // 48
        const h = Math.floor(bytes.length / bytesPerRow);
        canvas.width = IMG_W;
        canvas.height = h;
        const ctx = canvas.getContext('2d');
        const imgData = ctx.createImageData(IMG_W, h);
        const d = imgData.data;
        for (let row = 0; row < h; row++) {
          for (let col = 0; col < bytesPerRow; col++) {
            const b = bytes[row * bytesPerRow + col];
            for (let bit = 0; bit < 8; bit++) {
              const px = (row * IMG_W + col * 8 + bit) * 4;
              const on = (b >> (7 - bit)) & 1;
              d[px] = d[px+1] = d[px+2] = on ? 255 : 0;
              d[px+3] = 255;
            }
          }
        }
        ctx.putImageData(imgData, 0, 0);
        previewLoading.style.display = 'none';
        canvas.style.display = 'block';
      })
      .catch(() => {
        previewLoading.innerHTML = 'Failed to load preview.';
      });
  }

  function closePreview() {
    overlay.className = '';
    pendingPath = '';
  }

  function confirmPrint() {
    if (!pendingPath || printing) return;
    const path = pendingPath;
    closePreview();
    printCard(path);
  }

  function printCard(path) {
    if (printing) return;
    printing = true;
    showStatus('Printing\u2026', 'printing', 0);
    fetch('/print', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: 'path=' + encodeURIComponent(path)
    })
    .then(r => r.json())
    .then(() => { showStatus('Printed!', 'success', 3000); })
    .catch(() => { showStatus('Error!', 'error', 3000); })
    .finally(() => { printing = false; });
  }

  loadTokens();
</script>
</body>
</html>
)rawliteral";