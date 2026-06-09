# Infrared

IR TX is on G44.

ESP-IDF RMT is the intended implementation path for carrier-based IR protocols.
The current driver initializes the GPIO and exposes a raw mark/space API shape.
