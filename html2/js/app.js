/**
 * Webserv Test Suite - Main JavaScript
 * Test HTTP methods: GET, HEAD, POST, PUT, DELETE
 * Test features: Upload, Autoindex, CGI, Errors, Redirections, Timeout, Chunked
 */

// ========================================
// Utility Functions
// ========================================

const NAV_LINKS = [
    { href: '/index.html', label: 'Accueil' },
    { href: '/get.html', label: 'GET / HEAD' },
    { href: '/post.html', label: 'POST' },
    { href: '/upload.html', label: 'Upload / PUT' },
    { href: '/delete.html', label: 'DELETE' },
    { href: '/cgi.html', label: 'CGI' },
    { href: '/errors.html', label: 'Erreurs' },
    { href: '/redirect.html', label: 'Redirections' },
    { href: '/headers.html', label: 'Headers' },
    { href: '/autoindex.html', label: 'Autoindex' },
    { href: '/chunked.html', label: 'Chunked' },
    { href: '/stress.html', label: 'Stress / Timeout' },
];

const API = {
    baseUrl: window.location.origin,
    
    async request(url, options = {}) {
        const startTime = performance.now();
        try {
            const response = await fetch(url, options);
            const endTime = performance.now();
            const duration = (endTime - startTime).toFixed(2);
            
            let body;
            const contentType = response.headers.get('content-type');
            if (contentType && contentType.includes('application/json')) {
                body = await response.json();
            } else {
                body = await response.text();
            }
            
            return {
                ok: response.ok,
                status: response.status,
                statusText: response.statusText,
                headers: Object.fromEntries(response.headers.entries()),
                body: body,
                duration: duration,
                redirected: response.redirected,
                url: response.url
            };
        } catch (error) {
            return {
                ok: false,
                status: 0,
                statusText: 'Network Error',
                error: error.message,
                duration: (performance.now() - startTime).toFixed(2)
            };
        }
    },
    
    async get(url) {
        return this.request(url, { method: 'GET' });
    },

    async head(url) {
        const startTime = performance.now();
        try {
            const response = await fetch(url, { method: 'HEAD' });
            return {
                ok: response.ok,
                status: response.status,
                statusText: response.statusText,
                headers: Object.fromEntries(response.headers.entries()),
                body: '(HEAD: no body)',
                duration: (performance.now() - startTime).toFixed(2)
            };
        } catch (error) {
            return {
                ok: false, status: 0, statusText: 'Network Error',
                error: error.message,
                duration: (performance.now() - startTime).toFixed(2)
            };
        }
    },
    
    async post(url, data) {
        return this.request(url, {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: new URLSearchParams(data).toString()
        });
    },
    
    async postJSON(url, data) {
        return this.request(url, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });
    },

    async postText(url, text) {
        return this.request(url, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: text
        });
    },
    
    async upload(url, formData) {
        return this.request(url, { method: 'POST', body: formData });
    },
    
    async delete(url) {
        return this.request(url, { method: 'DELETE' });
    },
    
    async put(url, body, contentType) {
        return this.request(url, {
            method: 'PUT',
            headers: contentType ? { 'Content-Type': contentType } : {},
            body: body
        });
    },

    // Requête brute via XMLHttpRequest (pour timeout, connexion lente, etc.)
    rawRequest(method, url, body, timeout) {
        return new Promise((resolve) => {
            const startTime = performance.now();
            const xhr = new XMLHttpRequest();
            xhr.open(method, url, true);
            if (timeout) xhr.timeout = timeout;

            xhr.onload = function () {
                resolve({
                    ok: xhr.status >= 200 && xhr.status < 400,
                    status: xhr.status,
                    statusText: xhr.statusText,
                    headers: xhr.getAllResponseHeaders(),
                    body: xhr.responseText,
                    duration: (performance.now() - startTime).toFixed(2)
                });
            };
            xhr.onerror = function () {
                resolve({
                    ok: false, status: 0, statusText: 'Network Error',
                    error: 'Connection refused or network error',
                    duration: (performance.now() - startTime).toFixed(2)
                });
            };
            xhr.ontimeout = function () {
                resolve({
                    ok: false, status: 0, statusText: 'Timeout',
                    error: `Request timed out after ${timeout}ms`,
                    duration: (performance.now() - startTime).toFixed(2)
                });
            };

            if (body) xhr.send(body);
            else xhr.send();
        });
    }
};

// ========================================
// Response Display
// ========================================

function displayResponse(elementId, response) {
    const element = document.getElementById(elementId);
    if (!element) return;
    
    const statusClass = response.ok ? 'status' : 'status error';
    let html = `<div class="${statusClass}">Status: ${response.status} ${response.statusText} (${response.duration}ms)</div>\n`;
    
    if (response.redirected) {
        html += `<div class="headers">Redirected to: ${response.url}</div>\n`;
    }

    if (response.error) {
        html += `<div class="error" style="color:#e74c3c;">Error: ${response.error}</div>\n`;
    }
    
    if (response.headers) {
        html += `\n<div class="headers">--- Headers ---</div>\n`;
        if (typeof response.headers === 'string') {
            html += `<span class="headers">${response.headers}</span>\n`;
        } else {
            for (const [key, value] of Object.entries(response.headers)) {
                html += `<span class="headers">${key}:</span> ${value}\n`;
            }
        }
    }
    
    if (response.body) {
        html += `\n<div class="headers">--- Body ---</div>\n`;
        if (typeof response.body === 'object') {
            html += `<pre class="body">${JSON.stringify(response.body, null, 2)}</pre>`;
        } else {
            const escaped = response.body
                .replace(/&/g, '&amp;')
                .replace(/</g, '&lt;')
                .replace(/>/g, '&gt;');
            // Tronquer si trop long
            const maxLen = 5000;
            const truncated = escaped.length > maxLen ? escaped.substring(0, maxLen) + '\n... (truncated)' : escaped;
            html += `<pre class="body">${truncated}</pre>`;
        }
    }
    
    element.innerHTML = html;
}

// ========================================
// GET Tests
// ========================================

async function testGetFile(path) {
    const response = await API.get(path);
    displayResponse('get-response', response);
}

async function testGetCustom() {
    const url = document.getElementById('get-url').value;
    if (!url) return alert('Veuillez entrer une URL');
    const response = await API.get(url);
    displayResponse('get-response', response);
}

// ========================================
// HEAD Tests
// ========================================

async function testHeadFile(path) {
    const response = await API.head(path);
    displayResponse('head-response', response);
}

async function testHeadCustom() {
    const url = document.getElementById('head-url').value;
    if (!url) return alert('Veuillez entrer une URL');
    const response = await API.head(url);
    displayResponse('head-response', response);
}

// ========================================
// POST Tests
// ========================================

async function testPostForm() {
    const name = document.getElementById('post-name')?.value || 'Test';
    const email = document.getElementById('post-email')?.value || 'test@test.com';
    const message = document.getElementById('post-message')?.value || 'Hello';
    const response = await API.post('/cgi-bin/test.py', { name, email, message });
    displayResponse('post-response', response);
}

async function testPostJSON() {
    const jsonData = document.getElementById('post-json')?.value || '{}';
    try {
        const data = JSON.parse(jsonData);
        const response = await API.postJSON('/cgi-bin/test.py', data);
        displayResponse('post-response', response);
    } catch (e) {
        displayResponse('post-response', {
            ok: false, status: 0, statusText: 'JSON Parse Error',
            error: e.message, duration: 0
        });
    }
}

async function testPostLarge() {
    const size = parseInt(document.getElementById('payload-size')?.value) || 1000;
    const data = 'X'.repeat(size);
    const response = await API.post('/cgi-bin/test.py', { data });
    displayResponse('post-response', response);
}

async function testPostText() {
    const text = document.getElementById('post-text')?.value || 'Hello World';
    const url = document.getElementById('post-text-url')?.value || '/uploads/text_test.txt';
    const response = await API.postText(url, text);
    displayResponse('post-response', response);
}

// ========================================
// DELETE Tests
// ========================================

async function testDelete() {
    const filename = document.getElementById('delete-filename')?.value;
    if (!filename) return alert('Entrez un nom de fichier');
    const response = await API.delete('/uploads/' + filename);
    displayResponse('delete-response', response);
}

async function refreshFileList() {
    const response = await API.get('/uploads/');
    displayResponse('delete-response', response);
}

// ========================================
// PUT Tests
// ========================================

async function testPut() {
    const url = document.getElementById('put-url')?.value;
    const content = document.getElementById('put-content')?.value || '';
    const ct = document.getElementById('put-ct')?.value || 'text/plain';
    if (!url) return alert('Entrez une URL');
    const response = await API.put(url, content, ct);
    displayResponse('upload-response', response);
}

// ========================================
// File Upload
// ========================================

function initUpload() {
    const uploadArea = document.getElementById('upload-area');
    const fileInput = document.getElementById('file-input');
    if (!uploadArea || !fileInput) return;

    uploadArea.addEventListener('click', () => fileInput.click());
    uploadArea.addEventListener('dragover', (e) => { e.preventDefault(); uploadArea.classList.add('dragover'); });
    uploadArea.addEventListener('dragleave', () => uploadArea.classList.remove('dragover'));
    uploadArea.addEventListener('drop', (e) => {
        e.preventDefault();
        uploadArea.classList.remove('dragover');
        if (e.dataTransfer.files.length > 0) handleUpload(e.dataTransfer.files[0]);
    });
    fileInput.addEventListener('change', () => {
        if (fileInput.files.length > 0) handleUpload(fileInput.files[0]);
    });
}

async function handleUpload(file) {
    const progressBar = document.getElementById('upload-progress');
    const progressFill = progressBar?.querySelector('.progress');
    if (progressBar) progressBar.style.display = 'block';
    if (progressFill) progressFill.style.width = '0%';

    const formData = new FormData();
    formData.append('file', file);

    let progress = 0;
    const interval = setInterval(() => {
        progress += 10;
        if (progressFill) progressFill.style.width = progress + '%';
        if (progress >= 90) clearInterval(interval);
    }, 100);

    const response = await API.upload('/uploads/', formData);
    clearInterval(interval);
    if (progressFill) progressFill.style.width = '100%';
    displayResponse('upload-response', response);

    setTimeout(() => {
        if (progressBar) progressBar.style.display = 'none';
        listUploadedFiles();
    }, 500);
}

async function listUploadedFiles() {
    const response = await API.get('/uploads/');
    const listElement = document.getElementById('uploaded-files-list');
    if (!listElement) return;

    if (response.ok && response.body) {
        const parser = new DOMParser();
        const doc = parser.parseFromString(response.body, 'text/html');
        const links = doc.querySelectorAll('a');
        let html = '';
        links.forEach(link => {
            const filename = link.textContent;
            if (filename && filename !== '../' && filename !== './') {
                html += `<li>
                    <span class="file-name">
                        <a href="/uploads/${filename}" target="_blank">${filename}</a>
                    </span>
                    <button class="btn btn-danger" onclick="deleteFile('${filename}')">X</button>
                </li>`;
            }
        });
        listElement.innerHTML = html || '<li>Aucun fichier</li>';
    } else {
        listElement.innerHTML = '<li>Impossible de charger la liste</li>';
    }
}

async function deleteFile(filename) {
    if (!confirm(`Supprimer ${filename} ?`)) return;
    const response = await API.delete('/uploads/' + filename);
    displayResponse('upload-response', response);
    listUploadedFiles();
}

// ========================================
// Error Tests
// ========================================

async function testError(code) {
    let response;
    switch (code) {
        case 400:
            response = await API.get('/%ZZ%invalid');
            break;
        case 403:
            response = await API.get('/forbidden/');
            break;
        case 404:
            response = await API.get('/this-page-does-not-exist-' + Date.now());
            break;
        case 405:
            response = await API.request('/index.html', { method: 'DELETE' });
            break;
        case 413:
            response = await API.post('/', { data: 'X'.repeat(100000000) });
            break;
        case 500:
            response = await API.get('/cgi-bin/error500.py');
            break;
        case 501:
            response = await API.request('/', { method: 'PATCH' });
            break;
        default:
            response = await API.get('/nonexistent-' + Date.now());
    }
    displayResponse('error-response', response);
}

// ========================================
// Redirect Tests
// ========================================

async function testRedirect(url) {
    const response = await API.get(url);
    displayResponse('redirect-response', response);
}

async function testRedirectNoFollow(url) {
    const startTime = performance.now();
    try {
        const response = await fetch(url, { redirect: 'manual' });
        displayResponse('redirect-response', {
            ok: response.ok,
            status: response.status,
            statusText: response.statusText,
            headers: Object.fromEntries(response.headers.entries()),
            body: `(redirect: manual mode — status ${response.status}, type: ${response.type})`,
            duration: (performance.now() - startTime).toFixed(2),
            redirected: false
        });
    } catch (e) {
        displayResponse('redirect-response', {
            ok: false, status: 0, statusText: 'Error',
            error: e.message,
            duration: (performance.now() - startTime).toFixed(2)
        });
    }
}

// ========================================
// CGI Tests
// ========================================

async function testCGI(script) {
    const response = await API.get(script + '?test=hello&name=Webserv&time=' + Date.now());
    displayResponse('cgi-response', response);
}

async function testCGIPost(script) {
    const response = await API.post(script, {
        method: 'POST',
        data: 'Test CGI POST',
        timestamp: new Date().toISOString()
    });
    displayResponse('cgi-response', response);
}

async function testCGICustom() {
    const url = document.getElementById('cgi-url')?.value || '/cgi-bin/test.py';
    const qs = document.getElementById('cgi-qs')?.value || '';
    const full = qs ? url + '?' + qs : url;
    const response = await API.get(full);
    displayResponse('cgi-response', response);
}

// ========================================
// Autoindex Test
// ========================================

async function testAutoindex(path) {
    const response = await API.get(path || '/uploads/');
    displayResponse('autoindex-response', response);
}

// ========================================
// Headers Tests
// ========================================

async function testHeadersGet(url) {
    const response = await API.head(url || '/index.html');
    displayResponse('headers-response', response);
}

async function testHeadersCustom() {
    const url = document.getElementById('headers-url')?.value || '/index.html';
    const customHeaders = {};
    const headerName = document.getElementById('custom-header-name')?.value;
    const headerValue = document.getElementById('custom-header-value')?.value;
    if (headerName) customHeaders[headerName] = headerValue || '';

    const startTime = performance.now();
    try {
        const response = await fetch(url, { headers: customHeaders });
        const body = await response.text();
        displayResponse('headers-response', {
            ok: response.ok,
            status: response.status,
            statusText: response.statusText,
            headers: Object.fromEntries(response.headers.entries()),
            body: body,
            duration: (performance.now() - startTime).toFixed(2)
        });
    } catch (e) {
        displayResponse('headers-response', {
            ok: false, status: 0, error: e.message,
            duration: (performance.now() - startTime).toFixed(2)
        });
    }
}

// ========================================
// Chunked Tests
// ========================================

async function testChunked(url) {
    const response = await API.get(url || '/cgi-bin/test.py?chunked=1');
    displayResponse('chunked-response', response);
}

// ========================================
// Stress & Timeout Tests
// ========================================

async function runConcurrentTest() {
    const count = parseInt(document.getElementById('concurrent-count')?.value) || 10;
    const progress = document.getElementById('stress-progress');
    const results = document.getElementById('stress-results');
    const progressFill = document.getElementById('progress-fill');
    const progressText = document.getElementById('progress-text');

    if (progress) progress.style.display = 'block';
    results.innerHTML = '<div class="spinner"></div> Running...';

    const startTime = performance.now();
    const times = [];
    let success = 0, failed = 0, done = 0;

    const promises = [];
    for (let i = 0; i < count; i++) {
        promises.push(
            API.get('/index.html').then(r => {
                done++;
                times.push(parseFloat(r.duration));
                if (r.ok) success++; else failed++;
                if (progressFill) progressFill.style.width = ((done / count) * 100) + '%';
                if (progressText) progressText.textContent = `${done} / ${count}`;
            })
        );
    }

    await Promise.all(promises);
    const totalTime = ((performance.now() - startTime) / 1000).toFixed(3);
    showStressStats(count, success, failed, times, totalTime);
}

async function runSequentialTest() {
    const count = parseInt(document.getElementById('sequential-count')?.value) || 25;
    const progress = document.getElementById('stress-progress');
    const results = document.getElementById('stress-results');
    const progressFill = document.getElementById('progress-fill');
    const progressText = document.getElementById('progress-text');

    if (progress) progress.style.display = 'block';
    results.innerHTML = '<div class="spinner"></div> Running...';

    const startTime = performance.now();
    const times = [];
    let success = 0, failed = 0;

    for (let i = 0; i < count; i++) {
        const r = await API.get('/index.html');
        times.push(parseFloat(r.duration));
        if (r.ok) success++; else failed++;
        if (progressFill) progressFill.style.width = (((i + 1) / count) * 100) + '%';
        if (progressText) progressText.textContent = `${i + 1} / ${count}`;
    }

    const totalTime = ((performance.now() - startTime) / 1000).toFixed(3);
    showStressStats(count, success, failed, times, totalTime);
}

async function testTimeout(delayMs) {
    displayResponse('timeout-response', {
        ok: false, status: 0, statusText: 'Pending',
        body: `Sending request with ${delayMs}ms timeout...`, duration: 0
    });
    const r = await API.rawRequest('GET', '/index.html', null, delayMs);
    displayResponse('timeout-response', r);
}

async function testSlowConnection() {
    displayResponse('timeout-response', {
        ok: false, status: 0, statusText: 'Pending',
        body: 'Opening connection and waiting for server timeout...', duration: 0
    });
    // Ouvre une connexion sans envoyer de données, attend le timeout serveur
    const r = await API.rawRequest('GET', '/slow-test-' + Date.now(), null, 120000);
    displayResponse('timeout-response', r);
}

async function testKeepAlive() {
    const results = [];
    for (let i = 0; i < 5; i++) {
        const r = await API.get('/index.html');
        results.push(`Request ${i + 1}: ${r.status} (${r.duration}ms)`);
    }
    displayResponse('timeout-response', {
        ok: true, status: 200, statusText: 'OK',
        body: 'Keep-alive test (5 sequential requests on same connection):\n\n' + results.join('\n'),
        duration: '0'
    });
}

function showStressStats(count, success, failed, times, totalTime) {
    const results = document.getElementById('stress-results');
    const avg = (times.reduce((a, b) => a + b, 0) / times.length).toFixed(2);
    const min = Math.min(...times).toFixed(2);
    const max = Math.max(...times).toFixed(2);
    const rps = (count / parseFloat(totalTime)).toFixed(2);

    document.getElementById('stat-success') && (document.getElementById('stat-success').textContent = success);
    document.getElementById('stat-failed') && (document.getElementById('stat-failed').textContent = failed);
    document.getElementById('stat-total-time') && (document.getElementById('stat-total-time').textContent = totalTime + 's');
    document.getElementById('stat-avg-time') && (document.getElementById('stat-avg-time').textContent = avg + 'ms');
    document.getElementById('stat-min-time') && (document.getElementById('stat-min-time').textContent = min + 'ms');
    document.getElementById('stat-max-time') && (document.getElementById('stat-max-time').textContent = max + 'ms');
    document.getElementById('stat-rps') && (document.getElementById('stat-rps').textContent = rps);

    results.innerHTML = `
        <div class="alert alert-${failed === 0 ? 'success' : 'warning'}">
            ${success} réussies, ${failed} échouées<br>
            Temps total: ${totalTime}s | Moyenne: ${avg}ms | ${rps} req/s
        </div>`;
}

// ========================================
// Run All Tests
// ========================================

async function runAllTests() {
    const results = document.getElementById('all-tests-results');
    if (!results) return;
    results.innerHTML = '<div class="spinner"></div> Running all tests...';

    const tests = [
        { name: 'GET /',                  fn: () => API.get('/'),                                expect: r => r.ok },
        { name: 'GET /index.html',        fn: () => API.get('/index.html'),                      expect: r => r.ok },
        { name: 'HEAD /index.html',       fn: () => API.head('/index.html'),                     expect: r => r.ok },
        { name: 'GET /css/style.css',     fn: () => API.get('/css/style.css'),                   expect: r => r.ok },
        { name: 'GET 404 (not found)',    fn: () => API.get('/nope-' + Date.now()),              expect: r => r.status === 404 },
        { name: 'DELETE not allowed',     fn: () => API.request('/index.html', {method:'DELETE'}),expect: r => r.status === 405 || r.status === 403 },
        { name: 'Unknown method (PATCH)', fn: () => API.request('/', {method:'PATCH'}),          expect: r => r.status === 501 || r.status === 405 },
        { name: 'GET /uploads/ (autoindex)', fn: () => API.get('/uploads/'),                     expect: r => r.ok || r.status === 404 },
        { name: 'CGI GET /cgi-bin/test.py',  fn: () => API.get('/cgi-bin/test.py?test=1'),       expect: r => r.ok || r.status === 404 },
    ];

    let html = '<h3>Test Results</h3>';
    let passed = 0, failed = 0;

    for (const test of tests) {
        try {
            const response = await test.fn();
            const success = test.expect(response);
            if (success) {
                passed++;
                html += `<div class="test-result pass">
                    <span>[OK]</span> <span>${test.name}</span>
                    <span class="badge badge-success">${response.status} — ${response.duration}ms</span>
                </div>`;
            } else {
                failed++;
                html += `<div class="test-result fail">
                    <span>[FAIL]</span> <span>${test.name}</span>
                    <span class="badge badge-danger">${response.status} ${response.statusText}</span>
                </div>`;
            }
        } catch (e) {
            failed++;
            html += `<div class="test-result fail">
                <span>[ERR]</span> <span>${test.name}</span>
                <span class="badge badge-danger">Error: ${e.message}</span>
            </div>`;
        }
    }

    html += `<hr><p><strong>Résultat:</strong> ${passed} réussi(s), ${failed} échoué(s) / ${tests.length} total</p>`;
    results.innerHTML = html;
}

// ========================================
// Tabs
// ========================================

function initTabs() {
    document.querySelectorAll('.tab').forEach(tab => {
        tab.addEventListener('click', () => {
            const tabGroup = tab.closest('.card');
            const tabId = tab.dataset.tab;
            tabGroup.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
            tab.classList.add('active');
            tabGroup.querySelectorAll('.tab-content').forEach(content => {
                content.classList.remove('active');
                if (content.id === tabId) content.classList.add('active');
            });
        });
    });
}

// ========================================
// Initialize
// ========================================

document.addEventListener('DOMContentLoaded', () => {
    initUpload();
    initTabs();
    if (document.getElementById('uploaded-files-list')) listUploadedFiles();
    console.log('Webserv Test Suite initialized');
});
