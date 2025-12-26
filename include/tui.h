#ifndef TUI_H
#define TUI_H

#include "system_info.h"
#include <ncurses.h>
#include <vector>
#include <string>

enum class SortMode {
    CPU,
    MEMORY,
    PID,
    NAME
};

class TUI {
private:
    WINDOW* header_win;
    WINDOW* cpu_win;
    WINDOW* mem_win;
    WINDOW* process_win;
    WINDOW* help_win;
    WINDOW* status_win;
    
    int max_y, max_x;
    bool running;
    bool show_help;
    int selected_process;
    int process_scroll_offset;
    SortMode sort_mode;
    std::string filter_text;
    bool ascending;
    int refresh_interval;
    
    HardwareInfo hw_info;
    UtilizationInfo util_info;
    std::vector<ProcessInfo> all_processes;
    std::vector<ProcessInfo> filtered_processes;
    
    void initColors();
    void createWindows();
    void destroyWindows();
    void updateDimensions();
    
    void drawHeader();
    void drawCPUInfo();
    void drawMemoryInfo();
    void drawProcessList();
    void drawHelpScreen();
    void drawStatusBar();
    
    void updateData();
    void sortProcesses();
    void filterProcesses();
    std::vector<ProcessInfo> getAllProcesses();
    
    void handleInput(int ch);
    void handleResize();
    void killSelectedProcess();
    
    int getColorPair(double percent);
    std::string formatUptime(long seconds);
    std::string formatBytes(long bytes);
    
public:
    TUI();
    ~TUI();
    
    void run();
    void setRefreshInterval(int seconds);
};

#endif
