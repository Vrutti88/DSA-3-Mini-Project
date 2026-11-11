/**
 * Smart Autocomplete & Typo Correction - Master Frontend Controller
 * Connects UI seamlessly with Flask API & C++17 DSA Engine.
 */

document.addEventListener("DOMContentLoaded", () => {
    initNavigation();
    initSmartSearch();
    initTypoCorrection();
    initAlgorithmExplorer();
    initPerformanceDashboard();
    checkHealth();
});

// ---------------- 1. API HELPER ---------------- //
const API = {
    async get(endpoint) {
        try {
            const res = await fetch(endpoint);
            if (!res.ok) throw new Error(`HTTP error! status: ${res.status}`);
            return await res.json();
        } catch (err) {
            console.error(`API Error on ${endpoint}:`, err);
            return { status: "error", message: err.message };
        }
    },
    async post(endpoint, data = {}) {
        try {
            const res = await fetch(endpoint, {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(data)
            });
            return await res.json();
        } catch (err) {
            console.error(`API Error on POST ${endpoint}:`, err);
            return { status: "error", message: err.message };
        }
    }
};

// ---------------- 2. NAVIGATION CONTROLLER ---------------- //
function initNavigation() {
    const navButtons = document.querySelectorAll(".nav-btn");
    const tabPanes = document.querySelectorAll(".tab-pane");

    function activateTab(targetTab) {
        navButtons.forEach(b => {
            if (b.getAttribute("data-tab") === targetTab) {
                b.classList.add("active");
            } else {
                b.classList.remove("active");
            }
        });
        tabPanes.forEach(p => {
            if (p.id === targetTab) {
                p.classList.add("active");
            } else {
                p.classList.remove("active");
            }
        });

        if (targetTab === "tab-explorer") {
            drawTrieCanvas();
        } else if (targetTab === "tab-dashboard") {
            loadDashboardStats();
            loadBenchmarkTable();
        }
    }

    navButtons.forEach(btn => {
        btn.addEventListener("click", () => {
            const targetTab = btn.getAttribute("data-tab");
            window.location.hash = targetTab;
            activateTab(targetTab);
        });
    });

    // Check window location hash on page load
    const hash = window.location.hash.replace("#", "");
    if (hash && document.getElementById(hash)) {
        activateTab(hash);
    }
}

async function checkHealth() {
    const data = await API.get("/api/health");
    const label = document.getElementById("engineStatusLabel");
    if (data.status === "healthy" && label) {
        label.textContent = "C++ Engine: Active (C++17)";
    }
}

// ---------------- 3. PAGE 1: SMART SEARCH ---------------- //
function initSmartSearch() {
    const searchInput = document.getElementById("searchInput");
    const clearBtn = document.getElementById("clearBtn");
    const searchSpinner = document.getElementById("searchSpinner");
    const dropdown = document.getElementById("suggestionsDropdown");
    const suggestionsList = document.getElementById("suggestionsList");
    const searchMetaTime = document.getElementById("searchMetaTime");
    const queryStatsPill = document.getElementById("queryStatsPill");
    const pillCount = document.getElementById("pillCount");
    const pillTime = document.getElementById("pillTime");
    const didYouMeanBanner = document.getElementById("didYouMeanBanner");
    const dymOriginal = document.getElementById("dymOriginal");
    const dymCandidates = document.getElementById("dymCandidates");
    const presetTags = document.querySelectorAll(".preset-tag");

    let debounceTimer = null;
    let selectedIndex = -1;
    let currentSuggestions = [];

    // Keystroke input listener with debouncing (150ms)
    searchInput.addEventListener("input", () => {
        const query = searchInput.value.trim();
        clearBtn.style.display = searchInput.value ? "flex" : "none";

        clearTimeout(debounceTimer);
        if (!query) {
            hideDropdown();
            queryStatsPill.style.display = "none";
            didYouMeanBanner.style.display = "none";
            return;
        }

        searchSpinner.style.display = "block";
        debounceTimer = setTimeout(() => {
            performSearch(query);
        }, 120);
    });

    clearBtn.addEventListener("click", () => {
        searchInput.value = "";
        clearBtn.style.display = "none";
        hideDropdown();
        queryStatsPill.style.display = "none";
        didYouMeanBanner.style.display = "none";
        searchInput.focus();
    });

    // Keyboard navigation: Arrow Up, Down, Enter, Escape
    searchInput.addEventListener("keydown", (e) => {
        const items = suggestionsList.querySelectorAll(".suggestion-item");
        if (items.length === 0) return;

        if (e.key === "ArrowDown") {
            e.preventDefault();
            selectedIndex = (selectedIndex + 1) % items.length;
            updateSelection(items);
        } else if (e.key === "ArrowUp") {
            e.preventDefault();
            selectedIndex = (selectedIndex - 1 + items.length) % items.length;
            updateSelection(items);
        } else if (e.key === "Enter") {
            e.preventDefault();
            if (selectedIndex >= 0 && selectedIndex < currentSuggestions.length) {
                selectWord(currentSuggestions[selectedIndex].word);
            }
        } else if (e.key === "Escape") {
            hideDropdown();
        }
    });

    // Preset tag buttons
    presetTags.forEach(btn => {
        btn.addEventListener("click", () => {
            const q = btn.getAttribute("data-query");
            searchInput.value = q;
            clearBtn.style.display = "flex";
            performSearch(q);
        });
    });

    async function performSearch(query) {
        const data = await API.get(`/api/autocomplete?q=${encodeURIComponent(query)}&limit=6`);
        searchSpinner.style.display = "none";

        if (data.status !== "success") return;

        currentSuggestions = data.results || [];
        searchMetaTime.textContent = `${data.execution_time_us} μs`;

        // Update stats pill
        pillCount.textContent = currentSuggestions.length;
        pillTime.textContent = `${data.execution_time_us} μs`;
        queryStatsPill.style.display = "inline-flex";

        if (currentSuggestions.length > 0) {
            renderSuggestions(currentSuggestions, query);
            didYouMeanBanner.style.display = "none";
        } else {
            hideDropdown();
            // Automatically trigger typo correction fallback if 0 prefix matches
            checkDidYouMean(query);
        }
    }

    function renderSuggestions(suggestions, query) {
        suggestionsList.innerHTML = "";
        selectedIndex = -1;

        suggestions.forEach((item, idx) => {
            const li = document.createElement("li");
            li.className = "suggestion-item";
            li.setAttribute("data-index", idx);

            const word = item.word;
            let highlighted = word;
            const lowerWord = word.toLowerCase();
            const lowerQuery = query.toLowerCase();

            if (lowerWord.startsWith(lowerQuery)) {
                const prefixPart = word.substring(0, query.length);
                const remainder = word.substring(query.length);
                highlighted = `<span class="match-prefix">${escapeHtml(prefixPart)}</span><span class="word-remainder">${escapeHtml(remainder)}</span>`;
            }

            li.innerHTML = `
                <div class="suggestion-word">
                    <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="#94a3b8" stroke-width="2"><circle cx="11" cy="11" r="8"></circle><line x1="21" y1="21" x2="16.65" y2="16.65"></line></svg>
                    <span>${highlighted}</span>
                </div>
                <div class="suggestion-freq" title="Simulated Corpus Frequency">
                    <span>${item.frequency}</span>
                    <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="23 6 13.5 15.5 8.5 10.5 1 18"></polyline><polyline points="17 6 23 6 23 12"></polyline></svg>
                </div>
            `;

            li.addEventListener("click", () => {
                selectWord(word);
            });

            suggestionsList.appendChild(li);
        });

        dropdown.style.display = "block";
    }

    function updateSelection(items) {
        items.forEach((item, idx) => {
            if (idx === selectedIndex) {
                item.classList.add("selected");
                searchInput.value = currentSuggestions[idx].word;
            } else {
                item.classList.remove("selected");
            }
        });
    }

    function selectWord(word) {
        searchInput.value = word;
        hideDropdown();
        // Switch or trigger typo/compare if desired
    }

    function hideDropdown() {
        dropdown.style.display = "none";
        suggestionsList.innerHTML = "";
        selectedIndex = -1;
    }

    async function checkDidYouMean(query) {
        const typoData = await API.get(`/api/correct?q=${encodeURIComponent(query)}&max_dist=2&limit=4`);
        if (typoData.status === "success" && typoData.results && typoData.results.length > 0) {
            dymOriginal.textContent = query;
            dymCandidates.innerHTML = "";
            typoData.results.forEach(cand => {
                const pill = document.createElement("button");
                pill.className = "dym-pill";
                pill.textContent = `${cand.word} (dist: ${cand.edit_distance})`;
                pill.addEventListener("click", () => {
                    searchInput.value = cand.word;
                    performSearch(cand.word);
                });
                dymCandidates.appendChild(pill);
            });
            didYouMeanBanner.style.display = "flex";
        } else {
            didYouMeanBanner.style.display = "none";
        }
    }

    // Close dropdown on outside click
    document.addEventListener("click", (e) => {
        if (!e.target.closest(".search-box-wrapper")) {
            hideDropdown();
        }
    });
}

// ---------------- 4. PAGE 2: TYPO CORRECTION ---------------- //
function initTypoCorrection() {
    const typoInput = document.getElementById("typoInput");
    const maxDistSelect = document.getElementById("maxDistanceSelect");
    const typoLimitSelect = document.getElementById("typoLimitSelect");
    const btnCheckTypo = document.getElementById("btnCheckTypo");
    const typoExecutionTime = document.getElementById("typoExecutionTime");
    const typoTableWrapper = document.getElementById("typoTableWrapper");
    const typoCandidatesBody = document.getElementById("typoCandidatesBody");
    const typoEmptyState = document.getElementById("typoEmptyState");
    const presetButtons = document.querySelectorAll(".preset-btn");

    btnCheckTypo.addEventListener("click", () => {
        runTypoCheck();
    });

    typoInput.addEventListener("keydown", (e) => {
        if (e.key === "Enter") runTypoCheck();
    });

    presetButtons.forEach(btn => {
        btn.addEventListener("click", () => {
            const word = btn.getAttribute("data-word");
            typoInput.value = word;
            runTypoCheck();
        });
    });

    async function runTypoCheck() {
        const query = typoInput.value.trim();
        if (!query) return;

        const maxDist = maxDistSelect.value;
        const limit = typoLimitSelect.value;

        const data = await API.get(`/api/correct?q=${encodeURIComponent(query)}&max_dist=${maxDist}&limit=${limit}`);

        if (data.status !== "success") return;

        typoExecutionTime.textContent = `${data.execution_time_us} μs via Trie DP`;
        typoCandidatesBody.innerHTML = "";

        const results = data.results || [];
        if (results.length === 0) {
            typoTableWrapper.style.display = "none";
            typoEmptyState.style.display = "block";
            return;
        }

        typoTableWrapper.style.display = "block";
        typoEmptyState.style.display = "none";

        results.forEach((cand, idx) => {
            const tr = document.createElement("tr");
            tr.innerHTML = `
                <td><strong>#${idx + 1}</strong></td>
                <td><span style="font-size: 1.05rem; font-weight: 600; color: #2563eb;">${escapeHtml(cand.word)}</span></td>
                <td><span class="badge ${cand.edit_distance === 1 ? 'badge-accent' : 'badge-outline'}">Distance ${cand.edit_distance}</span></td>
                <td><span style="font-family: var(--font-mono);">${cand.frequency}</span></td>
                <td>
                    <button class="btn btn-outline-sm btn-adopt" data-word="${escapeHtml(cand.word)}">Adopt Word</button>
                </td>
            `;

            tr.querySelector(".btn-adopt").addEventListener("click", () => {
                const searchInput = document.getElementById("searchInput");
                const searchTabBtn = document.querySelector('[data-tab="tab-search"]');
                if (searchInput && searchTabBtn) {
                    searchInput.value = cand.word;
                    searchTabBtn.click();
                    searchInput.dispatchEvent(new Event("input"));
                }
            });

            typoCandidatesBody.appendChild(tr);
        });
    }

    // Initial run
    runTypoCheck();
}

// ---------------- 5. PAGE 3: ALGORITHM EXPLORER ---------------- //
function initAlgorithmExplorer() {
    const word1Input = document.getElementById("dpWord1");
    const word2Input = document.getElementById("dpWord2");
    const btnCompute = document.getElementById("btnComputeMatrix");

    btnCompute.addEventListener("click", () => {
        loadDPMatrix();
    });

    [word1Input, word2Input].forEach(inp => {
        inp.addEventListener("keydown", (e) => {
            if (e.key === "Enter") loadDPMatrix();
        });
    });

    loadDPMatrix();
}

async function loadDPMatrix() {
    const s1 = document.getElementById("dpWord1").value.trim() || "kitten";
    const s2 = document.getElementById("dpWord2").value.trim() || "sitting";

    const data = await API.get(`/api/dp-matrix?s1=${encodeURIComponent(s1)}&s2=${encodeURIComponent(s2)}`);
    if (data.status !== "success") return;

    const matrix = data.matrix;
    const distanceVal = document.getElementById("matrixDistanceVal");
    const container = document.getElementById("matrixTableContainer");

    if (distanceVal) distanceVal.textContent = data.distance;

    let html = '<table class="dp-table"><thead><tr><th>DP</th><th>ε</th>';
    for (let j = 0; j < s2.length; j++) {
        html += `<th>${s2[j]}</th>`;
    }
    html += '</tr></thead><tbody>';

    for (let i = 0; i <= s1.length; i++) {
        const rowHeader = (i === 0) ? 'ε' : s1[i - 1];
        html += `<tr><th>${rowHeader}</th>`;
        for (let j = 0; j <= s2.length; j++) {
            const val = matrix[i][j];
            let cellClass = "";
            if (i > 0 && j > 0 && s1[i - 1].toLowerCase() === s2[j - 1].toLowerCase()) {
                cellClass = "dp-cell-match";
            }
            html += `<td class="${cellClass}">${val}</td>`;
        }
        html += '</tr>';
    }
    html += '</tbody></table>';

    container.innerHTML = html;
}

function drawTrieCanvas() {
    const canvas = document.getElementById("trieCanvas");
    if (!canvas) return;
    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Nodes definition for demonstration: root -> p -> r -> o -> [gram, gress]
    const nodes = [
        { id: 0, label: "Root", x: 300, y: 30, isWord: false, r: 18 },
        { id: 1, label: "p", x: 300, y: 80, isWord: false, r: 16 },
        { id: 2, label: "r", x: 300, y: 130, isWord: false, r: 16 },
        { id: 3, label: "o", x: 300, y: 180, isWord: true, r: 16, freq: 400 },
        { id: 4, label: "g", x: 200, y: 220, isWord: false, r: 15 },
        { id: 5, label: "y", x: 420, y: 130, isWord: false, r: 15 } // for python
    ];

    const edges = [
        { from: 0, to: 1, label: "'p'" },
        { from: 1, to: 2, label: "'r'" },
        { from: 2, to: 3, label: "'o'" },
        { from: 3, to: 4, label: "'g'" },
        { from: 1, to: 5, label: "'y'" }
    ];

    // Draw Edges
    edges.forEach(e => {
        const from = nodes[e.from];
        const to = nodes[e.to];
        ctx.beginPath();
        ctx.moveTo(from.x, from.y);
        ctx.lineTo(to.x, to.y);
        ctx.strokeStyle = "#cbd5e1";
        ctx.lineWidth = 2;
        ctx.stroke();

        // Edge label
        const mx = (from.x + to.x) / 2;
        const my = (from.y + to.y) / 2;
        ctx.font = "11px Inter";
        ctx.fillStyle = "#64748b";
        ctx.fillText(e.label, mx + 6, my);
    });

    // Draw Nodes
    nodes.forEach(n => {
        ctx.beginPath();
        ctx.arc(n.x, n.y, n.r, 0, 2 * Math.PI);
        ctx.fillStyle = n.isWord ? "#dcfce7" : "#eff6ff";
        ctx.fill();
        ctx.lineWidth = 2;
        ctx.strokeStyle = n.isWord ? "#16a34a" : "#2563eb";
        ctx.stroke();

        ctx.font = "bold 12px Inter";
        ctx.fillStyle = n.isWord ? "#15803d" : "#1d4ed8";
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        ctx.fillText(n.label, n.x, n.y);
    });
}

// ---------------- 6. PAGE 4: PERFORMANCE DASHBOARD ---------------- //
function initPerformanceDashboard() {
    const btnRunCompare = document.getElementById("btnRunComparison");
    const compareInput = document.getElementById("compareQueryInput");
    const btnReRunBenchmark = document.getElementById("btnReRunBenchmark");

    btnRunCompare.addEventListener("click", () => {
        runHeadToHeadComparison();
    });

    compareInput.addEventListener("keydown", (e) => {
        if (e.key === "Enter") runHeadToHeadComparison();
    });

    btnReRunBenchmark.addEventListener("click", async () => {
        btnReRunBenchmark.disabled = true;
        btnReRunBenchmark.textContent = "Running Benchmark Suite...";
        const data = await API.post("/api/benchmark");
        btnReRunBenchmark.disabled = false;
        btnReRunBenchmark.textContent = "Re-Run Benchmark Suite";
        if (data.status === "success") {
            loadBenchmarkTable();
        }
    });

    runHeadToHeadComparison();
}

async function loadDashboardStats() {
    const data = await API.get("/api/dictionary/stats");
    if (data.status !== "success") return;

    document.getElementById("statWordCount").textContent = Number(data.word_count).toLocaleString();
    document.getElementById("statNodeCount").textContent = Number(data.trie_node_count).toLocaleString();
    document.getElementById("statMemory").textContent = `${data.trie_memory_mb} MB`;
}

async function runHeadToHeadComparison() {
    const query = document.getElementById("compareQueryInput").value.trim() || "pro";
    const data = await API.get(`/api/compare?q=${encodeURIComponent(query)}&limit=5`);

    if (data.status !== "success") return;

    document.getElementById("raceTrieTime").textContent = `${data.trie_time_us} μs`;
    document.getElementById("raceLinearTime").textContent = `${data.linear_time_us} μs`;
    document.getElementById("raceSpeedupFactor").textContent = `${data.speedup_factor}x Faster`;

    const trieList = document.getElementById("raceTrieList");
    const linearList = document.getElementById("raceLinearList");
    trieList.innerHTML = "";
    linearList.innerHTML = "";

    (data.trie_results || []).forEach(r => {
        const li = document.createElement("li");
        li.innerHTML = `<span>${r.word}</span> <span style="font-family: var(--font-mono);">${r.frequency}</span>`;
        trieList.appendChild(li);
    });

    (data.linear_results || []).forEach(r => {
        const li = document.createElement("li");
        li.innerHTML = `<span>${r.word}</span> <span style="font-family: var(--font-mono);">${r.frequency}</span>`;
        linearList.appendChild(li);
    });
}

async function loadBenchmarkTable() {
    const data = await API.get("/api/benchmark");
    if (data.status !== "success" || !data.results) return;

    const tbody = document.getElementById("benchmarkBody");
    tbody.innerHTML = "";

    data.results.forEach(row => {
        const tr = document.createElement("tr");
        tr.innerHTML = `
            <td><strong>${Number(row.dataset_size).toLocaleString()} words</strong></td>
            <td>${Number(row.trie_build_time_ms).toFixed(2)} ms</td>
            <td>${Number(row.trie_node_count).toLocaleString()}</td>
            <td>${(Number(row.trie_memory_kb) / 1024).toFixed(2)} MB</td>
            <td><strong style="color: #2563eb;">${Number(row.trie_search_avg_us).toFixed(2)} μs</strong></td>
            <td><span style="color: #e53e3e;">${Number(row.linear_search_avg_us).toFixed(2)} μs</span></td>
            <td><span class="badge badge-accent">${Number(row.search_speedup).toFixed(1)}x</span></td>
            <td><strong style="color: #16a34a;">${Number(row.trie_typo_avg_us).toFixed(2)} μs</strong></td>
            <td><span style="color: #e53e3e;">${Number(row.linear_typo_avg_us).toFixed(2)} μs</span></td>
            <td><span class="badge badge-primary">${Number(row.typo_speedup).toFixed(1)}x</span></td>
        `;
        tbody.appendChild(tr);
    });
}

function escapeHtml(text) {
    const map = {
        '&': '&amp;',
        '<': '&lt;',
        '>': '&gt;',
        '"': '&quot;',
        "'": '&#039;'
    };
    return text.replace(/[&<>"']/g, m => map[m]);
}
