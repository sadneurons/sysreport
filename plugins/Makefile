CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC -I../include
LDFLAGS = -shared

PLUGIN_DIR = .
BUILD_DIR = build

PLUGINS = $(BUILD_DIR)/uptime.so $(BUILD_DIR)/custom.so

all: $(BUILD_DIR) $(PLUGINS)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/uptime.so: example_uptime.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

$(BUILD_DIR)/custom.so: example_custom.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

install: all
	@mkdir -p /usr/lib/sysreport/plugins
	@cp $(BUILD_DIR)/*.so /usr/lib/sysreport/plugins/
	@echo "Plugins installed to /usr/lib/sysreport/plugins/"

.PHONY: all clean install
