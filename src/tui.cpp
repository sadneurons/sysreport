#include "tui.h"
#include "system_info.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <fstream>

// Color pair definitions
#define COLOR_PAIR_DEFAULT 1
#define COLOR_PAIR_HEADER 2
#define COLOR_PAIR_SELECTED 3
#define COLOR_PAIR_GREEN 4
#define COLOR_PAIR_YELLOW 5
#define COLOR_PAIR_RED 6
#define COLOR_PAIR_CYAN 7

TUI::TUI() : running(true), show_help(false), selected_process(0), 
             process_scroll_offset(0), sort_mode(SortMode::MEMORY), 
             ascending(false), refresh_interval(2) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    
    if (has_colors()) {
        start_color();
        initColors();
    }
    
    getmaxyx(stdscr, max_y, max_x);
    createWindows();
    
    hw_info = getHardwareInfo();
}

TUI::~TUI() {
    destroyWindows();
    endwin();
}

void TUI::initColors() {
    init_pair(COLOR_PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_PAIR_HEADER, COLOR_BLACK, COLOR_CYAN);
    init_pair(COLOR_PAIR_SELECTED, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_PAIR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_PAIR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_PAIR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_PAIR_CYAN, COLOR_CYAN, COLOR_BLACK);
}

void TUI::createWindows() {
    int header_height = 1;
    int cpu_height = 8;
    int mem_height = 6;
    int status_height = 1;
    
    header_win = newwin(header_height, max_x, 0, 0);
    cpu_win = newwin(cpu_height, max_x, header_height, 0);
    mem_win = newwin(mem_height, max_x, header_height + cpu_height, 0);
    process_win = newwin(max_y - header_height - cpu_height - mem_height - status_height, 
                         max_x, header_height + cpu_height + mem_height, 0);
    status_win = newwin(status_height, max_x, max_y - status_height, 0);
}

void TUI::destroyWindows() {
    if (header_win) delwin(header_win);
    if (cpu_win) delwin(cpu_win);
    if (mem_win) delwin(mem_win);
    if (process_win) delwin(process_win);
    if (status_win) delwin(status_win);
    if (help_win) delwin(help_win);
}

void TUI::updateDimensions() {
    getmaxyx(stdscr, max_y, max_x);
    destroyWindows();
    createWindows();
}

int TUI::getColorPair(double percent) {
    if (percent >= 90.0) return COLOR_PAIR_RED;
    if (percent >= 70.0) return COLOR_PAIR_YELLOW;
    return COLOR_PAIR_GREEN;
}

std::string TUI::formatUptime(long seconds) {
    long days = seconds / 86400;
    long hours = (seconds % 86400) / 3600;
    long minutes = (seconds % 3600) / 60;
    
    std::ostringstream oss;
    if (days > 0) oss << days << "d ";
    if (hours > 0 || days > 0) oss << hours << "h ";
    oss << minutes << "m";
    return oss.str();
}

std::string TUI::formatBytes(long bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = bytes;
    
    while (size >= 1024 && unit < 4) {
        size /= 1024;
        unit++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << size << " " << units[unit];
    return oss.str();
}

void TUI::drawHeader() {
    werase(header_win);
    wbkgd(header_win, COLOR_PAIR(COLOR_PAIR_HEADER));
    
    std::ostringstream header;
    header << " sysreport TUI - " << hw_info.os_info 
           << " | " << hw_info.cpu_model << " (" << hw_info.cpu_cores << " cores)";
    
    mvwprintw(header_win, 0, 0, "%s", header.str().c_str());
    wrefresh(header_win);
}

void TUI::drawCPUInfo() {
    werase(cpu_win);
    box(cpu_win, 0, 0);
    mvwprintw(cpu_win, 0, 2, "[ CPU ]");
    
    int color = getColorPair(util_info.cpu_percent);
    wattron(cpu_win, COLOR_PAIR(color) | A_BOLD);
    mvwprintw(cpu_win, 1, 2, "Usage: %5.1f%%", util_info.cpu_percent);
    wattroff(cpu_win, COLOR_PAIR(color) | A_BOLD);
    
    mvwprintw(cpu_win, 2, 2, "Load: %.2f, %.2f, %.2f", 
              util_info.load_avg_1, util_info.load_avg_5, util_info.load_avg_15);
    mvwprintw(cpu_win, 3, 2, "Uptime: %s", util_info.uptime.c_str());
    
    // Per-core display (first 16 cores)
    int cores_to_show = std::min(16, (int)util_info.cpu_per_core.size());
    mvwprintw(cpu_win, 4, 2, "Per-Core:");
    for (int i = 0; i < cores_to_show && i < 8; i++) {
        int core_color = getColorPair(util_info.cpu_per_core[i]);
        wattron(cpu_win, COLOR_PAIR(core_color));
        mvwprintw(cpu_win, 5, 2 + i * 8, "%2d:%3.0f%%", i, util_info.cpu_per_core[i]);
        wattroff(cpu_win, COLOR_PAIR(core_color));
    }
    if (cores_to_show > 8) {
        for (int i = 8; i < cores_to_show; i++) {
            int core_color = getColorPair(util_info.cpu_per_core[i]);
            wattron(cpu_win, COLOR_PAIR(core_color));
            mvwprintw(cpu_win, 6, 2 + (i - 8) * 8, "%2d:%3.0f%%", i, util_info.cpu_per_core[i]);
            wattroff(cpu_win, COLOR_PAIR(core_color));
        }
    }
    
    wrefresh(cpu_win);
}

void TUI::drawMemoryInfo() {
    werase(mem_win);
    box(mem_win, 0, 0);
    mvwprintw(mem_win, 0, 2, "[ Memory ]");
    
    int ram_color = getColorPair(util_info.ram_percent);
    wattron(mem_win, COLOR_PAIR(ram_color) | A_BOLD);
    mvwprintw(mem_win, 1, 2, "RAM:  %5ld MB / %5.1f%%", 
              util_info.used_ram_mb, util_info.ram_percent);
    wattroff(mem_win, COLOR_PAIR(ram_color) | A_BOLD);
    
    mvwprintw(mem_win, 2, 2, "Available: %5ld MB", util_info.available_ram_mb);
    
    if (util_info.used_swap_mb > 0) {
        int swap_color = getColorPair(util_info.swap_percent);
        wattron(mem_win, COLOR_PAIR(swap_color));
        mvwprintw(mem_win, 3, 2, "Swap: %5ld MB / %5.1f%%", 
                  util_info.used_swap_mb, util_info.swap_percent);
        wattroff(mem_win, COLOR_PAIR(swap_color));
    }
    
    // GPU if available
    if (!util_info.gpus.empty()) {
        const auto& gpu = util_info.gpus[0];
        int gpu_color = getColorPair(gpu.utilization_percent);
        wattron(mem_win, COLOR_PAIR(gpu_color));
        mvwprintw(mem_win, 4, 2, "GPU:  %5.1f%% | %4.0f/%4.0f MB | %4.1f°C", 
                  gpu.utilization_percent, gpu.memory_used_mb, 
                  gpu.memory_total_mb, gpu.temperature);
        wattroff(mem_win, COLOR_PAIR(gpu_color));
    }
    
    wrefresh(mem_win);
}

void TUI::drawProcessList() {
    werase(process_win);
    box(process_win, 0, 0);
    
    std::string sort_indicator;
    switch (sort_mode) {
        case SortMode::CPU: sort_indicator = "CPU"; break;
        case SortMode::MEMORY: sort_indicator = "MEM"; break;
        case SortMode::PID: sort_indicator = "PID"; break;
        case SortMode::NAME: sort_indicator = "NAME"; break;
    }
    
    mvwprintw(process_win, 0, 2, "[ Processes - Sort: %s %s | Filter: %s ]", 
              sort_indicator.c_str(), ascending ? "^" : "v",
              filter_text.empty() ? "none" : filter_text.c_str());
    
    // Header
    wattron(process_win, A_BOLD);
    mvwprintw(process_win, 1, 2, "%-8s %-6s %-6s %-40s", "PID", "CPU%", "MEM MB", "NAME");
    wattroff(process_win, A_BOLD);
    
    int win_height, win_width;
    getmaxyx(process_win, win_height, win_width);
    int available_lines = win_height - 3;
    
    for (int i = 0; i < available_lines && (i + process_scroll_offset) < (int)filtered_processes.size(); i++) {
        int idx = i + process_scroll_offset;
        const auto& proc = filtered_processes[idx];
        
        if (idx == selected_process) {
            wattron(process_win, COLOR_PAIR(COLOR_PAIR_SELECTED) | A_BOLD);
        }
        
        mvwprintw(process_win, 2 + i, 2, "%-8d %-6.1f %-6ld %-40s", 
                  proc.pid, proc.cpu_percent, proc.mem_mb, proc.name.c_str());
        
        if (idx == selected_process) {
            wattroff(process_win, COLOR_PAIR(COLOR_PAIR_SELECTED) | A_BOLD);
        }
    }
    
    wrefresh(process_win);
}

void TUI::drawHelpScreen() {
    if (!help_win) {
        int h = 20, w = 60;
        help_win = newwin(h, w, (max_y - h) / 2, (max_x - w) / 2);
    }
    
    werase(help_win);
    box(help_win, 0, 0);
    wbkgd(help_win, COLOR_PAIR(COLOR_PAIR_CYAN));
    
    mvwprintw(help_win, 0, 2, "[ Help - Press 'h' to close ]");
    
    mvwprintw(help_win, 2, 2, "Navigation:");
    mvwprintw(help_win, 3, 4, "Arrow Up/Down  - Move selection");
    mvwprintw(help_win, 4, 4, "Page Up/Down   - Scroll page");
    mvwprintw(help_win, 5, 4, "Home/End       - First/Last process");
    
    mvwprintw(help_win, 7, 2, "Sorting:");
    mvwprintw(help_win, 8, 4, "c - Sort by CPU");
    mvwprintw(help_win, 9, 4, "m - Sort by Memory");
    mvwprintw(help_win, 10, 4, "p - Sort by PID");
    mvwprintw(help_win, 11, 4, "n - Sort by Name");
    mvwprintw(help_win, 12, 4, "r - Reverse sort order");
    
    mvwprintw(help_win, 14, 2, "Actions:");
    mvwprintw(help_win, 15, 4, "k - Kill selected process");
    mvwprintw(help_win, 16, 4, "f - Filter processes (not impl)");
    mvwprintw(help_win, 17, 4, "q - Quit");
    mvwprintw(help_win, 18, 4, "h - Toggle this help");
    
    wrefresh(help_win);
}

void TUI::drawStatusBar() {
    werase(status_win);
    wbkgd(status_win, COLOR_PAIR(COLOR_PAIR_HEADER));
    
    mvwprintw(status_win, 0, 2, "Refresh: %ds | Processes: %zu/%zu | Press 'h' for help | 'q' to quit",
              refresh_interval, filtered_processes.size(), all_processes.size());
    
    wrefresh(status_win);
}

std::vector<ProcessInfo> TUI::getAllProcesses() {
    std::vector<ProcessInfo> processes;
    DIR* dir = opendir("/proc");
    if (!dir) return processes;
    
    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_DIR) continue;
        
        int pid = atoi(entry->d_name);
        if (pid <= 0) continue;
        
        ProcessInfo proc;
        proc.pid = pid;
        
        // Read process name
        std::string stat_path = "/proc/" + std::string(entry->d_name) + "/stat";
        std::ifstream stat_file(stat_path);
        if (!stat_file) continue;
        
        std::string line;
        std::getline(stat_file, line);
        
        size_t name_start = line.find('(');
        size_t name_end = line.find(')');
        if (name_start != std::string::npos && name_end != std::string::npos) {
            proc.name = line.substr(name_start + 1, name_end - name_start - 1);
        }
        
        // Read memory
        std::string status_path = "/proc/" + std::string(entry->d_name) + "/status";
        std::ifstream status_file(status_path);
        while (std::getline(status_file, line)) {
            if (line.find("VmRSS:") != std::string::npos) {
                std::istringstream iss(line);
                std::string label;
                long kb;
                iss >> label >> kb;
                proc.mem_mb = kb / 1024;
                break;
            }
        }
        
        proc.cpu_percent = 0.0; // Would need delta calculation
        processes.push_back(proc);
    }
    
    closedir(dir);
    return processes;
}

void TUI::sortProcesses() {
    switch (sort_mode) {
        case SortMode::CPU:
            std::sort(all_processes.begin(), all_processes.end(),
                     [this](const ProcessInfo& a, const ProcessInfo& b) {
                         return ascending ? a.cpu_percent < b.cpu_percent : a.cpu_percent > b.cpu_percent;
                     });
            break;
        case SortMode::MEMORY:
            std::sort(all_processes.begin(), all_processes.end(),
                     [this](const ProcessInfo& a, const ProcessInfo& b) {
                         return ascending ? a.mem_mb < b.mem_mb : a.mem_mb > b.mem_mb;
                     });
            break;
        case SortMode::PID:
            std::sort(all_processes.begin(), all_processes.end(),
                     [this](const ProcessInfo& a, const ProcessInfo& b) {
                         return ascending ? a.pid < b.pid : a.pid > b.pid;
                     });
            break;
        case SortMode::NAME:
            std::sort(all_processes.begin(), all_processes.end(),
                     [this](const ProcessInfo& a, const ProcessInfo& b) {
                         return ascending ? a.name < b.name : a.name > b.name;
                     });
            break;
    }
}

void TUI::filterProcesses() {
    filtered_processes.clear();
    
    if (filter_text.empty()) {
        filtered_processes = all_processes;
    } else {
        for (const auto& proc : all_processes) {
            if (proc.name.find(filter_text) != std::string::npos) {
                filtered_processes.push_back(proc);
            }
        }
    }
}

void TUI::updateData() {
    util_info = getUtilizationInfo();
    all_processes = getAllProcesses();
    sortProcesses();
    filterProcesses();
}

void TUI::handleResize() {
    endwin();
    refresh();
    updateDimensions();
}

void TUI::killSelectedProcess() {
    if (selected_process >= 0 && selected_process < (int)filtered_processes.size()) {
        int pid = filtered_processes[selected_process].pid;
        kill(pid, SIGTERM);
    }
}

void TUI::handleInput(int ch) {
    int win_height, win_width;
    getmaxyx(process_win, win_height, win_width);
    int available_lines = win_height - 3;
    
    switch (ch) {
        case 'q':
        case 'Q':
            running = false;
            break;
            
        case 'h':
        case 'H':
            show_help = !show_help;
            break;
            
        case KEY_UP:
            if (selected_process > 0) {
                selected_process--;
                if (selected_process < process_scroll_offset) {
                    process_scroll_offset = selected_process;
                }
            }
            break;
            
        case KEY_DOWN:
            if (selected_process < (int)filtered_processes.size() - 1) {
                selected_process++;
                if (selected_process >= process_scroll_offset + available_lines) {
                    process_scroll_offset = selected_process - available_lines + 1;
                }
            }
            break;
            
        case KEY_PPAGE: // Page Up
            selected_process = std::max(0, selected_process - available_lines);
            process_scroll_offset = std::max(0, process_scroll_offset - available_lines);
            break;
            
        case KEY_NPAGE: // Page Down
            selected_process = std::min((int)filtered_processes.size() - 1, 
                                       selected_process + available_lines);
            process_scroll_offset = std::min((int)filtered_processes.size() - available_lines,
                                            process_scroll_offset + available_lines);
            break;
            
        case KEY_HOME:
            selected_process = 0;
            process_scroll_offset = 0;
            break;
            
        case KEY_END:
            selected_process = filtered_processes.size() - 1;
            process_scroll_offset = std::max(0, (int)filtered_processes.size() - available_lines);
            break;
            
        case 'c':
        case 'C':
            sort_mode = SortMode::CPU;
            break;
            
        case 'm':
        case 'M':
            sort_mode = SortMode::MEMORY;
            break;
            
        case 'p':
        case 'P':
            sort_mode = SortMode::PID;
            break;
            
        case 'n':
        case 'N':
            sort_mode = SortMode::NAME;
            break;
            
        case 'r':
        case 'R':
            ascending = !ascending;
            break;
            
        case 'k':
        case 'K':
            killSelectedProcess();
            break;
            
        case KEY_RESIZE:
            handleResize();
            break;
    }
}

void TUI::run() {
    updateData();
    
    time_t last_update = time(nullptr);
    
    while (running) {
        // Update data periodically
        time_t now = time(nullptr);
        if (now - last_update >= refresh_interval) {
            updateData();
            last_update = now;
        }
        
        // Draw all windows
        drawHeader();
        drawCPUInfo();
        drawMemoryInfo();
        drawProcessList();
        drawStatusBar();
        
        if (show_help) {
            drawHelpScreen();
        }
        
        // Handle input
        int ch = getch();
        if (ch != ERR) {
            handleInput(ch);
        }
        
        // Small delay to prevent CPU spinning
        usleep(50000); // 50ms
    }
}

void TUI::setRefreshInterval(int seconds) {
    refresh_interval = seconds;
}
