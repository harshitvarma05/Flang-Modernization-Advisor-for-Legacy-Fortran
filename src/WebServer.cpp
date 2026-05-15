#include "Analyzer.hpp"
#include "Reporter.hpp"
#include "Transform.hpp"
#include "WebServer.hpp"

#include <algorithm>
#include <array>
#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <set>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace {
constexpr int BufferSize = 16384;

std::string jsonEscape(const std::string &value) {
  std::string out;
  for (char ch : value) {
    switch (ch) {
    case '"': out += "\\\""; break;
    case '\\': out += "\\\\"; break;
    case '\n': out += "\\n"; break;
    case '\r': out += "\\r"; break;
    case '\t': out += "\\t"; break;
    default:
      if (static_cast<unsigned char>(ch) < 0x20) {
        const char *digits = "0123456789abcdef";
        out += "\\u00";
        out.push_back(digits[(ch >> 4) & 0xf]);
        out.push_back(digits[ch & 0xf]);
      } else {
        out.push_back(ch);
      }
      break;
    }
  }
  return out;
}

std::string html() {
  return R"HTML(<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Flang Modernization Advisor</title>
  <style>
    :root { --bg:#000; --panel:#050505; --panel2:#0a0a0a; --ink:#f5f5f5; --muted:#8a8a8a; --line:#1e1e1e; --accent:#f5f5f5; --risk:#ff453a; --warn:#ffd60a; --safe:#30d158; }
    * { box-sizing:border-box; }
    html, body { min-height:100%; background:#000; }
    body { margin:0; color:var(--ink); font-family:Inter, ui-sans-serif, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif; }
    main { max-width:1040px; margin:0 auto; padding:22px 18px 40px; display:grid; gap:12px; }
    .panel { background:var(--panel); border:1px solid var(--line); border-radius:12px; padding:14px; }
    .output-panel { background:#000; border-color:#222; }
    .section-title { margin:0 0 10px; color:var(--muted); font-size:12px; font-weight:700; text-transform:uppercase; letter-spacing:.08em; }
    pre { margin:0; white-space:pre-wrap; background:#000; color:#f5f5f5; border:1px solid #171717; padding:14px; border-radius:10px; min-height:96px; max-height:260px; overflow:auto; font-size:12px; line-height:1.55; }
    .input-row { display:grid; grid-template-columns:1fr auto auto auto; gap:8px; align-items:center; }
    input { width:100%; height:40px; border:1px solid #252525; border-radius:8px; padding:0 11px; font-size:14px; background:#000; color:var(--ink); outline:none; }
    input:focus { border-color:#555; }
    button { height:40px; border:1px solid #292929; background:#080808; color:var(--ink); border-radius:8px; padding:0 12px; font-weight:650; cursor:pointer; }
    button:hover { background:#111; border-color:#444; }
    button.primary { background:#f5f5f5; border-color:#f5f5f5; color:#000; }
    .actions { display:flex; gap:8px; flex-wrap:wrap; margin-top:10px; }
    .summary { display:grid; grid-template-columns:repeat(4,minmax(120px,1fr)); gap:8px; }
    .metric { border:1px solid var(--line); border-radius:10px; padding:12px; background:#030303; }
    .metric strong { display:block; font-size:22px; line-height:1; margin-bottom:6px; }
    .metric span { color:var(--muted); font-size:12px; }
    .plan-compact { display:flex; justify-content:space-between; align-items:center; gap:12px; }
    .plan-compact strong { font-size:14px; }
    .plan-compact span { color:var(--muted); font-size:13px; }
    table { width:100%; border-collapse:collapse; font-size:13px; }
    th, td { border-bottom:1px solid #1f1f1f; padding:10px 8px; text-align:left; vertical-align:top; }
    th { color:#9a9a9a; font-size:11px; text-transform:uppercase; letter-spacing:.05em; }
    .badge { display:inline-block; border-radius:999px; padding:3px 8px; font-weight:750; font-size:12px; white-space:nowrap; }
    .safe { color:#001b0a; background:var(--safe); }
    .review-needed { color:#1f1600; background:var(--warn); }
    .risky { color:#210000; background:var(--risk); }
    .hidden { display:none !important; }
    .modal { position:fixed; inset:0; z-index:50; background:rgba(0,0,0,.76); display:flex; align-items:center; justify-content:center; padding:20px; }
    .dialog { width:min(1120px, 96vw); max-height:86vh; overflow:auto; background:#030303; border:1px solid #2b2b2b; border-radius:14px; padding:16px; box-shadow:0 24px 80px rgba(0,0,0,.7); }
    .dialog-head { display:flex; justify-content:space-between; align-items:center; gap:12px; margin-bottom:8px; }
    .dialog-head h2 { margin:0; font-size:16px; }
    .close { width:36px; padding:0; }
    @media (max-width:760px) { .input-row { grid-template-columns:1fr; } .summary { grid-template-columns:1fr 1fr; } .plan-compact { display:block; } .plan-compact button { margin-top:10px; width:100%; } }
  </style>
</head>
<body>
<main>
  <section class="panel output-panel">
    <p class="section-title">Output</p>
    <pre id="output">Choose a Fortran file or folder, then click Analyze.</pre>
  </section>

  <section class="panel">
    <div class="input-row">
      <input id="path" value="examples/case_study" spellcheck="false" aria-label="Fortran file or folder path">
      <button onclick="browse('file')">Browse File</button>
      <button onclick="browse('folder')">Browse Folder</button>
      <button class="primary" onclick="analyze()">Analyze</button>
    </div>
    <div class="actions">
      <button onclick="setPath('examples/case_study')">Case Study</button>
      <button onclick="setPath('examples/legacy')">Legacy Fixture</button>
      <button onclick="validateFlang()">Validate With Flang</button>
      <button onclick="dumpTree()">Generate Flang Tree</button>
      <button onclick="transformSafe()">Safe Transform</button>
    </div>
  </section>

  <section id="summary" class="summary hidden">
    <div class="metric"><strong id="files">0</strong><span>Files analyzed</span></div>
    <div class="metric"><strong id="findings">0</strong><span>Findings</span></div>
    <div class="metric"><strong id="risky">0</strong><span>Risky items</span></div>
    <div class="metric"><strong id="safe">0</strong><span>Safe items</span></div>
  </section>

  <section id="planCard" class="panel hidden">
    <div class="plan-compact">
      <div><strong>Modernization plan</strong><br><span id="planSummary">No analysis yet.</span></div>
      <button onclick="openPlan()">Open Plan</button>
    </div>
  </section>
</main>

<div id="planModal" class="modal hidden" onclick="closePlan(event)">
  <div class="dialog" onclick="event.stopPropagation()">
    <div class="dialog-head">
      <h2>Modernization Plan</h2>
      <button class="close" onclick="hidePlan()">×</button>
    </div>
    <table>
      <thead><tr><th>Priority</th><th>Pattern</th><th>Location</th><th>Effort</th><th>Safety</th><th>Recommendation</th></tr></thead>
      <tbody id="rows"></tbody>
    </table>
  </div>
</div>

<script>
const el = id => document.getElementById(id);
function qs() { return encodeURIComponent(el('path').value.trim()); }
function setPath(p) { el('path').value = p; analyze(); }
function badge(text) { return `<span class="badge ${text}">${text}</span>`; }
function showMessage(message) { el('output').textContent = message || 'No output.'; }
function openPlan() { el('planModal').classList.remove('hidden'); }
function hidePlan() { el('planModal').classList.add('hidden'); }
function closePlan(event) { if (event.target.id === 'planModal') hidePlan(); }
document.addEventListener('keydown', e => { if (e.key === 'Escape') hidePlan(); });
async function getJson(url) {
  showMessage('Running...');
  const r = await fetch(url);
  const text = await r.text();
  let data;
  try { data = JSON.parse(text); } catch { throw new Error(text); }
  if (!r.ok || data.ok === false) throw new Error(data.output || data.error || 'Request failed.');
  return data;
}
async function analyze() {
  try {
    const data = await getJson('/api/analyze?path=' + qs());
    const findings = data.findings || [];
    el('summary').classList.remove('hidden');
    el('planCard').classList.remove('hidden');
    el('files').textContent = data.files_analyzed || 0;
    el('findings').textContent = findings.length;
    const risky = findings.filter(f => f.safety === 'risky').length;
    const safe = findings.filter(f => f.safety === 'safe').length;
    el('risky').textContent = risky;
    el('safe').textContent = safe;
    el('planSummary').textContent = `${findings.length} findings, ${risky} risky, ${safe} safe.`;
    el('rows').innerHTML = findings.map(f => `<tr><td>${f.priority}</td><td>${f.pattern}</td><td>${f.location}</td><td>${f.effort}</td><td>${badge(f.safety)}</td><td>${f.recommendation}</td></tr>`).join('');
    showMessage(data.markdown);
  } catch (e) { showMessage(e.message); }
}
async function browse(type) {
  try {
    const data = await getJson('/api/browse?type=' + encodeURIComponent(type));
    if (data.path) el('path').value = data.path;
    showMessage(data.output || 'Selected ' + data.path);
  } catch (e) { showMessage(e.message); }
}
async function validateFlang() {
  try { const data = await getJson('/api/validate?path=' + qs()); showMessage(data.output); }
  catch(e) { showMessage(e.message); }
}
async function dumpTree() {
  try { const data = await getJson('/api/dump?path=' + qs()); showMessage(data.output); }
  catch(e) { showMessage(e.message); }
}
async function transformSafe() {
  try { const data = await getJson('/api/transform?path=' + qs()); showMessage(data.output); }
  catch(e) { showMessage(e.message); }
}
</script>
</body>
</html>)HTML";
}

std::string urlDecode(const std::string &value) {
  std::string out;
  for (size_t i = 0; i < value.size(); ++i) {
    if (value[i] == '%' && i + 2 < value.size()) {
      std::string hex = value.substr(i + 1, 2);
      out.push_back(static_cast<char>(std::strtol(hex.c_str(), nullptr, 16)));
      i += 2;
    } else if (value[i] == '+') {
      out.push_back(' ');
    } else {
      out.push_back(value[i]);
    }
  }
  return out;
}

std::map<std::string, std::string> parseQuery(const std::string &target) {
  std::map<std::string, std::string> query;
  auto pos = target.find('?');
  if (pos == std::string::npos) return query;
  std::stringstream stream(target.substr(pos + 1));
  std::string part;
  while (std::getline(stream, part, '&')) {
    auto eq = part.find('=');
    if (eq != std::string::npos) query[part.substr(0, eq)] = urlDecode(part.substr(eq + 1));
  }
  return query;
}

std::string pathOnly(const std::string &target) {
  auto pos = target.find('?');
  return pos == std::string::npos ? target : target.substr(0, pos);
}

std::string trimText(const std::string &value) {
  auto start = value.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) return "";
  auto end = value.find_last_not_of(" \t\r\n");
  return value.substr(start, end - start + 1);
}

std::filesystem::path resolveInputPath(const std::filesystem::path &input) {
  if (input.is_absolute() || std::filesystem::exists(input)) return input;
  auto cwd = std::filesystem::current_path();
  auto parentCandidate = cwd.parent_path() / input;
  if (std::filesystem::exists(parentCandidate)) return parentCandidate;
  auto grandParentCandidate = cwd.parent_path().parent_path() / input;
  if (std::filesystem::exists(grandParentCandidate)) return grandParentCandidate;
  return input;
}


std::string recommendation(const std::string &pattern) {
  if (pattern == "arithmetic-if") return "Replace with explicit IF/ELSE IF/ELSE branches after checking label fall-through.";
  if (pattern == "computed-goto") return "Replace branch table with SELECT CASE or procedure dispatch.";
  if (pattern == "equivalence") return "Isolate storage overlay and replace only after aliasing/binary-layout review.";
  if (pattern == "common-block") return "Migrate shared state to a module, preserving declaration order and initialization.";
  if (pattern == "implicit-typing") return "Add IMPLICIT NONE and explicit declarations per program unit.";
  if (pattern == "statement-function") return "Convert to an internal or module procedure.";
  if (pattern == "fixed-form") return "Convert to free-form source and normalize continuations/comments.";
  if (pattern == "assumed-size-array") return "Use assumed-shape arrays after introducing explicit interfaces.";
  if (pattern == "entry") return "Split alternate entries into separate procedures with explicit shared state.";
  return "Review construct and modernize locally.";
}

std::string findingsJson(ProjectAnalysis analysis) {
  std::sort(analysis.findings.begin(), analysis.findings.end(), [](const Finding &a, const Finding &b) {
    if (a.priority != b.priority) return a.priority > b.priority;
    if (a.location.file != b.location.file) return a.location.file < b.location.file;
    return a.location.line < b.location.line;
  });
  std::ostringstream out;
  out << "{\"root\":\"" << jsonEscape(analysis.root) << "\",\"files_analyzed\":" << analysis.files.size() << ",\"findings\":[";
  for (size_t i = 0; i < analysis.findings.size(); ++i) {
    const auto &f = analysis.findings[i];
    if (i) out << ",";
    out << "{\"priority\":" << f.priority
        << ",\"pattern\":\"" << jsonEscape(f.pattern)
        << "\",\"location\":\"" << jsonEscape(f.location.file + ":" + std::to_string(f.location.line))
        << "\",\"effort\":\"" << toString(f.effort)
        << "\",\"safety\":\"" << toString(f.safety)
        << "\",\"recommendation\":\"" << jsonEscape(recommendation(f.pattern)) << "\"}";
  }
  out << "],\"markdown\":\"" << jsonEscape(reportMarkdown(analysis)) << "\"}";
  return out.str();
}

bool isFortranFile(const std::filesystem::path &path) {
  std::string ext = path.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char ch) { return std::tolower(ch); });
  return ext == ".f" || ext == ".for" || ext == ".ftn" || ext == ".f77" || ext == ".f90" || ext == ".f95" || ext == ".f03" || ext == ".f08";
}

std::vector<std::filesystem::path> collectFortran(const std::filesystem::path &input) {
  std::vector<std::filesystem::path> files;
  if (std::filesystem::is_regular_file(input) && isFortranFile(input)) files.push_back(input);
  if (std::filesystem::is_directory(input)) {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(input)) {
      if (entry.is_regular_file() && isFortranFile(entry.path())) files.push_back(entry.path());
    }
  }
  std::sort(files.begin(), files.end());
  return files;
}

std::string shellQuote(const std::string &value) {
  std::string out = "'";
  for (char ch : value) out += ch == '\'' ? "'\\''" : std::string(1, ch);
  out += "'";
  return out;
}

std::string runCommand(const std::string &command, int &status) {
  std::array<char, 256> buffer{};
  std::string result;
  FILE *pipe = popen((command + " 2>&1").c_str(), "r");
  if (!pipe) { status = -1; return std::strerror(errno); }
  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) result += buffer.data();
  status = pclose(pipe);
  return result;
}

std::string browseJson(const std::string &type) {
  int status = 0;
  std::string script;
  if (type == "folder") {
    script = "osascript -e 'POSIX path of (choose folder with prompt \"Choose a Fortran source folder\")'";
  } else {
    script = "osascript -e 'POSIX path of (choose file with prompt \"Choose a Fortran source file\")'";
  }
  std::string result = trimText(runCommand(script, status));
  if (status != 0 || result.empty()) {
    return "{\"ok\":false,\"output\":\"Selection cancelled or unavailable.\"}";
  }
  return "{\"ok\":true,\"path\":\"" + jsonEscape(result) + "\",\"output\":\"Selected " + jsonEscape(result) + "\"}";
}

std::string validateJson(const std::filesystem::path &input) {
  auto files = collectFortran(resolveInputPath(input));
  if (files.empty()) return "{\"ok\":false,\"output\":\"No Fortran files found.\"}";
  std::ostringstream output;
  bool ok = true;
  for (const auto &file : files) {
    int status = 0;
    std::string cmd = "flang -fsyntax-only " + shellQuote(file.string());
    std::string result = runCommand(cmd, status);
    output << "$ " << cmd << "\n" << (result.empty() ? "OK\n" : result) << "\n";
    if (status != 0) ok = false;
  }
  return std::string("{\"ok\":") + (ok ? "true" : "false") + ",\"output\":\"" + jsonEscape(output.str()) + "\"}";
}

std::string safeName(const std::filesystem::path &path) {
  std::string name = path.filename().string();
  for (char &ch : name) if (!std::isalnum(static_cast<unsigned char>(ch))) ch = '_';
  return name;
}

std::string dumpJson(const std::filesystem::path &input) {
  auto files = collectFortran(resolveInputPath(input));
  if (files.empty()) return "{\"ok\":false,\"output\":\"No Fortran files found.\"}";
  std::filesystem::create_directories("docs/flang_dumps/web");
  std::ostringstream output;
  bool ok = true;
  for (const auto &file : files) {
    auto outPath = std::filesystem::path("docs/flang_dumps/web") / (safeName(file) + "_parse_tree.txt");
    int status = 0;
    std::string cmd = "flang -fc1 -fdebug-dump-parse-tree " + shellQuote(file.string()) + " > " + shellQuote(outPath.string());
    std::string result = runCommand(cmd, status);
    output << "$ " << cmd << "\n" << (status == 0 ? "Wrote " + outPath.string() + "\n" : result) << "\n";
    if (status != 0) ok = false;
  }
  return std::string("{\"ok\":") + (ok ? "true" : "false") + ",\"output\":\"" + jsonEscape(output.str()) + "\"}";
}

std::string transformJson(const std::filesystem::path &input) {
  auto files = applySafeTransformations(resolveInputPath(input), "transformed/web");
  std::ostringstream output;
  output << "Safe transformed files written: " << files.size() << "\n";
  for (const auto &file : files) output << "- " << file.string() << "\n";
  return "{\"ok\":true,\"output\":\"" + jsonEscape(output.str()) + "\"}";
}

std::string response(const std::string &body, const std::string &type = "application/json", int status = 200) {
  std::ostringstream out;
  out << "HTTP/1.1 " << status << " OK\r\nContent-Type: " << type << "; charset=utf-8\r\nContent-Length: " << body.size() << "\r\nConnection: close\r\n\r\n" << body;
  return out.str();
}

std::string handle(const std::string &request) {
  std::istringstream input(request);
  std::string method, target, version;
  input >> method >> target >> version;
  auto route = pathOnly(target);
  auto query = parseQuery(target);
  std::filesystem::path path = query.count("path") ? query["path"] : "examples/case_study";

  try {
    if (route == "/" || route == "/index.html") return response(html(), "text/html");
    if (route == "/api/browse") return response(browseJson(query.count("type") ? query["type"] : "file"));
    if (route == "/api/analyze") {
      ModernizationAnalyzer analyzer;
      return response(findingsJson(analyzer.analyzePath(resolveInputPath(path))));
    }
    if (route == "/api/validate") return response(validateJson(path));
    if (route == "/api/dump") return response(dumpJson(path));
    if (route == "/api/transform") return response(transformJson(path));
  } catch (const std::exception &ex) {
    return response("{\"ok\":false,\"output\":\"" + jsonEscape(ex.what()) + "\"}", "application/json", 500);
  }
  return response("Not found", "text/plain", 404);
}
} // namespace

int runWebServer(int port) {
  int serverFd = socket(AF_INET, SOCK_STREAM, 0);
  if (serverFd < 0) {
    std::cerr << "Could not create socket\n";
    return 1;
  }
  int opt = 1;
  setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(static_cast<uint16_t>(port));

  if (bind(serverFd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0) {
    std::cerr << "Could not bind to localhost:" << port << "\n";
    close(serverFd);
    return 1;
  }
  if (listen(serverFd, 16) < 0) {
    std::cerr << "Could not listen on port " << port << "\n";
    close(serverFd);
    return 1;
  }

  std::cout << "Flang Modernization Advisor UI running at http://localhost:" << port << "\n";
  std::cout << "Press Ctrl+C to stop.\n" << std::flush;
  while (true) {
    int client = accept(serverFd, nullptr, nullptr);
    if (client < 0) continue;
    std::array<char, BufferSize> buffer{};
    ssize_t count = recv(client, buffer.data(), buffer.size() - 1, 0);
    if (count > 0) {
      std::string reply = handle(std::string(buffer.data(), static_cast<size_t>(count)));
      send(client, reply.data(), reply.size(), 0);
    }
    close(client);
  }
  close(serverFd);
  return 0;
}
