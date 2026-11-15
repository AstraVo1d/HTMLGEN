#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>
namespace fs = std::filesystem;
using namespace std;
string escapeHTML(const string& text) {
    string result;
    for (char c : text) {
        switch (c) {
            case '&':  result += "&amp;"; break;
            case '<':  result += "&lt;"; break;
            case '>':  result += "&gt;"; break;
            case '"':  result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default:   result += c;
        }
    }

    return result;
}


string escapeCSS(const string& text) {
    string result;

    for (char c : text) {
        if (c == '"' || c == '\\')
            result += '\\';

        result += c;
    }

    return result;
}


bool isURL(const string& value) {
    return value.rfind("https://", 0) == 0 ||
           value.rfind("http://", 0) == 0;
}


string prepareImage(const string& input,
                    const string& filename) {

    if (input.empty())
        return "";

    if (isURL(input))
        return input;

    fs::path source(input);

    if (!fs::exists(source) ||
        !fs::is_regular_file(source)) {

        cout << "Warning: Image not found: "
             << input << endl;

        return "";
    }

    fs::create_directories("assets");

    string extension = source.extension().string();

    fs::path destination =
        fs::path("assets") / (filename + extension);

    fs::copy_file(
        source,
        destination,
        fs::copy_options::overwrite_existing
    );

    return destination.generic_string();
}


string readArticle() {
    string line;
    string content;

    cout << "\nWrite your article.\n";
    cout << "Enter a single dot (.) to finish:\n\n";

    while (getline(cin, line)) {

        if (line == ".")
            break;

        content += line + "\n";
    }

    return content;
}

int main() {

    cout << "HTML generator by AstraVo1d";
    string pageTitle;
    string linkURL;
    string linkText;
    string articleTitle;
    string footerTitle;
    string backgroundInput;

    cout << "page title: ";
    getline(cin, pageTitle);

    cout << "website link url: ";
    getline(cin, linkURL);

    cout << "link text: ";
    getline(cin, linkText);

    cout << "article title (H1): ";
    getline(cin, articleTitle);
    string article = readArticle();

    cout << "\nfooter heading ,h2 optional: ";
    getline(cin, footerTitle);
    cout << "\nbackground image path or url"
         << "(leave empty for black): ";

    getline(cin, backgroundInput);

    string background = prepareImage(
        backgroundInput,
        "background"
    );

    // Article images
    int imageCount = 0;

    cout << "\nnumber of images under article: ";
    cin >> imageCount;

    cin.ignore();

    if (imageCount < 0)
        imageCount = 0;

    vector<string> images;

    for (int i = 0; i < imageCount; i++) {

        string imagePath;

        cout << "image " << i + 1
             << " path or url: ";

        getline(cin, imagePath);

        string prepared = prepareImage(
            imagePath,
            "image_" + to_string(i + 1)
        );

        if (!prepared.empty())
            images.push_back(prepared);
    }
    ofstream output("index.html");

    if (!output.is_open()) {
        cerr << "error cannot create index.html\n";
        return 1;
    }
    output << "<!DOCTYPE html>\n";
    output << "<html lang=\"en\">\n";
    output << "<head>\n";
    output << "  <meta charset=\"UTF-8\">\n";
    output << "  <meta name=\"viewport\" "
              "content=\"width=device-width, initial-scale=1.0\">\n";
    output << "  <title>"
           << escapeHTML(pageTitle)
           << "</title>\n";
    output << "<style>\n";
    output << R"CSS(
h1 {
    text-align: center;
}

p {
    text-align: center;
    white-space: pre-line;
}

h2 {
    text-align: center;
}

a {
    display: inline-block;
    width: 100%;
    text-align: center;
}

body {
    font-family: Tahoma, sans-serif;
    background-color: #000000;
    color: white;
    margin: 22px;
    background-position: center;
    background-size: cover;
    background-attachment: fixed;
}

.article-images {
    text-align: center;
    margin-top: 20px;
}

.article-images img {
    display: block;
    max-width: 100%;
    height: auto;
    margin: 20px auto;
    border-radius: 3px;
}
)CSS";
    if (!background.empty()) {
        output << "\nbody {\n"
               << "    background-image: linear-gradient("
                  "rgba(0,0,0,0.45), rgba(0,0,0,0.45)), "
               << "url(\""
               << escapeCSS(background)
               << "\");\n"
               << "}\n";
    }
    output << "</style>\n";
    output << "</head>\n";
    output << "<body>\n";
    if (!articleTitle.empty()) {
        output << "<h1>"
               << escapeHTML(articleTitle)
               << "</h1>\n";
    }
    if (!linkURL.empty()) {
        output << "<a href=\""
               << escapeHTML(linkURL)
               << "\" target=\"_blank\" "
                  "rel=\"noopener noreferrer\">"
               << escapeHTML(linkText)
               << "</a>\n";
    }
    if (!article.empty()) {
        output << "<p>"
               << escapeHTML(article)
               << "</p>\n";
    }
    if (!images.empty()) {

        output << "<div class=\"article-images\">\n";

        for (const string& img : images) {

            output << "<img src=\""
                   << escapeHTML(img)
                   << "\" alt=\"article-image\">\n";
        }
        output << "</div>\n";
    }
    if (!footerTitle.empty()) {
        output << "<h2>"
               << escapeHTML(footerTitle)
               << "</h2>\n";
    }
    output << "</body>\n";
    output << "</html>\n";
    output.close();
    cout << "Done creating index.html\n";
    cout << "Please donate to the project to make it grow up (: , via email:\n";
    cout << "astravoid.service@proton.me";    
    return 0;
}
