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
#include <cmath>
#define CONSOLE_ACCENT_CHAR '~'


//todo:
        // exec <filename>
        // save <filename.brushlist>
        // load <filename.brushlist>
        // compile <filename.bsp>
        // optimize <start_seed> <end_seed>
        // sleep <seconds>
        // tp <position>
        // lookat <position>
        // noclip
        // tp_brush <position>
        //
        // It might be better to structure commands into their own file.
        //
        //
        // help (command)
        //typedef struct console_command
        //{
            // The actual function
            // std::function function;
            // The name of the invoked function
            // std::string convar_name;
            // Description of the function
            // std::string description;
            // Defines the number of arguments there are for each overload
            // std::vector<std::vector<uint8_t>> argument_types;


        //};
struct ConsoleArgument
{
    std::string argument_content;
    bool has_accent;
    ConsoleArgument(std::string argument)
    {
        if (argument[0] == CONSOLE_ACCENT_CHAR)
        {
            argument_content = argument;
            argument_content.erase(0, 1);
            has_accent = true;
        }
        else
        {
            argument_content = argument;
            has_accent = false;
        }
    }
    ConsoleArgument()
    {
        has_accent = false;
        argument_content = "";
    }
    // Conversion to int
    int as_int() const
    {
        char* end;
        long result = std::strtol(argument_content.c_str(), &end, 10);

        if (*end != '\0' || std::isspace(*end))
        {
            throw std::invalid_argument("Invalid integer argument: " + argument_content);
        }

        return static_cast<int>(result);
    }

    // Conversion to float
    float as_float() const
    {
        char* end;
        float result = std::strtof(argument_content.c_str(), &end);

        if (*end != '\0' || std::isspace(*end))
        {
            throw std::invalid_argument("Invalid float argument: " + argument_content);
        }

        return result;
    }
};

ConsoleArgument parse_argument(std::stringstream& stream)
{
    std::string string;
    stream >> string;
    return ConsoleArgument(string);
}

glm::vec3 parse_vec3(glm::vec3& offset, std::stringstream& stream)
{
    std::string arg1, arg2, arg3;

    // Read the next three arguments from the stream
    stream >> arg1 >> arg2 >> arg3;

    // Create ConsoleArgument instances for each argument
    std::array<ConsoleArgument, 3> arguments = { arg1, arg2, arg3 };

    glm::vec3 result(0.0f, 0.0f, 0.0f);

    // Iterate over each argument and calculate the result
    for (int i = 0; i < 3; ++i)
    {
        if (arguments[i].has_accent)
        {
            // If accented, treat as relative to the offset
            if (arguments[i].argument_content.empty())
            {
                result[i] = offset[i];  // Treat as 0 offset if empty after accent
            }
            else
            {
                result[i] = offset[i] + arguments[i].as_float();  // Add relative offset
            }
        }
        else
        {
            // If not accented, treat as an absolute value
            result[i] = arguments[i].as_float();
        }
    }
    return result;
}


template <typename Pawn>
class Console;


template <typename Pawn>
struct ConsoleCommand {
    std::function<void(Pawn*, Console<Pawn>&, std::string&)> func;
    std::string description = "No description available";

    // Default constructor
    ConsoleCommand()
        : func([](Pawn*, Console<Pawn>&, std::string&) {  }) {}

    // Parameterized constructor
    ConsoleCommand(std::function<void(Pawn*, Console<Pawn>&, std::string&)> f)
        : func(std::move(f)) {}

    ConsoleCommand(std::function<void(Pawn*, Console<Pawn>&, std::string&)> f, std::string description)
        : func(std::move(f)), description(description) {}

    // Move constructor
    ConsoleCommand(ConsoleCommand&& other) noexcept
        : func(std::move(other.func)), description(std::move(other.description)) {}

    // Move assignment operator
    ConsoleCommand& operator=(ConsoleCommand&& other) noexcept {
        if (this != &other) {
            func = std::move(other.func);
            //description = other.description;
        }
        return *this;
    }

    // Delete copy constructor and copy assignment operator
    ConsoleCommand(const ConsoleCommand&) = delete;
    ConsoleCommand& operator=(const ConsoleCommand&) = delete;

    // Method to execute with string input
    void execute(Pawn* wc, Console<Pawn>& console, std::string& args) {
        func(wc, console, args);
    }
};


//template <typename Pawn>
class Console {
public:
    using Pawn = void;
    static std::unordered_map<std::string, ConsoleCommand<Pawn>> commands;
    static Pawn* target_application;
    bool* open;
    Console(Pawn app_interface) {
        target_application = &app_interface;
        ClearLog();
        historyPos = -1;
        memset(inputBuf, 0, sizeof(inputBuf));
        AddLog("Console initialized");
    }

    virtual void AddLog(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log.emplace_back(fmt);  // Consider formatting here
        va_end(args);
    }

    virtual void ClearLog() {
        log.clear();
    }

    virtual void Draw(const char* title) {
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
        if (ImGui::Button("Exit Application"))
        {
            exit(1);
        }
        //std::cout << buttonSize.x << ", " << buttonSize.y;
        //exit(0);

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
            if (ExecuteCommand(inputStr))
            {
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

            it->second.execute(target_application, *this, args); // Pass args as std::string
            return 1;
        }
        else {
            AddLog("ERROR: Command not found");
            return 0;
        }
    }

    static void RegisterCommand(const std::string& name, std::function<void(Pawn*, Console<Pawn>&, std::string&)> func) {
        commands[name] = ConsoleCommand<Pawn>(func);
    }
    static void RegisterCommand(const std::string& name, std::function<void(Pawn*, Console<Pawn>&, std::string&)> func, std::string description) {
        commands[name] = ConsoleCommand<Pawn>(func);
        commands[name].description = description;
    }

    void print(const std::string& content) {
        log.emplace_back(content);
        scrollToBottom = true;
    }

    void println(const std::string& content) {
        log.emplace_back(content + "\n");
        scrollToBottom = true;
    }

protected:
    char inputBuf[256];
    std::vector<std::string> log;
    int historyPos;  // Index into command history
    bool scrollToBottom;

};

// Define the static member outside the class
template <typename Pawn>
std::unordered_map<std::string, ConsoleCommand<Pawn>> Console<Pawn>::commands;

template <typename Pawn>
Pawn* Console<Pawn>::target_application = nullptr;

// Usage with basic_console
#include "windowContent.h"

class basic_console : public Console {
public:


    std::vector<std::string> recorded_commands;
    bool is_recording;

    virtual bool ExecuteCommand(const std::string& commandLine) override {
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
            it->second.execute(target_application, *this, args);
            return 1;
        }
        else {
            AddLog("ERROR: Command not found");
        }
        return 0;
    }

    basic_console(windowContent& app_interface) : Console<windowContent>(app_interface) {
        // Register commands


        RegisterCommand("record_script", [this](windowContent* wc, Console<windowContent>& console, std::string& args) {
            if (args == "start") {
                is_recording = true;
                console.AddLog("Started recording commands.");
            }
            else if (args == "stop") {
                is_recording = false;
                console.AddLog("Stopped recording commands.");
            }
            else if (args == "clear")
            {
                recorded_commands.clear();
            }
            else {
                console.AddLog("Usage: record_script start|stop|clear");
            }
        }, "Starts or stops recording commands.\nUsage: record_script start|stop");

        RegisterCommand("script_copy", [this](windowContent* wc, Console<windowContent>& console, std::string& args) {
            if (recorded_commands.empty()) {
                console.AddLog("No commands recorded to copy.");
                return;
            }

            std::string combined_commands;
            for (const auto& cmd : recorded_commands) {
                combined_commands += cmd + "\n";
            }

            // Use ImGui's clipboard functionality
            ImGui::SetClipboardText(combined_commands.c_str());
            console.AddLog("Copied recorded commands to clipboard.");
        }, "Copies the recorded commands to the clipboard.");

        RegisterCommand("exit", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            exit(64);
        }, "Exits the application.\nUsage: exit Y/N");

        RegisterCommand("echo", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            console.AddLog(args.c_str());
        }, "Prints a message to this console\nUsage: echo <expression>");

        RegisterCommand("clear", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            console.ClearLog();
        }, "Clears this console\nUsage: Clear");

        RegisterCommand("add_brush", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
        std::stringstream argument_stream(args);
        glm::vec3 position = wc->camController->getPawn().getPosition();
        glm::vec3 dimensions;

        //
        position = parse_vec3(position, argument_stream);

        // relative location
        dimensions = parse_vec3(position, argument_stream);
        size_t last_index = wc->brushes.size();
        wc->brushes.push_back({ position, dimensions });
        wc->brushes[last_index].init(position, dimensions);
        //add_brush(position, dimensions);
        }, "Adds a brush to the scene with the specified location and dimensions\nUsage: add_brush 0 0 0 20 20 20");



        RegisterCommand("move_speed", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            float speed;
        std::stringstream argument_stream(args);
        argument_stream >> speed;
        wc->camController->move_speed = speed;
        }, "Sets the default move speed of the camera\nUsage: move_speed 40");

        RegisterCommand("sprint_speed_multiplier", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            float multiplier;
        std::stringstream argument_stream(args);
        argument_stream >> multiplier;
        wc->camController->sprint_multiplier = multiplier;
        }, "Sets the multplier that pressing 'shift' has on speed\nUsage: sprint_speed_multiplier 1.5");

        RegisterCommand("return", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            wc->camController->marked_for_processing = false;
        }, "Closes the console and returns to the editor\n Usage: return");

        RegisterCommand("move_vertex", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            std::stringstream argument_stream(args);
        int& index = wc->selected_vertex_index;
        auto& brushes = wc->brushes;
        auto vertices = brushes[wc->selected_brush_index].get_vertices();
        glm::vec3 location = parse_vec3(vertices[index].position, argument_stream);
        //vertices[index].position = location;
        brushes[wc->selected_brush_index].move_vertex(wc->selected_vertex_index, location);
        //console.AddLog("SYNTAX ERROR: move_vertex takes two arguments: <int> <float float float>");
        }, "Moves the vertex (defined by select_vertex) of the selected brush (defined by select_brush) to the specified location");

        RegisterCommand("select_brush", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            std::stringstream argument_stream(args);
        int temp;
        argument_stream >> temp;
        wc->selected_brush_index = std::clamp(temp, 0, static_cast<int>(wc->brushes.size() - 1));
        if (temp < 0 || temp >= wc->brushes.size()) {
            //invalid brush index
        }
        },"Selects a brush in the scene");

        RegisterCommand("select_vertex", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            std::stringstream argument_stream(args);
        int index;
        argument_stream >> index;
        if (index >= 0 && index <= 7) {
            wc->selected_vertex_index = index;
        }
        else {
            console.AddLog("SYNTAX ERROR: select_vertex takes one argument: <int>");
        }
        }, "Selects a vertex from the specified brush");

        RegisterCommand("tp", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            std::stringstream argument_stream(args);
        glm::vec3 position = parse_vec3(wc->camController->getPawn().getPosition(), argument_stream);
        glm::vec3 delta = position - wc->camController->getPawn().getPosition();
        wc->camController->getPawn().setPosition(position);
        wc->camController->getPawn().setTarget(wc->camController->getPawn().getTarget() + delta);
        }, "Teleports the player to the specified location");

        RegisterCommand("r_wireframe", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            bool use_wireframe;
        std::stringstream argument_stream(args);
        argument_stream >> use_wireframe;
        Brush::use_wireframe = use_wireframe;
        }, "toggles wireframe mode");

        RegisterCommand("delete_brush", [this](windowContent* wc, Console<windowContent>& console, std::string& args) {
        std::string index_string;
        std::stringstream argument_stream(args);

        argument_stream >> index_string;
        if (index_string == "all")
        {
            size_t i = wc->brushes.size() - 1;
            for (i =i; i > 0; i--)
            {
                wc->brushes[i].destroy();
                wc->brushes.pop_back();
                //wc->brushes[wc->brushes.end()].destroy();
                //wc->brushes.erase(wc->brushes.end());
            }
            wc->brushes[0].destroy();
            wc->brushes.pop_back();
            return;
        }

        int index = std::stoi(index_string);

        if (index >= 0 && index < wc->brushes.size()) {
            wc->brushes[index].destroy();
            wc->brushes.erase(wc->brushes.begin() + index);
            //console.AddLog("Deleted brush at index: " + std::to_string(index));
        }
        else {
            //console.AddLog("Invalid brush index: " + std::to_string(index) + "; no brush deleted.");
        }
        }, "Deletes a brush");

        RegisterCommand("exec", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            std::stringstream argument_stream(args);
            std::string filename;
            argument_stream >> filename;
            std::ifstream file(filename);
            if (!file.is_open()) {
                //console.AddLog("ERROR: Could not open file: " + filename);
                return;
            }

            std::string line;
            while (std::getline(file, line)) {
                console.ExecuteCommand(line);
            }
        }, "Executes a .cfg file");

        RegisterCommand("help", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            if (console.commands.find(args) != console.commands.end()) {
                console.AddLog(console.commands.at(args).description.c_str());
            }
            else
            {
                std::string message("Command '" + args + "' does not exist");
                console.AddLog(message.c_str());

            }
        }, "You don't have to scream at me!");

        RegisterCommand("duplicate_brush", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            //exit(64);
            Brush& target_brush = wc->brushes[wc->selected_brush_index];
            size_t target_index = wc->brushes.size();
            wc->brushes.push_back({});

            Brush& new_brush = wc->brushes[wc->brushes.size() - 1];
            new_brush.init({0, 0, 0}, {0,0,0});

            new_brush.update_vertices(target_brush.get_vertices());

            //new_brush_vertices = target_brush.get_vertices();

            //wc->brushes[target_index].update_vertices(target_brush.get_vertices());
            wc->selected_brush_index = target_index;
        //wc->brushes.push
        }, "Duplicates the selected brush and sets the selected brush to the new brush");

        // Absolute positioning needs implemented
        RegisterCommand("move_brush", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            std::stringstream argument_stream(args);
            Brush& target_brush = wc->brushes[wc->selected_brush_index];
            glm::vec3 delta_position;
            glm::vec3& origin = target_brush.get_origin();
            ConsoleArgument x = parse_argument(argument_stream);
            if (x.has_accent)
            {
                delta_position.x = x.as_float();
            }
            else
            {
                delta_position.x = 0;
            }
            ConsoleArgument y = parse_argument(argument_stream);
            if (y.has_accent)
            {
                delta_position.y = y.as_float();
            }
            else
            {
                delta_position.y = 0;
            }
            ConsoleArgument z = parse_argument(argument_stream);
            if (z.has_accent)
            {
                delta_position.z = z.as_float();
            }
            else
            {
                delta_position.z = 0;
            }
            std::vector<Brush_vertex_t>& target_vertices = target_brush.get_vertices();
            for (int i = 0; i < 8; i++)
            {
                target_vertices[i].position += delta_position;
            }
            target_brush.update_vertices(target_vertices);

        }, "Exits the application.\nUsage: exit Y/N");

        RegisterCommand("set_mat", [](windowContent* wc, Console<windowContent>& console, std::string& args) {
            exit(64);
        }, "Sets the material of a face");
    }
};