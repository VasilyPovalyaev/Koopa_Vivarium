idf_component_register(SRCS wifi_app.c main.c led_strip.c http_server.c DHT22.c
                    INCLUDE_DIRS "."
                    EMBED_FILES webpage/app.css webpage/app.js webpage/favicon.ico webpage/index.html webpage/jquery-3.3.1.min.js)
