#include "format.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/ioctl.h>
#include <unistd.h>

int getTerminalWidth() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col;
    }
    return 80; // Default fallback
}

std::string centerText(const std::string& text, int width) {
    if ((int)text.length() >= width) return text;
    int padding = (width - text.length()) / 2;
    return std::string(padding, ' ') + text;
}

std::string padRight(const std::string& text, int width) {
    if ((int)text.length() >= width) return text.substr(0, width);
    return text + std::string(width - text.length(), ' ');
}

std::string padLeft(const std::string& text, int width) {
    if ((int)text.length() >= width) return text.substr(0, width);
    return std::string(width - text.length(), ' ') + text;
}

std::string createSeparator(const std::string& title, int width) {
    if (width == -1) {
        width = getTerminalWidth();
    }
    
    if (title.empty()) {
        std::string result;
        for (int i = 0; i < width; i++) {
            result += BoxChars::HORIZONTAL;
        }
        return result;
    }
    
    std::string sep = " " + title + " ";
    int remaining = width - sep.length();
    if (remaining <= 0) return sep;
    
    int left = remaining / 2;
    int right = remaining - left;
    
    std::string result;
    for (int i = 0; i < left; i++) {
        result += BoxChars::HORIZONTAL;
    }
    result += sep;
    for (int i = 0; i < right; i++) {
        result += BoxChars::HORIZONTAL;
    }
    
    return result;
}

// Table implementation
Table::Table() {}

void Table::addColumn(const std::string& header, bool align_right) {
    TableColumn col;
    col.header = header;
    col.align_right = align_right;
    col.width = header.length();
    columns.push_back(col);
}

void Table::addRow(const std::vector<std::string>& row) {
    for (size_t i = 0; i < row.size() && i < columns.size(); i++) {
        columns[i].rows.push_back(row[i]);
        columns[i].width = std::max(columns[i].width, (int)row[i].length());
    }
}

void Table::calculateWidths() {
    // Already calculated in addRow
}

std::string Table::renderHeader() {
    std::ostringstream oss;
    
    // Top border
    oss << BoxChars::TOP_LEFT;
    for (size_t i = 0; i < columns.size(); i++) {
        for (int j = 0; j < columns[i].width + 2; j++) {
            oss << BoxChars::HORIZONTAL;
        }
        if (i < columns.size() - 1) {
            oss << BoxChars::T_DOWN;
        }
    }
    oss << BoxChars::TOP_RIGHT << "\n";
    
    // Header row
    oss << BoxChars::VERTICAL;
    for (size_t i = 0; i < columns.size(); i++) {
        oss << " " << padRight(columns[i].header, columns[i].width) << " ";
        oss << BoxChars::VERTICAL;
    }
    oss << "\n";
    
    return oss.str();
}

std::string Table::renderSeparator() {
    std::ostringstream oss;
    oss << BoxChars::T_RIGHT;
    for (size_t i = 0; i < columns.size(); i++) {
        for (int j = 0; j < columns[i].width + 2; j++) {
            oss << BoxChars::HORIZONTAL;
        }
        if (i < columns.size() - 1) {
            oss << BoxChars::CROSS;
        }
    }
    oss << BoxChars::T_LEFT << "\n";
    return oss.str();
}

std::string Table::renderRow(size_t row_idx) {
    std::ostringstream oss;
    oss << BoxChars::VERTICAL;
    for (size_t i = 0; i < columns.size(); i++) {
        std::string value = row_idx < columns[i].rows.size() ? columns[i].rows[row_idx] : "";
        if (columns[i].align_right) {
            oss << " " << padLeft(value, columns[i].width) << " ";
        } else {
            oss << " " << padRight(value, columns[i].width) << " ";
        }
        oss << BoxChars::VERTICAL;
    }
    oss << "\n";
    return oss.str();
}

std::string Table::render(bool use_colors) {
    calculateWidths();
    std::ostringstream oss;
    
    oss << renderHeader();
    oss << renderSeparator();
    
    // Find max rows
    size_t max_rows = 0;
    for (const auto& col : columns) {
        max_rows = std::max(max_rows, col.rows.size());
    }
    
    // Render rows
    for (size_t i = 0; i < max_rows; i++) {
        oss << renderRow(i);
    }
    
    // Bottom border
    oss << BoxChars::BOTTOM_LEFT;
    for (size_t i = 0; i < columns.size(); i++) {
        for (int j = 0; j < columns[i].width + 2; j++) {
            oss << BoxChars::HORIZONTAL;
        }
        if (i < columns.size() - 1) {
            oss << BoxChars::T_UP;
        }
    }
    oss << BoxChars::BOTTOM_RIGHT << "\n";
    
    return oss.str();
}
