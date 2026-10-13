#!/usr/bin/env python3
"""
Gourmet Recipe Book Manager - Local Web & API Bridge Server
Zero-dependency lightweight Python 3 server bridging the Web Frontend
and the C/C++ backend storage files (recipes_data.txt and recipes_cpp.txt).
"""

import http.server
import socketserver
import os
import json
import urllib.parse

PORT = 5050
DATA_FILE_C = "recipes_data.txt"
DATA_FILE_CPP = "recipes_cpp.txt"
FRONTEND_DIR = os.path.join(os.path.dirname(__file__), "frontend")


def parse_c_recipes_file(filepath):
    if not os.path.exists(filepath):
        return []

    recipes = []
    current = None

    with open(filepath, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            if line == "RECIPE_START":
                current = {"ingredients": [], "steps": []}
            elif line == "RECIPE_END":
                if current:
                    recipes.append(current)
                    current = None
            elif current is not None:
                if line.startswith("id:"):
                    current["id"] = int(line[3:])
                elif line.startswith("title:"):
                    current["title"] = line[6:]
                elif line.startswith("category:"):
                    current["category"] = line[9:]
                elif line.startswith("prep_time:"):
                    current["prep_time_min"] = int(line[10:])
                elif line.startswith("cook_time:"):
                    current["cook_time_min"] = int(line[10:])
                elif line.startswith("servings:"):
                    current["servings"] = int(line[9:])
                elif line.startswith("difficulty:"):
                    current["difficulty"] = line[11:]
                elif line.startswith("ing:"):
                    parts = line[4:].split("|")
                    if len(parts) >= 3:
                        try:
                            qty = float(parts[1])
                        except ValueError:
                            qty = 1.0
                        current["ingredients"].append({
                            "name": parts[0],
                            "quantity": qty,
                            "unit": parts[2]
                        })
                elif line.startswith("step:"):
                    current["steps"].append(line[5:])

    return recipes


def write_c_recipes_file(filepath, recipes):
    next_id = max([r.get("id", 0) for r in recipes], default=0) + 1
    with open(filepath, "w", encoding="utf-8") as f:
        f.write("# RecipeBook Data File (v1.0)\n")
        f.write(f"NEXT_ID:{next_id}\n")
        f.write(f"COUNT:{len(recipes)}\n")

        for r in recipes:
            f.write("RECIPE_START\n")
            f.write(f"id:{r.get('id', 1)}\n")
            f.write(f"title:{r.get('title', 'Untitled')}\n")
            f.write(f"category:{r.get('category', 'General')}\n")
            f.write(f"prep_time:{r.get('prep_time_min', 0)}\n")
            f.write(f"cook_time:{r.get('cook_time_min', 0)}\n")
            f.write(f"servings:{r.get('servings', 4)}\n")
            f.write(f"difficulty:{r.get('difficulty', 'Medium')}\n")

            ings = r.get("ingredients", [])
            f.write(f"ingredients_count:{len(ings)}\n")
            for ing in ings:
                f.write(f"ing:{ing.get('name', '')}|{ing.get('quantity', 1.0)}|{ing.get('unit', '')}\n")

            steps = r.get("steps", [])
            f.write(f"steps_count:{len(steps)}\n")
            for step in steps:
                f.write(f"step:{step}\n")

            f.write("RECIPE_END\n")


class RecipeRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=FRONTEND_DIR, **kwargs)

    def do_GET(self):
        parsed_url = urllib.parse.urlparse(self.path)

        if parsed_url.path == "/api/health":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.end_headers()
            self.wfile.write(b'{"status":"ok","bridge":"c_cpp_sync"}')
            return

        if parsed_url.path == "/api/recipes":
            data = parse_c_recipes_file(DATA_FILE_C)
            if not data:
                data = parse_c_recipes_file(DATA_FILE_CPP)

            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.end_headers()
            self.wfile.write(json.dumps(data).encode("utf-8"))
            return

        # Serve static frontend files
        return super().do_GET()

    def do_POST(self):
        parsed_url = urllib.parse.urlparse(self.path)

        if parsed_url.path == "/api/recipes":
            content_length = int(self.headers.get("Content-Length", 0))
            post_data = self.rfile.read(content_length)

            try:
                recipes = json.loads(post_data.decode("utf-8"))
                if isinstance(recipes, list):
                    write_c_recipes_file(DATA_FILE_C, recipes)
                    write_c_recipes_file(DATA_FILE_CPP, recipes)
                    self.send_response(200)
                    self.send_header("Content-Type", "application/json")
                    self.send_header("Access-Control-Allow-Origin", "*")
                    self.end_headers()
                    self.wfile.write(b'{"success":true,"synced":true}')
                    return
            except Exception as err:
                self.send_response(400)
                self.end_headers()
                self.wfile.write(json.dumps({"error": str(err)}).encode("utf-8"))
                return

        self.send_response(404)
        self.end_headers()

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()


def run():
    socketserver.TCPServer.allow_reuse_address = True
    with socketserver.TCPServer(("", PORT), RecipeRequestHandler) as httpd:
        print("\n=======================================================")
        print("Gourmet Recipe Book Web Server is LIVE!")
        print(f"Open in browser:  http://localhost:{PORT}")
        print(f"Synchronizing with: {DATA_FILE_C} and {DATA_FILE_CPP}")
        print("=======================================================\n")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nShutting down server.")


if __name__ == "__main__":
    run()
