"""
Flask Web Application & REST API Gateway for Smart Autocomplete & Typo Correction.
Integrates with C++17 Core Engine and serves modern frontend.
"""

import os
import sys
import json

# Add root directory to sys.path so backend modules import cleanly
BASE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
from backend.engine_client import EngineClient

# Determine directory paths
BASE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
FRONTEND_DIR = os.path.join(BASE_DIR, "frontend")
BENCHMARKS_DIR = os.path.join(BASE_DIR, "benchmarks")

app = Flask(__name__, static_folder=FRONTEND_DIR)
CORS(app)

client = EngineClient(
    binary_path=os.path.join(BASE_DIR, "bin", "engine"),
    dict_path=os.path.join(BASE_DIR, "data", "dictionary.txt")
)

# ----------------- STATIC FRONTEND ROUTES ----------------- #

@app.route("/")
def serve_index():
    return send_from_directory(FRONTEND_DIR, "index.html")

@app.route("/<path:path>")
def serve_static(path):
    if os.path.exists(os.path.join(FRONTEND_DIR, path)):
        return send_from_directory(FRONTEND_DIR, path)
    if path.startswith("benchmarks/"):
        rel_path = path[len("benchmarks/"):]
        return send_from_directory(BENCHMARKS_DIR, rel_path)
    return send_from_directory(FRONTEND_DIR, "index.html")

# ----------------- REST API ENDPOINTS ----------------- #

@app.route("/api/health", methods=["GET"])
def health_check():
    binary_exists = os.path.exists(client.binary_path)
    dict_exists = os.path.exists(client.dict_path)
    return jsonify({
        "status": "healthy" if (binary_exists and dict_exists) else "degraded",
        "engine_compiled": binary_exists,
        "dictionary_present": dict_exists,
        "language": "C++17 (Core Algorithms)",
        "backend": "Python Flask"
    })

@app.route("/api/autocomplete", methods=["GET"])
def autocomplete():
    q = request.args.get("q", "").strip()
    limit = request.args.get("limit", default=5, type=int)
    res = client.query_autocomplete(q, limit=limit)
    return jsonify(res)

@app.route("/api/correct", methods=["GET"])
def correct():
    q = request.args.get("q", "").strip()
    max_dist = request.args.get("max_dist", default=2, type=int)
    limit = request.args.get("limit", default=5, type=int)
    res = client.query_correct(q, max_dist=max_dist, limit=limit)
    return jsonify(res)

@app.route("/api/search", methods=["GET"])
def search():
    q = request.args.get("q", "").strip()
    limit = request.args.get("limit", default=5, type=int)
    res = client.query_search(q, limit=limit)
    return jsonify(res)

@app.route("/api/compare", methods=["GET"])
def compare():
    q = request.args.get("q", "").strip()
    limit = request.args.get("limit", default=5, type=int)
    res = client.query_compare(q, limit=limit)
    return jsonify(res)

@app.route("/api/dictionary/stats", methods=["GET"])
def dictionary_stats():
    res = client.query_stats()
    return jsonify(res)

@app.route("/api/dp-matrix", methods=["GET"])
def dp_matrix():
    s1 = request.args.get("s1", "kitten").strip()
    s2 = request.args.get("s2", "sitting").strip()
    res = client.query_dp_matrix(s1, s2)
    return jsonify(res)

@app.route("/api/benchmark", methods=["GET", "POST"])
def benchmark():
    json_path = os.path.join(BENCHMARKS_DIR, "results", "benchmark_results.json")
    run_now = request.args.get("run", "false").lower() == "true" or request.method == "POST"

    if run_now or not os.path.exists(json_path):
        res = client.run_benchmark()
        if os.path.exists(json_path):
            with open(json_path, "r", encoding="utf-8") as f:
                data = json.load(f)
            return jsonify({"status": "success", "results": data, "refreshed": True})
        return jsonify(res)

    with open(json_path, "r", encoding="utf-8") as f:
        data = json.load(f)
    return jsonify({"status": "success", "results": data, "refreshed": False})

if __name__ == "__main__":
    app.run(host="127.0.0.1", port=5001, debug=False)
