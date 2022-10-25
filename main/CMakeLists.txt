idf_component_register(SRCS wifi_app.c main.c led_strip.c http_server.c DHT22.c relays.c
                    INCLUDE_DIRS "."
                    EMBED_FILES website/home/app.css website/home/app.js website/home/favicon.ico website/home/index.html website/home/jquery-3.3.1.min.js website/settings/settings.html website/settings/settings.css website/settings/settings.js)
