#include "http_runtime.hpp"
#include "interpreter/interpreter.hpp"
#include "builtins/net/protocols/http/http_server.hpp"
#include "spirit/return_signal.hpp"

void HttpRuntime::addRoute(const HttpRoute &route)
{
    routes.push_back(std::move(route));
}

void HttpRuntime::serve(int port)
{
    auto server = std::make_shared<HttpServer>();
    bool hasRoot = false;

    for (const auto &route : routes)
    {
        std::string key = "GET " + route.path;
        hasRoot = hasRoot || route.path == "/";

        server->route(key, [this, route](const HttpRequest &req) {
            HttpResponse res;

            try
            {
                for (auto stmt : route.body)
                    interpreter.execute(stmt);

                res.body = "";
            }
            catch (const ReturnSignal &ret)
            {
                res.body = ret.value.toString();
            }

            res.headers["Content-Type"] = route.contentType;

            return res;
        });
    }

    if (!hasRoot)
    {
        server->route("GET /", [this](const HttpRequest &req)
        {
            HttpResponse res;
            res.body = defaultHomepage();
            return res;
        });
    }

    server->listen("0.0.0.0", port);

    std::cout << "🧠 SonOfMan HTTP Server\n\n";
    std::cout << "Local:\n";
    std::cout << "  → http://127.0.0.1:" << port << "/\n";
    std::cout << "  → http://localhost:" << port << "/\n\n";
    std::cout << "Network:\n";
    std::cout << "  → http://<your-ip>:" << port << "/\n\n";
    std::cout << "Try:\n";
    std::cout << "  curl http://localhost:"<< port << "/\n\n";

    std::cout << "Press CTRL+C to stop.\n";

    server->run();
}

std::string HttpRuntime::defaultHomepage()
{
    std::string home = R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>SonOfMan - Jesus Programming Language</title>

<style>
  body {
    margin: 0;
    font-family: system-ui, sans-serif;
    background: #0f172a;
    color: #e2e8f0;
  }
  .container {
    max-width: 1200px;
    margin: auto;
    padding: 1.5rem;
  }
  header {
    margin-bottom: 2rem;
  }
  h1 {
    margin: 0;
  }
  .muted {
    opacity: 0.8;
    font-style: italic;
    padding-left: 12px;
  }
  /* GRID */
  .grid {
    display: grid;
    gap: 1.5rem;
  }
  /* Mobile first: 1 column */
  .grid {
    grid-template-columns: 1fr;
  }
  /* Desktop: 2 columns */
  @media (min-width: 768px) {
    .grid {
      grid-template-columns: 1fr 1fr;
    }
  }
  .card {
    background: #111827;
    padding: 1.2rem;
    border-radius: 12px;
    box-shadow: 0 5px 20px rgba(0,0,0,0.4);
  }
  .code-block {
    position: relative;
  }
  .copy-btn {
        position: absolute;
        top: 8px;
        right: 8px;
        background: #1f2937;
        color: #e2e8f0;
        border: none;
        padding: 0.3rem 0.6rem;
        font-size: 0.75rem;
        border-radius: 6px;
        cursor: pointer;
        opacity: 0.7;
        transition: 0.2s;
  }
  .copy-btn:hover {
    opacity: 1;
    background: #374151;
  }
  .copy-btn.copied {
    background: #16a34a;
    color: white;
  }
  .copy-btn {
    opacity: 0;
  }
  .code-block:hover .copy-btn {
    opacity: 1;
  }
  code {
    display: block;
    background: #020617;
    padding: 0.8rem;
    border-radius: 8px;
    margin-top: 0.5rem;
    color: #38bdf8;
    white-space: pre;
    overflow-x: auto;
  }

  .full {
    grid-column: 1 / -1;
  }
</style>
</head>
<body>
<div class="container">
  <!-- HEADER -->
  <header>
    <h1>🧠 Jesus Programming Language</h1>
    <p><strong>SonOfMan HTTP Server</strong></p>

    <p class="muted">
      "For even the Son of Man did not come to be served, but to serve..." <b>— Mark 10:45</b>
    </p>
</header>
<h3>Define routes in different ways:</h3>
  <!-- GRID CONTENT -->
  <div class="grid">

    <!-- DEFAULT -->
    <div class="card">
      <h3>Default (HTML)</h3>
      <div class="code-block">
        <button class="copy-btn">Copy</button>
        <code>on http '/savior':
    return '&lt;h1&gt;Jesus&lt;/h1&gt;&lt;p&gt;Christ&lt;/p&gt;'
amen
</code>
      </div>
    </div>

    <!-- JSON -->
    <div class="card">
      <h3>JSON</h3>
       <div class="code-block">
        <button class="copy-btn">Copy</button>
      <code>on http '/api' -> json:
    return '{"name": "Jesus"}'
amen
</code>
      </div>
    </div>

    <!-- VARIABLE -->
    <div class="card">
      <h3>Custom (Variable)</h3>
       <div class="code-block">
        <button class="copy-btn">Copy</button>
      <code>create text pdf = 'application/pdf'

on http '/file' -> pdf:
    return 'A PDF file'
amen
</code>
       </div>
    </div>

    <!-- RAW -->
    <div class="card">
      <h3>Explicit MIME</h3>
       <div class="code-block">
        <button class="copy-btn">Copy</button>
      <code>on http '/plain' -> 'text/plain':
    return 'Jesus Christ is the only way'
amen
</code>
       </div>
    </div>

    <!-- FOOTER -->
    <div class="card full">
      <p>
        To override this page, define a route for  '<i>/</i>'
      </p>
       <div class="code-block">
        <button class="copy-btn">Copy</button>
       <code>on http '/':
    return '&lt;h1&gt;Hello, Jesus&lt;/h1&gt;'
amen
</code>
       </div>
    </div>
  </div>
</div>
<script>
  document.querySelectorAll('.copy-btn').forEach(button => {
    button.addEventListener('click', () => {
      const code = button.nextElementSibling.innerText;

      navigator.clipboard.writeText(code).then(() => {
        button.textContent = "Copied!";
        button.classList.add("copied");

        setTimeout(() => {
          button.textContent = "Copy";
          button.classList.remove("copied");
        }, 1500);
      });
    });
  });
</script>
</body>
</html>
)";

    return home;
}
