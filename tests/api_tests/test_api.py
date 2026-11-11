"""
API Integration Tests for Flask backend connecting to C++17 DSA engine.
"""

import unittest
import json
import os
import sys

# Add project root to sys.path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from backend.app import app

class TestAPI(unittest.TestCase):
    def setUp(self):
        self.client = app.test_client()

    def test_health(self):
        res = self.client.get("/api/health")
        self.assertEqual(res.status_code, 200)
        data = res.get_json()
        self.assertEqual(data["status"], "healthy")
        self.assertTrue(data["engine_compiled"])

    def test_autocomplete(self):
        res = self.client.get("/api/autocomplete?q=pro&limit=5")
        self.assertEqual(res.status_code, 200)
        data = res.get_json()
        self.assertEqual(data["status"], "success")
        self.assertGreater(data["count"], 0)
        self.assertEqual(data["results"][0]["word"], "program")
        self.assertEqual(data["results"][0]["frequency"], 900)

    def test_typo_correction(self):
        res = self.client.get("/api/correct?q=pythn&max_dist=2&limit=5")
        self.assertEqual(res.status_code, 200)
        data = res.get_json()
        self.assertEqual(data["status"], "success")
        self.assertGreater(data["count"], 0)
        self.assertEqual(data["results"][0]["word"], "python")
        self.assertEqual(data["results"][0]["edit_distance"], 1)

    def test_compare(self):
        res = self.client.get("/api/compare?q=pro&limit=5")
        self.assertEqual(res.status_code, 200)
        data = res.get_json()
        self.assertEqual(data["status"], "success")
        self.assertIn("trie_time_us", data)
        self.assertIn("linear_time_us", data)
        self.assertIn("speedup_factor", data)
        self.assertGreater(data["speedup_factor"], 1.0)

    def test_stats(self):
        res = self.client.get("/api/dictionary/stats")
        self.assertEqual(res.status_code, 200)
        data = res.get_json()
        self.assertEqual(data["status"], "success")
        self.assertGreater(data["word_count"], 0)
        self.assertGreater(data["trie_node_count"], 0)

    def test_dp_matrix(self):
        res = self.client.get("/api/dp-matrix?s1=kitten&s2=sitting")
        self.assertEqual(res.status_code, 200)
        data = res.get_json()
        self.assertEqual(data["status"], "success")
        self.assertEqual(data["distance"], 3)
        self.assertEqual(len(data["matrix"]), 7)
        self.assertEqual(len(data["matrix"][0]), 8)

    def test_benchmark_data(self):
        res = self.client.get("/api/benchmark")
        self.assertEqual(res.status_code, 200)
        data = res.get_json()
        self.assertEqual(data["status"], "success")
        self.assertIsInstance(data["results"], list)
        self.assertGreaterEqual(len(data["results"]), 5)

if __name__ == "__main__":
    unittest.main()

