"""
EngineClient: Manages secure communication between Python Flask and the compiled C++17 DSA Engine.
Executes C++ algorithms via Python subprocess with explicit argument arrays (shell=False) to guarantee
command injection prevention and parses structured JSON responses from the engine.
"""

import os
import subprocess
import json
import logging
import re
from typing import Dict, Any, Optional

logger = logging.getLogger(__name__)

class EngineClient:
    def __init__(self, binary_path: str = "bin/engine", dict_path: str = "data/dictionary.txt"):
        self.binary_path = os.path.abspath(binary_path)
        self.dict_path = os.path.abspath(dict_path)
        self._ensure_binary_exists()

    def _ensure_binary_exists(self):
        if not os.path.exists(self.binary_path):
            logger.info("Engine binary not found. Compiling via make...")
            res = subprocess.run(["make"], capture_output=True, text=True)
            if res.returncode != 0:
                raise RuntimeError(f"Failed to build C++ engine: {res.stderr}")

    def _sanitize(self, text: str) -> str:
        if not text:
            return ""
        # Strip characters that are not lowercase/uppercase letters, digits, or spaces
        clean = re.sub(r'[^a-zA-Z0-9]', '', text.strip().lower())
        return clean[:64] # Limit max length for safety

    def query_autocomplete(self, prefix: str, limit: int = 5) -> Dict[str, Any]:
        clean_prefix = self._sanitize(prefix)
        if not clean_prefix:
            return {
                "status": "success",
                "query": "",
                "count": 0,
                "execution_time_us": 0.0,
                "algorithm": "trie_prefix_search",
                "results": []
            }
        
        args = [
            self.binary_path,
            "--dict", self.dict_path,
            "--autocomplete", clean_prefix,
            "--limit", str(limit)
        ]
        return self._run_command(args)

    def query_correct(self, query: str, max_dist: int = 2, limit: int = 5) -> Dict[str, Any]:
        clean_query = self._sanitize(query)
        if not clean_query:
            return {
                "status": "success",
                "query": "",
                "count": 0,
                "execution_time_us": 0.0,
                "algorithm": "trie_dp_traversal",
                "results": []
            }

        args = [
            self.binary_path,
            "--dict", self.dict_path,
            "--correct", clean_query,
            "--max-dist", str(max_dist),
            "--limit", str(limit)
        ]
        return self._run_command(args)

    def query_search(self, query: str, limit: int = 5) -> Dict[str, Any]:
        clean_query = self._sanitize(query)
        if not clean_query:
            return {
                "status": "success",
                "query": "",
                "exact_match": False,
                "autocomplete_time_us": 0.0,
                "typo_time_us": 0.0,
                "autocomplete_results": [],
                "typo_corrections": []
            }

        args = [
            self.binary_path,
            "--dict", self.dict_path,
            "--search", clean_query,
            "--limit", str(limit)
        ]
        return self._run_command(args)

    def query_compare(self, prefix: str, limit: int = 5) -> Dict[str, Any]:
        clean_prefix = self._sanitize(prefix)
        args = [
            self.binary_path,
            "--dict", self.dict_path,
            "--compare", clean_prefix,
            "--limit", str(limit)
        ]
        return self._run_command(args)

    def query_stats(self) -> Dict[str, Any]:
        args = [
            self.binary_path,
            "--dict", self.dict_path,
            "--stats"
        ]
        return self._run_command(args)

    def query_dp_matrix(self, s1: str, s2: str) -> Dict[str, Any]:
        clean_s1 = self._sanitize(s1)
        clean_s2 = self._sanitize(s2)
        args = [
            self.binary_path,
            "--dict", self.dict_path,
            "--dp-matrix", clean_s1, clean_s2
        ]
        return self._run_command(args)

    def run_benchmark(self) -> Dict[str, Any]:
        args = [
            self.binary_path,
            "--dict", self.dict_path,
            "--benchmark"
        ]
        result = self._run_command(args)
        # Also ensure graphs are regenerated
        try:
            import sys
            graphs_script = os.path.abspath("benchmarks/generate_graphs.py")
            env = os.environ.copy()
            env["MPLCONFIGDIR"] = "/tmp/matplotlib"
            subprocess.run([sys.executable, graphs_script], env=env, check=True)
            result["graphs_generated"] = True
        except Exception as e:
            logger.warning("Graph generation warning: %s", e)
            result["graphs_generated"] = False
        return result

    def _run_command(self, args: list) -> Dict[str, Any]:
        try:
            proc = subprocess.run(
                args,
                capture_output=True,
                text=True,
                timeout=30.0,
                check=False
            )
            if proc.returncode != 0:
                logger.error("C++ engine error: %s", proc.stderr)
                return {"status": "error", "message": proc.stderr.strip() or "Engine error"}
            
            output = proc.stdout.strip()
            # Extract JSON substring if output has extra text
            start_idx = output.find('{')
            end_idx = output.rfind('}')
            if start_idx != -1 and end_idx != -1:
                json_str = output[start_idx:end_idx + 1]
                return json.loads(json_str)
            return {"status": "error", "message": "Invalid response format from C++ engine"}

        except subprocess.TimeoutExpired:
            return {"status": "error", "message": "C++ engine query timed out"}
        except Exception as e:
            logger.exception("Engine invocation exception")
            return {"status": "error", "message": str(e)}

