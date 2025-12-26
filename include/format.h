#ifndef FORMAT_H
#define FORMAT_H

#include <string>
#include <vector>

// Box drawing characters (UTF-8)
namespace BoxChars {
    const std::string TOP_LEFT = "┌";
    const std::string TOP_RIGHT = "┐";
    const std::string BOTTOM_LEFT = "└";
    const std::string BOTTOM_RIGHT = "┘";
    const std::string HORIZONTAL = "─";
    const std::string VERTICAL = "│";
    const std::string T_DOWN = "┬";
    const std::string T_UP = "┴";
    const std::string T_RIGHT = "├";
    const std::string T_LEFT = "┤";
    const std::string CROSS = "┼";
}

// Unicode icons
namespace Icons {
    const std::string CPU = "󰻠 ";
    const std::string MEMORY = "󰍛 ";
    const std::string DISK = "󰋊 ";
    const std::string NETWORK = "󰖩 ";
    const std::string PROCESS = "󰐱 ";
    const std::string TEMP = "󰔄 ";
    const std::string CLOCK = "󰥔 ";
    const std::string WARNING = "⚠ ";
    const std::string CHECK = "✓ ";
    const std::string ARROW_UP = "↑";
    const std::string ARROW_DOWN = "↓";
    const std::string PLUGIN = "󰏖 ";
}

// Table structure
struct TableColumn {
    std::string header;
    std::vector<std::string> rows;
    int width;
    bool align_right;
};

class Table {
public:
    Table();
    void addColumn(const std::string& header, bool align_right = false);
    void addRow(const std::vector<std::string>& row);
    std::string render(bool use_colors = true);
    
private:
    std::vector<TableColumn> columns;
    void calculateWidths();
    std::string renderHeader();
    std::string renderSeparator();
    std::string renderRow(size_t row_idx);
};

// Terminal utilities
int getTerminalWidth();
std::string centerText(const std::string& text, int width);
std::string padRight(const std::string& text, int width);
std::string padLeft(const std::string& text, int width);
std::string createSeparator(const std::string& title = "", int width = -1);

#endif // FORMAT_H
