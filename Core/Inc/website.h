const char* html_page =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=utf-8\r\n\r\n"
"<!DOCTYPE html><html lang=\"ru\">"
"<head><meta charset=\"UTF-8\">"
"<title>Настройка часов</title>"
"<style>"
"body{font-family:sans-serif;text-align:center;padding:2em;background:#f4f4f4;}"
"form{background:#fff;padding:2em;border-radius:10px;display:inline-block;box-shadow:0 0 10px #ccc;}"
"input,button{margin:0.5em 0;padding:0.7em;width:90%;font-size:1em;}"
"button{background:#007BFF;color:#fff;border:none;border-radius:5px;}"
"button:hover{background:#0056b3;}"
"</style></head><body>"
"<form action=\"/connect\" method=\"POST\">"
"<h2>Настройка часов</h2>"
"<input name=\"ssid\" placeholder=\"Имя сети (SSID)\" required><br>"
"<input name=\"password\" type=\"password\" placeholder=\"Пароль от Wi-Fi\" required><br>"
"<button type=\"submit\">Обновить настройки</button>"
"</form></body></html>";

const char* config_saved_page =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=utf-8\r\n\r\n"
"<!DOCTYPE html><html lang=\"ru\"><head><meta charset=\"UTF-8\">"
"<title>Настройки сохранены</title>"
"<style>"
"body{background:#f0f2f5;font-family:sans-serif;text-align:center;padding:2em;}"
".card{background:#fff;padding:2em;border-radius:12px;display:inline-block;box-shadow:0 4px 12px rgba(0,0,0,0.1);animation:fadeIn 1s ease;}"
"h1{color:#4CAF50;font-size:2em;margin-bottom:0.5em;}"
"p{color:#555;margin:1em 0;font-size:1.1em;}"
"@keyframes fadeIn{from{opacity:0;transform:scale(0.95);}to{opacity:1;transform:scale(1);}}"
"</style></head><body>"
"<div class=\"card\">"
"<h1>✅ Настройки сохранены!</h1>"
"<p>Данные Wi-Fi успешно записаны.</p>"
"</div>"
"</body></html>";

void extract_param(const char* body, const char* key, char* output, size_t max_len) {
    char* start = strstr(body, key);
    if (start) {
        start += strlen(key); // пропустить ключ
        char* end = strchr(start, '&');
        if (!end) end = strchr(start, '\0');
        size_t len = (end - start < max_len - 1) ? end - start : max_len - 1;
        strncpy(output, start, len);
        output[len] = '\0';
    } else {
        output[0] = '\0'; // не найдено
    }
}