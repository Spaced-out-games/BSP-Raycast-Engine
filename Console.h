#pragma once
#include "imgui.h"
#include <vector>
#include <string>
#include <cstring>  // For `strcmp`
#include <iostream>
#include <unordered_map>
#include <functional>
#include <cstdlib>   // For std::strtol, std::strtof
#include <cctype>    // For std::isspace
#include <array>
#include "glm_master.h"
#include <cmath>
#define CONSOLE_ACCENT_CHAR '~'

#define console_command_t std::function<void(Console&, std::string&)>

// ConsoleArgument struct
struct ConsoleArgument {
    std::string argument_content;
    bool has_accent;

    ConsoleArgument(std::string argument) {
        if (argument[0] == CONSOLE_ACCENT_CHAR) {
            argument_content = argument;
            argument_content.erase(0, 1);
            has_accent = true;
        }
        else {
            argument_content = argument;
            has_accent = false;
        }
    }

    ConsoleArgument() : has_accent(false), argument_content("") {}

    // Conversion to int
    int as_int() const {
        char* end;
        long result = std::strtol(argument_content.c_str(), &end, 10);

        if (*end != '\0' || std::isspace(*end)) {
            throw std::invalid_argument("Invalid integer argument: " + argument_content);
        }

        return static_cast<int>(result);
    }

    // Conversion to float
    float as_float() const {
        char* end;
        float result = std::strtof(argument_content.c_str(), &end);

        if (*end != '\0' || std::isspace(*end)) {
            throw std::invalid_argument("Invalid float argument: " + argument_content);
        }

        return result;
    }
};

// Parse arguments
ConsoleArgument parse_argument(std::stringstream& stream) {
    std::string string;
    stream >> string;
    return ConsoleArgument(string);
}

// Parse vec3 from arguments
glm::vec3 parse_vec3(glm::vec3& offset, std::stringstream& stream) {
    std::string arg1, arg2, arg3;

    // Read the next three arguments from the stream
    stream >> arg1 >> arg2 >> arg3;

    // Create ConsoleArgument instances for each argument
    std::array<ConsoleArgument, 3> arguments = { arg1, arg2, arg3 };

    glm::vec3 result(0.0f, 0.0f, 0.0f);

    // Iterate over each argument and calculate the result
    for (int i = 0; i < 3; ++i) {
        if (arguments[i].has_accent) {
            result[i] = offset[i] + arguments[i].as_float();
        }
        else {
            result[i] = arguments[i].as_float();
        }
    }
    return result;
}

class Console;

// ConsoleCommand struct
struct ConsoleCommand {
    //std::string hint;
    std::string description;
    console_command_t command;

    ConsoleCommand() = default;

    ConsoleCommand(std::string description, console_command_t command)
        : description(std::move(description)), command(std::move(command)) {}

    ConsoleCommand(ConsoleCommand&& other) noexcept
        : command(std::move(other.command)), description(std::move(other.description)) {}

    ConsoleCommand& operator=(ConsoleCommand&& other) noexcept {
        if (this != &other) {
            command = std::move(other.command);
            description = std::move(other.description);
        }
        return *this;
    }

    void execute(Console& console, std::string& arguments) {
        command(console, arguments);
    }
};

// Console class
struct Console {
protected:
    const char* title = "Console";
    char inputBuf[256]{};
    std::vector<std::string> log;
    int historyPos;
    bool scrollToBottom;

public:
    std::unordered_map<std::string, ConsoleCommand> commands;
    bool* open = nullptr;

    Console() : historyPos(-1), scrollToBottom(false) {
        ClearLog();
        AddLog("Console initialized");
    }

    void AddLog(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log.emplace_back(fmt);  // Consider formatting here
        va_end(args);
    }

    void ClearLog() {
        log.clear();
    }

    virtual void Draw() {
        ImGui::SetNextWindowSize(ImVec2(720, 720), ImGuiCond_FirstUseEver);
        ImVec2 buttonSize = { 120, 19 };

        if (!ImGui::Begin(title, open)) {
            ImGui::End();
            return;
        }

        if (ImGui::Button("Clear"))
            ClearLog();

        ImGui::SameLine();
        bool copyToClipboard = ImGui::Button("Copy");
        ImGui::SameLine();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImGui::SetCursorPosX(windowSize.x - buttonSize.x - 10);
        if (ImGui::Button("Exit Application")) {
            exit(1);
        }

        ImGui::Separator();

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetTextLineHeightWithSpacing() * 2), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (copyToClipboard)
            ImGui::LogToClipboard();

        for (const auto& message : log)
            ImGui::TextUnformatted(message.c_str());

        if (scrollToBottom)
            ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;

        ImGui::EndChild();
        ImGui::Separator();

        ImGui::PushItemWidth(-1);
        if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
            std::string inputStr(inputBuf);
            if (ExecuteCommand(inputStr)) {
                AddLog(inputStr.c_str());
            }
            memset(inputBuf, 0, sizeof(inputBuf));
        }

        if (ImGui::IsItemHovered() || ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
            ImGui::SetKeyboardFocusHere(1);
        }

        ImGui::PopItemWidth();
        ImGui::End();
    }

    virtual bool ExecuteCommand(const std::string& commandLine) {
        auto spaceIndex = commandLine.find(' ');
        std::string command = commandLine.substr(0, spaceIndex);
        std::string args = (spaceIndex != std::string::npos) ? commandLine.substr(spaceIndex + 1) : "";

        auto it = commands.find(command);
        if (it != commands.end()) {
            it->second.execute(*this, args);
            return true;
        }
        else {
            AddLog("ERROR: Command not found");
            return false;
        }
    }



    void RegisterCommand(const std::string& name ,const std::string& description = "No Description", console_command_t func = [](Console&, std::string&) {}) {
        commands[name] = ConsoleCommand(description, func);
    }

    void print(const std::string& content) {
        log.emplace_back(content);
        scrollToBottom = true;
    }

    void println(const std::string& content) {
        log.emplace_back(content + "\n");
        scrollToBottom = true;
    }
};

// define the convar struct here eventually


// basic_console class
class basic_console : public Console {
public:
    std::vector<std::string> recorded_commands;
    bool is_recording;

    basic_console();

    bool ExecuteCommand(const std::string& commandLine) override;
};

bool basic_console::ExecuteCommand(const std::string& commandLine)
{
    if (commandLine == "") { return 0; }
    auto spaceIndex = commandLine.find(' ');
    std::string command = commandLine.substr(0, spaceIndex);
    std::string args = (spaceIndex != std::string::npos) ? commandLine.substr(spaceIndex + 1) : "";

    // If recording, append the command to the recorded_commands vector
    if (is_recording) {
        recorded_commands.push_back(commandLine);
    }

    auto it = commands.find(command);
    if (it != commands.end()) {
        AddLog(commandLine.c_str());
        it->second.execute(*this, args);
        return 1;
    }
    else {
        AddLog("ERROR: Command not found");
    }
    return 0;
}
//#define console_command_t std::function<void(Console&, std::string&)>



basic_console::basic_console()
{
    RegisterCommand("exit", "Exits the application", [](Console& console, std::string& args) {
        std::cout << "Exiting application..." << std::endl;
        exit(0);
    });

    RegisterCommand("echo", "Prints a message to the screen", [](Console& console, std::string& args) {
        console.AddLog(args.c_str());
    });

    RegisterCommand("add_brush", "Prints a message to the screen", [](Console& console, std::string& args) {
        std::stringstream argument_stream(args);
        vec3 position = globals.current_client->camera->get_position();
        

        vec3 dimensions;
        position = parse_vec3(position, argument_stream);
        dimensions = parse_vec3(position, argument_stream);
        size_t last_index = globals.server->get_entity_container<ent_brush>().size();


        globals.server->get_entity_container<ent_brush>().emplace_back(position, dimensions);
    });

    RegisterCommand("r_wireframe", "Prints a message to the screen", [](Console& console, std::string& args) {
        bool use_wireframe;
        std::stringstream argument_stream(args);
        argument_stream >> use_wireframe;
        globals.set_render_mode(use_wireframe);
        //Brush::use_wireframe = use_wireframe;

    });

}