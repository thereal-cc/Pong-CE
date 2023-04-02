# ----------------------------
# Makefile Options
# ----------------------------

NAME = Pong
ICON = icon.png
DESCRIPTION = "Pong on the TI84 Plus CE"
COMPRESSED = YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
