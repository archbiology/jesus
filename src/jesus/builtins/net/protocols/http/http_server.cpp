#include <format>
#include "http_server.hpp"

bool HttpServer::wantsJson(const HttpRequest& req)
{
    auto it = req.headers.find("Accept");
    if (it == req.headers.end()) return false;

    return it->second.find("application/json") != std::string::npos;
}

std::string HttpServer::renderJsonError(const HttpError& err)
{
    std::ostringstream oss;

    oss << "{"
        << "\"code\":" << err.code << ","
        << "\"title\":\"" << err.title << "\","
        << "\"message\":\"" << err.message << "\","
        << "\"hint\":\"" << err.hint << "\","
        << "\"actionText\":\"" << err.actionText << "\","
        << "\"actionLink\":\"" << err.actionLink << "\""
        << "}";

    return oss.str();
}

void HttpServer::renderErrorPage(int code, const HttpRequest& req, HttpResponse &res)
{

    HttpError httpError = getHttpError(code);
    res.status = httpError.code;
    res.reason = httpError.title;

    if (wantsJson(req))
    {
        res.headers["Content-Type"] = "application/json; charset=utf-8";
        res.body = renderJsonError(httpError);
    }
    else
    {
        res.headers["Content-Type"] = "text/html; charset=utf-8";
        res.body = renderErrorTemplate(httpError);
    }

}

inline HttpError HttpServer::getHttpError(int code)
{
    switch (code)
    {
    case 400:
        return {400, "Bad Request",
                "The request could not be understood.",
                "Check the request format or parameters.",
                "Go back home", "/"};

    case 403:
        return {403, "Forbidden",
                "You do not have permission to access this resource.",
                "This resource may require authentication.",
                "Go to homepage", "/"};

    case 404:
        return {404, "Not Found",
                "The page does not exist.",
                "It may have been moved or deleted.",
                "Go to homepage", "/"};

    case 405:
        return {405, "Method Not Allowed",
                "This HTTP method is not supported here.",
                "Try using GET or POST instead.",
                "Go to homepage", "/"};

    case 408:
        return {408, "Request Timeout",
                "The server took too long to respond.",
                "Try again later.",
                "Go to homepage", "/"};

    case 413:
        return {413, "Payload Too Large",
                "The request is too large to process.",
                "Reduce the size of your request body.",
                "Go to homepage", "/"};

    case 429:
        return {429, "Too Many Requests",
                "You are sending requests too quickly.",
                "Please slow down.",
                "Go to homepage", "/"};

    case 500:
    default:
        return {500, "Internal Server Error",
                "Something went wrong while processing your request.",
                "The server encountered an unexpected condition.",
                "Go to homepage", "/"};
    }
}

std::string HttpServer::renderErrorTemplate(HttpError &error)
{
    return std::format(R"(
<!DOCTYPE html>
<html>
<head>
  <title>{0} - SonOfMan</title>
  <style>
    body {{
      font-family: system-ui;
      background: #0f172a;
      color: #e2e8f0;
      display: flex;
      align-items: center;
      justify-content: center;
      height: 100vh;
      margin: 0;
    }}
    .box {{
      text-align: center;
      max-width: 600px;
    }}
    h1 {{
      font-size: 3rem;
      margin-bottom: 0;
    }}
    p {{
      opacity: 0.85;
    }}
    a {{
      color: #38bdf8;
      text-decoration: none;
    }}
  </style>
</head>
<body>
  <div class="box">
    <h1>{0}</h1>
    <p><strong>{1}</strong></p>
    <p>{2}</p>
    <p style="margin-top:1rem; opacity:0.7">{3}</p>
    <p><a href="{5}">{4}</a></p>
  </div>
</body>
</html>
)", error.code, error.title, error.message, error.hint, error.actionText, error.actionLink);
}
