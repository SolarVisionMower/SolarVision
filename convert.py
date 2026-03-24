import argparse
import markdown
from pathlib import Path

boilerplate1 = """<!-- abc -->
<!DOCTYPE html>
<head>
<meta charset="utf-8" />
<title>
"""

boilerplate2 = """
</title>
<style>
body{
    margin: 0 auto;
    font-family: Georgia, Palatino, serif;
    color: #444444;
    line-height: 1;
    max-width: 960px;
    padding: 30px;
}
h1, h2, h3, h4 {
    color: #111111;
    font-weight: 400;
}
h1, h2, h3, h4, h5, p {
    margin-bottom: 24px;
    padding: 0;
}
h1 {
    font-size: 48px;
}
h2 {
    font-size: 36px;
    margin: 24px 0 6px;
}
h3 {
    font-size: 24px;
}
h4 {
    font-size: 21px;
}
h5 {
    font-size: 18px;
}
a {
    color: #0099ff;
    margin: 0;
    padding: 0;
    vertical-align: baseline;
}
ul, ol {
    padding: 0;
    margin: 0;
}
li {
    line-height: 24px;
}
li ul, li ul {
    margin-left: 24px;
}
p, ul, ol {
    font-size: 16px;
    line-height: 24px;
    max-width: 960px;
}
pre {
    padding: 0px 24px;
    max-width: 960px;
    white-space: pre-wrap;
    background-color: #e0e0e0;
}
code {
    font-family: Consolas, Monaco, Andale Mono, monospace;
    line-height: 1.5;
    font-size: 13px;
    padding: 3px;
    background-color: #e0e0e0;
    border-radius: 5px;
}
aside {
    display: block;
    float: right;
    width: 390px;
}
blockquote {
    margin: 1em 2em;
    max-width: 960px;
}
blockquote p {
    color: #666;
    max-width: 960px;
}
hr {
    max-width: 960px;
    text-align: left;
    margin: 0 auto 0 0;
    color: #999;
}
table {
    border-collapse: collapse;
    margin: 10pt auto;
    border: 1px solid #CCC;
}
table thead {
    background-color: #EEE;
}
table thead td {
    color: #666;
}
table td {
    padding: 0.5em 1em;
    border: 1px solid #CCC;
}
img, figcaption {
	max-width: 960px;
	text-align: center;
	margin: 0 auto;
	display: block;
}
figcaption {
	margin: 7px auto;
}
</style>
</head>
<body>
"""

boilerplate3 = """
</body>
"""

def delete_md(root_path):
    deleted_files = []
    for html_file in root_path.rglob("*.html"):
        try:
            with open(html_file, "r", encoding="utf-8") as f:
                first_line = f.readline().strip()
            if first_line == "<!-- abc -->":
                html_file.unlink()
                deleted_files.append(html_file)
        except (IOError, OSError):
            continue

    if deleted_files:
        print(f"Deleted {len(deleted_files)} HTML files:")
        for name in deleted_files:
            print(f"- {name}")
    else:
        print("No tagged HTML files found to delete.")

def list_html_files(root_path):
    print(f"Listing all HTML files in: {root_path.absolute()}\n")
    # Using rglob to find all .html files in subdirectories
    html_files = list(root_path.rglob("*.html"))
    
    if html_files:
        for f in html_files:
            print(f"- {f}")
        print(f"\nTotal HTML files found: {len(html_files)}")
    else:
        print("No HTML files found.")

def convert_md_to_html(root_path):
    if not root_path.is_dir():
        print(f"Error: {root_dir} is not a valid directory.")
        return

    converted_files = []

    # Recursively glob for markdown files
    # Supports both .md and .markdown extensions
    md_files = list(root_path.rglob("*.md")) + list(root_path.rglob("*.markdown"))

    for md_file in md_files:
        # Define the output HTML path
        html_file = md_file.with_suffix(".html")
        title     = md_file.name[:-3]

        # Explicitly delete the existing HTML file if it exists
        if html_file.exists():
            html_file.unlink()

        # Read Markdown and convert to HTML
        with open(md_file, "r", encoding="utf-8") as f:
            md_content = f.read()
            html_content = markdown.markdown(md_content)

        # Write the HTML file
        with open(html_file, "w", encoding="utf-8") as f:
            f.write(boilerplate1)
            f.write(title)
            f.write(boilerplate2)
            f.write(html_content)
            f.write(boilerplate3)
        
        converted_files.append(md_file)

    # Print results
    if converted_files:
        print("\nSuccessfully converted the following files:")
        for name in converted_files:
            print(f"- {name}")
    else:
        print("No Markdown files found to convert.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Recursively convert Markdown files to HTML.")
    parser.add_argument("directory", help="The root directory to start searching from.")
    parser.add_argument(
        "--list",
        action="store_true",
        help="Recursively list all HTML files"
    )
    parser.add_argument(
        "--delete",
        action="store_true",
        help="Delete all HTML files created by this tool"
    )

    args = parser.parse_args()
    root_path = Path(args.directory)

    if args.list:
        list_html_files(root_path)
    elif args.delete:
        delete_md(root_path)
    else:
        convert_md_to_html(root_path)
