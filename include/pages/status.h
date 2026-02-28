#pragma once

#include <Arduino.h>

static const char STATUS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
  <title>System Status</title>
  <style>
    :root { --bg: #0f0f1a; --surface: #1a1a2e; --surface2: #16213e; --accent: #e94560;
            --text: #eee; --muted: #8888aa; --radius: 10px; --green: #2ecc71; --yellow: #f1c40f; --red: #e74c3c; }
    * { box-sizing: border-box; margin: 0; padding: 0; -webkit-tap-highlight-color: transparent; }
    body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
           background: var(--bg); color: var(--text); padding: 16px; }
    h1 { text-align: center; font-size: 1.3em; margin-bottom: 6px; letter-spacing: 0.5px;
         background: linear-gradient(135deg, var(--accent), #ff6b6b); -webkit-background-clip: text;
         -webkit-text-fill-color: transparent; }
    .back { display: block; text-align: center; color: var(--muted); font-size: 0.85em;
            margin-bottom: 16px; text-decoration: none; }
    .back:active { color: var(--accent); }
    .section { margin-bottom: 16px; }
    .section-title { font-size: 0.75em; text-transform: uppercase; letter-spacing: 1px;
                     color: var(--muted); margin-bottom: 8px; padding-left: 4px; }
    .panel { background: var(--surface2); border-radius: var(--radius); padding: 14px 16px; }
    .row { display: flex; justify-content: space-between; align-items: center;
           padding: 8px 0; border-bottom: 1px solid rgba(255,255,255,0.05); }
    .row:last-child { border-bottom: none; }
    .label { color: var(--muted); font-size: 0.88em; }
    .value { font-weight: 600; font-size: 0.92em; text-align: right; }
    .bar-wrap { width: 100%; height: 8px; background: var(--surface); border-radius: 4px; margin-top: 6px; }
    .bar-fill { height: 100%; border-radius: 4px; transition: width 0.5s; }
    .bar-green { background: var(--green); }
    .bar-yellow { background: var(--yellow); }
    .bar-red { background: var(--red); }
    .refresh-info { text-align: center; font-size: 0.75em; color: var(--muted); margin-top: 12px; }
  </style>
</head>
<body>
  <h1>System Status</h1>
  <a class="back" href="/">&larr; Back to Printer</a>
  <div id="content"><div style="text-align:center;color:var(--muted);padding:40px;">Loading...</div></div>
  <div class="refresh-info">Auto-refreshes every 2s</div>
<script>
  function fmt(bytes) {
    if (bytes >= 1048576) return (bytes/1048576).toFixed(1) + ' MB';
    if (bytes >= 1024) return (bytes/1024).toFixed(1) + ' KB';
    return bytes + ' B';
  }
  function pct(used, total) { return total > 0 ? Math.round(used/total*100) : 0; }
  function barClass(p) { return p > 85 ? 'bar-red' : p > 60 ? 'bar-yellow' : 'bar-green'; }
  function bar(used, total, invert) {
    const p = pct(used, total);
    const cls = invert ? barClass(100-p) : barClass(p);
    return '<div class="bar-wrap"><div class="bar-fill ' + cls + '" style="width:' + p + '%"></div></div>';
  }
  function row(label, value, extra) {
    return '<div class="row"><span class="label">' + label + '</span><span class="value">' + value + '</span></div>' + (extra||'');
  }

  function render(d) {
    const heapUsed = d.heap_total - d.heap_free;
    const heapPct = pct(heapUsed, d.heap_total);
    const psramUsed = d.psram_total - d.psram_free;
    const psramPct = pct(psramUsed, d.psram_total);
    const heapMinPct = pct(d.heap_total - d.heap_min_free, d.heap_total);

    let h = '';

    h += '<div class="section"><div class="section-title">System</div><div class="panel">';
    h += row('Uptime', d.uptime_str);
    h += row('Chip', d.chip_model + ' rev ' + d.chip_revision);
    h += row('Cores', d.chip_cores);
    h += row('CPU Frequency', d.cpu_freq + ' MHz');
    h += row('SDK', d.sdk_version);
    h += '</div></div>';

    h += '<div class="section"><div class="section-title">Memory</div><div class="panel">';
    h += row('Heap', fmt(heapUsed) + ' / ' + fmt(d.heap_total) + ' (' + heapPct + '%)', bar(heapUsed, d.heap_total));
    h += row('Heap Low Watermark', fmt(d.heap_total - d.heap_min_free) + ' peak (' + heapMinPct + '%)', bar(d.heap_total - d.heap_min_free, d.heap_total));
    if (d.psram_total > 0) {
      h += row('PSRAM', fmt(psramUsed) + ' / ' + fmt(d.psram_total) + ' (' + psramPct + '%)', bar(psramUsed, d.psram_total));
    }
    h += '</div></div>';

    h += '<div class="section"><div class="section-title">Storage</div><div class="panel">';
    h += row('SD Card', d.sd_used + ' / ' + d.sd_total + ' MB', bar(d.sd_used, d.sd_total));
    h += row('Flash', (d.flash_size/1024).toFixed(0) + ' MB');
    h += row('Firmware', d.sketch_size + ' KB / ' + (d.sketch_size + d.sketch_free) + ' KB',
             bar(d.sketch_size, d.sketch_size + d.sketch_free));
    h += '</div></div>';

    h += '<div class="section"><div class="section-title">Network</div><div class="panel">';
    h += row('WiFi Clients', d.wifi_clients);
    h += row('AP IP', '192.168.4.1');
    h += '</div></div>';

    document.getElementById('content').innerHTML = h;
  }

  function poll() {
    fetch('/api/status').then(r=>r.json()).then(render).catch(()=>{});
  }
  poll();
  setInterval(poll, 2000);
</script>
</body>
</html>
)rawliteral";